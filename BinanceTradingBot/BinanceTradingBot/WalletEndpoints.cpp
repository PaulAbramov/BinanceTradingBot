#include "WalletEndpoints.h"

/*
* All Coins' Information (USER_DATA)
* 
* Get information of coins (available for deposit and withdraw) for user.
* 
* GET /sapi/v1/capital/config/getall (HMAC SHA256)
* 
* Weight(IP): 10
* 
* Parameters:
* Name			Type			Mandatory	Description
* recvWindow	UNSGINED SHORT	NO			The value cannot be greater than 60000
* timestamp		UNSGINED SHORT	YES
*/
void WalletEndpoints::GetWalletAllCoinsInformationQuery(string& _url, string& _queryString, const unsigned short _recvWindow) const
{
	_url += "/sapi/v1/capital/config/getall?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Daily Account Snapshot (USER_DATA)
*
* GET /sapi/v1/accountSnapshot (HMAC SHA256)
*
* Weight(IP): 2400
*
* Parameters:
* Name			Type			Mandatory	Description
* type			STRING			YES			"SPOT", "MARGIN", "FUTURES"
* startTime		UNSGINED SHORT	NO
* endTime		UNSGINED SHORT	NO	
* limit			INT				NO			min 5, max 30, default 5
* recvWindow	UNSGINED SHORT	NO	
* timestamp		UNSGINED SHORT	YES
*/
void WalletEndpoints::GetWalletDailyAccountSnapshotQuery(string& _url, string& _queryString, const ESnapshotType _snapshotType, const time_t _startTime, const time_t _endTime, const int _limit, const unsigned short _recvWindow) const
{
	_url += "/sapi/v1/accountSnapshot?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	// Which type we want to have
	_queryString.append("&type=");
	switch (_snapshotType)
	{
	case ESnapshotType::SPOT:
		_queryString.append("SPOT");
		break;
	case ESnapshotType::MARGIN:
		_queryString.append("MARGIN");
		break;
	case ESnapshotType::FUTURES:
		_queryString.append("FUTURES");
		break;
	default:
		break;
	}

	if(_limit >= 5 && _limit <= 30)
	{
		_queryString.append("&limit=");
		_queryString.append(to_string(_limit));
	}
	
	// Set the start and endTime if set
	if (_startTime != 0 && _endTime != 0)
	{
		_queryString.append("&startTime=");
		_queryString.append(to_string(_startTime));

		_queryString.append("&endTime=");
		_queryString.append(to_string(_endTime));
	}

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Disable Fast Withdraw Switch (USER_DATA)
*
* POST /sapi/v1/account/disableFastWithdrawSwitch (HMAC SHA256)
*
* Weight(IP): 1
*
* Parameters:
* Name			Type			Mandatory	Description
* recvWindow	ushort			NO			The value cannot be greater than 60000
* timestamp		UNSGINED SHORT	YES
*/
void WalletEndpoints::PostWalletDisableFastWithdrawQuery(string& _url, string& _queryString, const unsigned short _recvWindow) const
{
	_url += "/sapi/v1/account/disableFastWithdrawSwitch?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Enable Fast Withdraw Switch (USER_DATA)
*
* POST /sapi/v1/account/enableFastWithdrawSwitch (HMAC SHA256)
*
* Weight(IP): 1
*
* Parameters:
* Name			Type			Mandatory	Description
* recvWindow	ushort			NO			The value cannot be greater than 60000
* timestamp		UNSGINED SHORT	YES
*/
void WalletEndpoints::PostWalletEnableFastWithdrawQuery(string& _url, string& _queryString, const unsigned short _recvWindow) const
{
	_url += "/sapi/v1/account/enableFastWithdrawSwitch?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Withdraw(USER_DATA)
*
* Submit a withdraw request.
* 
* POST /sapi/v1/capital/withdraw/apply (HMAC SHA256)
*
* Weight(IP): 1
*
* Parameters:
* Name					Type			Mandatory	Description
* coin					STRING			YES	
* withdrawOrderId		STRING			NO			client id for withdraw
* network				STRING			NO	
* address				STRING			YES	
* addressTag			STRING			NO			Secondary address identifier for coins like XRP,XMR etc.
* amount				DECIMAL			YES	
* transactionFeeFlag	BOOLEAN			NO			When making internal transfer, true for returning the fee to the destination account; false for returning the fee back to the departure account. Default false.
* name					STRING			NO			Description of the address. Space in name should be encoded into %20.
* recvWindow			ushort			NO			The value cannot be greater than 60000
* timestamp				UNSGINED SHORT	YES
* 
* If network not send, return with default network of the coin.
* You can get network and isDefault in networkList of a coin in the response of Get /sapi/v1/capital/config/getall (HMAC SHA256).
*/
void WalletEndpoints::PostWalletWithdrawQuery(string& _url, string& _queryString, const string& _coin, const string& _withdrawOrderId, const string& _network, const string& _address, const string& _addressTag, const double _amount, const string& _name, const bool _transactionFeeFlag, const unsigned short _recvWindow ) const
{
	if (_coin.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'coin'");
		return;
	}
	if (_address.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'address'");
		return;
	}
	if (_amount == 0)
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'amount'");
		return;
	}

	_url += "/sapi/v1/capital/withdraw/apply?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	_queryString.append("&coin=");
	_queryString.append(_coin);

	if (!_withdrawOrderId.empty())
	{
		_queryString.append("&withdrawOrderId=");
		_queryString.append(_withdrawOrderId);
	}

	if (!_network.empty())
	{
		_queryString.append("&network=");
		_queryString.append(_network);
	}

	_queryString.append("&address=");
	_queryString.append(_address);

	if (!_addressTag.empty())
	{
		_queryString.append("&addressTag=");
		_queryString.append(_addressTag);
	}

	_queryString.append("&amount=");
	_queryString.append(to_string(_amount));

	// TODO test this
	_queryString.append("&transactionFeeFlag=");
	_queryString.append(to_string(_transactionFeeFlag));

	if (!_name.empty())
	{
		_queryString.append("&name=");
		_queryString.append(_name);
	}

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Deposit History(supporting network) (USER_DATA)
*
* Fetch deposit history.
*
* GET /sapi/v1/capital/deposit/hisrec (HMAC SHA256)
*
* Weight(IP): 1
*
* Parameters:
* Name					Type			Mandatory	Description
* coin					STRING			NO	
* status				INT				NO			0(0:pending,6: credited but cannot withdraw, 1:success)
* startTime				UNSGINED SHORT	NO			Default: 90 days from current timestamp
* endTime				UNSGINED SHORT	NO			Default: present timestamp
* offset				INT				NO			Default:0
* limit					INT				NO			Default:1000, Max:1000
* recvWindow			UNSGINED SHORT	NO	
* timestamp				UNSGINED SHORT	YES
*
* Please notice the default startTime and endTime to make sure that time interval is within 0-90 days.
* If both startTime and endTime are sent, time between startTime and endTime must be less than 90 days.
*/
void WalletEndpoints::GetWalletDepositHistoryQuery(string& _url, string& _queryString, const string& _coin, const int _status, const time_t _startTime, const time_t _endTime, const int _offset, const int _limit, const unsigned short _recvWindow) const
{
	_url += "/sapi/v1/capital/deposit/hisrec?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	if (!_coin.empty())
	{
		_queryString.append("&coin=");
		_queryString.append(_coin);
	}

	
	_queryString.append("&status=");
	_queryString.append(to_string(_status));

	if (_startTime != 0 && _endTime != 0)
	{
		_queryString.append("&startTime=");
		_queryString.append(to_string(_startTime));

		_queryString.append("&endTime=");
		_queryString.append(to_string(_endTime));
	}

	if(_offset > 0)
	{
		_queryString.append("&offset=");
		_queryString.append(to_string(_offset));
	}

	if(_limit > 0)
	{
		_queryString.append("&limit=");
		_queryString.append(to_string(_limit));
	}

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Withdraw History (supporting network) (USER_DATA)
*
* Fetch withdraw history.
*
* GET /sapi/v1/capital/withdraw/history (HMAC SHA256)
*
* Weight(IP): 1
*
* Parameters:
* Name					Type			Mandatory	Description
* coin					STRING			NO
* withdrawOrderId		STRING			NO	
* status				INT				NO			0(0:Email Sent,1:Cancelled 2:Awaiting Approval 3:Rejected 4:Processing 5:Failure 6:Completed)
* startTime				UNSGINED SHORT	NO			Default: 90 days from current timestamp
* endTime				UNSGINED SHORT	NO			Default: present timestamp
* offset				INT				NO			Default:0
* limit					INT				NO			Default:1000, Max:1000
* recvWindow			UNSGINED SHORT	NO
* timestamp				UNSGINED SHORT	YES
* 
* network may not be in the response for old withdraw.
* Please notice the default startTime and endTime to make sure that time interval is within 0-90 days.
* If both startTime and endTime are sent, time between startTime and endTime must be less than 90 days.
*/
void WalletEndpoints::GetWalletWithdrawtHistoryQuery(string& _url, string& _queryString, const string& _coin, const string& _withdrawOrderId, const int _status, const time_t _startTime, const time_t _endTime, const int _offset, const int _limit, const unsigned short _recvWindow) const
{
	_url += "/sapi/v1/capital/withdraw/history?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	if (!_coin.empty())
	{
		_queryString.append("&coin=");
		_queryString.append(_coin);
	}

	if (!_withdrawOrderId.empty())
	{
		_queryString.append("&withdrawOrderId=");
		_queryString.append(_withdrawOrderId);
	}

	_queryString.append("&status=");
	_queryString.append(to_string(_status));

	if (_startTime != 0 && _endTime != 0)
	{
		_queryString.append("&startTime=");
		_queryString.append(to_string(_startTime));

		_queryString.append("&endTime=");
		_queryString.append(to_string(_endTime));
	}

	if (_offset > 0)
	{
		_queryString.append("&offset=");
		_queryString.append(to_string(_offset));
	}

	if (_limit > 0)
	{
		_queryString.append("&limit=");
		_queryString.append(to_string(_limit));
	}

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Deposit Address (supporting network) (USER_DATA)
*
* Fetch deposit address with network.
*
* GET /sapi/v1/capital/deposit/address (HMAC SHA256)
*
* Weight(IP): 10
*
* Parameters:
* Name					Type			Mandatory	Description
* coin					STRING			YES	
* network				STRING			NO	
* recvWindow			UNSGINED SHORT	NO	
* timestamp				UNSGINED SHORT	YES
*
* If network is not send, return with default network of the coin.
* You can get network and isDefault in networkList in the response of Get /sapi/v1/capital/config/getall (HMAC SHA256).
*/
void WalletEndpoints::GetWalletDepositAddressQuery(string& _url, string& _queryString, const string& _coin, const string& _network, const unsigned short _recvWindow) const
{
	if (_coin.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'coin'");
		return;
	}

	_url += "/sapi/v1/capital/deposit/address?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	_queryString.append("&coin=");
	_queryString.append(_coin);

	if (!_network.empty())
	{
		_queryString.append("&network=");
		_queryString.append(_network);
	}

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}
