#include "WebSocketSession.h"

// Resolver and socket require an io_context
WebSocketSession::WebSocketSession(net::io_context& _ioc,
	ssl::context& _ctx,
	char const* _host,
	char const* _port,
	char const* _target,
	const Logger& _logger)
	: ioContext(_ioc),
	sslContext(_ctx),
	tcpResolver(ioContext),
	host(_host),
	hostAndPort(),
	port(_port),
	target(_target),
	logger(_logger)
{
}

void WebSocketSession::StartWebSocketSession()
{
	while (!terminate) 
	{
		ws = make_shared<websocket::stream<beast::ssl_stream<tcp::socket>>>(ioContext, sslContext);

		try 
		{
			ConnectAndReceive(); //initalizes websocket and polls continuously for messages until socket closed or an exception occurs
		}
		catch (beast::system_error const& _systemerror) {
			oss << "exception: " << _systemerror.code() << ", " << _systemerror.code().message();
			logger->writeErrorEntry(oss.str());
			oss.clear();
			Sleep(1000); //just wait a little while to not spam
		}

		ws.reset();
	}

	logger->writeInfoEntry("websocketsession finished.");
}

void WebSocketSession::StopWebSocketSession()
{
	terminate = true;
}

void WebSocketSession::ConnectAndReceive()
{
	logger->writeInfoEntry("Connecting to the websocket...");

	// Look up the domain name
	auto const tcpResolverResults = tcpResolver.resolve(host, port);

	// Make the connection on the IP address we get from a lookup
	tcpEndPoint = net::connect(get_lowest_layer(*ws), tcpResolverResults);

	// Set SNI Hostname (many hosts need this to handshake successfully)
	if (!SSL_set_tlsext_host_name(ws->next_layer().native_handle(), host.c_str())) 
	{
		throw beast::system_error(beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()), "Failed to set SNI Hostname");
	}

	// Update the _host string. This will provide the value of the
	// Host HTTP header during the WebSocket handshake.
	// See https://tools.ietf.org/html/rfc7230#section-5.4
	hostAndPort = host + ':' + to_string(tcpEndPoint.port());

	// Perform the SSL handshake
	ws->next_layer().handshake(ssl::stream_base::client);

	// Set a decorator to change the User-Agent of the handshake
	ws->set_option(websocket::stream_base::decorator(
		[](websocket::request_type& req)
		{
			req.set(http::field::user_agent,
				string(BOOST_BEAST_VERSION_STRING) +
				" websocket-client-coro");
		}));

	// Perform the websocket handshake
	ws->handshake(hostAndPort, target);

	//Read Messages
	try 
	{
		while (true) //expect to read continuously until a connection failure
		{
			if (!terminate) 
			{
				ws->read(buffer);

				if (buffer.size() > 0)
				{
					oss << beast::make_printable(buffer.data());
					logger->writeErrorEntry(oss.str());
					oss.clear();
				}
				buffer.clear();
			}
			else 
			{
				ws->close(boost::beast::websocket::close_code::normal);
			}
		}
	}
	catch (beast::system_error const& _systemerror) 
	{
		if (_systemerror.code() == websocket::error::closed)
		{
			logger->writeWarnEntry("socket was closed.");
		}
		else 
		{
			oss << "ConnectAndReceive() failed: " << _systemerror.code() << ", " << _systemerror.code().message() << ", " << _systemerror.what();
			logger->writeErrorEntry(oss.str());
			oss.clear();
		}
	}
	catch (exception& _exception) 
	{
		oss << "ConnectAndReceive() failed: " << _exception.what();
		logger->writeErrorEntry(oss.str());
		oss.clear();
	}
}