#pragma once
#define ELPP_THREAD_SAFE
#define ELPP_FORCE_USE_STD_THREAD

#include <chrono>
#include <utility>
#include <nlohmann/json.hpp>

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

	std::string LongToString(const int64_t _longDate) const;
	time_t StringToTime(std::string _dateString) const;
public:
	SQLManager(const Logger& _logger, int _symbolAmount);

	bool AddAssetToDb(const std::string& _answer) const;

	void AddTradeToDb(const Trade& _trade, double _takeProfit = 0) const;
	void UpdateTradeInDb(const Trade& _trade) const;
	void RemoveTradeFromDb(const std::string& _clientOrderId) const;

	std::vector<Trade> GetActiveTradesFromDb(const std::string& _symbol) const;
};
