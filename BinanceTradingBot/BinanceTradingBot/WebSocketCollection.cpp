#include "WebSocketCollection.h"

#include <utility>

WebSocketCollection::WebSocketCollection(net::io_context& _ioc, char const* _host, char const* _port, const Logger& _logger)
	: ioContext(_ioc), host(_host), port(_port), logger(_logger)
{
}

WebSocketCollection::handle WebSocketCollection::AggregateTrade(const std::vector<string>& _symbols, const std::function<bool(const string&)>& _callbackFunction)
{
    return StartChannel(_symbols, "aggTrade", _callbackFunction);
}

WebSocketCollection::handle WebSocketCollection::Trade(const std::vector<string>& _symbols, const std::function<bool(const string&)>& _callbackFunction)
{
    return StartChannel(_symbols, "trade", _callbackFunction);
}

WebSocketCollection::handle WebSocketCollection::KlineCandleStick(const std::vector<string>& _symbols, const EIntervals _interval, const std::function<bool(const string&)>& _callbackFunction)
{
    string type;
    switch(_interval)
    {
        using enum EIntervals;

    case ONEMINUTE:
        type = "kline_1m";
        break;
    case THREEMINUTES:
        type = "kline_3m";
        break;
    case FIVEMINUTES:
        type = "kline_5m";
        break;
    case FIFTEENMINUTES:
        type = "kline_15m";
        break;
    case THIRTYMINUTES:
        type = "kline_30m";
        break;
    case ONEHOUR:
        type = "kline_1h";
        break;
    case TWOHOURS:
        type = "kline_2h";
        break;
    case FOURHOURS:
        type = "kline_4h";
        break;
    case SIXHOURS:
        type = "kline_6h";
        break;
    case EIGHTHOURS:
        type = "kline_8h";
        break;
    case TWELVEHOURS:
        type = "kline_12h";
        break;
    case ONEDAY:
        type = "kline_1d";
        break;
    case THREEDAYS:
        type = "kline_3d";
        break;
    case ONEWEEK:
        type = "kline_1w";
        break;
    case ONEMONTH:
        type = "kline_1M";
        break;
    case NONE:
        type = "";
    	break;
    default: 
        type = "";
        break;
    }

    return StartChannel(_symbols, type, _callbackFunction);
}

WebSocketCollection::handle WebSocketCollection::MiniTicker(const std::vector<string>& _symbols, const std::function<bool(const string&)>& _callbackFunction)
{
    return StartChannel(_symbols, "miniTicker", _callbackFunction);
}

WebSocketCollection::handle WebSocketCollection::Ticker(const std::vector<string>& _symbols, const std::function<bool(const string&)>& _callbackFunction)
{
    return StartChannel(_symbols, "ticker", _callbackFunction);
}

WebSocketCollection::handle WebSocketCollection::BookTicker(const std::vector<string>& _symbols, const std::function<bool(const string&)>& _callbackFunction)
{
    return StartChannel(_symbols, "bookTicker", _callbackFunction);
}

WebSocketCollection::handle WebSocketCollection::PartialBookDepth(const std::vector<string>& _symbols,
	const EDepthLevel _depthLevel, const EFrequency _frequency,
	const std::function<bool(const string&)>& _callbackFunction)
{
    string type{ "depth" };
    type += eDepthLevelToString.at(_depthLevel);
    type += "@";
    type += eFrequencyToString.at(_frequency);

    return StartChannel(_symbols, type, _callbackFunction);
}

WebSocketCollection::handle WebSocketCollection::DiffBookDepth(const std::vector<string>& _symbols,
	const EFrequency _frequency,
	const std::function<bool(const string&)>& _callbackFunction)
{
    string type{ "depth@" };
	type += eFrequencyToString.at(_frequency);

    return StartChannel(_symbols, type, _callbackFunction);
}

/// <summary>
/// Also pass a callbackmethod from the main function to handle the returnvalue
/// </summary>
/// <param name="_symbols"></param>
/// <param name="_type"></param>
/// <param name="_callbackFunction"></param>
/// <returns></returns>
WebSocketCollection::handle WebSocketCollection::StartChannel(const std::vector<string>& _symbols, const string& _type, const std::function<bool(const string&)>& _callbackFunction)
{
    auto webSocketSession{ std::make_shared<WebSocketSession>(ioContext, host, port, logger) };
    auto* websocketHandle{ webSocketSession.get() };
	std::weak_ptr<WebSocketSession> weakPointerWebSocketSession{ webSocketSession };

    const std::string channelToConnect{ MakeChannel(_symbols, _type) };

    websocketHandle->StartWebSocketSession(channelToConnect, _callbackFunction, std::move(webSocketSession));

    RemoveDeadWebsockets();

    map.emplace(websocketHandle, std::move(weakPointerWebSocketSession));

    return websocketHandle;
}

string WebSocketCollection::MakeChannel(const std::vector<string>& _symbols, const string& _type)
{
    std::string result;
    if(_symbols.size() == 1)
    {
        //"/ws/bnbbusd@depth@100ms"
        result += "/ws/";
        result += CreateChannelString(_symbols[0], _type);
    }
    else if(_symbols.size() > 1)
    {
        //"/stream?streams=bnbbusd@aggTrade/btcbusd@aggTrade"
        result += "/stream?streams=";

        for (const auto& symbol : _symbols)
        {
            result += CreateChannelString(symbol, _type);

            if(_symbols.back() != symbol)
            {
                result += "/";
            }
        }
    }

    return result;
}

string WebSocketCollection::CreateChannelString(const string& _symbol, const string& _type)
{
    std::string result;
    result += _symbol;

    if (*_symbol.c_str() != '!')
    {
        boost::algorithm::to_lower(result);
    }

    result += '@';
    result += _type;

    return result;
}

/// <summary>
/// Find the iterator to the handle, get the session and stop the session.
/// Remove the iterator from the map
/// </summary>
/// <param name="_websocketHandle"></param>
void WebSocketCollection::UnsubscribeChannel(handle _websocketHandle)
{
    const auto iterator{ map.find(_websocketHandle) };
    if(iterator == map.end())
    {
        return;
    }

    if (const auto websocketSession{ iterator->second.lock() })
    {
        websocketSession->StopWebSocketSession();
    }

    map.erase(iterator);

    RemoveDeadWebsockets();
}

/// <summary>
/// Get the sessions and stop them.
/// Remove the iterator from the map
/// </summary>
void WebSocketCollection::UnsubscribeAllChannels()
{
    for (auto iterator{ map.begin() }; iterator != map.end();)
    {
        if (const auto websocketSession{ iterator->second.lock() })
        {
            websocketSession->StopWebSocketSession();
        }

        iterator = map.erase(iterator);
    }
}

/// <summary>
/// Remove websockets if their weak_ptr is empty
///
/// Weak_ptr.lock() Creates a new std::shared_ptr that shares ownership of the managed object.
/// If there is no managed object, i.e. '*this' is empty, then the returned shared_ptr also is empty.
/// Effectively returns expired() ? shared_ptr<T>() : shared_ptr<T>(*this), executed atomically.
/// </summary>
void WebSocketCollection::RemoveDeadWebsockets()
{
    for (auto iterator{ map.begin() }; iterator != map.end();)
    {
	    if(!iterator->second.lock())
	    {
            iterator = map.erase(iterator);
	    }
        else
        {
            ++iterator;
        }
    }
}