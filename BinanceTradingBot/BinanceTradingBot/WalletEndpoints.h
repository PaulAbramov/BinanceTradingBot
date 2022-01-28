#pragma once

#include "APIHead.h"

enum class ESnapshotType : int
{
	SPOT = 0,
	MARGIN,
	FUTURES
};

class WalletEndpoints : public APIHead
{
private:

public:
	WalletEndpoints() = default;
	WalletEndpoints(const Logger& _logger) : APIHead(_logger){}

	void GetWalletAllCoinsInformationQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0) const;
	void GetWalletDailyAccountSnapshotQuery(string& _url, string& _queryString, ESnapshotType _snapshotType, time_t _startTime, time_t _endTime, int _limit = 5, unsigned short _recvWindow = 0) const;
	void PostWalletDisableFastWithdrawQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0) const;
	void PostWalletEnableFastWithdrawQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0) const;
	void PostWalletWithdrawQuery(string& _url, string& _queryString, const string& _coin, const string& _withdrawOrderId,
	                             const string& _network, const string& _address, const string& _addressTag, const string& _amount,
	                             const string& _name, bool _transactionFeeFlag = false, unsigned short _recvWindow = 0) const;
	void GetWalletDepositHistoryQuery(string& _url, string& _queryString, const string& _coin, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000, unsigned short _recvWindow = 0) const;
	void GetWalletWithdrawtHistoryQuery(string& _url, string& _queryString, const string& _coin, const string&
	                                    _withdrawOrderId, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000, unsigned short _recvWindow = 0) const;
	void GetWalletDepositAddressQuery(string& _url, string& _queryString, const string& _coin, const string& _network, unsigned short _recvWindow = 0) const;
};