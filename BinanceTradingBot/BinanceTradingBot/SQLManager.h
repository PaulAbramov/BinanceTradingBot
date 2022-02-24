#pragma once
#define ELPP_THREAD_SAFE
#define ELPP_FORCE_USE_STD_THREAD

#include <iostream>

#include <nlohmann/json.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Config.h"
#include "SQLConnectionFactory.h"
#include "ConnectionPool.h"
#include "Trade.h"
#include "JsonHelper.h"

class SQLManager
{
private:
	std::shared_ptr<ConnectionPool<SAConnection>> pool;

	std::string LongToString(const int64_t _longDate) const;
	time_t StringToTime(std::string _dateString) const;
	std::shared_ptr<SAConnection> BorrowConnection() const;

public:
	SQLManager(int _symbolAmount, const Config& _config);

	bool AddAssetToDb(const std::string& _answer) const;

	bool AddTradeToDb(const Trade& _trade, double _takeProfit = 0) const;
	bool UpdateTradeInDb(const Trade& _trade) const;
	bool RemoveTradeFromDb(const std::string& _clientOrderId) const;

	std::vector<Trade> GetActiveTradesFromDb(const std::string& _symbol) const;
};
