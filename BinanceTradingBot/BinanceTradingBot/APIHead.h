#pragma once

#include <chrono>
#include <boost/assign/list_of.hpp>
#include <boost/unordered_map.hpp>

#include "magic_enum.hpp"
#include "LoggingFacility.h"
#include "root_certificates.h"

using boost::assign::map_list_of;

using namespace std;

class APIHead
{
private:

protected:
	Logger logger;

	string SetTimeStampAndRecvWindow(unsigned short _recvWindow = 0) const;

public:
	APIHead() = default;
	explicit APIHead(const Logger& _logger) : logger(_logger) {}
};