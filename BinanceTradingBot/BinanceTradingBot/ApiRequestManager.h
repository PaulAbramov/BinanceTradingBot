#pragma once

#define BINANCE_APIENDPOINT "https://api.binance.com"
#define BINANCE_HOST "stream.binance.com"
#define BINANCE_PORT "9443"

/*
 * API Endpoints:
 * https://api.binance.com
 * https://api1.binance.com
 * https://api2.binance.com
 * https://api3.binance.com
 */

#define CURL_STATICLIB 
#include <curl/curl.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "LoggingFacility.h"
#include "WalletEndpoints.h"
#include "SpotAccountEndpoints.h"
#include "MarketDataEndpoints.h"

using namespace std;

class ApiRequestManager
{
private:
	CURL* curl;
	Logger logger;
	string apiKey;
	string secretKey;

	WalletEndpoints walletEndpoints;
	SpotAccountEndpoints spotAccountEndpoints;

	void SetSecretAndApiKeyInUrl(string& _url, vector <string>& _extraHttpHeader, string _querystring);
	void SetSecretAndApiKeyInPostData(vector <string>& _extraHttpHeader, string& _postData);
	void CurlAPIWithHeader(string& _url, string& _strResult, string& _postData, string& _action, string _queryString, bool _setSecretAndApiKey = false);
	string HmacSha256(const string _secretKey, const string _querystring);
	string B2AHex(char* _byteArray, int _n);
	static size_t WebRequestCallback(void* _content, size_t _size, size_t _nmemb, string* _buffer);

public:
	ApiRequestManager(const Logger& _logger, string _apiKey, string _secretKey);
	void CleanUpApiRequestManager();

#pragma region SpotAccount
	void GetSpotAccountCurrentOrderCountUsage();
	void GetSpotAccountTradeList(string _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, unsigned short _fromId, int _limit = 500);
	void GetSpotAccountInformation();
	void GetSpotAccountQueryOpenOco();
	void GetSpotAccountQueryAllOco(unsigned short _fromId, time_t _startTime, time_t _endTime, int _limit = 500);
	void GetSpotAccountQueryOco(unsigned short _orderListId, string _originalClientOrderId);
	void DeleteSpotAccountCancelOco(string _symbol, unsigned short _orderListId, string _listClientOrderId, string _newClientOrderId);
	void PostSpotAccountNewOcoOrder(string _symbol, string _listClientOrderId, double _quantity, string _limitClientOrderId, double _price, double _limitIcebergQuantity, string _stopClientOrderId, double _stopPrice, double _stopLimitPrice, double _stopIcebergQuantity, ETimeInForce _stopLimitTimeInForce, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE);
	void GetSpotAccountAllOrders(string _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, int _limit = 500);
	void GetSpotAccountCurrentOpenOrders(string _symbol);
	void GetSpotAccountQueryOrder(string _symbol, unsigned short _orderId, string _originalClientOrderId);
	void DeleteSpotAccountCancelAllOpenOrdersOnSymbol(string _symbol);
	void DeleteSpotAccountCancelOrder(string _symbol, unsigned short _orderId, string _originalClientOrderId, string _newClientOrderId);
	void PostSpotAccountNewOrder(string _symbol, ETimeInForce _timeInForce, double _quantity, double _quoteOrderQuantity, double _price, string _newClientOrderId, double _stopPrice, double _icebergQuantity, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE, EOrderType _orderType = EOrderType::NONE);
#pragma endregion

#pragma region Wallet
	void GetWalletAllCoinsInformation();
	void GetWalletDailyAccountSnapshot(SnapshotType _snapshotType, time_t _startTime, time_t _endTime, int _limit = 5);
	void PostWalletWithdraw(string _coin, string _withdrawOrderId, string _network, string _address, string _addressTag, double _amount, string _name, bool _transactionFeeFlag = false);
	void GetWalletDepositHistory(string _coin, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000);
	void GetWalletWithdrawtHistory(string _coin, string _withdrawOrderId, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000);
	void GetWalletDepositAddress(string _coin, string _network);
#pragma endregion
};
