#include "WebSocketSession.h"

using namespace std;

// Resolver and socket require an io_context
WebSocketSession::WebSocketSession(net::io_context& _ioc,
	string _host,
	string _port,
	const Logger& _logger)
	: ioContext(_ioc),
	sslContext( ssl::context::tlsv12_client ),
	tcpResolver(ioContext),
	ws(ioContext, sslContext),
	host(std::move(_host)),
	port(std::move(_port)),
	logger(_logger)
{
}

void WebSocketSession::StartWebSocketSession(string _target, std::function<bool(const string&)> _callbackFunction, holder_type _holder)
{
	tcpResolver.async_resolve(host, port,
		[this, _target = std::move(_target), _callbackFunction = std::move(_callbackFunction), _holder = std::move(_holder)]
	(boost::system::error_code _errorcode, const boost::asio::ip::tcp::resolver::results_type& _resultsType) mutable
	{
		if (_errorcode)
		{
			if (!terminate)
			{
				logger->WriteErrorEntry(_errorcode.message());
			}
		}
		else
		{
			ConnectWebSocketSession(_target, _resultsType, std::move(_callbackFunction), std::move(_holder));
		}
	});
}

void WebSocketSession::StopWebSocketSession()
{
	terminate = true;

	if (ws.next_layer().next_layer().is_open()) 
	{
		boost::system::error_code errorCode;
		ws.close(boost::beast::websocket::close_code::normal, errorCode);
	}
}

void WebSocketSession::AsyncStopWebSocketSession(holder_type _holder)
{
	terminate = true;

	if (ws.next_layer().next_layer().is_open()) 
	{
		ws.async_close(boost::beast::websocket::close_code::normal
			, [holder = std::move(_holder)](const boost::system::error_code&){}
		);
	}
}


void WebSocketSession::ConnectWebSocketSession(string _target, const boost::asio::ip::tcp::resolver::results_type& _resultsType, std::function<bool(const string&)> _callbackFunction, holder_type _holder)
{
	if(!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host.c_str()))
	{
		const auto errorCode{ boost::beast::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()) };

		logger->WriteErrorEntry(errorCode.message());

		return;
	}

	boost::asio::async_connect(ws.next_layer().next_layer(),
		_resultsType.begin(),
		_resultsType.end(),
		[this, _target = std::move(_target), _callbackFunction = std::move(_callbackFunction), _holder = std::move(_holder)]
		(const boost::system::error_code _errorCode, const boost::asio::ip::tcp::resolver::iterator&) mutable 
	{
		if (_errorCode) 
		{
			if (!terminate)
			{
				logger->WriteErrorEntry(_errorCode.message());
			}
		}
		else 
		{
			OnConnectedWebSocketSession(_target, std::move(_callbackFunction), std::move(_holder));
		}
	});
}

void WebSocketSession::OnConnectedWebSocketSession(string _target, std::function<bool(const string&)> _callbackFunction, holder_type _holder)
{
	ws.control_callback(
		[this]
	(boost::beast::websocket::frame_type _kind, boost::beast::string_view _payload) mutable {
			(void)_kind; (void)_payload;
			//std::cout << "control_callback(" << this << "): kind=" << static_cast<int>(kind) << ", payload=" << payload.data() << std::endl;
			ws.async_pong(
				boost::beast::websocket::ping_data{}
				, [](boost::beast::error_code _errorCode)
				{
					(void)_errorCode; /*std::cout << "control_callback_cb(" << this << "): ec=" << ec << std::endl;*/
				}
			);
		}
	);

	ws.next_layer().async_handshake(boost::asio::ssl::stream_base::client,
		[this, _target = std::move(_target), _callbackFunction = std::move(_callbackFunction), _holder = std::move(_holder)]
		(boost::system::error_code _errorCode) mutable 
		{
			if (_errorCode) 
			{
				if (!terminate)
				{
					logger->WriteErrorEntry(_errorCode.message());
				}
			}
			else {
				OnAsyncSSLHandshake(_target, std::move(_callbackFunction), std::move(_holder));
			}
		});
}

void WebSocketSession::OnAsyncSSLHandshake(const string& _target, std::function<bool(const string&)> _callbackFunction, holder_type _holder)
{
	ws.async_handshake(host, _target,
		[this, _callbackFunction = std::move(_callbackFunction), _holder = std::move(_holder)]
		(boost::system::error_code _errorCode) mutable
		{
			StartReadWebSocketSession(_errorCode, std::move(_callbackFunction), std::move(_holder));
		}
	);
}

void WebSocketSession::StartReadWebSocketSession(boost::system::error_code _errorCode, std::function<bool(const string&)> _callbackFunction, holder_type _holder)
{
	if (_errorCode) 
	{
		if (!terminate) 
		{
			logger->WriteErrorEntry(_errorCode.message());
		}

		StopWebSocketSession();

		return;
	}

	ws.async_read(buffer,
		[this, _callbackFunction = std::move(_callbackFunction), _holder = std::move(_holder)]
		(boost::system::error_code _errorCode, std::size_t _readData) mutable
		{
			OnReadWebSocketSession(_errorCode, _readData, std::move(_callbackFunction), std::move(_holder));
		});
}

void WebSocketSession::OnReadWebSocketSession(boost::system::error_code _errorCode, std::size_t _readData,
	std::function<bool(const string&)> _callbackFunction, holder_type _holder)
{
	if (_errorCode)
	{
		if (!terminate)
		{
			logger->WriteErrorEntry(_errorCode.message());
		}

		StopWebSocketSession();

		return;
	}

	const auto size{ buffer.size() };

	std::string strbuf;
	strbuf.reserve(size);

	for (const auto& it: buffer.data()) 
	{
		strbuf.append(static_cast<const char*>(it.data()), it.size());
	}
	buffer.consume(buffer.size());

	if (!_callbackFunction(strbuf)) {
		StopWebSocketSession();
	}
	else {
		StartReadWebSocketSession(boost::system::error_code{}, std::move(_callbackFunction), std::move(_holder));
	}
}
