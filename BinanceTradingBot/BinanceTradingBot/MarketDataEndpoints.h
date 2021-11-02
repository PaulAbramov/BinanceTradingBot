#pragma once

#include "APIHead.h"

class MarketDataEndpoints : public APIHead
{
private:

public:
	MarketDataEndpoints() = default;
	MarketDataEndpoints(const Logger& _logger) : APIHead(_logger) {}

	void GeMarketOrderbookQuery(string& _url, string& _queryString, string _symbol, int _limit = 100);
	void GeMarketRecentTradesListQuery(string& _url, string& _queryString, string _symbol, int _limit = 500);
	void GeMarketOldTradeLookupQuery(string& _url, string& _queryString, string _symbol, long _fromId, int _limit = 500);
	void GeMarketCandlestickDataQuery(string& _url, string& _queryString, string _symbol, EIntervals _interval, time_t _startTime, time_t _endTime, int _limit = 500);
	void GeMarketCurrentAveragePriceQuery(string& _url, string& _queryString, string _symbol);
	void GeMarketTickerChangeStatisticQuery(string& _url, string& _queryString, string _symbol);
	void GeMarketSymbolPriceTickerQuery(string& _url, string& _queryString, string _symbol);
	void GeMarketSymbolOrderbookTickerQuery(string& _url, string& _queryString, string _symbol);
};