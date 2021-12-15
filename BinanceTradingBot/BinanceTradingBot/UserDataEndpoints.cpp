#include "UserDataEndpoints.h"

/*
* Create a ListenKey (USER_STREAM)
*
* Start a new user data stream. The stream will close after 60 minutes unless a keepalive is sent. If the account has an active listenKey, that listenKey will be returned and its validity will be extended for 60 minutes.
*
* POST /api/v3/userDataStream
*
* Weight: 1
*/
void UserDataEndpoints::PostUserDataCreateListenKeySpot(string& _url)
{
	_url += "/api/v3/userDataStream";
}

/*
* Ping/Keep-alive a ListenKey (USER_STREAM)
*
* Keepalive a user data stream to prevent a time out. User data streams will close after 60 minutes. It's recommended to send a ping about every 30 minutes.
*
* PUT /api/v3/userDataStream
*
* Weight: 1
*
* Parameters:
* Name				Type			Mandatory	Description
* listenKey			STRING			YES
*/
void UserDataEndpoints::PutUserDataKeepAliveListenKeySpot(string& _url, string& _queryString, const string _listenKey) const
{
	if (_listenKey.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'listenKey'");
		return;
	}

	_url += "/api/v3/userDataStream?";

	_queryString.append("&listenKey=");
	_queryString.append(_listenKey);

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}

/*
* Close a ListenKey (USER_STREAM)
*
* Close out a user data stream.
*
* DELETE /api/v3/userDataStream
*
* Weight: 1
*
* Parameters:
* Name				Type			Mandatory	Description
* listenKey			STRING			YES
*/
void UserDataEndpoints::DeleteUserDataCloseListenKeySpot(string& _url, string& _queryString, const string _listenKey) const
{
	if (_listenKey.empty())
	{
		logger->WriteWarnEntry("Did not set mandatory parameter 'listenKey'");
		return;
	}

	_url += "/api/v3/userDataStream?";

	_queryString.append("&listenKey=");
	_queryString.append(_listenKey);

	logger->WriteInfoEntry("Put together following string: " + _url + " and querystring: " + _queryString);
}
