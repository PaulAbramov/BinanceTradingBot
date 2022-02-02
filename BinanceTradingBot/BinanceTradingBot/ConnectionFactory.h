#pragma once

#include "sqlapi/SQLAPI.h"

#include <memory>

class ConnectionFactory
{
protected:
	~ConnectionFactory() = default;
public:
	virtual std::shared_ptr<SAConnection> Create() = 0;
};
