#include "JsonHelper.h"

nlohmann::json JsonHelper::ParseStringToJson(const std::string& _stringToParse)
{
	try
	{
		return nlohmann::json::parse(_stringToParse);
	}
	catch (const std::exception& e)
	{
		FileLogger::WriteErrorEntry(e.what());
		FileLogger::WriteErrorEntry("error");

		return nlohmann::json{};
	}
}
