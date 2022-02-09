#include "SQLConnectionFactory.h"

#include <iostream>

using namespace std;

SqlConnectionFactory::SqlConnectionFactory(const string& _server, const string& _username, const string& _password)
{
	server = _server;
	username = _username;
	password = _password;
}

shared_ptr<SAConnection> SqlConnectionFactory::Create(eSAClient _client)
{
	// Create the connection
	const shared_ptr<SAConnection> connection(new SAConnection());

	try
	{
		// Connect
		connection->Connect(_TSA(server.c_str()), _TSA(username.c_str()), _TSA(password.c_str()), _client);
	}
	catch (SAException& exception)
	{
		cout << "connection exception." << endl;
		cout << exception.ErrText().GetMultiByteChars() << endl;
	}
	
	return static_pointer_cast<SAConnection>(connection);
}
