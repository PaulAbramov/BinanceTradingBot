#include <chrono>
#include <filesystem>
#include <future>
#include <iostream>

#include "WebSocketCollection.h"
#include "ConfigurationManager.h"
#include "FileLogger.h"
#include "StandardOutputLogger.h"
#include "ApiRequestManager.h"
#include "APIEnums.h"
#include "ConnectionPool.h"

#include "SQLAPI.h"
#include "SQLConnectionFactory.h"

using namespace std;
using namespace std::chrono;

namespace fs = std::filesystem;

static Logger InitializeLogger()
{
	string filename = "Logger.conf";

	// if configuration file exists - use FilesystemLogger
	if (ifstream filestream(filename); filestream.good())
	{
		//Check if logs directory exists, if not create it
		if(!fs::exists("logs"))
		{
			fs::create_directory("logs");
		}

		// Load configuration from file
		el::Configurations config(filename);
		// Actually reconfigure all loggers instead
		el::Loggers::reconfigureAllLoggers(config);

		// Get the current date
		struct tm newtime{};
		time_t now = std::time(nullptr);
		localtime_s(&newtime, &now);

		// Make a string with the current date
		ostringstream oss;
		oss << "logs/" << put_time(&newtime, "%d-%m-%Y_%H-%M-%S") << ".log";

		// set globally the filepath to the file with the currentdate
		config.setGlobally(el::ConfigurationType::Filename, oss.str());
		el::Loggers::reconfigureLogger("default", config);

		return make_shared<FileLogger>();
	}
	else
	{
		return make_shared<StandardOutputLogger>();
	}
}

std::string LongToString(const int64_t _longDate)
{
	char buff[128];

	const duration<int64_t, std::milli> duration (_longDate);
	const auto timePoint = system_clock::time_point(duration_cast<system_clock::duration>(duration));
	const std::time_t inTimeT = system_clock::to_time_t(timePoint);
	struct tm newtime {};
	localtime_s(&newtime, &inTimeT);
	strftime(buff, 128, "%Y-%m-%d %H:%M:%S", &newtime);
	std::string resDate(buff);

	return resDate;
}

int main(int argc, char** argv)
{
	Logger logger = InitializeLogger();
	logger->WriteInfoEntry("Logger initialized");

	ConfigurationManager configManager = ConfigurationManager(logger);
	Config config = configManager.LoadConfig();

	ApiRequestManager manager = ApiRequestManager(logger, config.api_key, config.secret_key);

	std::vector<string> symbols{ "BTCUSDT", "BNBBUSD", "ETHUSDT", "BUSDUSDT" };

	shared_ptr<SqlConnectionFactory> connectionFactory(new SqlConnectionFactory("localhost@Binance;MARS Connection=True;", "", ""));
	shared_ptr<ConnectionPool<SAConnection>> pool(new ConnectionPool<SAConnection>(symbols.size(), connectionFactory));

	// The io_context is required for all I/O
	net::io_context ioc;

	vector<void*> handles;

	WebSocketCollection websocketCollection{ ioc, BINANCE_HOST, BINANCE_PORT, logger };

	auto oneMinuteCandlestickHandle = websocketCollection.KlineCandleStick(symbols, EIntervals::ONEMINUTE,
		[_logger = logger, _pool = pool](auto _answer)
		{
			_logger->WriteInfoEntry(_answer.c_str());

			auto jsonResult = nlohmann::json::parse(_answer);

			if(jsonResult.contains("data"))
			{
				jsonResult = jsonResult["data"];
			}

			const string symbol = jsonResult["s"];
			const string interval = jsonResult["k"]["i"];
			const time_t candleCloseTime = jsonResult["k"]["T"];
			const string open = jsonResult["k"]["o"];
			const string high = jsonResult["k"]["h"];
			const string low = jsonResult["k"]["l"];
			const string close = jsonResult["k"]["c"];

			const string dateTime = LongToString(candleCloseTime);

			shared_ptr<SAConnection> connection;
			try
			{
				connection = _pool->Borrow();
			}
			catch(ConnectionUnavailable& exception)
			{
				return true;
			}

			bool existingEntry = false;
			try
			{
				SACommand select(&(*connection), _TSA("SELECT ASSET, DATETIME FROM ASSETS WHERE DateTime = :1 AND Asset = :2"));

				select.Param(1).setAsDateTime() = _TSA(dateTime.c_str());
				select.Param(2).setAsString() = _TSA(symbol.c_str());
				select.Execute();

				try
				{
					while (select.FetchNext())
					{
						string getSymbol = select[1].asString().GetMultiByteChars();
						getSymbol.erase(ranges::remove(getSymbol, ' ').begin(), getSymbol.end());
						if (getSymbol == symbol)
						{
							existingEntry = true;

							SACommand update(&(*connection), _TSA("UPDATE ASSETS SET CandleHigh = :1, CandleLow = :2, CandleClose = :3 WHERE DateTime = :4 AND Asset = :5; "));

							update.Param(1).setAsDouble() = stod(high);
							update.Param(2).setAsDouble() = stod(low);
							update.Param(3).setAsDouble() = stod(close);
							update.Param(4).setAsDateTime() = _TSA(dateTime.c_str());
							update.Param(5).setAsString() = _TSA(symbol.c_str());
							update.Execute();
						}

						break;
					}
				}
				catch (SAException& exception)
				{
					(&(*connection))->Rollback();
					_logger->WriteErrorEntry("inner exception");
					_logger->WriteErrorEntry(exception.ErrText().GetMultiByteChars());
					_pool->Unborrow(connection);
					return false;
				}
			}
			catch (SAException& exception2)
			{
				(&(*connection))->Rollback();
				_logger->WriteErrorEntry("outer exception");
				_logger->WriteErrorEntry(exception2.ErrText().GetMultiByteChars());
				_pool->Unborrow(connection);
				return false;
			}

			try
			{
				if (!existingEntry)
				{
					SACommand insert(&(*connection), _TSA("INSERT INTO ASSETS(Asset, Interval, DateTime, CandleOpen, CandleHigh, CandleLow, CandleClose) VALUES(:1, :2, :3, :4, :5, :6, :7)"));

					insert.Param(1).setAsString() = _TSA(symbol.c_str());
					insert.Param(2).setAsString() = _TSA(interval.c_str());
					insert.Param(3).setAsDateTime() = _TSA(dateTime.c_str());
					insert.Param(4).setAsDouble() = stod(open);
					insert.Param(5).setAsDouble() = stod(high);
					insert.Param(6).setAsDouble() = stod(low);
					insert.Param(7).setAsDouble() = stod(close);

					insert.Execute();
				}
			}
			catch (SAException& exc)
			{
				(&(*connection))->Rollback();
				_logger->WriteErrorEntry("third exception");
				_logger->WriteErrorEntry(exc.ErrText().GetMultiByteChars());
				_pool->Unborrow(connection);
				return false;
			}

			_pool->Unborrow(connection);

			return true;
		});

	handles.emplace_back(oneMinuteCandlestickHandle);

	int counter = 0;
	while (!handles.empty()) 
	{
		if (ioc.stopped()) 
		{
			ioc.restart();
		}
		if (!ioc.poll())
		{
			// TODO create messagelist to send commands or attach websockets to ioc
			if (counter > 1000) 
			{
				ioc.post([_logger = logger, _manager = manager]()
					{
						auto test = _manager.GetWalletWithdrawtHistory("", "", 6, 0, 0, 0);

						_logger->WriteInfoEntry(test);

					});

				ioc.post([_logger = logger, _manager = manager]()
					{
						auto test = _manager.GetSpotAccountCurrentOrderCountUsage ();

						_logger->WriteInfoEntry(test);

					});

				counter = 0;
			}
			else 
			{
				counter++;
				std::this_thread::sleep_for(milliseconds(3));
			}
		}
	}

	return EXIT_SUCCESS;
}