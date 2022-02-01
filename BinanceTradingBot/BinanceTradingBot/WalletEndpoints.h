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

	void GetWalletAllCoinsInformationQuery(std::string& _url, std::string& _queryString, unsigned short _recvWindow = 0) const;
	void GetWalletDailyAccountSnapshotQuery(std::string& _url, std::string& _queryString, ESnapshotType _snapshotType, time_t _startTime, time_t _endTime, int _limit = 5, unsigned short _recvWindow = 0) const;
	void PostWalletDisableFastWithdrawQuery(std::string& _url, std::string& _queryString, unsigned short _recvWindow = 0) const;
	void PostWalletEnableFastWithdrawQuery(std::string& _url, std::string& _queryString, unsigned short _recvWindow = 0) const;
	void PostWalletWithdrawQuery(std::string& _url, std::string& _queryString, const std::string& _coin, const std::string& _withdrawOrderId,
	                             const std::string& _network, const std::string& _address, const std::string& _addressTag, const std::string& _amount,
	                             const std::string& _name, bool _transactionFeeFlag = false, unsigned short _recvWindow = 0) const;
	void GetWalletDepositHistoryQuery(std::string& _url, std::string& _queryString, const std::string& _coin, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000, unsigned short _recvWindow = 0) const;
	void GetWalletWithdrawtHistoryQuery(std::string& _url, std::string& _queryString, const std::string& _coin, const std::string&
	                                    _withdrawOrderId, int _status, time_t _startTime, time_t _endTime, int _offset, int _limit = 1000, unsigned short _recvWindow = 0) const;
	void GetWalletDepositAddressQuery(std::string& _url, std::string& _queryString, const std::string& _coin, const std::string& _network, unsigned short _recvWindow = 0) const;
};