#pragma once

#include "APIHead.h"

enum class SnapshotType : int
{
	spot = 0,
	margin,
	futures
};

class WalletEndpoints : public APIHead
{
private:

public:
	WalletEndpoints() = default;
	WalletEndpoints(const Logger& _logger) : APIHead(_logger){}

	void GetWalletAllCoinsInformationQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void GetWalletDailyAccountSnapshotQuery(string& _url, string& _queryString, SnapshotType _snapshotType, time_t _startTime, time_t _endTime, int _limit = 5, unsigned short _recvWindow = 0);
	void PostWalletDisableFastWithdrawQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void PostWalletEnableFastWithdrawQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void PostWalletWithdrawQuery(string& _url, string& _queryString, string _coin, string _withdrawOrderId, string _network, string _address, string _addressTag, double _amount, string _name, bool _transactionFeeFlag = false, unsigned short _recvWindow = 0);
	void GetWalletDepositHistoryQuery(string& _url, string& _queryString, string _coin, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000, unsigned short _recvWindow = 0);
	void GetWalletWithdrawtHistoryQuery(string& _url, string& _queryString, string _coin, string _withdrawOrderId, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000, unsigned short _recvWindow = 0);
	void GetWalletDepositAddressQuery(string& _url, string& _queryString, string _coin, string _network, unsigned short _recvWindow = 0);
};