#pragma once

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

#include "root_certificates.h"
#include "LoggingFacility.h"

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
	ssl::context sslContext;
	tcp::resolver tcpResolver;
	websocket::stream<beast::ssl_stream<tcp::socket>> ws;
	beast::multi_buffer buffer;
	string host;
	string hostAndPort;
	string port;
	tcp::endpoint tcpEndPoint;
	bool terminate = false;
	Logger logger;
	ostringstream oss;

	using holder_type = std::shared_ptr<void>;

	void ConnectWebSocketSession(string _target, const boost::asio::ip::tcp::resolver::results_type& _resultsType, std::function<bool(const string&)> _callbackFunction, holder_type _holder);
	void OnConnectedWebSocketSession(string _target, std::function<bool(const string&)> _callbackFunction, holder_type _holder);
	void OnAsyncSSLHandshake(const string& _target, std::function<bool(const string&)> _callbackFunction, holder_type _holder);
	void StartReadWebSocketSession(boost::system::error_code _errorCode, std::function<bool(const string&)> _callbackFunction, holder_type _holder);
	void OnReadWebSocketSession(boost::system::error_code _errorCode, std::size_t _readData, std::function<bool(const string&)> _callbackFunction, holder_type _holder);

public:
	explicit WebSocketSession(net::io_context& _ioc,
		string _host,
		string _port,
		const Logger& _logger);

	void StopWebSocketSession();
	void AsyncStopWebSocketSession(holder_type _holder);
	void StartWebSocketSession(string _target, std::function<bool(const string&)> _callbackFunction, holder_type _holder);
};