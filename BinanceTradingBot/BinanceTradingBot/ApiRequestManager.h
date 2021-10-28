#pragma once

#define CURL_STATICLIB 
#include <curl/curl.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "LoggingFacility.h"

using namespace std;

class ApiRequestManager
{
private:
	Logger logger;

public:
	ApiRequestManager(const Logger& _logger) : logger(_logger) {}
	void CurlAPIWithHeader(string& _url, string& _result_json, vector <string>& _extra_http_header, string& _post_data, string& _action);

	static size_t WebRequestCallback(void* _content, size_t _size, size_t _nmemb, string* _buffer);
};
