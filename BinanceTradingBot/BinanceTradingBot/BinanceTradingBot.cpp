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

#include "SQLAPI.h"

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

int main(int argc, char** argv)
{
	SAConnection connection;

	try 
	{
		connection.Connect(_TSA("localhost@Binance"), _TSA(""), _TSA(""), SA_SQLServer_Client);
		printf("We are connected!\n");

		/*
		The rest of the tutorial goes here!
		*/

		connection.Disconnect();
		printf("We are disconnected!\n");
	}
	catch (SAException& x) 
	{
		connection.Rollback();
		printf("%s\n", x.ErrText().GetMultiByteChars());
	}

	Logger logger = InitializeLogger();
	logger->WriteInfoEntry("Logger initialized");

	ConfigurationManager configManager = ConfigurationManager(logger);
	Config config = configManager.LoadConfig();

	ApiRequestManager manager = ApiRequestManager(logger, config.api_key, config.secret_key);

	auto test = manager.GetSpotAccountTradeList("BNBBUSD", 0, 0, 0, 0);

	auto test2 = nlohmann::json::parse(test);
	//manager.GetWalletWithdrawtHistory("", "", 6, 0, 0, 0);
	//manager.GetAggregatedTradeStreams({"test"});

	// The io_context is required for all I/O
	net::io_context ioc;

	WebSocketCollection websocketCollection{ ioc, BINANCE_HOST, BINANCE_PORT, logger };

	std::vector<string> symbols{"BNBBUSD"};
	auto tradebnbHandle = websocketCollection.DiffBookDepth(symbols, EFrequency::HUNDREDMILI,
		[_logger = logger](auto _answer)
		{
			_logger->WriteInfoEntry(_answer.c_str());

			return true;
		});

	ioc.run();

	// wird einmal ausgeführt und man kann über post noch Tasks anhängen
	//ioc.poll();
	//ioc.post();

	//string inputStr;
	//while (inputStr != "x") 
	//{
	//	if (ioc.stopped()) 
	//	{
	//		ioc.restart();
	//	}
	//	if (!ioc.poll()) 
	//	{
	//		cin >> inputStr;
	//
	//		if (inputStr == "s")
	//		{
	//			ioc.poll();
	//		}
	//		else {
	//			std::this_thread::sleep_for(std::chrono::milliseconds(3));
	//		}
	//	}
	//}

	//auto futureLambda = std::async([](WebSocketCollection _webSocketCollection)
	//	{
	//		string inputStr;
	//		while (inputStr != "x") {
	//			cout << "Press 's' to stop. Press 'x' to Exit." << endl;
	//			cin >> inputStr;
	//
	//			if (inputStr == "s" || inputStr == "x")
	//			{ //stop & exit
	//				_webSocketCollection.UnsubscribeAllChannels();
	//			}
	//		}
	//	}, websocketCollection);
	//
	//futureLambda.get();


	/*while (true) {
		try {
			ioctx.run();
			break;
		}
		catch (const std::exception& ex) {
			std::cerr << "std::exception: what: " << ex.what() << std::endl;

			ioctx.restart();
		}
	}*/
	
	return EXIT_SUCCESS;
}