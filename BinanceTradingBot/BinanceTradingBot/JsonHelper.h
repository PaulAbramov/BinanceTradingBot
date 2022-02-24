#pragma once

#include <nlohmann/json.hpp>

#include "FileLogger.h"

class JsonHelper
{
private:

protected:

public:
	static nlohmann::basic_json<> ParseStringToJson(const std::string& _stringToParse);
};