#include "AccountInformation.h"

/*
* Get current account information. (SIGNED)
* 
* GET /api/v3/account
* 
* Parameters:
* Name			Type	Mandatory	Description
* recvWindow	ushort	NO			The value cannot be greater than 60000
* timestamp		long	YES
*/
void AccountInformation::GetAccountInformationQuery(string& _url, string& _queryString, unsigned short _recvWindow)
{
	_url += "/api/v3/account?";
	
	_queryString = SetTimeStampAndRecvWindow(_recvWindow);
	
	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Daily Account Snapshot (USER_DATA)
*
* GET /sapi/v1/accountSnapshot (HMAC SHA256)
*
* Weight(IP): 2400
* 
* Parameters:
* Name			Type	Mandatory	Description
* type			STRING	YES			"SPOT", "MARGIN", "FUTURES"
* startTime		LONG	NO	
* endTime		LONG	NO	
* limit			INT		NO			min 5, max 30, default 5
* recvWindow	LONG	NO			The value cannot be greater than 60000
* timestamp		LONG	YES
*/
void AccountInformation::GetAccountSnapshotQuery(string& _url, string& _queryString, SnapshotType _snapshotType, time_t _startTime, time_t _endTime, int _limit, unsigned short _recvWindow)
{
	_url += "/sapi/v1/accountSnapshot?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	// Which type we want to have
	_queryString.append("&type=");
	switch (_snapshotType)
	{
	case SnapshotType::spot:
		_queryString.append("spot");
		break;
	case SnapshotType::margin:
		_queryString.append("margin");
		break;
	case SnapshotType::futures:
		_queryString.append("futures");
		break;
	default:
		break;
	}

	// TODO have to try this
	_queryString.append("&limit=");
	_queryString.append(to_string(_limit));

	// Set the start and endTime if set
	if (_startTime != 0 && _endTime != 0) 
	{
		_queryString.append("&startTime=");
		_queryString.append(to_string(_startTime));

		_queryString.append("&endTime=");
		_queryString.append(to_string(_endTime));
	}

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}



/*
* Account Status (USER_DATA)
*
* GET /sapi/v1/account/status
* 
* Weight(IP): 1
*
* Parameters:
* Name			Type	Mandatory	Description
* recvWindow	ushort	NO			The value cannot be greater than 60000
* timestamp		long	YES
*/
void AccountInformation::GetAccountStatusQuery(string& _url, string& _queryString, unsigned short _recvWindow)
{
	_url += "/sapi/v1/account/status?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Account API Trading Status (USER_DATA)
*
* GET /sapi/v1/account/apiTradingStatus (HMAC SHA256)
*
* Weight(IP): 1
*
* Parameters:
* Name			Type	Mandatory	Description
* recvWindow	ushort	NO			The value cannot be greater than 60000
* timestamp		long	YES
*/
void AccountInformation::GetAccountTradingStatus(string& _url, string& _queryString, unsigned short _recvWindow)
{
	_url += "/sapi/v1/account/apiTradingStatus?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Get API Key Permission (USER_DATA)
*
* GET /sapi/v1/account/apiRestrictions (HMAC SHA256)
*
* Weight(IP): 1
*
* Parameters:
* Name			Type	Mandatory	Description
* recvWindow	ushort	NO			The value cannot be greater than 60000
* timestamp		long	YES
*/
void AccountInformation::GetAccountAPIKeyPermissions(string& _url, string& _queryString, unsigned short _recvWindow)
{
	_url += "/sapi/v1/account/apiRestrictions?";

	_queryString = SetTimeStampAndRecvWindow(_recvWindow);

	logger->writeInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

