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
	ApiRequestManager(const Logger& logger) : logger(logger) {}
	void CurlAPIWithHeader(string& url, string& result_json, vector <string>& extra_http_header, string& post_data, string& action);

	static size_t my_write(void* content, size_t size, size_t nmemb, string* buffer);
};
