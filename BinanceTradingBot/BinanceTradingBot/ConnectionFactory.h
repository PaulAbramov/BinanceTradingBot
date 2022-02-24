#pragma once

#include "sqlapi/SQLAPI.h"

#include <memory>

#include "FileLogger.h"

class ConnectionFactory
{
protected:
	~ConnectionFactory() = default;
public:
	virtual std::shared_ptr<SAConnection> Create(eSAClient _client) = 0;
};
