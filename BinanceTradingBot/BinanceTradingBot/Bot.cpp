#include "Bot.h"

Bot::Bot(const Logger& _logger, const Config& _config): manager(_logger, _config.api_key, _config.secret_key)
{
	logger = _logger;
	config = _config;
}

bool Bot::Run() const
{
	AutomaticRun();

	logger->WriteWarnEntry("Closing Programm.");

	return EXIT_SUCCESS;
}

void Bot::AutomaticRun() const
{
	net::io_context ioc;

	clock_t start = clock();

	while (true)
	{
		if (ioc.stopped())
		{
			ioc.restart();
		}
		if (!ioc.poll())
		{
			// TODO create messagelist to send commands or attach websockets to ioc
			// 15sekunden wegen api warten
			// Free:	 1 API request / 15 seconds
			// Basic:	 5 API requests / 15 seconds
			// Pro :	30 API requests / 15 seconds
			// Expert : 75 API requests / 15 seconds
			if (((static_cast<float>(clock()) - start) / CLOCKS_PER_SEC) > 15)
			{
				ioc.post([this, &start, _logger = logger, _manager = manager, _config = config]()
					{
						string result;
						string url = "https://api.taapi.io/stochrsi?secret=";
						url += _config.taapi_secret;
						url += "&exchange=binance&symbol=BTC/USDT&interval=1m&backtracks=10";

						_manager.CurlAPI(url, result);

						//_logger->WriteInfoEntry(result);

						auto jsonResult = nlohmann::json::parse(result);

						if(jsonResult[0]["valueFastK"] > 20 && jsonResult[1]["valueFastK"] < 20)
						{
							//_logger->WriteInfoEntry("BTCUSDT jetzt KAUFEN");
							//auto test = _manager.GetMarketDataSymbolPriceTicker("BTCUSDT");
							//_logger->WriteInfoEntry(test);

							auto trade = _manager.PostSpotAccountNewOrder("BTCBUSD", ETimeInForce::NONE, 0, 20, 0, "1", 0, 0, ENewOrderResponseType::FULL, ESide::BUY, EOrderType::MARKET);

							auto jsonResultTrade = nlohmann::json::parse(trade);


						}

						if (jsonResult[0]["valueFastK"] > 80 && jsonResult[1]["valueFastK"] < 80)
						{
							_logger->WriteInfoEntry("BTCUSDT jetzt VERKAUFEN");
							auto test = _manager.GetMarketDataSymbolPriceTicker("BTCUSDT");
							_logger->WriteInfoEntry(test);
						}




						//const string symbol = jsonResult["s"];
						//const string interval = jsonResult["k"]["i"];
						//const time_t candleCloseTime = jsonResult["k"]["T"];
						//const string open = jsonResult["k"]["o"];
						//const string high = jsonResult["k"]["h"];
						//const string low = jsonResult["k"]["l"];
						//const string close = jsonResult["k"]["c"];

						//const string dateTime = LongToString(candleCloseTime);

						start = clock();
					});

				//ioc.post([_logger = logger, _manager = manager]()
				//	{
				//		auto test = _manager.GetSpotAccountCurrentOrderCountUsage();
				//
				//		_logger->WriteInfoEntry(test);
				//
				//	});
			}
			else
			{
				std::this_thread::sleep_for(milliseconds(3));
			}
		}
	}
}

void Bot::ManualRun()
{
	const shared_ptr<SqlConnectionFactory> connectionFactory(new SqlConnectionFactory("localhost@Binance;MARS Connection=True;", "", ""));
	shared_ptr<ConnectionPool<SAConnection>> pool(new ConnectionPool<SAConnection>(symbols.size(), connectionFactory));

	net::io_context ioc;

	vector<void*> handles;

	WebSocketCollection websocketCollection{ ioc, BINANCE_HOST, BINANCE_PORT, logger };

	auto fifteenMinuteCandlestickHandle = websocketCollection.KlineCandleStick(symbols, EIntervals::FIFTEENMINUTES,
		[this, &pool](auto _answer)
		{
			return HandleRequest(pool, _answer);
		});

	auto oneHourCandlestickHandle = websocketCollection.KlineCandleStick(symbols, EIntervals::ONEHOUR,
		[this, &pool](auto _answer)
		{
			return HandleRequest(pool, _answer);
		});

	auto oneDayCandlestickHandle = websocketCollection.KlineCandleStick(symbols, EIntervals::ONEDAY,
		[this, &pool](auto _answer)
		{
			return HandleRequest(pool, _answer);
		});

	handles.emplace_back(fifteenMinuteCandlestickHandle);
	handles.emplace_back(oneHourCandlestickHandle);
	handles.emplace_back(oneDayCandlestickHandle);

	clock_t start = clock();
	while (!handles.empty())
	{
		if (ioc.stopped())
		{
			ioc.restart();
		}
		if (!ioc.poll())
		{
			// TODO create messagelist to send commands or attach websockets to ioc
			// 15sekunden wegen api warten
			// Free:	 1 API request / 15 seconds
			// Basic:	 5 API requests / 15 seconds
			// Pro :	30 API requests / 15 seconds
			// Expert : 75 API requests / 15 seconds
			if (((static_cast<float>(clock()) - start) / CLOCKS_PER_SEC) > 15)
			{
				ioc.post([this, &start, _logger = logger, _manager = manager, _config = config]()
					{
						string result;
						string url = "https://api.taapi.io/rsi?secret=";
						url += _config.taapi_secret;
						url += "&exchange=binance&symbol=BTC/USDT&interval=1h&backtracks=10";

						_manager.CurlAPI(url, result);

						_logger->WriteInfoEntry(result);

						start = clock();
					});

				ioc.post([this, &start, _logger = logger, _manager = manager]()
					{
						auto test = _manager.GetSpotAccountCurrentOrderCountUsage();

						_logger->WriteInfoEntry(test);
					});

			}
			else
			{
				std::this_thread::sleep_for(milliseconds(3));
			}
		}
	}
}

bool Bot::HandleRequest(const shared_ptr<ConnectionPool<SAConnection>>& _pool, const string& _answer) const
{
	logger->WriteInfoEntry(_answer.c_str());
	
	auto jsonResult = nlohmann::json::parse(_answer);
	
	if (jsonResult.contains("data"))
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
	catch (ConnectionUnavailable& exception)
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
			logger->WriteErrorEntry("inner exception");
			logger->WriteErrorEntry(exception.ErrText().GetMultiByteChars());
			_pool->Unborrow(connection);
			return false;
		}
	}
	catch (SAException& exception2)
	{
		(&(*connection))->Rollback();
		logger->WriteErrorEntry("outer exception");
		logger->WriteErrorEntry(exception2.ErrText().GetMultiByteChars());
		_pool->Unborrow(connection);
		return false;
	}

	// Insert Asset
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
		logger->WriteErrorEntry("third exception");
		logger->WriteErrorEntry(exc.ErrText().GetMultiByteChars());
		_pool->Unborrow(connection);
		return false;
	}
	
	_pool->Unborrow(connection);
	
	return true;
}

string Bot::LongToString(const int64_t _longDate) const
{
	char buff[128];

	const duration<int64_t, std::milli> duration(_longDate);
	const auto timePoint = system_clock::time_point(duration_cast<system_clock::duration>(duration));
	const std::time_t inTimeT = system_clock::to_time_t(timePoint);
	struct tm newtime {};
	localtime_s(&newtime, &inTimeT);
	strftime(buff, 128, "%Y-%m-%d %H:%M:%S", &newtime);
	std::string resDate(buff);

	return resDate;
}