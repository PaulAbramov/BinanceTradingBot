#pragma once

#include "APIHead.h"
#include "APIEnums.h"

class SpotAccountEndpoints : public APIHead
{
private:
	const boost::unordered_map<ESide, const char*> eSidesToString = map_list_of
	(ESide::NONE, "NONE")
	(ESide::BUY, "BUY")
	(ESide::SELL, "SELL");

	const boost::unordered_map<EOrderType, const char*> eOrderTypeToString = map_list_of
	(EOrderType::NONE, "NONE")
	(EOrderType::LIMIT, "LIMIT")
	(EOrderType::MARKET, "MARKET")
	(EOrderType::STOPLOSS, "STOP_LOSS")
	(EOrderType::STOPLOSSLIMIT, "STOP_LOSS_LIMIT")
	(EOrderType::TAKEPROFIT, "TAKE_PROFIT")
	(EOrderType::TAKEPROFITLIMIT, "TAKE_PROFIT_LIMIT")
	(EOrderType::LIMITMAKER, "LIMIT_MAKER");

	const boost::unordered_map<ENewOrderResponseType, const char*> eNewOrderResponseTypeToString = map_list_of
	(ENewOrderResponseType::ACK, "ACK")
	(ENewOrderResponseType::NONE, "NONE")
	(ENewOrderResponseType::RESULT, "RESULT")
	(ENewOrderResponseType::FULL, "FULL");

	const boost::unordered_map<ETimeInForce, const char*> eTimeInForceToString = map_list_of
	(ETimeInForce::NONE, "NONE")
	(ETimeInForce::GTC, "GTC")
	(ETimeInForce::IOC, "IOC")
	(ETimeInForce::FOK, "FOK");

public:
	SpotAccountEndpoints() = default;
	SpotAccountEndpoints(const Logger& _logger) : APIHead(_logger) {}

	void PostSpotAccountNewOrderQuery(string& _url, string& _queryString, string _symbol, ETimeInForce _timeInForce, double _quantity, double _quoteOrderQuantity, double _price, string _newClientOrderId, double _stopPrice, double _icebergQuantity, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE, EOrderType _orderType = EOrderType::NONE, unsigned short _recvWindow = 0);
	void DeleteSpotAccountCancelOrderQuery(string& _url, string& _queryString, string _symbol, unsigned short _orderId, string _originalClientOrderId, string _newClientOrderId, unsigned short _recvWindow = 0);
	void DeleteSpotAccountCancelAllOpenOrdersOnSymbolQuery(string& _url, string& _queryString, string _symbol, unsigned short _recvWindow = 0);
	void GetSpotAccountQueryOrderQuery(string& _url, string& _queryString, string _symbol, unsigned short _orderId, string _originalClientOrderId, unsigned short _recvWindow = 0);
	void GetSpotAccountCurrentOpenOrdersQuery(string& _url, string& _queryString, string _symbol, unsigned short _recvWindow = 0);
	void GetSpotAccountAllOrdersQuery(string& _url, string& _queryString, string _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, int _limit = 500, unsigned short _recvWindow = 0);
	void PostSpotAccountNewOcoOrderQuery(string& _url, string& _queryString, string _symbol, string _listClientOrderId, double _quantity, string _limitClientOrderId, double _price, double _limitIcebergQuantity, string _stopClientOrderId, double _stopPrice, double _stopLimitPrice, double _stopIcebergQuantity, ETimeInForce _stopLimitTimeInForce, ENewOrderResponseType _newOrderResponseType = ENewOrderResponseType::FULL, ESide _side = ESide::NONE, unsigned short _recvWindow = 0);
	void DeleteSpotAccountCancelOcoQuery(string& _url, string& _queryString, string _symbol, unsigned short _orderListId, string _listClientOrderId, string _newClientOrderId, unsigned short _recvWindow = 0);
	void GetSpotAccountQueryOcoQuery(string& _url, string& _queryString, unsigned short _orderListId, string _originalClientOrderId, unsigned short _recvWindow = 0);
	void GetSpotAccountQueryAllOcoQuery(string& _url, string& _queryString, unsigned short _fromId, time_t _startTime, time_t _endTime, int _limit = 500, unsigned short _recvWindow = 0);
	void GetSpotAccountQueryOpenOcoQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void GetSpotAccountInformationQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void GetSpotAccountTradeListQuery(string& _url, string& _queryString, string _symbol, unsigned short _orderId, time_t _startTime, time_t _endTime, unsigned short _fromId, int _limit = 500, unsigned short _recvWindow = 0);
	void GetSpotAccountCurrentOrderCountUsageQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
};