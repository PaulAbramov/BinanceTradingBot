#pragma once

#include "ApiRequestManager.h"
#include "Config.h"
#include "ConnectionFactory.h"
#include "ConnectionPool.h"
#include "SQLConnectionFactory.h"
#include "LoggingFacility.h"
#include "WebSocketCollection.h"

using namespace std;
using namespace std::chrono;

class Bot
{
private:
	Logger logger;
	Config config;
	vector<string> symbols{ "BNBBUSD" };
	ApiRequestManager manager;

	bool HandleRequest(const shared_ptr<ConnectionPool<SAConnection>>& _pool, const string& _answer) const;
	string LongToString(const int64_t _longDate) const;

	void ManualRun();
	void AutomaticRun() const;

public:
	Bot(const Logger& _logger, const Config& _config);
	bool Run() const;
};