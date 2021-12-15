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
#include <utility>
#include <json.hpp>

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

	void SetSecretAndApiKeyInUrl(string& _url, vector <string>& _extraHttpHeader, string _querystring) const;
	void SetSecretAndApiKeyInPostData(vector <string>& _extraHttpHeader, string& _postData) const;
	void CurlAPIWithHeader(string& _url, string& _strResult, string& _postData, const string& _action, const string&
	                       _queryString, bool _setSecretAndApiKey = false) const;
	string HmacSha256(const string& _querystring) const;
	static string B2AHex(const char* _byteArray, int _n);
	static size_t WebRequestCallback(void* _content, size_t _size, size_t _nmemb, string* _buffer);

public:
	ApiRequestManager(const Logger& _logger, string _apiKey, string _secretKey);
	void CleanUpApiRequestManager() const;

#pragma region SpotAccount
	void GetSpotAccountCurrentOrderCountUsage() const;
	void GetSpotAccountTradeList(const string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, unsigned short _fromId, int _limit = 500) const;
	void GetSpotAccountInformation() const;
	void GetSpotAccountQueryOpenOco() const;
	void GetSpotAccountQueryAllOco(unsigned short _fromId, time_t _startTime, time_t _endTime, int _limit = 500) const;
	void GetSpotAccountQueryOco(unsigned short _orderListId, const string& _originalClientOrderId) const;
	void DeleteSpotAccountCancelOco(const string& _symbol, unsigned short _orderListId, const string& _listClientOrderId,
	                                const string& _newClientOrderId) const;
	void PostSpotAccountNewOcoOrder(const string& _symbol, const string& _listClientOrderId, double _quantity,
	                                const string& _limitClientOrderId, double _price, double _limitIcebergQuantity,
	                                const string& _stopClientOrderId, double _stopPrice, double _stopLimitPrice, double _stopIcebergQuantity, ETimeInForce _stopLimitTimeInForce, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE);
	void GetSpotAccountAllOrders(const string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, int _limit = 500) const;
	void GetSpotAccountCurrentOpenOrders(const string& _symbol) const;
	void GetSpotAccountQueryOrder(const string& _symbol, unsigned short _orderId, const string& _originalClientOrderId) const;
	void DeleteSpotAccountCancelAllOpenOrdersOnSymbol(const string& _symbol) const;
	void DeleteSpotAccountCancelOrder(const string& _symbol, unsigned short _orderId, const string& _originalClientOrderId, const string& _newClientOrderId) const;
	void PostSpotAccountNewOrder(const string& _symbol, ETimeInForce _timeInForce, double _quantity, double _quoteOrderQuantity, double _price,
	                             const string& _newClientOrderId, double _stopPrice, double _icebergQuantity, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE, EOrderType _orderType = EOrderType::NONE);
#pragma endregion

#pragma region Wallet
	void GetWalletAllCoinsInformation() const;
	void GetWalletDailyAccountSnapshot(ESnapshotType _snapshotType, time_t _startTime, time_t _endTime, int _limit = 5) const;
	void PostWalletWithdraw(const string& _coin, const string& _withdrawOrderId, const string& _network, const string&
	                        _address, const string& _addressTag, double _amount, const string& _name, bool _transactionFeeFlag = false) const;
	void GetWalletDepositHistory(const string& _coin, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000) const;
	void GetWalletWithdrawtHistory(const string& _coin, const string& _withdrawOrderId, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000) const;
	void GetWalletDepositAddress(const string& _coin, const string& _network) const;
#pragma endregion
};
