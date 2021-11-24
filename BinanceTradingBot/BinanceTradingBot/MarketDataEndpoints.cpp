#include "MarketDataEndpoints.h"

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
void MarketDataEndpoints::GeMarketOrderbookQuery(string& _url, string& _queryString, string _symbol, int _limit)
{
	if (_symbol.empty())
	{
		logger->writeWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/depth?";

	_queryString.append("symbol=");
	_queryString.append(_symbol);

	_queryString.append("&limit=");
	_queryString.append(to_string(_limit));

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
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
void MarketDataEndpoints::GeMarketRecentTradesListQuery(string& _url, string& _queryString, string _symbol, int _limit)
{
	if (_symbol.empty())
	{
		logger->writeWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/trades?";

	_queryString.append("symbol=");
	_queryString.append(_symbol);

	_queryString.append("&limit=");
	_queryString.append(to_string(_limit));

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
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
void MarketDataEndpoints::GeMarketOldTradeLookupQuery(string& _url, string& _queryString, string _symbol, unsigned short _fromId, int _limit)
{
	if (_symbol.empty())
	{
		logger->writeWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/trades?";

	_queryString.append("symbol=");
	_queryString.append(_symbol);

	_queryString.append("&limit=");
	_queryString.append(to_string(_limit));

	_queryString.append("&fromId=");
	_queryString.append(to_string(_fromId));

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
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
void MarketDataEndpoints::GeMarketCandlestickDataQuery(string& _url, string& _queryString, string _symbol, EIntervals _interval, time_t _startTime, time_t _endTime, int _limit)
{
	if (_symbol.empty())
	{
		logger->writeWarnEntry("Did not set mandatory parameter 'symbol'");
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

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
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
void MarketDataEndpoints::GeMarketCurrentAveragePriceQuery(string& _url, string& _queryString, string _symbol)
{
	if (_symbol.empty())
	{
		logger->writeWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/avgPrice?";

	_queryString.append("symbol=");
	_queryString.append(_symbol);

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
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
void MarketDataEndpoints::GeMarketTickerChangeStatisticQuery(string& _url, string& _queryString, string _symbol)
{
	_url += "/api/v3/ticker/24hr";

	if (!_symbol.empty())
	{
		_queryString.append("?symbol=");
		_queryString.append(_symbol);
	}
	
	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
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
void MarketDataEndpoints::GeMarketSymbolPriceTickerQuery(string& _url, string& _queryString, string _symbol)
{
	_url += "/api/v3/ticker/price";

	if (!_symbol.empty())
	{
		_queryString.append("?symbol=");
		_queryString.append(_symbol);
	}

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
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
void MarketDataEndpoints::GeMarketSymbolOrderbookTickerQuery(string& _url, string& _queryString, string _symbol)
{
	_url += "/api/v3/ticker/bookTicker";

	if (!_symbol.empty())
	{
		_queryString.append("?symbol=");
		_queryString.append(_symbol);
	}

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}
