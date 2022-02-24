#include "ConfigurationManager.h"

using namespace std;

/*
* Check if the configfile exists and open it
* If it is not existent then create a new one
*/
Config ConfigurationManager::LoadConfig()
{
	FileLogger::WriteErrorEntry("Load Configuration...");

	Config config{};
	
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

			jsonObject = JsonHelper::ParseStringToJson(configText);
			if (jsonObject.empty())
			{
				return config;
			}

			InitializeConfig(config, jsonObject);

			FileLogger::WriteErrorEntry("Configuration successfully loaded.");
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
		FileLogger::WriteErrorEntry(e.what());
	}

	return config;
}

/*
* Create and prefill the config file
*/
Config ConfigurationManager::CreateConfig()
{
	FileLogger::WriteErrorEntry("Create Configuration...");

	Config config;

	jsonObject["TaapiSecret"] = "";
	jsonObject["ApiKey"] = "";
	jsonObject["SecretKey"] = "";
	jsonObject["GnuplotPath"] = "";

	jsonObject["SqlServer"] = "";
	jsonObject["SqlUserName"] = "";
	jsonObject["SqlPassword"] = "";

	//how much % profit should we take ? 0,8% ?
	//	0,075% fees per position, selling and buying : 0,15%
	//	0,1% fees per position without BNB, selling and buying : 0,2%
	jsonObject["MaxTrades"] = 1;
	jsonObject["TakeProfit"] = 0.005;				// 0,5%
	jsonObject["FirstSafetyOrder"] = 0.0125;		// 1,25%
	jsonObject["SecondSafetyOrder"] = 0.0275;		// 2,75%
	jsonObject["ThirdSafetyOrder"] = 0.0425;		// 4,25%
	jsonObject["FourthSafetyOrder"] = 0.1;		//10,00%

	jsonObject["InitialBuy"] = 10;				//10
	jsonObject["FirstSafetyOrderAmount"] = 16.5;  //16,5
	jsonObject["SecondSafetyOrderAmount"] = 20;	//20
	jsonObject["ThirdSafetyOrderAmount"] = 20;	//20
	jsonObject["FourthSafetyOrderAmount"] = 33.5; //33,5

	jsonObject["RsiBuy1DThreshold"] = 45;			//45
	jsonObject["RsiBuy1HThreshold"] = 40;			//40
	jsonObject["RsiBuy15MThreshold"] = 35;		//35
	jsonObject["RsiSell1DThreshold"] = 50;		//50
	jsonObject["RsiSell1HThreshold"] = 55;		//55
	jsonObject["RsiSell15MThreshold"] = 65;		//65

	configFile.open(configFileName, ios::out);
	if (!configFile.is_open())
	{
		return config;
	}

	configFile << setw(4) << jsonObject << endl;
	configFile.close();

	InitializeConfig(config, jsonObject);

	//FileLogger::WriteInfoEntry("Configuration successfully created.");
	FileLogger::WriteErrorEntry("Configuration successfully created.");
	return config;
}

void ConfigurationManager::SaveConfig(Config& _config)
{
	Config config;
}

void ConfigurationManager::InitializeConfig(Config& _config, const nlohmann::json& _jsonObject)
{
	_config.TaapiSecret = _jsonObject["TaapiSecret"];
	_config.ApiKey = _jsonObject["ApiKey"];
	_config.SecretKey = _jsonObject["SecretKey"];
	_config.GnuplotPath = _jsonObject["GnuplotPath"];

	_config.SqlServer = _jsonObject["SqlServer"];
	_config.SqlUserName = _jsonObject["SqlUserName"];
	_config.SqlPassword = _jsonObject["SqlPassword"];

	_config.MaxTrades = _jsonObject["MaxTrades"];
	_config.TakeProfit = _jsonObject["TakeProfit"];
	_config.FirstSafetyOrder = _jsonObject["FirstSafetyOrder"];
	_config.SecondSafetyOrder = _jsonObject["SecondSafetyOrder"];
	_config.ThirdSafetyOrder = _jsonObject["ThirdSafetyOrder"];
	_config.FourthSafetyOrder = _jsonObject["FourthSafetyOrder"];

	_config.InitialBuy = _jsonObject["InitialBuy"];
	_config.FirstSafetyOrderAmount = _jsonObject["FirstSafetyOrderAmount"];
	_config.SecondSafetyOrderAmount = _jsonObject["SecondSafetyOrderAmount"];
	_config.ThirdSafetyOrderAmount = _jsonObject["ThirdSafetyOrderAmount"];
	_config.FourthSafetyOrderAmount = _jsonObject["FourthSafetyOrderAmount"];

	_config.RsiBuy1DThreshold = _jsonObject["RsiBuy1DThreshold"];
	_config.RsiBuy1HThreshold = _jsonObject["RsiBuy1HThreshold"];
	_config.RsiBuy15MThreshold = _jsonObject["RsiBuy15MThreshold"];
	_config.RsiSell1DThreshold = _jsonObject["RsiSell1DThreshold"];
	_config.RsiSell1HThreshold = _jsonObject["RsiSell1HThreshold"];
	_config.RsiSell15MThreshold = _jsonObject["RsiSell15MThreshold"];
}
