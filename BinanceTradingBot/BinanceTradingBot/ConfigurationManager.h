#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <json.hpp>
#include <iomanip>

#include "Config.h"
#include "LoggingFacility.h"

using namespace std;

class ConfigurationManager
{
private:
	const string configFileName = "config.json";
	nlohmann::json jsonObject;
	fstream configFile;
	Logger logger;

	void InitializeConfig(Config& _config, const nlohmann::json& _jsonObject);
public:
	ConfigurationManager(const Logger& logger) : logger(logger) {}
	Config Load();
	Config Create();
	void Safe(Config& _config);
};