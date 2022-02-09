#pragma once

#include "APIHead.h"
#include "APIEnums.h"

class SpotAccountEndpoints : public APIHead
{
private:
	std::map<ESide, const char*> eSidesToString
	{
		{ ESide::NONE, "NONE" },
		{ ESide::BUY, "BUY" },
		{ ESide::SELL, "SELL" }
	};

	std::map<EOrderType, const char*> eOrderTypeToString
	{
		{ EOrderType::NONE, "NONE" },
		{ EOrderType::LIMIT, "LIMIT" },
		{ EOrderType::MARKET, "MARKET" },
		{ EOrderType::STOPLOSS, "STOP_LOSS" },
		{ EOrderType::STOPLOSSLIMIT, "STOP_LOSS_LIMIT" },
		{ EOrderType::TAKEPROFIT, "TAKE_PROFIT" },
		{ EOrderType::TAKEPROFITLIMIT, "TAKE_PROFIT_LIMIT" },
		{ EOrderType::LIMITMAKER, "LIMIT_MAKER" }
	};

	std::map<ENewOrderResponseType, const char*> eNewOrderResponseTypeToString
	{
		{ ENewOrderResponseType::ACK, "ACK" },
		{ ENewOrderResponseType::NONE, "NONE" },
		{ ENewOrderResponseType::RESULT, "RESULT" },
		{ ENewOrderResponseType::FULL, "FULL" }
	};

	std::map<ETimeInForce, const char*> eTimeInForceToString
	{
		{ ETimeInForce::NONE, "NONE" },
		{ ETimeInForce::GTC, "GTC" },
		{ ETimeInForce::IOC, "IOC" },
		{ ETimeInForce::FOK, "FOK" }
	};

public:
	SpotAccountEndpoints() = default;
	SpotAccountEndpoints(const Logger& _logger) : APIHead(_logger) {}

	void PostSpotAccountNewOrderQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, ETimeInForce _timeInForce, const std::string& _quantity, const std::string& _quoteOrderQuantity, const std::string& _price,
	                                  const std::string& _newClientOrderId, const std::string& _stopPrice, const std::string& _icebergQuantity, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE, EOrderType _orderType = EOrderType::NONE, unsigned short _recvWindow = 0) const;
	void DeleteSpotAccountCancelOrderQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, unsigned short _orderId,
	                                       const std::string& _originalClientOrderId, const std::string& _newClientOrderId, unsigned short _recvWindow = 0) const;
	void DeleteSpotAccountCancelAllOpenOrdersOnSymbolQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, unsigned short _recvWindow = 0) const;
	void GetSpotAccountQueryOrderQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, unsigned short _orderId,
	                                   const std::string& _originalClientOrderId, unsigned short _recvWindow = 0) const;
	void GetSpotAccountCurrentOpenOrdersQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, unsigned short _recvWindow = 0) const;
	void GetSpotAccountAllOrdersQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, int _limit = 500, unsigned short _recvWindow = 0) const;
	void PostSpotAccountNewOcoOrderQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, const std::string&
	                                     _listClientOrderId, const std::string& _quantity, const std::string& _limitClientOrderId, const std::string& _price, const std::string& _limitIcebergQuantity,
	                                     const std::string& _stopClientOrderId, const std::string& _stopPrice, const std::string& _stopLimitPrice, const std::string& _stopIcebergQuantity, ETimeInForce _stopLimitTimeInForce, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE, unsigned short _recvWindow = 0) const;
	void DeleteSpotAccountCancelOcoQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, unsigned short _orderListId,
	                                     const std::string& _listClientOrderId, const std::string& _newClientOrderId, unsigned short _recvWindow = 0) const;
	void GetSpotAccountQueryOcoQuery(std::string& _url, std::string& _queryString, unsigned short _orderListId, const std::string&
	                                 _originalClientOrderId, unsigned short _recvWindow = 0) const;
	void GetSpotAccountQueryAllOcoQuery(std::string& _url, std::string& _queryString, unsigned short _fromId, time_t _startTime, time_t _endTime, int _limit = 500, unsigned short _recvWindow = 0) const;
	void GetSpotAccountQueryOpenOcoQuery(std::string& _url, std::string& _queryString, unsigned short _recvWindow = 0) const;
	void GetSpotAccountInformationQuery(std::string& _url, std::string& _queryString, unsigned short _recvWindow = 0) const;
	void GetSpotAccountTradeListQuery(std::string& _url, std::string& _queryString, const std::string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, unsigned short _fromId, int _limit = 500, unsigned short _recvWindow = 0) const;
	void GetSpotAccountCurrentOrderCountUsageQuery(std::string& _url, std::string& _queryString, unsigned short _recvWindow = 0) const;
};