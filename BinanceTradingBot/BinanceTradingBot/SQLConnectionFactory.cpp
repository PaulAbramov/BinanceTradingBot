#include "SQLConnectionFactory.h"

SqlConnectionFactory::SqlConnectionFactory(const string& _server, const string& _username, const string& _password)
{
	server = _server;
	username = _username;
	password = _password;
}

shared_ptr<SAConnection> SqlConnectionFactory::Create()
{
	// Create the connection
	const shared_ptr<SAConnection> connection(new SAConnection());

	// Connect
	connection->Connect(_TSA(server.c_str()), _TSA(username.c_str()), _TSA(password.c_str()), SA_SQLServer_Client);

	return std::static_pointer_cast<SAConnection>(connection);
}