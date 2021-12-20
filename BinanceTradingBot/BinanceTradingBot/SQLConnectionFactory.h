#pragma once
#include <string>

#include "ConnectionFactory.h"

using namespace std;

class SqlConnectionFactory final : public ConnectionFactory
{
private:
	string server;
	string username;
	string password;

public:
	SqlConnectionFactory(const string& _server, const string& _username, const string& _password);

	// Any exceptions thrown here should be caught elsewhere
	shared_ptr<SAConnection> Create() override;
};
