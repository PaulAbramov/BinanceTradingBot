#include "APIHead.h"

/*
* Set timestamp and recvWindow in querystring
*/
string APIHead::SetTimeStampAndRecvWindow(unsigned short _recvWindow)
{
	chrono::milliseconds ms = duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

	// When the request got created
	string querystring("timestamp=");
	querystring.append(to_string(ms.count()));

	// For how long the request is valid
	if (_recvWindow > 0)
	{
		querystring.append("&recvWindow=");
		querystring.append(to_string(_recvWindow));
	}

	return querystring;
}