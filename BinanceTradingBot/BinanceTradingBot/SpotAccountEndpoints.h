#pragma once

#include "APIHead.h"
#include "APIEnums.h"

class SpotAccountEndpoints : public APIHead
{
private:
	boost::unordered_map<ESide, const char*> eSidesToString = map_list_of
	(ESide::NONE, "NONE")
	(ESide::BUY, "BUY")
	(ESide::SELL, "SELL");

	boost::unordered_map<EOrderType, const char*> eOrderTypeToString = map_list_of
	(EOrderType::NONE, "NONE")
	(EOrderType::LIMIT, "LIMIT")
	(EOrderType::MARKET, "MARKET")
	(EOrderType::STOPLOSS, "STOP_LOSS")
	(EOrderType::STOPLOSSLIMIT, "STOP_LOSS_LIMIT")
	(EOrderType::TAKEPROFIT, "TAKE_PROFIT")
	(EOrderType::TAKEPROFITLIMIT, "TAKE_PROFIT_LIMIT")
	(EOrderType::LIMITMAKER, "LIMIT_MAKER");

	boost::unordered_map<ENewOrderResponseType, const char*> eNewOrderResponseTypeToString = map_list_of
	(ENewOrderResponseType::ACK, "ACK")
	(ENewOrderResponseType::NONE, "NONE")
	(ENewOrderResponseType::RESULT, "RESULT")
	(ENewOrderResponseType::FULL, "FULL");

	boost::unordered_map<ETimeInForce, const char*> eTimeInForceToString = map_list_of
	(ETimeInForce::NONE, "NONE")
	(ETimeInForce::GTC, "GTC")
	(ETimeInForce::IOC, "IOC")
	(ETimeInForce::FOK, "FOK");

public:
	SpotAccountEndpoints() = default;
	SpotAccountEndpoints(const Logger& _logger) : APIHead(_logger) {}

	void PostSpotAccountNewOrderQuery(string& _url, string& _queryString, const string& _symbol, ETimeInForce _timeInForce, double _quantity, double _quoteOrderQuantity, double _price,
	                                  const string& _newClientOrderId, double _stopPrice, double _icebergQuantity, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE, EOrderType _orderType = EOrderType::NONE, unsigned short _recvWindow = 0);
	void DeleteSpotAccountCancelOrderQuery(string& _url, string& _queryString, const string& _symbol, unsigned short _orderId,
	                                       const string& _originalClientOrderId, const string& _newClientOrderId, unsigned short _recvWindow = 0) const;
	void DeleteSpotAccountCancelAllOpenOrdersOnSymbolQuery(string& _url, string& _queryString, const string& _symbol, unsigned short _recvWindow = 0) const;
	void GetSpotAccountQueryOrderQuery(string& _url, string& _queryString, const string& _symbol, unsigned short _orderId,
	                                   const string& _originalClientOrderId, unsigned short _recvWindow = 0) const;
	void GetSpotAccountCurrentOpenOrdersQuery(string& _url, string& _queryString, const string& _symbol, unsigned short _recvWindow = 0) const;
	void GetSpotAccountAllOrdersQuery(string& _url, string& _queryString, const string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, int _limit = 500, unsigned short _recvWindow = 0) const;
	void PostSpotAccountNewOcoOrderQuery(string& _url, string& _queryString, const string& _symbol, const string&
	                                     _listClientOrderId, double _quantity, const string& _limitClientOrderId, double _price, double _limitIcebergQuantity,
	                                     const string& _stopClientOrderId, double _stopPrice, double _stopLimitPrice, double _stopIcebergQuantity, ETimeInForce _stopLimitTimeInForce, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE, unsigned short _recvWindow = 0);
	void DeleteSpotAccountCancelOcoQuery(string& _url, string& _queryString, const string& _symbol, unsigned short _orderListId,
	                                     const string& _listClientOrderId, const string& _newClientOrderId, unsigned short _recvWindow = 0) const;
	void GetSpotAccountQueryOcoQuery(string& _url, string& _queryString, unsigned short _orderListId, const string&
	                                 _originalClientOrderId, unsigned short _recvWindow = 0) const;
	void GetSpotAccountQueryAllOcoQuery(string& _url, string& _queryString, unsigned short _fromId, time_t _startTime, time_t _endTime, int _limit = 500, unsigned short _recvWindow = 0) const;
	void GetSpotAccountQueryOpenOcoQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0) const;
	void GetSpotAccountInformationQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0) const;
	void GetSpotAccountTradeListQuery(string& _url, string& _queryString, const string& _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, unsigned short _fromId, int _limit = 500, unsigned short _recvWindow = 0) const;
	void GetSpotAccountCurrentOrderCountUsageQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0) const;
};