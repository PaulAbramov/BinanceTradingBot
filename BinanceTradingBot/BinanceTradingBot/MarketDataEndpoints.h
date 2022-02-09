#pragma once

#include "APIHead.h"
#include "APIEnums.h"

class MarketDataEndpoints : public APIHead
{
private:
	std::map<EIntervals, const char*> eIntervalsToString
	{
		{ EIntervals::ONEMINUTE, "1m" },
		{ EIntervals::THREEMINUTES, "3m" },
		{ EIntervals::FIVEMINUTES, "5m" },
		{ EIntervals::FIFTEENMINUTES, "15m" },
		{ EIntervals::THIRTYMINUTES, "30m" },
		{ EIntervals::ONEHOUR, "1h" },
		{ EIntervals::TWOHOURS, "2h" },
		{ EIntervals::FOURHOURS, "4h" },
		{ EIntervals::SIXHOURS, "6h" },
		{ EIntervals::EIGHTHOURS, "8h" },
		{ EIntervals::TWELVEHOURS, "12h" },
		{ EIntervals::ONEDAY, "1d" },
		{ EIntervals::THREEDAYS, "3d" },
		{ EIntervals::ONEWEEK, "1w" },
		{ EIntervals::ONEMONTH, "1M" }
	};

public:
	MarketDataEndpoints() = default;
	MarketDataEndpoints(const Logger& _logger) : APIHead(_logger) {}

	void GetMarketTimeQuery(std::string& _url) const;
	void GetMarketOrderbookQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, int _limit = 100) const;
	void GetMarketRecentTradesListQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, int _limit = 500) const;
	void GetMarketOldTradeLookupQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, unsigned short _fromId, int _limit = 500) const;
	void GetMarketCandlestickDataQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, EIntervals _interval, time_t _startTime, time_t _endTime, int _limit = 500) const;
	void GetMarketCurrentAveragePriceQuery(std::string& _url, std::string& _queryString, const std::string& _symbol) const;
	void GetMarketTickerChangeStatisticQuery(std::string& _url, std::string& _queryString, const std::string& _symbol) const;
	void GetMarketSymbolPriceTickerQuery(std::string& _url, std::string& _queryString, const std::string& _symbol) const;
	void GetMarketSymbolOrderbookTickerQuery(std::string& _url, std::string& _queryString, const std::string& _symbol) const;
};