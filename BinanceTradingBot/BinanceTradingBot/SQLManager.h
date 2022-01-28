#pragma once
#define ELPP_THREAD_SAFE
#define ELPP_FORCE_USE_STD_THREAD

#include <chrono>
#include <utility>
#include <json.hpp>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "ConnectionPool.h"
#include "LoggingFacility.h"
#include "ConnectionFactory.h"
#include "SQLConnectionFactory.h"
#include "Trade.h"

class SQLManager
{
private:
	Logger logger;
	std::shared_ptr<ConnectionPool<SAConnection>> pool;

	string LongToString(const int64_t _longDate) const;
	time_t StringToTime(string _dateString) const;
public:
	SQLManager(const Logger& _logger, int _symbolAmount);

	bool AddAssetToDb(const string& _answer) const;

	void AddTradeToDb(const Trade& _trade, double _takeProfit = 0) const;
	void UpdateTradeInDb(const Trade& _trade) const;
	void RemoveTradeFromDb(const string& _clientOrderId) const;

	vector<Trade> GetActiveTradesFromDb(const string& _symbol) const;
};