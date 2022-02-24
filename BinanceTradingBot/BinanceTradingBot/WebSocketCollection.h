#pragma once

#include <map>

#include <boost/algorithm/string/case_conv.hpp>

#include "WebSocketSession.h"
#include "APIEnums.h"

class WebSocketCollection
{
private:
	//pointer to a method
	using handle = void*;

	std::map<EDepthLevel, const char*> eDepthLevelToString
	{
		{ EDepthLevel::NONE, "" },
		{ EDepthLevel::FIVE, "5" },
		{ EDepthLevel::TEN, "10" },
		{ EDepthLevel::TWENTY, "20" }
	};

	std::map<EFrequency, const char*> eFrequencyToString
	{ 
		{ EFrequency::NONE, "" },
		{ EFrequency::HUNDREDMILI, "100ms" },
		{ EFrequency::THOUSANDMILI, "1000ms" }
	};

	net::io_context& ioContext;
	std::string host;
	char const* port;
	std::map<handle, std::weak_ptr<WebSocketSession>> map;

	handle StartChannel(const std::vector<std::string>& _symbols, const std::string& _type, const std::function<bool(const std::string&)>& _callbackFunction);
	static std::string MakeChannel(const std::vector<std::string>& _symbols, const std::string& _type);
	static std::string CreateChannelString(const std::string& _symbol, const std::string& _type);

	void RemoveDeadWebsockets();

public:
	explicit WebSocketCollection(net::io_context& _ioc,
		char const* _host,
		char const* _port);

	void UnsubscribeChannel(handle _websocketHandle);
	void UnsubscribeAllChannels();

	handle AggregateTrade(const std::vector<std::string>& _symbols, const std::function<bool(const std::string&)>& _callbackFunction);
	handle Trade(const std::vector<std::string>& _symbols, const std::function<bool(const std::string&)>& _callbackFunction);
	handle KlineCandleStick(const std::vector<std::string>& _symbols, const EIntervals _interval, const std::function<bool(const std::string&)>& _callbackFunction);
	handle MiniTicker(const std::vector<std::string>& _symbols, const std::function<bool(const std::string&)>& _callbackFunction);
	handle Ticker(const std::vector<std::string>& _symbols, const std::function<bool(const std::string&)>& _callbackFunction);
	handle BookTicker(const std::vector<std::string>& _symbols, const std::function<bool(const std::string&)>& _callbackFunction);
	handle PartialBookDepth(const std::vector<std::string>& _symbols, const EDepthLevel _depthLevel, const EFrequency _frequency, const std::function<bool(const std::string&)>& _callbackFunction);
	handle DiffBookDepth(const std::vector<std::string>& _symbols, const EFrequency _frequency, const std::function<bool(const std::string&)>& _callbackFunction);
};