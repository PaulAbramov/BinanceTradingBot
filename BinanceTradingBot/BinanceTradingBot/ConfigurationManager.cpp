﻿#include "ConfigurationManager.h"

using namespace std;

/*
* Check if the configfile exists and open it
* If it is not existent then create a new one
*/
Config ConfigurationManager::LoadConfig()
{
	logger->WriteInfoEntry("Load Configuration...");
	Config config;
	
	//throw these errors
	configFile.exceptions(fstream::failbit | fstream::badbit);

	try 
	{
		if (fs::exists(configFileName))
		{
			configFile.open(configFileName, ios::in);

			string configText;
			char buffer[200];

			while (configFile.peek() != EOF && configFile.getline(buffer, 200))
			{
				configText.append(buffer);
			}

			jsonObject = nlohmann::json::parse(configText);

			InitializeConfig(config, jsonObject);

			logger->WriteInfoEntry("Configuration successfully loaded.");
			configFile.close();
		}
		else
		{
			return CreateConfig();
		}
	}
	catch (system_error& e) 
	{
		cerr << e.what() << endl;
		enum { BUFFER_SIZE = 200 };
		char buffer[BUFFER_SIZE];
		cerr << "Error: " << strerror_s(buffer, errno) << endl;
		logger->WriteErrorEntry(e.what());
	}

	return config;
}

/*
* Create and prefill the config file
*/
Config ConfigurationManager::CreateConfig()
{
	logger->WriteInfoEntry("Create Configuration...");

	Config config;

	jsonObject["taapi_secret"] = "";
	jsonObject["api_key"] = "";
	jsonObject["secret_key"] = "";
	jsonObject["gnuplot_path"] = "";

	configFile.open(configFileName, ios::out);
	if (!configFile.is_open())
	{
		return config;
	}

	configFile << setw(4) << jsonObject << endl;
	configFile.close();

	InitializeConfig(config, jsonObject);

	logger->WriteInfoEntry("Configuration successfully created.");
	return config;
}

void ConfigurationManager::SafeConfig(Config& _config)
{
	Config config;
}

void ConfigurationManager::InitializeConfig(Config& _config, const nlohmann::json& _jsonObject)
{
	_config.taapi_secret = _jsonObject["taapi_secret"];
	_config.api_key = _jsonObject["api_key"];
	_config.secret_key = _jsonObject["secret_key"];
	_config.gnuplot_path = _jsonObject["gnuplot_path"];
}
