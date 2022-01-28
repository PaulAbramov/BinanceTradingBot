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

#include "ApiRequestManager.h"
#include "Config.h"
#include "LoggingFacility.h"
#include "SQLManager.h"
#include "WebSocketCollection.h"
#include "Trade.h"

using namespace std;
using namespace std::chrono;

class Bot
{
private:
	boost::unordered_map<EIntervals, const char*> eIntervalsToString{ map_list_of
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
		(EIntervals::ONEMONTH, "1M") };

#pragma region outsource to configfile
	const int maxtrades{ 1 };

	//wie viel % Profit soll er mitnehmen ? 0,8% ?
	//	0,075% pro Postion somit kaufen und verkaufen : 0,15%
	//	0,1% pro Position ohne BNB somit kaufen und verkaufen : 0,2%
	const double takeprofit{ 0.005 };				// 0,5%
	const double firstSafetyOrder{ 0.0125 };		// 1,25%
	const double secondSafetyOrder{ 0.0275 };		// 2,75%
	const double thirdSafetyOrder{ 0.0425 };		// 4,25%
	const double fourthSafetyOrder{ 0.1 };			//10,00%

	const int initialBuy{ 10 };						//10
	const double firstSafetyOrderAmount{ 16.5 };	//16,5
	const double secondSafetyOrderAmount{ 20 };		//20
	const double thirdSafetyOrderAmount{ 20 };		//20
	const double fourthSafetyOrderAmount{ 33.5 };	//33,5

	const double maxOrderValue{ initialBuy + firstSafetyOrderAmount + secondSafetyOrderAmount + thirdSafetyOrderAmount + fourthSafetyOrderAmount };

	const double rsiBuy1DThreshold{ 45 };			//45
	const double rsiBuy1HThreshold{ 40 };			//40
	const double rsiBuy15MThreshold{ 35 };			//35

	const double rsiSell1DThreshold{ 50 };			//50
	const double rsiSell1HThreshold{ 55 };			//55
	const double rsiSell15MThreshold{ 65 };			//65

	vector<EIntervals> intervals{ { EIntervals::FIFTEENMINUTES, EIntervals::ONEHOUR, EIntervals::ONEDAY } };

#pragma endregion
	double busdBalance;
	std::stringstream ss;

	Logger logger;
	Config config;
	vector<string> symbols{ "BNBBUSD" };
	ApiRequestManager manager;
	SQLManager sqlManager;
	shared_ptr<ConnectionPool<SAConnection>> pool;
	map<string, future<void>> runningTrades;

	double GetBalanceForCoin(const string& _symbol) const;

	string RoundValueToDecimalValue(const double& _valueToRound, const int _decimalPosition) const;
	bool CreateTradeAndSafeInDb(const string& _symbol, const string& _quantity, const string& _price, const string& _clientOrderId, ESide _side, double _takeProfit = 0) const;
	string CreateProfitTakerForSafetyTrade(const string& _symbol, int _tradeNumber, const string& _takeProfitPrice) const;
	Trade CreateTradeObjectFromJsonToInsert(const string& _tradeResponse) const;

	void CheckTrades(const string& _symbol) const;

	void ManualRun(net::io_context *_ioc);
	void DCARun(net::io_context *_ioc);

public:
	Bot(const Logger& _logger, const Config& _config);
	bool Run();
};