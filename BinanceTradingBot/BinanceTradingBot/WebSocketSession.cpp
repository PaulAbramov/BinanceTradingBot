#include "WebSocketSession.h"

// Resolver and socket require an io_context
WebSocketSession::WebSocketSession(net::io_context& ioc,
	ssl::context& ctx,
	char const* host,
	char const* port,
	char const* target)
	: ioContext(ioc),
	sslContext(ctx),
	tcpResolver(ioContext),
	host(host),
	hostAndPort(),
	port(port),
	target(target)
{
}

void WebSocketSession::Start()
{
	while (!terminate) {

		ws = std::make_shared<websocket::stream<beast::ssl_stream<tcp::socket>>>(ioContext, sslContext);

		try {
			ConnectAndReceive(); //initalizes websocket and polls continuously for messages until socket closed or an exception occurs
		}
		catch (beast::system_error const& se) {
			std::cout << "exception: " << se.code() << ", " << se.code().message() << std::endl;
			Sleep(1000); //just wait a little while to not spam
		}

		ws.reset();
	}

	//if we get to here process has been terminated
}

void WebSocketSession::Stop() {
	terminate = true;
}

void WebSocketSession::ConnectAndReceive()
{
	// Look up the domain name
	auto const tcpResolverResults = tcpResolver.resolve(host, port);

	// Make the connection on the IP address we get from a lookup
	tcpEndPoint = net::connect(get_lowest_layer(*ws), tcpResolverResults);

	// Set SNI Hostname (many hosts need this to handshake successfully)
	if (!SSL_set_tlsext_host_name(ws->next_layer().native_handle(), host.c_str())) {
		throw beast::system_error(
			beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()), "Failed to set SNI Hostname");
	}
	// Update the _host string. This will provide the value of the
	// Host HTTP header during the WebSocket handshake.
	// See https://tools.ietf.org/html/rfc7230#section-5.4
	hostAndPort = host + ':' + std::to_string(tcpEndPoint.port());

	// Perform the SSL handshake
	ws->next_layer().handshake(ssl::stream_base::client);

	// Set a decorator to change the User-Agent of the handshake
	ws->set_option(websocket::stream_base::decorator(
		[](websocket::request_type& req)
		{
			req.set(http::field::user_agent,
				std::string(BOOST_BEAST_VERSION_STRING) +
				" websocket-client-coro");
		}));

	// Perform the websocket handshake
	ws->handshake(hostAndPort, target);

	//Read Messages
	try {
		while (true) //expect to read continuously until a connection failure
		{
			if (!terminate) {
				ws->read(buffer);

				if (buffer.size() > 0)
				{
					std::cout << beast::make_printable(buffer.data()) << std::endl;
				}
				buffer.clear();
			}
			else {
				ws->close(boost::beast::websocket::close_code::normal);
			}

		}
	}
	catch (beast::system_error const& se) {
		if (se.code() == websocket::error::closed) {
			std::cout << "socket was closed." << std::endl;
		}
		else {
			std::cout << "exception: " << se.code() << ", " << se.code().message() << ", " << se.what();
		}
	}
	catch (std::exception& ex) {
		std::cout << "exception: " << ex.what();
	}
}