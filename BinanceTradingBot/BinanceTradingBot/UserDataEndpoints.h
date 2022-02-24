#pragma once

#include <string>

#include "FileLogger.h"

/*
* The base API endpoint is: https://api.binance.com
* A User Data Stream listenKey is valid for 60 minutes after creation.
* Doing a PUT on a listenKey will extend its validity for 60 minutes.
* Doing a DELETE on a listenKey will close the stream and invalidate the listenKey.
* Doing a POST on an account with an active listenKey will return the currently active listenKey and extend its validity for 60 minutes.
* The base websocket endpoint is: wss://stream.binance.com:9443
* User Data Streams are accessed at /ws/<listenKey> or /stream?streams=<listenKey>
* A single connection to stream.binance.com is only valid for 24 hours; expect to be disconnected at the 24 hour mark
* 
* Payload: Account Update
* outboundAccountPosition is sent any time an account balance has changed and contains the assets that were possibly changed by the event that generated the balance change.
* 
* Payload: Balance Update
* Balance Update occurs during the following:
* Deposits or withdrawals from the account
* Transfer of funds between accounts (e.g. Spot to Margin)
* 
* Payload: Order Update
* Orders are updated with the executionReport event.
* Check the Public API Definitions and below for relevant enum definitions.
* Average price can be found by doing Z divided by z.
* Execution types:
* NEW - The order has been accepted into the engine.
* CANCELED - The order has been canceled by the user.
* REPLACED (currently unused)
* REJECTED - The order has been rejected and was not processed. (This is never pushed into the User Data Stream)
* TRADE - Part of the order or all of the order's quantity has filled.
* EXPIRED - The order was canceled according to the order type's rules (e.g. LIMIT FOK orders with no fill, LIMIT IOC or MARKET orders that partially fill) or by the exchange, (e.g. orders canceled during liquidation, orders canceled during maintenance)
* If the order is an OCO, an event will be displayed named ListStatus in addition to the executionReport event.
*/

class UserDataEndpoints
{
private:

public:
	void PostUserDataCreateListenKeySpot(std::string& _url);
	void PutUserDataKeepAliveListenKeySpot(std::string& _url, std::string& _queryString, std::string _listenKey) const;
	void DeleteUserDataCloseListenKeySpot(std::string& _url, std::string& _queryString, std::string _listenKey) const;
};