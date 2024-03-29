#pragma once

#include <string>

#include "ConnectionFactory.h"

class SqlConnectionFactory final : public ConnectionFactory
{
private:
	std::string server;
	std::string username;
	std::string password;

public:
	SqlConnectionFactory(const std::string& _server, const std::string& _username, const std::string& _password);

	// Any exceptions thrown here should be caught elsewhere
	std::shared_ptr<SAConnection> Create(eSAClient _client) override;
};
