#pragma once

enum class EIntervals
{
	NONE = 1,
	ONEMINUTE = 2,
	THREEMINUTES = 3,
	FIVEMINUTES,
	FIFTEENMINUTES,
	THIRTYMINUTES,
	ONEHOUR,
	TWOHOURS,
	FOURHOURS,
	SIXHOURS,
	EIGHTHOURS,
	TWELVEHOURS,
	ONEDAY,
	THREEDAYS,
	ONEWEEK,
	ONEMONTH,
};

enum class ESide
{
	NONE,
	BUY,
	SELL
};

enum class EOrderType
{
	NONE,
	LIMIT,
	MARKET,
	STOPLOSS,
	STOPLOSSLIMIT,
	TAKEPROFIT,
	TAKEPROFITLIMIT,
	LIMITMAKER
};

enum class ENewOrderResponseType
{
	NONE,
	ACK,
	RESULT,
	FULL,
};

/*
* Time in force (timeInForce):
* 
* This sets how long an order will be active before expiration.
* 
* Status	Description
* GTC		Good Til Canceled
*			An order will be on the book unless the order is canceled.
* 
* IOC		Immediate Or Cancel
*			An order will try to fill the order as much as it can before the order expires.
* 
* FOK		Fill or Kill
*			An order will expire if the full order cannot be filled upon execution.
*/
enum class ETimeInForce
{
	NONE,
	GTC,
	IOC,
	FOK,
};

enum class EFrequency
{
	NONE = 0,
	HUNDREDMILI = 100,
	THOUSANDMILI = 1000
};

enum class EDepthLevel
{
	NONE = 0,
	FIVE = 5,
	TEN = 10,
	TWENTY = 20
};