#pragma once



using namespace std;

enum class SnapshotType : int
{
	spot = 0,
	margin,
	futures
};

// TODO rework as the same architecture as on binance?
class AccountInformation
{
private:
	Logger logger;

	string SetTimeStampAndRecvWindow(unsigned short _recvWindow = 0);

public:
	AccountInformation() = default;
	AccountInformation(const Logger& _logger) : logger(_logger) {}
	void GetAccountInformationQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void GetAccountSnapshotQuery(string& _url, string& _queryString, SnapshotType _snapshotType, time_t _startTime, time_t _endTime, int _limit = 5, unsigned short _recvWindow = 0);
	void PostDisableFastWithdrawQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void PostEnableFastWithdrawQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void GetAccountStatusQuery(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void GetAccountTradingStatus(string& _url, string& _queryString, unsigned short _recvWindow = 0);
	void GetAccountAPIKeyPermissions(string& _url, string& _queryString, unsigned short _recvWindow = 0);
};