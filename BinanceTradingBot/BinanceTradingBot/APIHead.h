#pragma once

#include <chrono>
#include <boost/assign/list_of.hpp>
#include <boost/unordered_map.hpp>

#include "magic_enum.hpp"
#include "LoggingFacility.h"
#include "root_certificates.h"

using boost::assign::map_list_of;

using namespace std;

enum class EIntervals
{
	ONEMINUTE,
	THREEMINUTES,
	FIVEMINUTES,
	FIFTEENMINUTES,
	THIRTYMINUTES,
	ONEHOUR,
	TWOHOURS,
	FOURHOURS,
	SIXHOURS,
	EIGHTHOURS,
	TWELVEHOURS,
	ONEDAY,
	THREEDAYS,
	ONEWEEK,
	ONEMONTH,
};

class APIHead
{
private:

protected:
	Logger logger;

	const boost::unordered_map<EIntervals, const char*> eIntervalsToString = map_list_of
	(EIntervals::ONEMINUTE, "1m")
	(EIntervals::THREEMINUTES, "3m")
	(EIntervals::FIVEMINUTES, "5m")
	(EIntervals::FIFTEENMINUTES, "15m")
	(EIntervals::THIRTYMINUTES, "30m")
	(EIntervals::ONEHOUR, "1h")
	(EIntervals::TWOHOURS, "2h")
	(EIntervals::FOURHOURS, "4h")
	(EIntervals::SIXHOURS, "6h")
	(EIntervals::EIGHTHOURS, "8h")
	(EIntervals::TWELVEHOURS, "12h")
	(EIntervals::ONEDAY, "1d")
	(EIntervals::THREEDAYS, "3d")
	(EIntervals::ONEWEEK, "1w")
	(EIntervals::ONEMONTH, "1M");

	string SetTimeStampAndRecvWindow(unsigned short _recvWindow = 0);

public:
	APIHead() = default;
	APIHead(const Logger& _logger) : logger(_logger) {}
};