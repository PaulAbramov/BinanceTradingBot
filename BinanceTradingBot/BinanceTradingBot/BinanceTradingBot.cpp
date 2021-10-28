// BinanceTradingBot.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "WebSocketSession.h"
#include "ConfigurationManager.h"
#include "FileLogger.h"
#include "StandardOutputLogger.h"

#include <chrono>
#include <filesystem>
#include <iostream>

using namespace std;
using namespace std::chrono;

namespace fs = std::filesystem;

static Logger InitializeLogger()
{
	string filename = "Logger.conf";
	ifstream filestream(filename);

	// if configuration file exists - use FilesystemLogger
	if (filestream.good())
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
		struct tm newtime;
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
	Logger logger = InitializeLogger();
	logger->writeInfoEntry("Logger initialized");

	ConfigurationManager configManager = ConfigurationManager(logger);
	Config config = configManager.LoadConfig();

	auto host = config.host.c_str();
	auto port = config.port.c_str();
	auto target = config.target.c_str();
	auto text = "";

	// The io_context is required for all I/O
	net::io_context ioc;
	// The SSL context is required, and holds certificates
	ssl::context ctx{ ssl::context::tlsv12_client };
	ctx.set_verify_mode(ssl::verify_none);
	// This holds the root certificate used for verification
	load_root_certificates(ctx);

	// Run the I/O service. The call will return when
	// the socket is closed.
	ioc.run();

	string inputStr;
	thread t;

	shared_ptr<WebSocketSession> websocketSession;

	while (inputStr != "x") {
		cout << "Press 'r' to run. Press 's' to stop. Press 'x' to Exit." << endl;
		cin >> inputStr;

		if (inputStr == "r") { //run
			if (websocketSession.use_count() == 0) { //should not run twice
				t = thread(
					[&websocketSession, &ioc, &ctx, host, port, target, logger]
					{
						websocketSession = make_shared<WebSocketSession>(ioc, ctx, host, port, target, logger);
						websocketSession->StartWebSocketSession(); //within a thread so doesn't block
					});
			}
		}
		else if (inputStr == "s" || inputStr == "x") { //stop & exit
			if (websocketSession.use_count()) { //running
				websocketSession->StopWebSocketSession();
				t.join();
				t.~thread();
				websocketSession.reset();
			}
			
		}
	}
	
	return EXIT_SUCCESS;
}