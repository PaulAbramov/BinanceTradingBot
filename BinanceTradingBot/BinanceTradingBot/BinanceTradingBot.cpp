#define ELPP_THREAD_SAFE
#define ELPP_FORCE_USE_STD_THREAD

#include <chrono>
#include <filesystem>
#include <future>

#include "WebSocketCollection.h"
#include "ConfigurationManager.h"
#include "FileLogger.h"
#include "StandardOutputLogger.h"
#include "Bot.h"

#include "GnuplotIoStream.h"

using namespace std;
using namespace std::chrono;

namespace fs = std::filesystem;

static Logger InitializeLogger()
{
	string filename{ "Logger.conf" };

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
		time_t now{ std::time(nullptr) };
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
	Logger logger{ InitializeLogger() };
	logger->WriteInfoEntry("Logger initialized");

	ConfigurationManager configManager{ ConfigurationManager(logger) };
	Config config{ configManager.LoadConfig() };

	string path{ "\"" };
	path += config.gnuplot_path;
	path += "\"";

	Gnuplot gp(path);
	//Gnuplot gp("\"..\\..\\gnuplot\\bin\\gnuplot.exe\"");

	Bot firstBot{ Bot(logger, config) };

	return firstBot.Run();

	return EXIT_SUCCESS;
}

/*
 * Use Gnuplot:
 *
 * std::random_device rd;
 *	std::mt19937 mt(rd());
 *	std::normal_distribution<double> normdist(0., 1.);
 *
 *	std::vector<double> v0, v1;
 *
 *	for(int i = 0; i < 1000; i++)
 *	{
 *		v0.push_back(normdist(mt));
 *		v1.push_back(normdist(mt));
 *	}
 *
 *	std::partial_sum(v0.begin(), v0.end(), v0.begin());
 *	std::partial_sum(v1.begin(), v1.end(), v1.begin());
 *
 *	// The dashes "-" are datainput, so gnuplot knows there is somethin incoming (standard input)
 *	// "\n" has to be set, else it throws an error (linebreak)
 *	// gp object has to have the exact path
 *
 *	gp << "set title 'Graph of two random lines'\n";
 *	gp << "plot '-' with lines title 'v0',"
 *	<< "'-' with lines title 'v1'\n";
 *
 *	gp.send(v0);
 *	gp.send(v1);
 *
 *	std::cin.get();
 */