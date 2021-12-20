#pragma once
#include <memory>
#include <SQLAPI.h>

class ConnectionFactory
{
protected:
	~ConnectionFactory() = default;
public:
	virtual std::shared_ptr<SAConnection> Create() = 0;
};
