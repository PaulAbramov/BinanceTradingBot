#include <chrono>
#include <filesystem>
#include <future>

#include "WebSocketCollection.h"
#include "ConfigurationManager.h"
#include "FileLogger.h"
#include "StandardOutputLogger.h"
#include "Bot.h"

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

int main()
{
	Logger logger = InitializeLogger();
	logger->WriteInfoEntry("Logger initialized");

	ConfigurationManager configManager = ConfigurationManager(logger);
	Config config = configManager.LoadConfig();

	Bot firstBot = Bot(logger, config);

	return firstBot.Run();

	return EXIT_SUCCESS;
}