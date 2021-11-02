#pragma once

#define BINANCE_HOST "https://api.binance.com"

/*
 * API Endpoints:
 * https://api.binance.com
 * https://api1.binance.com
 * https://api2.binance.com
 * https://api3.binance.com
 */

#define CURL_STATICLIB 
#include <curl/curl.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "LoggingFacility.h"

using namespace std;

class ApiRequestManager
{
private:
	CURL* curl;
	Logger logger;
	string apiKey;
	string secretKey;
	//AccountInformation accountInformation;

	void SetSecretAndApiKeyInUrl(string& _url, vector <string>& _extraHttpHeader, string _querystring);
	void CurlAPIWithHeader(string& _url, string& _strResult, vector <string>& _extraHttpHeader, string& _postData, string& _action, string _queryString);
	string HmacSha256(const string _secretKey, const string _querystring);
	string B2AHex(char* _byteArray, int _n);
	static size_t WebRequestCallback(void* _content, size_t _size, size_t _nmemb, string* _buffer);


public:
	ApiRequestManager(const Logger& _logger, string _apiKey, string _secretKey);
	void CleanUpApiRequestManager();
	void GetAccountInformation();
};
