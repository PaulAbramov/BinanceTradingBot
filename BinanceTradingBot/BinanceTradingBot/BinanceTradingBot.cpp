// BinanceTradingBot.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "WebSocketSession.h"
#include "ConfigurationManager.h"
#include "FileLogger.h"
#include "StandardOutputLogger.h"

#include <chrono>
#include <filesystem>
#include <iostream>

#define BINANCE_HOST "https://api.binance.com"

using namespace std;
using namespace std::chrono;

namespace fs = std::filesystem;

/*
 * API Endpoints:
 * https://api.binance.com
 * https://api1.binance.com
 * https://api2.binance.com
 * https://api3.binance.com
 */

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
	Config config = configManager.Load();

		string url(BINANCE_HOST);
		url += "/api/v3/account?";
		string action = "GET";

		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		// Wann die Abfrage erstellt worden ist
		string querystring("timestamp=");
		querystring.append(to_string(ms.count()));

		// Wie lange die Abfrage aktiv ist
		//if (recvWindow > 0) {
		//	querystring.append("&recvWindow=");
		//	querystring.append(to_string(recvWindow));
		//}

		unsigned char* digest;
		digest = HMAC(EVP_sha256(), config.secret_key.c_str(), strlen(config.secret_key.c_str()), (unsigned char*)querystring.c_str(), strlen(querystring.c_str()), NULL, NULL);
		
		const static std::string HexCodes = "0123456789abcdef";
		string HexString;
		for (int i = 0; i < 32; ++i) {
			unsigned char BinValue = digest[i];
			HexString += HexCodes[(BinValue >> 4) & 0x0F];
			HexString += HexCodes[BinValue & 0x0F];
		}

		string signature = HexString;
		querystring.append("&signature=");
		querystring.append(signature);

		url.append(querystring);

		vector <string> extra_http_header;
		string header_chunk("X-MBX-APIKEY: ");
		header_chunk.append(config.api_key);
		extra_http_header.push_back(header_chunk);

		string post_data = "";


	//curl_global_cleanup();
	//
	//std::cout << result << endl;

	

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
						websocketSession->Start(); //within a thread so doesn't block
					});
			}
		}
		else if (inputStr == "s" || inputStr == "x") { //stop & exit
			if (websocketSession.use_count()) { //running
				websocketSession->Stop();
				t.join();
				t.~thread();
				websocketSession.reset();
			}
			
		}
	}
	
	return EXIT_SUCCESS;
}