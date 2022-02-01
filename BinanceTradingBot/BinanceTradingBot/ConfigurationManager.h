#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <utility>

#include "Config.h"
#include "LoggingFacility.h"

namespace fs = std::filesystem;

class ConfigurationManager
{
private:
	const std::string configFileName{ "config.json" };
	nlohmann::json jsonObject;
	std::fstream configFile;
	Logger logger;

	static void InitializeConfig(Config& _config, const nlohmann::json& _jsonObject);
public:
	ConfigurationManager(const Logger& _logger) : logger(_logger) {}
	Config LoadConfig();
	Config CreateConfig();
	void SafeConfig(Config& _config);
};