#define ELPP_THREAD_SAFE

#include "Bot.h"

Bot::Bot(const Logger& _logger, const Config& _config): manager(_logger, _config.api_key, _config.secret_key)
{
	logger = std::make_shared<Logger>(_logger);
	config = _config;
}

bool Bot::Run()
{
	busdBalance = GetBalanceForCoin("BUSD");

	const shared_ptr<SqlConnectionFactory> connectionFactory(new SqlConnectionFactory("localhost@Binance;MARS Connection=True;", "", ""));
	shared_ptr<ConnectionPool<SAConnection>> pool(new ConnectionPool<SAConnection>(symbols.size() * 2, connectionFactory)); // double the size of the symbols, for trading and updating the Db

	net::io_context ioc;
	
	ioc.post([this, &ioc, &pool]()
	{
		std::thread t([this, &ioc, &pool]()
			{
				DCARun(&ioc, pool);
			});
		t.detach();

		//std::thread t2([this, &ioc, &pool]()
		//	{
		//		ManualRun(&ioc, pool);
		//	});
		//t2.detach();
	});

	while (true)
	{
		if (ioc.stopped())
		{
			ioc.restart();
		}
		if (!ioc.poll())
		{
			if (false)
			{
			}
			else
			{
				std::this_thread::sleep_for(milliseconds(3));
			}
		}
	}

	(*logger)->WriteWarnEntry("Closing Programm.");

	return EXIT_SUCCESS;
}

void Bot::DCARun(net::io_context *_ioc, shared_ptr<ConnectionPool<SAConnection>> _pool)
{
	clock_t start = clock();
	while (true)
	{
		// 15sekunden wegen api warten
		// Free:	 1 API request / 15 seconds
		// Basic:	 5 API requests / 15 seconds
		// Pro :	30 API requests / 15 seconds
		// Expert : 75 API requests / 15 seconds
		if (((static_cast<float>(clock()) - start) / CLOCKS_PER_SEC) > 5)
		{
			for(auto& symbol : symbols)
			{
				vector<float> rsiVector;

				for (auto& interval : intervals)
				{
					string result;
					ss << "https://api.taapi.io/rsi?secret=" << config.taapi_secret << "&exchange=binance&symbol=BNB/BUSD&interval=" << eIntervalsToString.at(interval);
					string url = ss.str();
					ss.str(std::string());
					ss.clear();

					manager.CurlAPI(url, result);

					auto jsonResult = nlohmann::json::parse(result);
					if (!jsonResult["value"].is_number_float())
					{
						(*logger)->WriteWarnEntry("did not get value");
						break;
					}

					auto rsiValue = jsonResult["value"];
					rsiVector.push_back(rsiValue);

					std::this_thread::sleep_for(milliseconds(2000));
				}

				rsiVector[0] = 20;
				rsiVector[1] = 20;
				rsiVector[2] = 20;

				if (rsiVector[0] < rsiBuy15MThreshold && rsiVector[1] < rsiBuy1HThreshold && rsiVector[2] < rsiBuy1DThreshold)
				{
					ss << "Buying condition met for " << symbol << ". Check for coin and order status.";
					(*logger)->WriteWarnEntry(ss.str());
					ss.str(std::string());
					ss.clear();

					bool activeTrade = false;
					auto activeTrades = GetActiveDealsFromDb(_pool, symbol);
					for(const auto& trade : activeTrades)
					{
						if(trade.Asset == symbol)
						{
							activeTrade = true;
							break;
						}
					}

					//if coin is not in deal and we have < maxdeals
					if(activeTrade || activeTrades.size() >= maxdeals)
					{
						ss << symbol << " has a open Trade or maxdeals is exceeding.";
						(*logger)->WriteWarnEntry(ss.str());
						ss.str(std::string());
						ss.clear();

						continue;
					}

#pragma region initialTrade
					//open up a new trade with given parameters
					//auto tradeResponse = manager.PostSpotAccountNewOrder(symbol, ETimeInForce::NONE, 0, initialBuy, 0, "", 0, 0, ENewOrderResponseType::FULL, ESide::BUY, EOrderType::MARKET);
					//
					//auto trade = CreateTradeObjectFromJsonToInsert(tradeResponse);
					//
					////add the trade to the database
					//AddTradeToDb(_pool, trade);
#pragma endregion
					auto initialBuyPrice = 480.2; //trade.EntryPrice;
					const auto firstSafetyOrderPrice = initialBuyPrice - initialBuyPrice * firstSafetyOrder;
					const auto secondSafetyOrderPrice = firstSafetyOrderPrice - firstSafetyOrderPrice * secondSafetyOrder;
					const auto thirdSafetyOrderPrice = secondSafetyOrderPrice - secondSafetyOrderPrice * thirdSafetyOrder;
					const auto fourthSafetyOrderPrice = thirdSafetyOrderPrice - thirdSafetyOrderPrice * fourthSafetyOrder;

					//float takeProfitOrderPrice;
					//if(trade.TakeProfit * trade.Amount > 10)
					//{
					//	takeProfitOrderPrice = trade.TakeProfit;
					//}
					//else
					//{
					//	takeProfitOrderPrice = 10 / trade.Amount;
					//}

					
					const auto firstSafetyOrderAmountInQuantity = RoundValueToDecimalValue(firstSafetyOrderAmount / firstSafetyOrderPrice, 3);
					const auto secondSafetyOrderAmountInQuantity = RoundValueToDecimalValue(secondSafetyOrderAmount / secondSafetyOrderPrice, 3);
					const auto thirdSafetyOrderAmountInQuantity = RoundValueToDecimalValue(thirdSafetyOrderAmount / thirdSafetyOrderPrice, 3);
					const auto fourthSafetyOrderAmountInQuantity = RoundValueToDecimalValue(fourthSafetyOrderAmount / fourthSafetyOrderPrice, 3);
					const auto firstSafetyOrderPriceString = RoundValueToDecimalValue(firstSafetyOrderPrice, 2);
					const auto secondSafetyOrderPriceString = RoundValueToDecimalValue(secondSafetyOrderPrice, 2);
					const auto thirdSafetyOrderPriceString = RoundValueToDecimalValue(thirdSafetyOrderPrice, 2);
					const auto fourthSafetyOrderPriceString = RoundValueToDecimalValue(fourthSafetyOrderPrice, 2);


					////open up a new deal with given parameters
					//tradeResponse = manager.PostSpotAccountNewOrder(symbol, ETimeInForce::GTC, trade.Amount, 0, takeProfitOrderPrice, "", 0, 0, ENewOrderResponseType::FULL, ESide::SELL, EOrderType::LIMIT);
					//
					//trade = CreateTradeObjectFromJsonToInsert(tradeResponse);
					////add the trade to the database
					//AddTradeToDb(_pool, trade);

					//open up a new deal with given parameters
					auto tradeResponse = manager.PostSpotAccountNewOrder(symbol, ETimeInForce::GTC, firstSafetyOrderAmountInQuantity, "", firstSafetyOrderPriceString, "", "", "", ENewOrderResponseType::FULL, ESide::BUY, EOrderType::LIMIT);

					auto trade = CreateTradeObjectFromJsonToInsert(tradeResponse);
					//add the trade to the database
					AddTradeToDb(_pool, trade);

					ss << "Buying condition met for " << symbol << ".";
					(*logger)->WriteWarnEntry(ss.str());
					ss.str(std::string());
					ss.clear();
				}
				if(rsiVector[0] > rsiSell15MThreshold && rsiVector[1] > rsiSell1HThreshold && rsiVector[2] > rsiSell1DThreshold)
				{
					ss << "Selling condition met for " << symbol << ". Check for coin and order status.";
					(*logger)->WriteWarnEntry(ss.str());
					ss.str(std::string());
					ss.clear();
					//check on trade or balance and change DB entry
				}
			}
			start = clock();
		}
		else
		{
			std::this_thread::sleep_for(milliseconds(3));
		}
	}
}

void Bot::ManualRun(net::io_context *_ioc, shared_ptr<ConnectionPool<SAConnection>> _pool)
{
	
	vector<void*> handles;

	WebSocketCollection websocketCollection{ *_ioc, BINANCE_HOST, BINANCE_PORT, *logger };

	auto fifteenMinuteCandlestickHandle = websocketCollection.KlineCandleStick(symbols, EIntervals::FIFTEENMINUTES,
		[this, &_pool](auto _answer)
		{
			return AddAssetToDb(_pool, _answer);
		});

	auto oneHourCandlestickHandle = websocketCollection.KlineCandleStick(symbols, EIntervals::ONEHOUR,
		[this, &_pool](auto _answer)
		{
			return AddAssetToDb(_pool, _answer);
		});

	auto oneDayCandlestickHandle = websocketCollection.KlineCandleStick(symbols, EIntervals::ONEDAY,
		[this, &_pool](auto _answer)
		{
			return AddAssetToDb(_pool, _answer);
		});

	handles.emplace_back(fifteenMinuteCandlestickHandle);
	handles.emplace_back(oneHourCandlestickHandle);
	handles.emplace_back(oneDayCandlestickHandle);

	clock_t start = clock();
	while (!handles.empty())
	{
		if (_ioc->stopped())
		{
			_ioc->restart();
		}
		if (!_ioc->poll())
		{
			// TODO create messagelist to send commands or attach websockets to ioc
			// 15sekunden wegen api warten
			// Free:	 1 API request / 15 seconds
			// Basic:	 5 API requests / 15 seconds
			// Pro :	30 API requests / 15 seconds
			// Expert : 75 API requests / 15 seconds
			if (((static_cast<float>(clock()) - start) / CLOCKS_PER_SEC) > 15)
			{
				_ioc->post([this, &start, _logger = logger, _manager = manager]()
					{
						auto test = _manager.GetSpotAccountCurrentOrderCountUsage();

						(*logger)->WriteInfoEntry(test);
						start = clock();
					});
			}
			else
			{
				std::this_thread::sleep_for(milliseconds(3));
			}
		}
	}
}

bool Bot::AddAssetToDb(const shared_ptr<ConnectionPool<SAConnection>>& _pool, const string& _answer) const
{
	auto jsonResult = nlohmann::json::parse(_answer);
	if (jsonResult.contains("data"))
	{
		jsonResult = jsonResult["data"];
	}
	
	const string symbol = jsonResult["s"];
	const string interval = jsonResult["k"]["i"];
	const time_t candleCloseTime = jsonResult["k"]["T"];
	const string open = jsonResult["k"]["o"];
	const string high = jsonResult["k"]["h"];
	const string low = jsonResult["k"]["l"];
	const string close = jsonResult["k"]["c"];
	
	const string dateTime = LongToString(candleCloseTime);
	
	shared_ptr<SAConnection> connection;
	try
	{
		connection = _pool->Borrow();
	}
	catch (ConnectionUnavailable& exception)
	{
		return true;
	}
	
	bool existingEntry = false;
	try
	{
		SACommand select(&(*connection), _TSA("SELECT ASSET, DATETIME FROM ASSETS WHERE DateTime = :1 AND Asset = :2"));
	
		select.Param(1).setAsDateTime() = _TSA(dateTime.c_str());
		select.Param(2).setAsString() = _TSA(symbol.c_str());
		select.Execute();
	
		try
		{
			while (select.FetchNext())
			{
				string getSymbol = select[1].asString().GetMultiByteChars();
				getSymbol.erase(ranges::remove(getSymbol, ' ').begin(), getSymbol.end());
				if (getSymbol == symbol)
				{
					existingEntry = true;
	
					SACommand update(&(*connection), _TSA("UPDATE ASSETS SET CandleHigh = :1, CandleLow = :2, CandleClose = :3 WHERE DateTime = :4 AND Asset = :5; "));
	
					update.Param(1).setAsDouble() = stod(high);
					update.Param(2).setAsDouble() = stod(low);
					update.Param(3).setAsDouble() = stod(close);
					update.Param(4).setAsDateTime() = _TSA(dateTime.c_str());
					update.Param(5).setAsString() = _TSA(symbol.c_str());
					update.Execute();
				}
	
				break;
			}
		}
		catch (SAException& exception)
		{
			(&(*connection))->Rollback();
			(*logger)->WriteErrorEntry("inner exception");
			(*logger)->WriteErrorEntry(exception.ErrText().GetMultiByteChars());
			_pool->Unborrow(connection);
			return false;
		}
	}
	catch (SAException& exception2)
	{
		(&(*connection))->Rollback();
		(*logger)->WriteErrorEntry("outer exception");
		(*logger)->WriteErrorEntry(exception2.ErrText().GetMultiByteChars());
		_pool->Unborrow(connection);
		return false;
	}

	// Insert Asset
	try
	{
		if (!existingEntry)
		{
			SACommand insert(&(*connection), _TSA("INSERT INTO ASSETS(Asset, Interval, DateTime, CandleOpen, CandleHigh, CandleLow, CandleClose) VALUES(:1, :2, :3, :4, :5, :6, :7)"));
	
			insert.Param(1).setAsString() = _TSA(symbol.c_str());
			insert.Param(2).setAsString() = _TSA(interval.c_str());
			insert.Param(3).setAsDateTime() = _TSA(dateTime.c_str());
			insert.Param(4).setAsDouble() = stod(open);
			insert.Param(5).setAsDouble() = stod(high);
			insert.Param(6).setAsDouble() = stod(low);
			insert.Param(7).setAsDouble() = stod(close);
	
			insert.Execute();
		}
	}
	catch (SAException& exc)
	{
		(&(*connection))->Rollback();
		(*logger)->WriteErrorEntry("third exception");
		(*logger)->WriteErrorEntry(exc.ErrText().GetMultiByteChars());
		_pool->Unborrow(connection);
		return false;
	}
	
	_pool->Unborrow(connection);
	
	return true;
}

string Bot::LongToString(const int64_t _longDate) const
{
	char buff[128];

	const duration<int64_t, std::milli> duration(_longDate);
	const auto timePoint = system_clock::time_point(duration_cast<system_clock::duration>(duration));
	const std::time_t inTimeT = system_clock::to_time_t(timePoint);
	struct tm newtime {};
	localtime_s(&newtime, &inTimeT);
	strftime(buff, 128, "%Y-%m-%d %H:%M:%S", &newtime);
	std::string resDate(buff);

	return resDate;
}

time_t Bot::StringToTime(string _dateString) const
{
	ranges::replace(_dateString, 'T', ' ');
	std::tm tmTime = boost::posix_time::to_tm(boost::posix_time::time_from_string(_dateString));

	return mktime(&tmTime);
}

double Bot::GetBalanceForCoin(const string& _symbol) const
{
	string accInfo = manager.GetSpotAccountInformation();

	auto jsonResultAccInfo = nlohmann::json::parse(accInfo);
	auto balances = jsonResultAccInfo["balances"];

	for (auto& balance : balances)
	{
		if (balance["asset"] == _symbol)
		{
			const string balanceToReturn = balance["free"];
			return stod(balanceToReturn);
		}
	}

	return 0;
}

vector<Trade> Bot::GetActiveDealsFromDb(const shared_ptr<ConnectionPool<SAConnection>>& _pool, const string& _symbol) const
{
	vector<Trade> activeTrades;
	shared_ptr<SAConnection> connection;
	try
	{
		connection = _pool->Borrow();
	}
	catch (ConnectionUnavailable& exception)
	{
		(*logger)->WriteErrorEntry("Connection in pool unavailable.");
		return activeTrades;
	}

	SACommand select(&(*connection), _TSA("SELECT * FROM TRADES WHERE Asset = :1 AND Exitprice = 0"));
	select.Param(1).setAsString() = _TSA(_symbol.c_str());
	select.Execute();

	try
	{
		while(select.FetchNext())
		{
			time_t exitTime = 0;
			double earnings = 0;
			double winLoss = 0;
			if(!select[11].isNull())
			{
				exitTime = StringToTime(select[11].asString().GetMultiByteChars());
				earnings = select[12].asNumeric();
				winLoss = select[13].asNumeric();
			}

			activeTrades.push_back(
				{
					select[1].asLong(),
					select[2].asString().GetMultiByteChars(),
					select[3].asNumeric(),
					select[4].asNumeric(),
					StringToTime(select[5].asString().GetMultiByteChars()),
					select[6].asString().GetMultiByteChars(),
					select[7].asNumeric(),
					select[8].asNumeric(),
					select[9].asNumeric(),
					select[10].asNumeric(),
					exitTime,
					earnings,
					winLoss
				});
		}
	}
	catch (SAException& exception)
	{
		(&(*connection))->Rollback();
		(*logger)->WriteErrorEntry("inner exception");
		(*logger)->WriteErrorEntry(exception.ErrText().GetMultiByteChars());
		_pool->Unborrow(connection);
		return activeTrades;
	}

	_pool->Unborrow(connection);
	return activeTrades;
}

string Bot::RoundValueToDecimalValue(const double& _valueToRound, const int _decimalPosition) const
{
	const int roundToDecimal = pow(10, _decimalPosition);
	const auto doubleToReturn = roundf(_valueToRound * roundToDecimal) / roundToDecimal;

	stringstream ss;
	ss << fixed;
	ss.precision(_decimalPosition); // set # places after decimal
	ss << doubleToReturn;
	return ss.str();
}

Trade Bot::CreateTradeObjectFromJsonToInsert(const string& _tradeResponse) const
{
	auto jsonResultTrade = nlohmann::json::parse(_tradeResponse);

	Trade trade;

	trade.TradeId = jsonResultTrade["orderId"];
	trade.InvestedAmount = initialBuy;
	trade.BotName = "DCA";
	trade.Asset = jsonResultTrade["symbol"];
	trade.StopLoss = 0;
	trade.ExitPrice = 0;

	string price;
	if(!jsonResultTrade["fills"].empty())
	{
		price = jsonResultTrade["fills"][0]["price"];
	}
	else
	{
		price = jsonResultTrade["price"];
	}
	
	trade.EntryPrice = stod(price);
	trade.TakeProfit = trade.EntryPrice + trade.EntryPrice * takeprofit;

	// time now
	trade.EntryTime = std::time(nullptr);

	const string amount = jsonResultTrade["origQty"];
	trade.Amount = stod(amount);

	return trade;
}

void Bot::AddTradeToDb(const shared_ptr<ConnectionPool<SAConnection>>& _pool, const Trade& _trade) const
{
	shared_ptr<SAConnection> connection;
	try
	{
		connection = _pool->Borrow();
	}
	catch (ConnectionUnavailable& exception)
	{
		(*logger)->WriteErrorEntry("Connection in pool unavailable.");
		return;
	}

	try
	{
		SACommand insert(&(*connection), _TSA("INSERT INTO TRADES(OrderID, BotName, InvestedAmount, EntryPrice, EntryTime, Asset, Amount, StopLoss, TakeProfit, ExitPrice) VALUES(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10)"));

		const string entryTime = LongToString(_trade.EntryTime);

		insert.Param(1).setAsLong() = _trade.TradeId;
		insert.Param(2).setAsString() = _TSA(_trade.BotName.c_str());
		insert.Param(3).setAsNumeric() = _trade.InvestedAmount;
		insert.Param(4).setAsNumeric() = _trade.EntryPrice;
		insert.Param(5).setAsDateTime() = _TSA(entryTime.c_str());;
		insert.Param(6).setAsString() = _TSA(_trade.Asset.c_str());
		insert.Param(7).setAsNumeric() = _trade.Amount;
		insert.Param(8).setAsNumeric() = _trade.StopLoss;
		insert.Param(9).setAsNumeric() = _trade.TakeProfit;
		insert.Param(10).setAsNumeric() = _trade.ExitPrice;

		insert.Execute();
	}
	catch (SAException& exc)
	{
		(&(*connection))->Rollback();
		(*logger)->WriteErrorEntry("third exception");
		(*logger)->WriteErrorEntry(exc.ErrText().GetMultiByteChars());
	}

	_pool->Unborrow(connection);
}
