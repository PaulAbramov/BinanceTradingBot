#pragma once

#include <map>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/unordered_map.hpp>

#include "WebSocketSession.h"
#include "APIEnums.h"

class WebSocketCollection
{
private:
	//pointer to a method
	using handle = void*;

	boost::unordered_map<EDepthLevel, const char*> eDepthLevelToString{ boost::assign::map_list_of
	(EDepthLevel::NONE, "")
	(EDepthLevel::FIVE, "5")
	(EDepthLevel::TEN, "10")
	(EDepthLevel::TWENTY, "20") };

	boost::unordered_map<EFrequency, const char*> eFrequencyToString{ boost::assign::map_list_of
	(EFrequency::NONE, "")
	(EFrequency::HUNDREDMILI, "100ms")
	(EFrequency::THOUSANDMILI, "1000ms") };

	net::io_context& ioContext;
	std::string host;
	char const* port;
	Logger logger;
	std::pmr::map<handle, std::weak_ptr<WebSocketSession>> map;

	handle StartChannel(const std::vector<std::string>& _symbols, const std::string& _type, const std::function<bool(const std::string&)>& _callbackFunction);
	static std::string MakeChannel(const std::vector<std::string>& _symbols, const std::string& _type);
	static std::string CreateChannelString(const std::string& _symbol, const std::string& _type);

	void RemoveDeadWebsockets();

public:
	explicit WebSocketCollection(net::io_context& _ioc,
		char const* _host,
		char const* _port,
		const Logger& _logger);

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