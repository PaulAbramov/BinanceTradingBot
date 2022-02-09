#pragma once

#include <map>
#include <string>

#include "LoggingFacility.h"
//#include "root_certificates.h"

class APIHead
{
private:

protected:
	Logger logger;

public:
	APIHead() = default;
	explicit APIHead(const Logger& _logger) : logger(_logger) {}
};