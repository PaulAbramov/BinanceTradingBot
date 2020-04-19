#pragma once

#include "root_certificates.h"

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
private:
	net::io_context& ioContext;
	ssl::context& sslContext;
	tcp::resolver tcpResolver;
	std::shared_ptr<websocket::stream<beast::ssl_stream<tcp::socket>>> ws;
	beast::flat_buffer buffer;
	std::string host;
	std::string hostAndPort;
	char const* port;
	std::string target;
	std::string text;
	tcp::endpoint tcpEndPoint;
	bool terminate = false;

	void ConnectAndReceive();

public:
	explicit WebSocketSession(net::io_context& ioc, ssl::context& ctx,
		char const* host,
		char const* port,
		char const* target);

	void Stop();
	void Start();
};