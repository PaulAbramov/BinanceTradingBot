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

namespace fs = std::filesystem;

class ConfigurationManager
{
private:
	const string configFileName = "config.json";
	nlohmann::json jsonObject;
	fstream configFile;
	Logger logger;

	static void InitializeConfig(Config& _config, const nlohmann::json& _jsonObject);
public:
	ConfigurationManager(const shared_ptr<LoggingFacility>& _logger) : logger(_logger) {}
	Config LoadConfig();
	Config CreateConfig();
	void SafeConfig(Config& _config);
};