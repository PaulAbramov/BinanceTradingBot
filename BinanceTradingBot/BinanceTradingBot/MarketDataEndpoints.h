#pragma once

#include "APIHead.h"
#include "APIEnums.h"

class MarketDataEndpoints : public APIHead
{
private:
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

public:
	MarketDataEndpoints() = default;
	MarketDataEndpoints(const Logger& _logger) : APIHead(_logger) {}

	void GetMarketOrderbookQuery(string& _url, string& _queryString, const string& _symbol, int _limit = 100) const;
	void GetMarketRecentTradesListQuery(string& _url, string& _queryString, const string& _symbol, int _limit = 500) const;
	void GetMarketOldTradeLookupQuery(string& _url, string& _queryString, const string& _symbol, unsigned short _fromId, int _limit = 500) const;
	void GetMarketCandlestickDataQuery(string& _url, string& _queryString, const string& _symbol, EIntervals _interval, time_t _startTime, time_t _endTime, int _limit = 500) const;
	void GetMarketCurrentAveragePriceQuery(string& _url, string& _queryString, const string& _symbol) const;
	void GetMarketTickerChangeStatisticQuery(string& _url, string& _queryString, const string& _symbol) const;
	void GetMarketSymbolPriceTickerQuery(string& _url, string& _queryString, const string& _symbol) const;
	void GetMarketSymbolOrderbookTickerQuery(string& _url, string& _queryString, const string& _symbol) const;
};