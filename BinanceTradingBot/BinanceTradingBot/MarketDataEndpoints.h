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

	void GeMarketOrderbookQuery(string& _url, string& _queryString, string _symbol, int _limit = 100);
	void GeMarketRecentTradesListQuery(string& _url, string& _queryString, string _symbol, int _limit = 500);
	void GeMarketOldTradeLookupQuery(string& _url, string& _queryString, string _symbol, unsigned short _fromId, int _limit = 500);
	void GeMarketCandlestickDataQuery(string& _url, string& _queryString, string _symbol, EIntervals _interval, time_t _startTime, time_t _endTime, int _limit = 500);
	void GeMarketCurrentAveragePriceQuery(string& _url, string& _queryString, string _symbol);
	void GeMarketTickerChangeStatisticQuery(string& _url, string& _queryString, string _symbol);
	void GeMarketSymbolPriceTickerQuery(string& _url, string& _queryString, string _symbol);
	void GeMarketSymbolOrderbookTickerQuery(string& _url, string& _queryString, string _symbol);
};