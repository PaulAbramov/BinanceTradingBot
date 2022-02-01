#pragma once

#include <chrono>
#include <boost/assign/list_of.hpp>
#include <boost/unordered_map.hpp>
#include <utility>

#include "magic_enum.hpp"
#include "LoggingFacility.h"
#include "root_certificates.h"

class APIHead
{
private:

protected:
	Logger logger;

public:
	APIHead() = default;
	explicit APIHead(const Logger& _logger) : logger(_logger) {}
};