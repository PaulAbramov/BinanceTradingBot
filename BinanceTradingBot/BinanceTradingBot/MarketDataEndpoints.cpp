#include "MarketDataEndpoints.h"

void MarketDataEndpoints::GetMarketTimeQuery(string& _url) const
{
	_url += "/api/v3/time";
}

/*
* Order Book
*
* GET /api/v3/depth
*
* Weight(IP):
* Limit					Weight
* 5, 10, 20, 50, 100	1
* 500					5
* 1000					10
* 5000					50
*
* Parameters:
* Name			Type	Mandatory	Description
* symbol		STRING	YES	
* limit			INT		NO			Default 100; max 5000. Valid limits:[5, 10, 20, 50, 100, 500, 1000, 5000]
*/
void MarketDataEndpoints::GetMarketOrderbookQuery(string& _url, string& _queryString, const string& _symbol, const int _limit) const
{
	if (_symbol.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/depth?";

	_queryString.append("symbol=");
	_queryString.append(_symbol);

	_queryString.append("&limit=");
	_queryString.append(to_string(_limit));
}

/*
* Recent Trades List
*
* GET /api/v3/trades
*
* Weight(IP): 1
*
* Parameters:
* Name			Type	Mandatory	Description
* symbol		STRING	YES
* limit			INT		NO			Default 500; max 1000
*/
void MarketDataEndpoints::GetMarketRecentTradesListQuery(string& _url, string& _queryString, const string& _symbol, const int _limit) const
{
	if (_symbol.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/trades?";

	_queryString.append("symbol=");
	_queryString.append(_symbol);

	_queryString.append("&limit=");
	_queryString.append(to_string(_limit));
}

/*
* Old Trade Lookup (MARKET_DATA)
* 
* Get older market trades.
*
* GET /api/v3/historicalTrades
*
* Weight(IP): 5
*
* Parameters:
* Name			Type			Mandatory	Description
* symbol		STRING			YES
* limit			INT				NO			Default 500; max 1000
* fromId		UNSGINED SHORT	NO			Trade id to fetch from. Default gets most recent trades.
*/
void MarketDataEndpoints::GetMarketOldTradeLookupQuery(string& _url, string& _queryString, const string& _symbol, const unsigned short _fromId, const int _limit) const
{
	if (_symbol.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/trades?";

	_queryString.append("symbol=");
	_queryString.append(_symbol);

	_queryString.append("&limit=");
	_queryString.append(to_string(_limit));

	_queryString.append("&fromId=");
	_queryString.append(to_string(_fromId));
}

/*
* Kline/Candlestick Data
* 
* Kline/candlestick bars for a symbol.
* Klines are uniquely identified by their open time.
*
* GET /api/v3/klines
*
* Weight(IP): 1
*
* Parameters:
* Name			Type			Mandatory	Description
* symbol		STRING			YES
* interval		ENUM			YES	
* startTime		UNSGINED SHORT	NO	
* endTime		UNSGINED SHORT	NO	
* limit			INT				NO			Default 500; max 1000.
* 
* If startTime and endTime are not sent, the most recent klines are returned.
*/
void MarketDataEndpoints::GetMarketCandlestickDataQuery(string& _url, string& _queryString, const string& _symbol, const EIntervals _interval, const time_t _startTime, const time_t _endTime, const int _limit) const
{
	if (_symbol.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/klines?";

	_queryString.append("symbol=");
	_queryString.append(_symbol);

	_queryString.append("&interval=");
	_queryString.append(eIntervalsToString.at(_interval));

	_queryString.append("&limit=");
	_queryString.append(to_string(_limit));

	if (_startTime != 0 && _endTime != 0)
	{
		_queryString.append("&startTime=");
		_queryString.append(to_string(_startTime));

		_queryString.append("&endTime=");
		_queryString.append(to_string(_endTime));
	}
}

/*
* Current Average Price
*
* Current average price for a symbol
*
* GET /api/v3/avgPrice
*
* Weight(IP): 1
*
* Parameters:
* Name			Type	Mandatory	Description
* symbol		STRING	YES
*/
void MarketDataEndpoints::GetMarketCurrentAveragePriceQuery(string& _url, string& _queryString, const string& _symbol) const
{
	if (_symbol.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/avgPrice?";

	_queryString.append("symbol=");
	_queryString.append(_symbol);
}

/*
* 24hr Ticker Price Change Statistics
*
* 24 hour rolling window price change statistics. Careful when accessing this with no symbol.
*
* GET /api/v3/ticker/24hr
*
* Weight(IP): 
* 1 for a single symbol
* 40 when the symbol parameter is omitted
*
* Parameters:
* Name			Type	Mandatory	Description
* symbol		STRING	NO
* 
* If the symbol is not sent, tickers for all symbols will be returned in an array.
*/
void MarketDataEndpoints::GetMarketTickerChangeStatisticQuery(string& _url, string& _queryString, const string& _symbol) const
{
	_url += "/api/v3/ticker/24hr";

	if (!_symbol.empty())
	{
		_queryString.append("?symbol=");
		_queryString.append(_symbol);
	}
}

/*
* Symbol Price Ticker
*
* Latest price for a symbol or symbols.
*
* GET /api/v3/ticker/price
*
* Weight(IP):
* 1 for a single symbol
* 2 when the symbol parameter is omitted
*
* Parameters:
* Name			Type	Mandatory	Description
* symbol		STRING	NO
*
* If the symbol is not sent, tickers for all symbols will be returned in an array.
*/
void MarketDataEndpoints::GetMarketSymbolPriceTickerQuery(string& _url, string& _queryString, const string& _symbol) const
{
	_url += "/api/v3/ticker/price";

	if (!_symbol.empty())
	{
		_queryString.append("?symbol=");
		_queryString.append(_symbol);
	}
}

/*
* Symbol Order Book Ticker
*
* Best price/qty on the order book for a symbol or symbols.
*
* GET /api/v3/ticker/bookTicker
*
* Weight(IP):
* 1 for a single symbol
* 2 when the symbol parameter is omitted
*
* Parameters:
* Name			Type	Mandatory	Description
* symbol		STRING	NO
*
* If the symbol is not sent, tickers for all symbols will be returned in an array.
*/
void MarketDataEndpoints::GetMarketSymbolOrderbookTickerQuery(string& _url, string& _queryString, const string& _symbol) const
{
	_url += "/api/v3/ticker/bookTicker";

	if (!_symbol.empty())
	{
		_queryString.append("?symbol=");
		_queryString.append(_symbol);
	}
}
