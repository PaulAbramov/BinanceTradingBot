#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Config.h"
#include "FileLogger.h"
#include "JsonHelper.h"

namespace fs = std::filesystem;

class ConfigurationManager
{
private:
	const std::string configFileName{ "config.json" };
	nlohmann::json jsonObject;
	std::fstream configFile;

	void InitializeConfig(Config& _config, const nlohmann::json& _jsonObject);
public:
	Config LoadConfig();
	Config CreateConfig();
	void SaveConfig(Config& _config);
};