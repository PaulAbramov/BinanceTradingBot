#pragma once
#define ELPP_THREAD_SAFE
#define ELPP_FORCE_USE_STD_THREAD

#include <future>

#include "ApiManager.h"
#include "SymbolExchangeInfo.h"
#include "Config.h"
#include "SQLManager.h"
#include "WebSocketCollection.h"
#include "JsonHelper.h"

class Bot
{
private:
	std::map<EIntervals, const char*> eIntervalsToString
	{
		{EIntervals::ONEMINUTE, "1m"},
		{EIntervals::THREEMINUTES, "3m"},
		{EIntervals::FIVEMINUTES, "5m"},
		{EIntervals::FIFTEENMINUTES, "15m"},
		{EIntervals::THIRTYMINUTES, "30m"},
		{EIntervals::ONEHOUR, "1h"},
		{EIntervals::TWOHOURS, "2h"},
		{EIntervals::FOURHOURS, "4h"},
		{EIntervals::SIXHOURS, "6h"},
		{EIntervals::EIGHTHOURS, "8h"},
		{EIntervals::TWELVEHOURS, "12h"},
		{EIntervals::ONEDAY, "1d"},
		{EIntervals::THREEDAYS, "3d"},
		{EIntervals::ONEWEEK, "1w"},
		{EIntervals::ONEMONTH, "1M"}
	};

	std::vector<EIntervals> intervals{ { EIntervals::FIFTEENMINUTES, EIntervals::ONEHOUR, EIntervals::ONEDAY } };

	std::vector<std::string> symbols{ "BNBBUSD", "MATICBUSD", "AVAXBUSD", "ALGOBUSD" };
	std::vector<SymbolExchangeInfo> symbolsInformations;

	double maxOrderValue;
	double busdBalance;

	Config config;
	ApiManager manager;
	SQLManager sqlManager;
	std::shared_ptr<ConnectionPool<SAConnection>> pool;
	std::map<std::string, std::future<void>> runningTrades;

	double GetBalanceForCoin(const std::string& _symbol) const;

	std::string RoundValueToDecimalValue(const double& _valueToRound, const int _decimalPosition) const;
	bool CreateTradeAndSafeInDb(const std::string& _symbol, double _quantity, double _price, const std::string& _clientOrderId, ESide _side, double _takeProfit ) const;
	std::string CreateProfitTakerForSafetyTrade(const std::string& _symbol, int _tradeNumber, const std::string& _takeProfitPrice) const;
	Trade CreateTradeObjectFromJsonToInsert(const std::string& _tradeResponse) const;

	void CheckTrades(const std::string& _symbol) const;

	void ManualRun(net::io_context *_ioc);
	void DCARun(net::io_context *_ioc);

public:
	Bot(const Config& _config);
	bool Run();
};