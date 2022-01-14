#include "ApiRequestManager.h"

ApiRequestManager::ApiRequestManager(const Logger& _logger, string _apiKey, string _secretKey) : logger(_logger)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	apiKey = std::move(_apiKey);
	secretKey = std::move(_secretKey);

	marketDataEndpoints = MarketDataEndpoints(_logger);
	walletEndpoints = WalletEndpoints(_logger);
	spotAccountEndpoints = SpotAccountEndpoints(_logger);
}

void ApiRequestManager::CleanUpApiRequestManager() const
{
	logger->WriteInfoEntry("Cleaning up the ApiRequestManager.");

	curl_global_cleanup();
}

#pragma region MarketData
string ApiRequestManager::GetMarketDataCandlestick(const string& _symbol, EIntervals _interval, time_t _startTime, time_t _endTime, int _limit) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	marketDataEndpoints.GetMarketCandlestickDataQuery(url, queryString, _symbol, _interval, _startTime, _endTime, _limit);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, false);

	return strResult;
}

string ApiRequestManager::GetMarketDataSymbolPriceTicker(const string& _symbol) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	marketDataEndpoints.GetMarketSymbolPriceTickerQuery(url, queryString, _symbol);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, false);

	return strResult;
}
#pragma endregion

#pragma region SpotAccount
/*
* Displays the user's current order count usage for all intervals.
*/
string ApiRequestManager::GetSpotAccountCurrentOrderCountUsage() const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.GetSpotAccountCurrentOrderCountUsageQuery(url, queryString);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES	
* orderId			UNSIGNED SHORT	NO			This can only be used in combination with symbol.
* startTime			LONG			NO	
* endTime			LONG			NO	
* fromId			UNSIGNED SHORT	NO			TradeId to fetch from. Default gets most recent trades.
* limit				INT				NO			Default 500; max 1000.
* 
* If fromId is set, it will get id >= that fromId. Otherwise most recent trades are returned.
*/
string ApiRequestManager::GetSpotAccountTradeList(const string& _symbol, const unsigned short _orderId, const time_t _startTime, const time_t _endTime, const unsigned short _fromId, const int _limit) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.GetSpotAccountTradeListQuery(url, queryString, _symbol, _orderId, _startTime, _endTime, _fromId, _limit);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

string ApiRequestManager::GetSpotAccountInformation() const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.GetSpotAccountInformationQuery(url, queryString);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

string ApiRequestManager::GetSpotAccountQueryOpenOco() const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.GetSpotAccountQueryOpenOcoQuery(url, queryString);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Retrieves all OCO based on provided optional parameters
*
* Parameters:
* Name				Type			Mandatory	Description
* fromId			UNSIGNED SHORT	NO			If supplied, neither startTime or endTime can be provided
* startTime			LONG			NO	
* endTime			LONG			NO	
* limit				INT				NO			Default Value: 500; Max Value: 1000
*/
string ApiRequestManager::GetSpotAccountQueryAllOco(const unsigned short _fromId, const time_t _startTime, const time_t _endTime, const int _limit) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.GetSpotAccountQueryAllOcoQuery(url, queryString, _fromId, _startTime, _endTime, _limit);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Retrieves a specific OCO based on provided optional parameters
*
* Parameters:
* Name					Type			Mandatory	Description
* orderListId			UNSIGNED SHORT	NO			Either orderListId or origClientOrderId must be provided
* origClientOrderId		STRING			NO			Either orderListId or origClientOrderId must be provided
*/
string ApiRequestManager::GetSpotAccountQueryOco(const unsigned short _orderListId, const string& _originalClientOrderId) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.GetSpotAccountQueryOcoQuery(url, queryString, _orderListId, _originalClientOrderId);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Cancel an entire Order List.
*
* Parameters:
* Name					Type			Mandatory	Description
* symbol				STRING			YES
* orderListId			UNSIGNED SHORT	NO			Either orderListId or listClientOrderId must be provided
* listClientOrderId		STRING			NO			Either orderListId or listClientOrderId must be provided
* newClientOrderId		STRING			NO			Used to uniquely identify this cancel. Automatically generated by default
*
* Additional notes:
* Canceling an individual leg will cancel the entire OCO
*/
string ApiRequestManager::DeleteSpotAccountCancelOco(const string& _symbol, const unsigned short _orderListId,
                                                   const string& _listClientOrderId, const string& _newClientOrderId) const
{
	string postData;
	string strResult;
	const string action = "DELETE";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.DeleteSpotAccountCancelOcoQuery(url, queryString, _symbol, _orderListId, _listClientOrderId, _newClientOrderId);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
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
string ApiRequestManager::PostSpotAccountNewOcoOrder(const string& _symbol, const string& _listClientOrderId, const string& _quantity,
                                                   const string& _limitClientOrderId, const string& _price, const string& _limitIcebergQuantity,
                                                   const string& _stopClientOrderId, const string& _stopPrice, const string& _stopLimitPrice, const string& _stopIcebergQuantity, const ETimeInForce _stopLimitTimeInForce,
                                                   const ENewOrderResponseType _newOrderResponseType, const ESide _side) const
{
	string postData;
	string strResult;
	const string action = "POST";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.PostSpotAccountNewOcoOrderQuery(url, queryString, _symbol, _listClientOrderId, _quantity, _limitClientOrderId, _price, _limitIcebergQuantity, _stopClientOrderId, _stopPrice, _stopLimitPrice, _stopIcebergQuantity, _stopLimitTimeInForce, _newOrderResponseType, _side);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Get all account orders; active, canceled, or filled.
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES
* orderId			UNSIGNED SHORT	NO	
* startTime			LONG			NO	
* endTime			LONG			NO	
* limit				INT				NO			Default 500; max 1000.
*
* If orderId is set, it will get orders >= that orderId. Otherwise most recent orders are returned.
* For some historical orders cummulativeQuoteQty will be < 0, meaning the data is not available at this time.
* If startTime and/or endTime provided, orderId is not required.
*/
string ApiRequestManager::GetSpotAccountAllOrders(const string& _symbol, const unsigned short _orderId, const time_t _startTime, const time_t _endTime, const int _limit) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.GetSpotAccountAllOrdersQuery(url, queryString, _symbol, _orderId, _startTime, _endTime, _limit);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Get all open orders on a symbol. Careful when accessing this with no symbol.
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			NO
*
* If the symbol is not sent, orders for all symbols will be returned in an array.
*/
string ApiRequestManager::GetSpotAccountCurrentOpenOrders(const string& _symbol) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.GetSpotAccountCurrentOpenOrdersQuery(url, queryString, _symbol);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Check an order's status.
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES
* orderId			UNSIGNED SHORT	NO
* origClientOrderId	STRING			NO
*
* Either orderId or origClientOrderId must be sent.
* For some historical orders cummulativeQuoteQty will be < 0, meaning the data is not available at this time.
*/
string ApiRequestManager::GetSpotAccountQueryOrder(const string& _symbol, const unsigned short _orderId, const string& _originalClientOrderId) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.GetSpotAccountQueryOrderQuery(url, queryString, _symbol, _orderId, _originalClientOrderId);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Cancel all Open Orders on a Symbol (TRADE)
*
* Cancels all active orders on a symbol.
* This includes OCO orders.
*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES
*/
string ApiRequestManager::DeleteSpotAccountCancelAllOpenOrdersOnSymbol(const string& _symbol) const
{
	string postData;
	string strResult;
	const string action = "DELETE";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.DeleteSpotAccountCancelAllOpenOrdersOnSymbolQuery(url, queryString, _symbol);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Parameters:
* Name				Type			Mandatory	Description
* symbol			STRING			YES
* orderId			UNSIGNED SHORT	NO
* origClientOrderId	STRING			NO
* newClientOrderId	STRING			NO			Used to uniquely identify this cancel. Automatically generated by default.
*
* Either orderId or origClientOrderId must be sent.
*/
string ApiRequestManager::DeleteSpotAccountCancelOrder(const string& _symbol, const unsigned short _orderId, const string& _originalClientOrderId, const string& _newClientOrderId) const
{
	string postData;
	string strResult;
	const string action = "DELETE";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.DeleteSpotAccountCancelOrderQuery(url, queryString, _symbol, _orderId, _originalClientOrderId, _newClientOrderId);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* New Order (TRADE)
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
string ApiRequestManager::PostSpotAccountNewOrder(const string& _symbol, const ETimeInForce _timeInForce, const string& _quantity,
                                                const string& _quoteOrderQuantity, const string& _price, const string& _newClientOrderId, const string& _stopPrice, const string& _icebergQuantity,
                                                const ENewOrderResponseType _newOrderResponseType, const ESide _side, const EOrderType _orderType) const
{
	string postData;
	string strResult;
	const string action = "POST";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	spotAccountEndpoints.PostSpotAccountNewOrderQuery(url, queryString, _symbol, _timeInForce, _quantity, _quoteOrderQuantity, _price, _newClientOrderId, _stopPrice, _icebergQuantity, _newOrderResponseType, _side, _orderType);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}
#pragma endregion

#pragma region Wallet
/*
* Get information of coins (available for deposit and withdraw) for user.
*/
string ApiRequestManager::GetWalletAllCoinsInformation() const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	walletEndpoints.GetWalletAllCoinsInformationQuery(url, queryString);

	CurlAPIWithHeader(url, strResult, postData, action, queryString);

	return strResult;
}

/*
* Daily Account Snapshot (USER_DATA)
*
* Weight(IP): 2400
*
* Parameters:
* Name			Type			Mandatory	Description
* type			STRING			YES			"SPOT", "MARGIN", "FUTURES"
* startTime		UNSGINED SHORT	NO
* endTime		UNSGINED SHORT	NO
* limit			INT				NO			min 5, max 30, default 5
*/
string ApiRequestManager::GetWalletDailyAccountSnapshot(const ESnapshotType _snapshotType, const time_t _startTime, const time_t _endTime, const int _limit) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	walletEndpoints.GetWalletDailyAccountSnapshotQuery(url, queryString, _snapshotType, _startTime, _endTime, _limit);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Withdraw(USER_DATA)
*
* Submit a withdraw request.
*
* Weight(IP): 1
*
* Parameters:
* Name					Type			Mandatory	Description
* coin					STRING			YES
* withdrawOrderId		STRING			NO			client id for withdraw
* network				STRING			NO
* address				STRING			YES
* addressTag			STRING			NO			Secondary address identifier for coins like XRP,XMR etc.
* amount				DECIMAL			YES
* transactionFeeFlag	BOOLEAN			NO			When making internal transfer, true for returning the fee to the destination account; false for returning the fee back to the departure account. Default false.
* name					STRING			NO			Description of the address. Space in name should be encoded into %20.
*
* If network not send, return with default network of the coin.
* You can get network and isDefault in networkList of a coin in the response of Get /sapi/v1/capital/config/getall (HMAC SHA256).
*/
string ApiRequestManager::PostWalletWithdraw(const string& _coin, const string& _withdrawOrderId, const string& _network, const string& _address, const string& _addressTag, const string& _amount, const string& _name, const bool _transactionFeeFlag) const
{
	string postData;
	string strResult;
	const string action = "POST";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	walletEndpoints.PostWalletWithdrawQuery(url, queryString, _coin, _withdrawOrderId, _network,_address, _addressTag, _amount, _name, _transactionFeeFlag);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Deposit History(supporting network) (USER_DATA)
*
* Fetch deposit history.
*
* Weight(IP): 1
*
* Parameters:
* Name					Type			Mandatory	Description
* coin					STRING			NO
* status				INT				NO			0(0:pending,6: credited but cannot withdraw, 1:success)
* startTime				UNSGINED SHORT	NO			Default: 90 days from current timestamp
* endTime				UNSGINED SHORT	NO			Default: present timestamp
* offset				INT				NO			Default:0
* limit					INT				NO			Default:1000, Max:1000
*
* Please notice the default startTime and endTime to make sure that time interval is within 0-90 days.
* If both startTime and endTime are sent, time between startTime and endTime must be less than 90 days.
*/
string ApiRequestManager::GetWalletDepositHistory(const string& _coin, const int _status, const time_t _startTime, const time_t _endTime, const int _offset, const int _limit) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	walletEndpoints.GetWalletDepositHistoryQuery(url, queryString, _coin, _status, _startTime, _endTime, _offset, _limit);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Withdraw History (supporting network) (USER_DATA)
*
* Fetch withdraw history.
*
* Weight(IP): 1
*
* Parameters:
* Name					Type			Mandatory	Description
* coin					STRING			NO
* withdrawOrderId		STRING			NO
* status				INT				NO			0(0:Email Sent,1:Cancelled 2:Awaiting Approval 3:Rejected 4:Processing 5:Failure 6:Completed)
* startTime				UNSGINED SHORT	NO			Default: 90 days from current timestamp
* endTime				UNSGINED SHORT	NO			Default: present timestamp
* offset				INT				NO			Default:0
* limit					INT				NO			Default:1000, Max:1000
*
* network may not be in the response for old withdraw.
* Please notice the default startTime and endTime to make sure that time interval is within 0-90 days.
* If both startTime and endTime are sent, time between startTime and endTime must be less than 90 days.
*/
string ApiRequestManager::GetWalletWithdrawtHistory(const string& _coin, const string& _withdrawOrderId, const int _status, const time_t _startTime, const time_t _endTime, const int _offset, const int _limit) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	walletEndpoints.GetWalletWithdrawtHistoryQuery(url, queryString, _coin, _withdrawOrderId, _status, _startTime, _endTime, _offset, _limit);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}

/*
* Deposit Address (supporting network) (USER_DATA)
*
* Fetch deposit address with network.
*
* Weight(IP): 10
*
* Parameters:
* Name					Type			Mandatory	Description
* coin					STRING			YES
* network				STRING			NO
*
* If network is not send, return with default network of the coin.
* You can get network and isDefault in networkList in the response of Get /sapi/v1/capital/config/getall (HMAC SHA256).
*/
string ApiRequestManager::GetWalletDepositAddress(const string& _coin, const string& _network) const
{
	string postData;
	string strResult;
	const string action = "GET";

	string url(BINANCE_APIENDPOINT);
	string queryString;

	walletEndpoints.GetWalletDepositAddressQuery(url, queryString, _coin, _network);

	CurlAPIWithHeader(url, strResult, postData, action, queryString, true);

	return strResult;
}
#pragma endregion

/*
* make a webrequest
*/
void ApiRequestManager::CurlAPIWithHeader(string& _url, string& _strResult, string& _postData, const string& _action, const string& _queryString, const bool _setSecretAndApiKey) const
{
	if (apiKey.empty() || secretKey.empty())
	{
		logger->WriteWarnEntry("API Key or/and Secret Key has not been set.");
		return;
	}

	if (CURL* curl = curl_easy_init())
	{
		vector <string> extraHttpHeader;
		if(_setSecretAndApiKey)
		{
			if(_queryString.empty())
			{
				SetSecretAndApiKeyInPostData(extraHttpHeader, _postData);
			}
			else
			{
				SetSecretAndApiKeyInUrl(_url, extraHttpHeader, _queryString);
			}
		}
		else
		{
			_url.append(_queryString);
		}

		SetCurlOptions(curl, _url, _strResult);

		if (!extraHttpHeader.empty())
		{
			struct curl_slist* chunk = nullptr;
			for (auto& i : extraHttpHeader)
			{
				chunk = curl_slist_append(chunk, i.c_str());
			}
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		}

		if (!_postData.empty() || _action == "POST" || _action == "PUT" || _action == "DELETE") 
		{
			if (_action == "PUT" || _action == "DELETE") 
			{
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, _action.c_str());
			}

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _postData.c_str());
		}

		PerformCurl(curl);

		curl_easy_cleanup(curl);
	}
	else
	{
		logger->WriteErrorEntry("Could not initialize cURL.");
	}
}

void ApiRequestManager::SetSecretAndApiKeyInUrl(string& _url, vector <string>& _extraHttpHeader, string _querystring) const
{
	const string signature = HmacSha256(_querystring);
	_querystring.append("&signature=");
	_querystring.append(signature);

	_url.append(_querystring);

	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append(apiKey);
	_extraHttpHeader.push_back(header_chunk);
}

void ApiRequestManager::SetSecretAndApiKeyInPostData(vector<string>& _extraHttpHeader, string& _postData) const
{
	const string signature = HmacSha256(_postData);
	_postData.append("&signature=");
	_postData.append(signature);

	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append(apiKey);
	_extraHttpHeader.push_back(header_chunk);
}

/*
* Callbackfunction to retrieve the webrequest
*/
size_t ApiRequestManager::WebRequestCallback(void* _content, const size_t _size, const size_t _nmemb, string* _buffer)
{
	const size_t totalsize = _size * _nmemb;
	_buffer->append(static_cast<char*>(_content), totalsize);

	return totalsize;
}

string ApiRequestManager::HmacSha256(const string& _querystring) const
{
	unsigned char* digest = HMAC(EVP_sha256(), secretKey.c_str(), strlen(secretKey.c_str()),
	                             (unsigned char*) _querystring.c_str(), strlen(_querystring.c_str()), nullptr, nullptr);
	return B2AHex(reinterpret_cast<char*>(digest), 32);
}

string ApiRequestManager::B2AHex(const char* _byteArray, int _n)
{
	const static string hexCodes = "0123456789abcdef";
	string hexString;
	for (int i = 0; i < 32; ++i) 
	{
		unsigned char binValue = _byteArray[i];
		hexString += hexCodes[(binValue >> 4) & 0x0F];
		hexString += hexCodes[binValue & 0x0F];
	}

	return hexString;
}

void ApiRequestManager::CurlAPI(const string& _url, string& _stringResult) const
{
	if (CURL* curl = curl_easy_init())
	{
		SetCurlOptions(curl, _url, _stringResult);

		PerformCurl(curl);

		curl_easy_cleanup(curl);
	}
	else
	{
		logger->WriteErrorEntry("Could not initialize cURL.");
	}
}

void ApiRequestManager::SetCurlOptions(CURL* _curl, const string _url, string& _strResult)
{
	//logger->WriteInfoEntry("making cUrl call to " + _url);

	curl_easy_setopt(_curl, CURLOPT_URL, _url.c_str());
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, ApiRequestManager::WebRequestCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_strResult);
	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, false);
}

void ApiRequestManager::PerformCurl(CURL* _curl) const
{
	if (const CURLcode res = curl_easy_perform(_curl); res != CURLE_OK)
	{
		logger->WriteErrorEntry(curl_easy_strerror(res));
		cerr << "CURL error: " << res << endl;
	}
}
