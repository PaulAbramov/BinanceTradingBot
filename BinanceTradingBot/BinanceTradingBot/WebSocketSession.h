#pragma once

#include "root_certificates.h"
#include "LoggingFacility.h"

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using namespace std;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
private:
	net::io_context& ioContext;
	ssl::context& sslContext;
	tcp::resolver tcpResolver;
	shared_ptr<websocket::stream<beast::ssl_stream<tcp::socket>>> ws;
	beast::flat_buffer buffer;
	string host;
	string hostAndPort;
	char const* port;
	string target;
	string text;
	tcp::endpoint tcpEndPoint;
	bool terminate = false;
	Logger logger;
	ostringstream oss;

	void ConnectAndReceive();

public:
	explicit WebSocketSession(net::io_context& _ioc, ssl::context& _ctx,
		char const* _host,
		char const* _port,
		char const* _target,
		const Logger& _logger);

	void Stop();
	void Start();
};