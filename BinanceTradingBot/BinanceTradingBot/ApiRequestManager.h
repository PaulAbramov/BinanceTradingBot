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
	Logger logger;
	string apiKey;
	string secretKey;

	MarketDataEndpoints marketDataEndpoints;
	WalletEndpoints walletEndpoints;
	SpotAccountEndpoints spotAccountEndpoints;

	void SetSecretAndApiKeyInUrl(string& _url, vector <string>& _extraHttpHeader, string _querystring) const;
	void SetSecretAndApiKeyInPostData(vector <string>& _extraHttpHeader, string& _postData) const;
	void CurlAPIWithHeader(string& _url, string& _strResult, string& _postData, const string& _action, string& _queryString, bool _setSecretAndApiKey = false, bool _getServerTime = true) const;
	string HmacSha256(const string& _querystring) const;
	static string B2AHex(const unsigned char* _byteArray, int _n);
	static size_t WebRequestCallback(void* _content, size_t _size, size_t _nmemb, string* _buffer);

	static void SetCurlOptions(CURL* _curl, const string _url, string& _strResult);
	void PerformCurl(CURL* _curl) const;

public:
	ApiRequestManager(const Logger& _logger, string _apiKey, string _secretKey);
	void CleanUpApiRequestManager() const;

	void CurlAPI(string& _url, string& _stringResult, bool _getServerTime = true) const;

#pragma region MarketData
	string GetMarketDataTime() const;
	string GetMarketDataCandlestick(const string& _symbol, EIntervals _interval, time_t _startTime, time_t _endTime, int _limit = 500) const;
	string GetMarketDataSymbolPriceTicker(const string& _symbol) const;
#pragma endregion

#pragma region SpotAccount
	string GetSpotAccountCurrentOrderCountUsage() const;
	string GetSpotAccountTradeList(const string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, unsigned short _fromId, int _limit = 500) const;
	string GetSpotAccountInformation() const;
	string GetSpotAccountQueryOpenOco() const;
	string GetSpotAccountQueryAllOco(unsigned short _fromId, time_t _startTime, time_t _endTime, int _limit = 500) const;
	string GetSpotAccountQueryOco(unsigned short _orderListId, const string& _originalClientOrderId) const;
	string DeleteSpotAccountCancelOco(const string& _symbol, unsigned short _orderListId, const string& _listClientOrderId, const string& _newClientOrderId) const;
	string PostSpotAccountNewOcoOrder(const string& _symbol, const string& _listClientOrderId, const string& _quantity,
	                                const string& _limitClientOrderId, const string& _price, const string& _limitIcebergQuantity,
	                                const string& _stopClientOrderId, const string& _stopPrice, const string& _stopLimitPrice, const string& _stopIcebergQuantity, ETimeInForce _stopLimitTimeInForce, 
									ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE) const;
	string GetSpotAccountAllOrders(const string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, int _limit = 500) const;
	string GetSpotAccountCurrentOpenOrders(const string& _symbol) const;
	string GetSpotAccountQueryOrder(const string& _symbol, unsigned short _orderId, const string& _originalClientOrderId) const;
	string DeleteSpotAccountCancelAllOpenOrdersOnSymbol(const string& _symbol) const;
	string DeleteSpotAccountCancelOrder(const string& _symbol, unsigned short _orderId, const string& _originalClientOrderId, const string& _newClientOrderId) const;
	string PostSpotAccountNewOrder(const string& _symbol, ETimeInForce _timeInForce, const string& _quantity, const string& _quoteOrderQuantity, const string& _price,
	                             const string& _newClientOrderId, const string& _stopPrice, const string& _icebergQuantity, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, 
	                             ESide _side = ESide::NONE, EOrderType _orderType = EOrderType::NONE) const;
#pragma endregion

#pragma region Wallet
	string GetWalletAllCoinsInformation() const;
	string GetWalletDailyAccountSnapshot(ESnapshotType _snapshotType, time_t _startTime, time_t _endTime, int _limit = 5) const;
	string PostWalletWithdraw(const string& _coin, const string& _withdrawOrderId, const string& _network, const string&
	                        _address, const string& _addressTag, const string& _amount, const string& _name, bool _transactionFeeFlag = false) const;
	string GetWalletDepositHistory(const string& _coin, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000) const;
	string GetWalletWithdrawtHistory(const string& _coin, const string& _withdrawOrderId, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000) const;
	string GetWalletDepositAddress(const string& _coin, const string& _network) const;
#pragma endregion
};