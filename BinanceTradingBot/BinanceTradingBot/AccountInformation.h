#pragma once

#define BINANCE_HOST "https://api.binance.com"
/*
 * API Endpoints:
 * https://api.binance.com
 * https://api1.binance.com
 * https://api2.binance.com
 * https://api3.binance.com
 */

#include <chrono>

#include "LoggingFacility.h"
#include "root_certificates.h"

using namespace std;

class AccountInformation
{
private:
	Logger logger;

public:
	AccountInformation(const Logger& _logger) : logger(_logger) {}
	void GetAccountInformation(string _secretKey, string _apiKey, unsigned short _recvWindow);
};