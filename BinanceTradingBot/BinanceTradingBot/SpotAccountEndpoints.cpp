#include "SpotAccountEndpoints.h"

using namespace std;

/*
* New Order (TRADE)
*
* POST /api/v3/order (HMAC SHA256)
*
* Weight(UID): 1
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES	
* side				ENUM			YES	
* type				ENUM			YES	
* timeInForce		ENUM			NO	
* quantity			DECIMAL			NO	
* quoteOrderQty		DECIMAL			NO	
* price				DECIMAL			NO	
* newClientOrderId	STRING			NO			A unique id among open orders. Automatically generated if not sent.
* stopPrice			DECIMAL			NO			Used with STOP_LOSS, STOP_LOSS_LIMIT, TAKE_PROFIT, and TAKE_PROFIT_LIMIT orders.
* icebergQty		DECIMAL			NO			Used with LIMIT, STOP_LOSS_LIMIT, and TAKE_PROFIT_LIMIT to create an iceberg order.
* newOrderRespType	ENUM			NO			Set the response JSON. ACK, RESULT, or FULL; MARKET and LIMIT order types default to FULL, all other orders default to ACK.
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSGINED SHORT	YES
* 
* 
* Additional		mandatory parameters based on type:
* Type				Additional mandatory parameters
* LIMIT				timeInForce, quantity, price
* MARKET			quantity or quoteOrderQty
* STOP_LOSS			quantity, stopPrice
* STOP_LOSS_LIMIT	timeInForce, quantity, price, stopPrice
* TAKE_PROFIT		quantity, stopPrice
* TAKE_PROFIT_LIMIT	timeInForce, quantity, price, stopPrice
* LIMIT_MAKER		quantity, price
* 
* Other info:
* LIMIT_MAKER are LIMIT orders that will be rejected if they would immediately match and trade as a taker.
* STOP_LOSS and TAKE_PROFIT will execute a MARKET order when the stopPrice is reached.
* Any LIMIT or LIMIT_MAKER type order can be made an iceberg order by sending an icebergQty.
* Any order with an icebergQty MUST have timeInForce set to GTC.
* MARKET orders using the quantity field specifies the amount of the base asset the user wants to buy or sell at the market price.
* For example, sending a MARKET order on BTCUSDT will specify how much BTC the user is buying or selling.
* MARKET orders using quoteOrderQty specifies the amount the user wants to spend (when buying) or receive (when selling) the quote asset; the correct quantity will be determined based on the market liquidity and quoteOrderQty.
* Using BTCUSDT as an example:
* On the BUY side, the order will buy as many BTC as quoteOrderQty USDT can.
* On the SELL side, the order will sell as much BTC needed to receive quoteOrderQty USDT.
* MARKET orders using quoteOrderQty will not break LOT_SIZE filter rules; the order will execute a quantity that will have the notional value as close as possible to quoteOrderQty.
* same newClientOrderId can be accepted only when the previous one is filled, otherwise the order will be rejected.
* Trigger order price rules against market price for both MARKET and LIMIT versions:
* 
* Price above market price: STOP_LOSS BUY, TAKE_PROFIT SELL
* Price below market price: STOP_LOSS SELL, TAKE_PROFIT BUY
*/
void SpotAccountEndpoints::PostSpotAccountNewOrderQuery(string& _url, string& _queryString, const string& _symbol, const ETimeInForce _timeInForce, const string& _quantity, const string& _quoteOrderQuantity, const string& _price, const string& _newClientOrderId, const string& _stopPrice, const string& _icebergQuantity, const ENewOrderResponseType _newOrderResponseType, const ESide _side, const EOrderType _orderType, const unsigned short _recvWindow) const
{
	if (_symbol.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}
	if (_side == ESide::NONE)
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'side'");
		return;
	}

	const string orderType{ eOrderTypeToString.at(_orderType) };

	switch (_orderType)
	{
		using enum EOrderType;

	case NONE:
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'orderType'");
		return;
		break;
	case LIMIT:
		if (_timeInForce == ETimeInForce::NONE)
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'timeInForce' with " + orderType);
			return;
		}
		if (_quantity.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'quantity' with " + orderType);
			return;
		}
		if (_price.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'price' with " + orderType);
			return;
		}
		break;
	case MARKET:
		if (_quantity.empty() && _quoteOrderQuantity.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'quantity' or 'quoteOrderQuantity' with " + orderType);
			return;
		}
		break;
	case STOPLOSS:
		if (_quantity.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'quantity' with " + orderType);
			return;
		}
		if (_stopPrice.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'stopPrice' with " + orderType);
			return;
		}
		break;
	case STOPLOSSLIMIT:
		if (_timeInForce == ETimeInForce::NONE)
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'timeInForce' with " + orderType);
			return;
		}
		if (_quantity.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'quantity' with " + orderType);
			return;
		}
		if (_price.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'price' with " + orderType);
			return;
		}
		if (_stopPrice.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'stopPrice' with " + orderType);
			return;
		}
		break;
	case TAKEPROFIT:
		if (_quantity.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'quantity' with " + orderType);
			return;
		}
		if (_stopPrice.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'stopPrice' with " + orderType);
			return;
		}
		break;
	case TAKEPROFITLIMIT:
		if (_timeInForce == ETimeInForce::NONE)
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'timeInForce' with " + orderType);
			return;
		}
		if (_quantity.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'quantity' with " + orderType);
			return;
		}
		if (_price.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'price' with " + orderType);
			return;
		}
		if (_stopPrice.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'stopPrice' with " + orderType);
			return;
		}
		break;
	case LIMITMAKER:
		if (_quantity.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'quantity' with " + orderType);
			return;
		}
		if (_price.empty())
		{
			FileLogger::WriteWarnEntry("Did not set mandatory parameter 'price' with " + orderType);
			return;
		}
		break;
	}

	_url += "/api/v3/order?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	//	symbol
	_queryString.append("&symbol=");
	_queryString.append(_symbol);

	//	side
	_queryString.append("&side=");
	_queryString.append(eSidesToString.at(_side));

	//	type
	_queryString.append("&type=");
	_queryString.append(eOrderTypeToString.at(_orderType));
	
	//	timeInForce
	if (_timeInForce != ETimeInForce::NONE)
	{
		_queryString.append("&timeInForce=");
		_queryString.append(eTimeInForceToString.at(_timeInForce));
	}

	//	quantity
	if (!_quantity.empty())
	{
		_queryString.append("&quantity=");
		_queryString.append(_quantity);
	}
	
	//	quoteOrderQty
	if (!_quoteOrderQuantity.empty())
	{
		_queryString.append("&quoteOrderQty=");
		_queryString.append(_quoteOrderQuantity);
	}

	//	price
	if (!_price.empty())
	{
		_queryString.append("&price=");
		_queryString.append(_price);
	}

	//	newClientOrderId
	if (!_newClientOrderId.empty())
	{
		_queryString.append("&newClientOrderId=");
		_queryString.append(_newClientOrderId);
	}
	
	// stopPrice
	// Used with STOP_LOSS, STOP_LOSS_LIMIT, TAKE_PROFIT, and TAKE_PROFIT_LIMIT orders.
	if (!_stopPrice.empty())
	{
		switch (_orderType)
		{
			using enum EOrderType;

		case STOPLOSS:
		case STOPLOSSLIMIT:
		case TAKEPROFIT:
		case TAKEPROFITLIMIT:

			_queryString.append("&stopPrice=");
			_queryString.append(_stopPrice);
			break;
		}
	}
	
	// icebergQty
	// Used with LIMIT, STOP_LOSS_LIMIT, and TAKE_PROFIT_LIMIT to create an iceberg order.
	if (!_icebergQuantity.empty())
	{
		switch (_orderType)
		{
			using enum EOrderType;

		case LIMIT:
		case STOPLOSSLIMIT:
		case TAKEPROFITLIMIT:

			_queryString.append("&icebergQty=");
			_queryString.append(_icebergQuantity);
			break;
		}
		
	}

	//	newOrderRespType
	if (_newOrderResponseType != ENewOrderResponseType::NONE)
	{
		_queryString.append("&newOrderRespType=");
		_queryString.append(eNewOrderResponseTypeToString.at(_newOrderResponseType));
	}
}

/*
* Cancel Order (TRADE)
*
* DELETE /api/v3/order (HMAC SHA256)
*
* Weight(IP): 1
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES	
* orderId			UNSIGNED SHORT	NO	
* origClientOrderId	STRING			NO	
* newClientOrderId	STRING			NO			Used to uniquely identify this cancel. Automatically generated by default.
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
* 
* Either orderId or origClientOrderId must be sent.
*/
void SpotAccountEndpoints::DeleteSpotAccountCancelOrderQuery(string& _url, string& _queryString, const string& _symbol, const unsigned short _orderId, const string& _originalClientOrderId, const string& _newClientOrderId, const unsigned short _recvWindow) const
{
	if (_symbol.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}
	if (_orderId == 0 && _originalClientOrderId.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'orderId' or 'originalClientOrderId'");
		return;
	}

	_url += "/api/v3/order?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	//	symbol
	_queryString.append("&symbol=");
	_queryString.append(_symbol);

	//	orderId
	if (_orderId != 0)
	{
		_queryString.append("&orderId=");
		_queryString.append(to_string(_orderId));
	}
	
	//	origClientOrderId
	if (!_originalClientOrderId.empty())
	{
		_queryString.append("&origClientOrderId=");
		_queryString.append(_originalClientOrderId);
	}
	
	//	newClientOrderId
	if (!_newClientOrderId.empty())
	{
		_queryString.append("&newClientOrderId=");
		_queryString.append(_newClientOrderId);
	}
}

/*
* Cancel all Open Orders on a Symbol (TRADE)
* 
* Cancels all active orders on a symbol.
* This includes OCO orders.
*
* DELETE /api/v3/openOrders
*
* Weight(IP): 1
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*/
void SpotAccountEndpoints::DeleteSpotAccountCancelAllOpenOrdersOnSymbolQuery(string& _url, string& _queryString, const string& _symbol, const unsigned short _recvWindow) const
{
	if (_symbol.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/openOrders?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	//	symbol
	_queryString.append("&symbol=");
	_queryString.append(_symbol);
}

/*
* Query Order (USER_DATA)
*
* Check an order's status.
*
* GET /api/v3/order (HMAC SHA256)
*
* Weight(IP): 2
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES
* orderId			UNSIGNED SHORT	NO	
* origClientOrderId	STRING			NO
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*
* Either orderId or origClientOrderId must be sent.
* For some historical orders cummulativeQuoteQty will be < 0, meaning the data is not available at this time.
*/
void SpotAccountEndpoints::GetSpotAccountQueryOrderQuery(string& _url, string& _queryString, const string& _symbol, const unsigned short _orderId, const string& _originalClientOrderId, const unsigned short _recvWindow) const
{
	if (_symbol.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}
	if (_orderId == 0 && _originalClientOrderId.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'orderId' or 'originalClientOrderId'");
		return;
	}

	_url += "/api/v3/order?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	//	symbol
	_queryString.append("&symbol=");
	_queryString.append(_symbol);

	if (_orderId != 0)
	{
		_queryString.append("&orderId=");
		_queryString.append(to_string(_orderId));
	}

	//	origClientOrderId
	if (!_originalClientOrderId.empty())
	{
		_queryString.append("&origClientOrderId=");
		_queryString.append(_originalClientOrderId);
	}
}

/*
* Current Open Orders (USER_DATA)
*
* Get all open orders on a symbol. Careful when accessing this with no symbol.
*
* GET /api/v3/openOrders (HMAC SHA256)
*
* Weight(IP): 3 for a single symbol; 40 when the symbol parameter is omitted;
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			NO
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*
* If the symbol is not sent, orders for all symbols will be returned in an array.
*/
void SpotAccountEndpoints::GetSpotAccountCurrentOpenOrdersQuery(string& _url, string& _queryString, const string& _symbol, const unsigned short _recvWindow) const
{
	_url += "/api/v3/openOrders?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	//	symbol
	if (!_symbol.empty())
	{
		_queryString.append("&symbol=");
		_queryString.append(_symbol);
	}
}

/*
* All Orders (USER_DATA)
*
* Get all account orders; active, canceled, or filled.
*
* GET /api/v3/allOrders (HMAC SHA256)
*
* Weight(IP): 10 with symbol
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES
* orderId			UNSIGNED SHORT	NO	
* startTime			LONG			NO	
* endTime			LONG			NO	
* limit				INT				NO			Default 500; max 1000.
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*
* If orderId is set, it will get orders >= that orderId. Otherwise most recent orders are returned.
* For some historical orders cummulativeQuoteQty will be < 0, meaning the data is not available at this time.
* If startTime and/or endTime provided, orderId is not required.
*/
void SpotAccountEndpoints::GetSpotAccountAllOrdersQuery(string& _url, string& _queryString, const string& _symbol, const unsigned short _orderId, const time_t _startTime, const time_t _endTime, const int _limit, const unsigned short _recvWindow) const
{
	if (_symbol.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}
	if (_startTime == 0 && _endTime == 0 && _orderId == 0)
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'starttime'/'endtime'/'orderId'");
		return;
	}

	_url += "/api/v3/allOrders?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	_queryString.append("&symbol=");
	_queryString.append(_symbol);

	if (_orderId != 0)
	{
		_queryString.append("&orderId=");
		_queryString.append(to_string(_orderId));
	}

	if (_startTime != 0)
	{
		_queryString.append("&startTime=");
		_queryString.append(to_string(_startTime));
	}

	if (_endTime != 0)
	{
		_queryString.append("&endTime=");
		_queryString.append(to_string(_endTime));
	}

	if (_limit != 0)
	{
		_queryString.append("&limit=");
		_queryString.append(to_string(_limit));
	}
}

/*
* New OCO (TRADE)
*
* POST /api/v3/order/oco (HMAC SHA256)
*
* Weight(UID): 1
*
* Parameters:
* Name					Type			Mandatory	Description
* symbol				STRING			YES
* listClientOrderId		STRING			NO			A unique Id for the entire orderList
* side					ENUM			YES	
* quantity				DECIMAL			YES	
* limitClientOrderId	STRING			NO			A unique Id for the limit order
* price					DECIMAL			YES	
* limitIcebergQty		DECIMAL			NO	
* stopClientOrderId		STRING			NO			A unique Id for the stop loss/stop loss limit leg
* stopPrice				DECIMAL			YES	
* stopLimitPrice		DECIMAL			NO			If provided, stopLimitTimeInForce is required.
* stopIcebergQty		DECIMAL			NO	
* stopLimitTimeInForce	ENUM			NO			Valid values are GTC/FOK/IOC
* newOrderRespType		ENUM			NO			Set the response JSON.
* recvWindow			UNSGINED SHORT	NO			The value cannot be greater than 60000
* timestamp				UNSGINED SHORT	YES
*
*
* Other Info:
* Price Restrictions:
* SELL: Limit Price > Last Price > Stop Price
* BUY: Limit Price < Last Price < Stop Price
* Quantity Restrictions:
* Both legs must have the same quantity
* ICEBERG quantities however do not have to be the same.
* Order Rate Limit
* OCO counts as 2 orders against the order rate limit.
*/
void SpotAccountEndpoints::PostSpotAccountNewOcoOrderQuery(string& _url, string& _queryString, const string& _symbol, const string& _listClientOrderId, const string& _quantity, const string& _limitClientOrderId, const string& _price, const string& _limitIcebergQuantity, const string& _stopClientOrderId, const string& _stopPrice, const string& _stopLimitPrice, const string& _stopIcebergQuantity, const ETimeInForce _stopLimitTimeInForce, const ENewOrderResponseType _newOrderResponseType, const ESide _side, const unsigned short _recvWindow) const
{
	if (_symbol.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}
	if (_side == ESide::NONE)
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'side'");
		return;
	}
	if (_quantity.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'quantity'");
		return;
	}
	if (_price.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'price'");
		return;
	}
	if (_stopPrice.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'stopPrice'");
		return;
	}
	if (!_stopLimitPrice.empty() && _stopLimitTimeInForce == ETimeInForce::NONE)
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'stopLimitTimeInForce' with 'stopLimitPrice'");
		return;
	}

	_url += "/api/v3/order?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	// symbol
	_queryString.append("&symbol=");
	_queryString.append(_symbol);

	// listClientOrderId
	if (!_listClientOrderId.empty())
	{
		_queryString.append("&listClientOrderId=");
		_queryString.append(_listClientOrderId);
	}

	// side
	_queryString.append("&side=");
	_queryString.append(eSidesToString.at(_side));

	// quantity
	_queryString.append("&quantity=");
	_queryString.append(_quantity);

	// limitClientOrderId
	if (!_limitClientOrderId.empty())
	{
		_queryString.append("&limitClientOrderId=");
		_queryString.append(_limitClientOrderId);
	}

	// price
	_queryString.append("&price=");
	_queryString.append(_price);

	// limitIcebergQty
	if (!_limitIcebergQuantity.empty())
	{
		_queryString.append("&limitIcebergQty=");
		_queryString.append(_limitIcebergQuantity);
	}

	// stopClientOrderId
	if (!_stopClientOrderId.empty())
	{
		_queryString.append("&stopClientOrderId=");
		_queryString.append(_stopClientOrderId);
	}

	//	stopPrice
	_queryString.append("&stopPrice=");
	_queryString.append(_stopPrice);

	//	stopLimitPrice
	if (!_stopLimitPrice.empty())
	{
		_queryString.append("&stopLimitPrice=");
		_queryString.append(_stopLimitPrice);
	}

	//	stopIcebergQty
	if (!_stopIcebergQuantity.empty())
	{
		_queryString.append("&stopIcebergQty=");
		_queryString.append(_stopIcebergQuantity);
	}

	//	stopLimitTimeInForce
	if (_stopLimitTimeInForce != ETimeInForce::NONE)
	{
		_queryString.append("&stopLimitTimeInForce=");
		_queryString.append(eTimeInForceToString.at(_stopLimitTimeInForce));
	}

	//	newOrderRespType
	if (_newOrderResponseType != ENewOrderResponseType::RESULT)
	{
		_queryString.append("&newOrderRespType=");
		_queryString.append(eNewOrderResponseTypeToString.at(_newOrderResponseType));
	}
}

/*
* Cancel OCO (TRADE)
*
* Cancel an entire Order List.
*
* DELETE /api/v3/orderList (HMAC SHA256)
*
* Weight(IP): 1
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES
* orderListId		UNSIGNED SHORT	NO			Either orderListId or listClientOrderId must be provided
* listClientOrderId	STRING			NO			Either orderListId or listClientOrderId must be provided
* newClientOrderId	STRING			NO			Used to uniquely identify this cancel. Automatically generated by default
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*
* Additional notes:
* Canceling an individual leg will cancel the entire OCO
*/
void SpotAccountEndpoints::DeleteSpotAccountCancelOcoQuery(string& _url, string& _queryString, const string& _symbol, const unsigned short _orderListId, const string& _listClientOrderId, const string& _newClientOrderId, const unsigned short _recvWindow) const
{
	if (_symbol.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}
	if (_orderListId == 0 && _listClientOrderId.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter '_orderListId' or 'listClientOrderId'");
		return;
	}

	_url += "/api/v3/orderList?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	// symbol
	_queryString.append("&symbol=");
	_queryString.append(_symbol);

	// orderListId
	if (_orderListId > 0)
	{
		_queryString.append("&orderListId=");
		_queryString.append(to_string(_orderListId));
	}

	// listClientOrderId
	if (!_listClientOrderId.empty())
	{
		_queryString.append("&listClientOrderId=");
		_queryString.append(_listClientOrderId);
	}
	
	// newClientOrderId
	if (!_newClientOrderId.empty())
	{
		_queryString.append("&newClientOrderId=");
		_queryString.append(_newClientOrderId);
	}
}

/*
* Query OCO (USER_DATA)
*
* Retrieves a specific OCO based on provided optional parameters
*
* GET /api/v3/orderList (HMAC SHA256)
*
* Weight(IP): 2
*
* Parameters:
* Name				Type			Mandatory	Description
* orderListId		UNSIGNED SHORT	NO			Either orderListId or origClientOrderId must be provided
* origClientOrderId	STRING			NO			Either orderListId or origClientOrderId must be provided
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*/
void SpotAccountEndpoints::GetSpotAccountQueryOcoQuery(string& _url, string& _queryString, const unsigned short _orderListId, const string& _originalClientOrderId, const unsigned short _recvWindow) const
{
	if (_orderListId == 0 && _originalClientOrderId.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'orderListId' or 'originalClientOrderId'");
		return;
	}

	_url += "/api/v3/orderList?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	// orderListId
	if (_orderListId > 0)
	{
		_queryString.append("&orderListId=");
		_queryString.append(to_string(_orderListId));
	}

	// origClientOrderId
	if (!_originalClientOrderId.empty())
	{
		_queryString.append("&origClientOrderId=");
		_queryString.append(_originalClientOrderId);
	}
}

/*
* Query all OCO (USER_DATA)
*
* Retrieves all OCO based on provided optional parameters
*
* GET /api/v3/allOrderList (HMAC SHA256)
*
* Weight(IP): 10
*
* Parameters:
* Name				Type			Mandatory	Description
* fromId			UNSIGNED SHORT	NO			If supplied, neither startTime or endTime can be provided
* startTime			LONG			NO	
* endTime			LONG			NO	
* limit				INT				NO			Default Value: 500; Max Value: 1000
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*/
void SpotAccountEndpoints::GetSpotAccountQueryAllOcoQuery(string& _url, string& _queryString, const unsigned short _fromId, const time_t _startTime, const time_t _endTime, const int _limit, const unsigned short _recvWindow) const
{
	if (_fromId > 0 && (_startTime > 0 ||_endTime > 0))
	{
		FileLogger::WriteWarnEntry("Did set too much parameters 'fromId' and 'startTime' or 'startTime'");
		return;
	}

	_url += "/api/v3/allOrderList?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	// fromId
	if (_fromId > 0)
	{
		_queryString.append("&fromId=");
		_queryString.append(to_string(_fromId));
	}

	// startTime
	if (_startTime != 0)
	{
		_queryString.append("&startTime=");
		_queryString.append(to_string(_startTime));
	}

	// endTime
	if (_endTime != 0)
	{
		_queryString.append("&endTime=");
		_queryString.append(to_string(_endTime));
	}

	// limit
	if (_limit > 0)
	{
		_queryString.append("&limit=");
		_queryString.append(to_string(_limit));
	}
}

/*
* Query Open OCO (USER_DATA)
*
* GET /api/v3/openOrderList (HMAC SHA256)
*
* Weight(IP): 3
*
* Parameters:
* Name				Type			Mandatory	Description
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*/
void SpotAccountEndpoints::GetSpotAccountQueryOpenOcoQuery(string& _url, string& _queryString, const unsigned short _recvWindow) const
{
	_url += "/api/v3/openOrderList?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}
}

/*
* Account Information (USER_DATA)
*
* GET /api/v3/account (HMAC SHA256)
*
* Weight(IP): 10
*
* Parameters:
* Name				Type			Mandatory	Description
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*/
void SpotAccountEndpoints::GetSpotAccountInformationQuery(string& _url, string& _queryString, const unsigned short _recvWindow) const
{
	_url += "/api/v3/account?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}
}

/*
* Account Trade List (USER_DATA)
*
* GET /api/v3/myTrades (HMAC SHA256)
*
* Weight(IP): 10
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES	
* orderId			UNSIGNED SHORT	NO			This can only be used in combination with symbol.
* startTime			LONG			NO	
* endTime			LONG			NO	
* fromId			UNSIGNED SHORT	NO			TradeId to fetch from. Default gets most recent trades.
* limit				INT				NO			Default 500; max 1000.
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
* 
* If fromId is set, it will get id >= that fromId. Otherwise most recent trades are returned.
*/
void SpotAccountEndpoints::GetSpotAccountTradeListQuery(string& _url, string& _queryString, const string& _symbol, const unsigned short _orderId, const time_t _startTime, const time_t _endTime, const unsigned short _fromId, const int _limit, const unsigned short _recvWindow) const
{
	if (_symbol.empty())
	{
		FileLogger::WriteWarnEntry("Did not set mandatory parameter 'symbol'");
		return;
	}

	_url += "/api/v3/myTrades?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}

	// symbol
	_queryString.append("&symbol=");
	_queryString.append(_symbol);

	// orderId
	if (_orderId > 0)
	{
		_queryString.append("&orderId=");
		_queryString.append(to_string(_orderId));
	}

	// startTime
	if (_startTime != 0)
	{
		_queryString.append("&startTime=");
		_queryString.append(to_string(_startTime));
	}

	// endTime
	if (_endTime != 0)
	{
		_queryString.append("&endTime=");
		_queryString.append(to_string(_endTime));
	}

	// fromId
	if (_fromId > 0)
	{
		_queryString.append("&fromId=");
		_queryString.append(to_string(_fromId));
	}

	// limit
	if (_limit > 0)
	{
		_queryString.append("&limit=");
		_queryString.append(to_string(_limit));
	}
}

/*
* Query Current Order Count Usage (TRADE)
*
* Displays the user's current order count usage for all intervals.
* 
* GET /api/v3/rateLimit/order
*
* Weight(IP): 20
*
* Parameters:
* Name				Type			Mandatory	Description
* recvWindow		UNSIGNED SHORT	NO			The value cannot be greater than 60000
* timestamp			UNSIGNED SHORT	YES
*/
void SpotAccountEndpoints::GetSpotAccountCurrentOrderCountUsageQuery(string& _url, string& _queryString, const unsigned short _recvWindow) const
{
	_url += "/api/v3/rateLimit/order?";

	if (_recvWindow > 0)
	{
		_queryString.append("recvWindow=");
		_queryString.append(to_string(_recvWindow));
	}
}
