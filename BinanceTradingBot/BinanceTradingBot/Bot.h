#pragma once
#define ELPP_THREAD_SAFE
#define ELPP_FORCE_USE_STD_THREAD

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <future>
#include <string>
#include <mutex>
#include <cmath>
#include <array>
#include <charconv>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "ApiRequestManager.h"
#include "Config.h"
#include "ConnectionFactory.h"
#include "ConnectionPool.h"
#include "SQLConnectionFactory.h"
#include "LoggingFacility.h"
#include "WebSocketCollection.h"
#include "Trade.h"

using namespace std;
using namespace std::chrono;

class Bot
{
private:
	boost::unordered_map<EIntervals, const char*> eIntervalsToString = map_list_of
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

#pragma region outsource to configfile
	const int maxdeals = 1;

	//wie viel % Profit soll er mitnehmen ? 0,8% ?
	//	0,075% pro Postion somit kaufen und verkaufen : 0,15%
	//	0,1% pro Position ohne BNB somit kaufen und verkaufen : 0,2%
	const float takeprofit = 0.005;					// 0,5%
	const float firstSafetyOrder = 0.0125;			// 1,25%
	const float secondSafetyOrder = 0.0275;			// 2,75%
	const float thirdSafetyOrder = 0.0125;			// 4,25%
	const float fourthSafetyOrder = 0.0125;			//10,00%

	const int initialBuy = 10;						//10
	const float firstSafetyOrderAmount = 16.5;		//16,5
	const float secondSafetyOrderAmount = 20;		//20
	const float thirdSafetyOrderAmount = 20;		//20
	const float fourthSafetyOrderAmount = 33.5;		//33,5

	const float maxOrderValue = initialBuy + firstSafetyOrderAmount + secondSafetyOrderAmount + thirdSafetyOrderAmount + fourthSafetyOrderAmount;

	const float rsiBuy1DThreshold = 45;				//45
	const float rsiBuy1HThreshold = 40;				//40
	const float rsiBuy15MThreshold = 35;			//35

	const float rsiSell1DThreshold = 50;			//50
	const float rsiSell1HThreshold = 55;			//55
	const float rsiSell15MThreshold = 65;			//65

	vector<EIntervals> intervals = { EIntervals::FIFTEENMINUTES, EIntervals::ONEHOUR, EIntervals::ONEDAY };

#pragma endregion
	float busdBalance;
	std::stringstream ss;

	shared_ptr<Logger> logger;
	Config config;
	vector<string> symbols{ "BNBBUSD" };
	ApiRequestManager manager;

	bool AddAssetToDb(const shared_ptr<ConnectionPool<SAConnection>>& _pool, const string& _answer) const;
	string LongToString(const int64_t _longDate) const;
	time_t StringToTime(string _dateString) const;
	double GetBalanceForCoin(const string& _symbol) const;
	vector<Trade> GetActiveDealsFromDb(const shared_ptr<ConnectionPool<SAConnection>>& _pool, const string& _symbol) const;

	string RoundValueToDecimalValue(const double& _valueToRound, const int _decimalPosition) const;
	Trade CreateTradeObjectFromJsonToInsert(const string& _tradeResponse) const;
	void AddTradeToDb(const shared_ptr<ConnectionPool<SAConnection>>& _pool, const Trade& _tradeResponse) const;

	void ManualRun(net::io_context *_ioc, shared_ptr<ConnectionPool<SAConnection>> _pool);
	void DCARun(net::io_context *_ioc, shared_ptr<ConnectionPool<SAConnection>> _pool);

public:
	Bot(const Logger& _logger, const Config& _config);
	bool Run();
};