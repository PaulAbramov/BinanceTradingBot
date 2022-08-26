#pragma once

#define CURL_STATICLIB

constexpr auto BINANCE_APIENDPOINT = "https://api.binance.com";
constexpr auto BINANCE_HOST = "stream.binance.com";
constexpr auto BINANCE_PORT = "9443";

/*
 * API Endpoints:
 * https://api.binance.com
 * https://api1.binance.com
 * https://api2.binance.com
 * https://api3.binance.com
 */

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "WalletEndpoints.h"
#include "SpotAccountEndpoints.h"
#include "MarketDataEndpoints.h"
#include "JsonHelper.h"

class ApiManager
{
private:
	std::string apiKey;
	std::string secretKey;

	MarketDataEndpoints marketDataEndpoints{};
	WalletEndpoints walletEndpoints{};
	SpotAccountEndpoints spotAccountEndpoints{};

	void SetSecretAndApiKeyInUrl(std::string& _url, std::vector<std::string>& _extraHttpHeader, std::string _querystring) const;
	void SetSecretAndApiKeyInPostData(std::vector<std::string>& _extraHttpHeader, std::string& _postData) const;
	void CurlAPIWithHeader(std::string& _url, std::string& _strResult, std::string& _postData, const std::string& _action, std::string& _queryString, bool _setSecretAndApiKey = false, bool _getServerTime = true) const;
	std::string HmacSha256(const std::string& _querystring) const;
	static std::string B2AHex(const unsigned char* _byteArray, int _n);
	static size_t WebRequestCallback(void* _content, size_t _size, size_t _nmemb, std::string* _buffer);
	static size_t FileDownloadCallback(void* _content, size_t _size, size_t _nmemb, FILE* _stream);

	static void SetCurlOptions(CURL* _curl, const std::string _url, std::string& _strResult);
	void PerformCurl(CURL* _curl) const;

public:
	ApiManager(std::string _apiKey, std::string _secretKey);
	~ApiManager();

	void CurlAPI(std::string& _url, std::string& _stringResult, bool _getServerTime = true) const;
	void CurlFile() const;

#pragma region MarketData
	std::string GetMarketDataTime() const;
	std::string GetMarketExchangeInformation(std::vector<std::string>& _symbols) const;
	std::string GetMarketDataCandlestick(const std::string& _symbol, EIntervals _interval, time_t _startTime, time_t _endTime, int _limit = 500) const;
	std::string GetMarketDataSymbolPriceTicker(const std::string& _symbol) const;
#pragma endregion

#pragma region SpotAccount
	std::string GetSpotAccountCurrentOrderCountUsage() const;
	std::string GetSpotAccountTradeList(const std::string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, unsigned short _fromId, int _limit = 500) const;
	std::string GetSpotAccountInformation() const;
	std::string GetSpotAccountQueryOpenOco() const;
	std::string GetSpotAccountQueryAllOco(unsigned short _fromId, time_t _startTime, time_t _endTime, int _limit = 500) const;
	std::string GetSpotAccountQueryOco(unsigned short _orderListId, const std::string& _originalClientOrderId) const;
	std::string DeleteSpotAccountCancelOco(const std::string& _symbol, unsigned short _orderListId, const std::string& _listClientOrderId, const std::string& _newClientOrderId) const;
	std::string PostSpotAccountNewOcoOrder(const std::string& _symbol, const std::string& _listClientOrderId, const std::string& _quantity,
	                                const std::string& _limitClientOrderId, const std::string& _price, const std::string& _limitIcebergQuantity,
	                                const std::string& _stopClientOrderId, const std::string& _stopPrice, const std::string& _stopLimitPrice, const std::string& _stopIcebergQuantity, ETimeInForce _stopLimitTimeInForce,
									ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE) const;
	std::string GetSpotAccountAllOrders(const std::string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, int _limit = 500) const;
	std::string GetSpotAccountCurrentOpenOrders(const std::string& _symbol) const;
	std::string GetSpotAccountQueryOrder(const std::string& _symbol, unsigned short _orderId, const std::string& _originalClientOrderId) const;
	std::string DeleteSpotAccountCancelAllOpenOrdersOnSymbol(const std::string& _symbol) const;
	std::string DeleteSpotAccountCancelOrder(const std::string& _symbol, unsigned short _orderId, const std::string& _originalClientOrderId, const std::string& _newClientOrderId) const;
	std::string PostSpotAccountNewOrder(const std::string& _symbol, ETimeInForce _timeInForce, const std::string& _quantity, const std::string& _quoteOrderQuantity, const std::string& _price,
	                             const std::string& _newClientOrderId, const std::string& _stopPrice, const std::string& _icebergQuantity, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL,
	                             ESide _side = ESide::NONE, EOrderType _orderType = EOrderType::NONE) const;
#pragma endregion

#pragma region Wallet
	std::string GetWalletAllCoinsInformation() const;
	std::string GetWalletDailyAccountSnapshot(ESnapshotType _snapshotType, time_t _startTime, time_t _endTime, int _limit = 5) const;
	std::string PostWalletWithdraw(const std::string& _coin, const std::string& _withdrawOrderId, const std::string& _network, const std::string&
	                        _address, const std::string& _addressTag, const std::string& _amount, const std::string& _name, bool _transactionFeeFlag = false) const;
	std::string GetWalletDepositHistory(const std::string& _coin, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000) const;
	std::string GetWalletWithdrawtHistory(const std::string& _coin, const std::string& _withdrawOrderId, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000) const;
	std::string GetWalletDepositAddress(const std::string& _coin, const std::string& _network) const;
#pragma endregion
};