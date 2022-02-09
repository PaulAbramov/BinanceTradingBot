#define ELPP_THREAD_SAFE

#include "Bot.h"

using namespace std;

Bot::Bot(const Logger& _logger, const Config& _config) : manager(_logger, _config.ApiKey, _config.SecretKey), sqlManager(_logger, symbols.size(), _config)
{
	logger = _logger;
	config = _config;

	maxOrderValue = config.InitialBuy + config.FirstSafetyOrderAmount + config.SecondSafetyOrderAmount + config.ThirdSafetyOrderAmount + config.FourthSafetyOrderAmount;
}

bool Bot::Run()
{
	net::io_context ioc;
	
	ioc.post([this, &ioc]()
	{
		std::thread t([this, &ioc]()
			{
				DCARun(&ioc);
			});
		t.detach();

		std::thread t2([this, &ioc]()
			{
				ManualRun(&ioc);
			});
		t2.detach();
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
				std::this_thread::sleep_for(chrono::milliseconds(3));
			}
		}
	}

	logger->WriteWarnEntry("Closing Program.");

	return EXIT_SUCCESS;
}

void Bot::DCARun(net::io_context *_ioc)
{
	clock_t start{ clock() };
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
				if (auto iterator{ runningTrades.find(symbol) }; iterator != runningTrades.end())
				{
					if(iterator->second.wait_for(chrono::seconds(0)) != std::future_status::ready)
					{
						continue;
					}
					else
					{
						ss << "Trade with symbol " << symbol << " has been completed.";
						logger->WriteInfoEntry(ss.str());
						ss.str(std::string());
						ss.clear();

						runningTrades.erase(iterator);
					}
				}

				auto activeTrades{ sqlManager.GetActiveTradesFromDb(symbol) };
				//if coin is not in trade and we have < maxtrades
				if (!activeTrades.empty() || activeTrades.size() >= config.MaxTrades)
				{
					if (!activeTrades.empty())
					{
						if (auto iterator{ runningTrades.find(symbol) }; iterator == runningTrades.end())
						{
							runningTrades.insert(make_pair(symbol, std::async(std::launch::async, [this, &symbol, &activeTrades]()
								{
									CheckTrades(symbol);
								})));
						}
					}

					ss << symbol << " has a open Trade or maxtrades is exceeding.";
					logger->WriteWarnEntry(ss.str());
					ss.str(std::string());
					ss.clear();

					continue;
				}

				busdBalance = GetBalanceForCoin("BUSD");
				if (busdBalance < maxOrderValue)
				{
					continue;
				}

				vector<double> rsiVector;

				for (auto& interval : intervals)
				{
					string result;
					ss << "https://api.taapi.io/rsi?secret=" << config.TaapiSecret << "&exchange=binance&symbol=BNB/BUSD&interval=" << eIntervalsToString.at(interval);
					string url{ ss.str() };
					ss.str(std::string());
					ss.clear();

					manager.CurlAPI(url, result);

					auto jsonResult{ nlohmann::json::parse(result) };
					if (!jsonResult["value"].is_number_float())
					{
						logger->WriteWarnEntry("did not get value");
						break;
					}

					auto rsiValue{ jsonResult["value"] };
					rsiVector.push_back(rsiValue);

					std::this_thread::sleep_for(chrono::milliseconds(2000));
				}

				if (rsiVector[0] < config.RsiBuy15MThreshold && rsiVector[1] < config.RsiBuy1HThreshold && rsiVector[2] < config.RsiBuy1DThreshold)
				{
					ss << "Buying condition met for " << symbol << ". Check for coin and order status.";
					logger->WriteWarnEntry(ss.str());
					ss.str(std::string());
					ss.clear();

					runningTrades.insert(make_pair(symbol, std::async(std::launch::async, [this, &symbol]()
					{
#pragma region initialTrade
						auto tradeResponse{ manager.PostSpotAccountNewOrder(symbol, ETimeInForce::NONE, "", to_string(config.InitialBuy), "", symbol + "initialTrade", "", "", ENewOrderResponseType::FULL, ESide::BUY, EOrderType::MARKET) };
						auto trade{ CreateTradeObjectFromJsonToInsert(tradeResponse) };
						sqlManager.AddTradeToDb(trade);
#pragma endregion
						auto initialBuyPrice{ trade.EntryPrice };
						const auto firstSafetyOrderPrice{ initialBuyPrice - initialBuyPrice * config.FirstSafetyOrder };
						const auto secondSafetyOrderPrice{ firstSafetyOrderPrice - firstSafetyOrderPrice * config.SecondSafetyOrder };
						const auto thirdSafetyOrderPrice{ secondSafetyOrderPrice - secondSafetyOrderPrice * config.ThirdSafetyOrder };
						const auto fourthSafetyOrderPrice{ thirdSafetyOrderPrice - thirdSafetyOrderPrice * config.FourthSafetyOrder };

						double takeProfitOrderPrice;
						if(trade.TakeProfit * trade.Amount > 10)
						{
							takeProfitOrderPrice = trade.TakeProfit;
						}
						else
						{
							takeProfitOrderPrice = 10 / trade.Amount;
						}

						const auto firstSafetyOrderAmountInQuantity{ RoundValueToDecimalValue(config.FirstSafetyOrderAmount / firstSafetyOrderPrice, 3) };
						const auto secondSafetyOrderAmountInQuantity{ RoundValueToDecimalValue(config.SecondSafetyOrderAmount / secondSafetyOrderPrice, 3) };
						const auto thirdSafetyOrderAmountInQuantity{ RoundValueToDecimalValue(config.ThirdSafetyOrderAmount / thirdSafetyOrderPrice, 3) };
						const auto fourthSafetyOrderAmountInQuantity{ RoundValueToDecimalValue(config.FourthSafetyOrderAmount / fourthSafetyOrderPrice, 3) };
						const auto firstSafetyOrderPriceString{ RoundValueToDecimalValue(firstSafetyOrderPrice, 1) };
						const auto secondSafetyOrderPriceString{ RoundValueToDecimalValue(secondSafetyOrderPrice, 1) };
						const auto thirdSafetyOrderPriceString{ RoundValueToDecimalValue(thirdSafetyOrderPrice, 1) };
						const auto fourthSafetyOrderPriceString{ RoundValueToDecimalValue(fourthSafetyOrderPrice, 1) };
						const auto takeProfitOrderPriceString{ RoundValueToDecimalValue(takeProfitOrderPrice, 1) };

						//Takeprofit
						if(!CreateTradeAndSafeInDb(symbol, to_string(trade.Amount), takeProfitOrderPriceString, symbol + "takeprofit", ESide::SELL, takeProfitOrderPrice))
						{
							logger->WriteErrorEntry("placing takeprofittrade failed.");
						}

						//Safetytrades
						if (!CreateTradeAndSafeInDb(symbol, firstSafetyOrderAmountInQuantity, firstSafetyOrderPriceString, symbol + "safetytrade1", ESide::BUY, takeProfitOrderPrice))
						{
							logger->WriteErrorEntry("placing safetytrade1 failed.");
						}
						if (!CreateTradeAndSafeInDb(symbol, secondSafetyOrderAmountInQuantity, secondSafetyOrderPriceString, symbol + "safetytrade2", ESide::BUY, takeProfitOrderPrice))
						{
							logger->WriteErrorEntry("placing safetytrade2 failed.");
						}
						if (!CreateTradeAndSafeInDb(symbol, thirdSafetyOrderAmountInQuantity, thirdSafetyOrderPriceString, symbol + "safetytrade3", ESide::BUY, takeProfitOrderPrice))
						{
							logger->WriteErrorEntry("placing safetytrade3 failed.");
						}
						if (!CreateTradeAndSafeInDb(symbol, fourthSafetyOrderAmountInQuantity, fourthSafetyOrderPriceString, symbol + "safetytrade4", ESide::BUY, takeProfitOrderPrice))
						{
							logger->WriteErrorEntry("placing safetytrade4 failed.");
						}

						ss << "Placed initialtrade, takeprofit and the safetytrades for symbol: " << symbol << ".";
						logger->WriteWarnEntry(ss.str());
						ss.str(std::string());
						ss.clear();

						CheckTrades(symbol);
				})));

				}
				if(rsiVector[0] > config.RsiSell15MThreshold && rsiVector[1] > config.RsiSell1HThreshold && rsiVector[2] > config.RsiSell1DThreshold)
				{
					ss << "Selling condition met for " << symbol << ". Check for coin and order status.";
					logger->WriteWarnEntry(ss.str());
					ss.str(std::string());
					ss.clear();
					//check on trade or balance and change DB entry
				}
			}
			start = clock();
		}
		else
		{
			std::this_thread::sleep_for(chrono::milliseconds(3));
		}
	}
}

void Bot::CheckTrades(const string& _symbol) const
{
	while (true)
	{
		// check which trades got filled
		// check if takeprofit was hit
		// if it was hit, then update initialtrade and the safetytrades
		// cancel not hit safetytrades and remove their db entries
		// if it was not hit, then set the takeprofits for the safetytrades
		auto result{ manager.GetSpotAccountQueryOrder(_symbol, 0, _symbol + "takeprofit") };
		auto jsonResult{ nlohmann::json::parse(result) };

		vector<string> safetyTradesToDelete;

		safetyTradesToDelete.push_back(CreateProfitTakerForSafetyTrade(_symbol, 1, jsonResult["price"]));
		safetyTradesToDelete.push_back(CreateProfitTakerForSafetyTrade(_symbol, 2, jsonResult["price"]));
		safetyTradesToDelete.push_back(CreateProfitTakerForSafetyTrade(_symbol, 3, jsonResult["price"]));
		safetyTradesToDelete.push_back(CreateProfitTakerForSafetyTrade(_symbol, 4, jsonResult["price"]));

		safetyTradesToDelete.erase(ranges::remove_if(safetyTradesToDelete,
			[](std::string const& _safetyTrade)
			{
				return _safetyTrade.empty();
			}).begin(),
				safetyTradesToDelete.end());

		if (jsonResult["status"] == "FILLED")
		{
			// Delete not hit trades
			for (const auto& clientOrderIdToDelete : safetyTradesToDelete)
			{
				auto cancelOrderResult{ manager.DeleteSpotAccountCancelOrder(_symbol, 0, clientOrderIdToDelete, "") };
				sqlManager.RemoveTradeFromDb(clientOrderIdToDelete);
			}

			// if it was hit, then update initialtrade and the safetytrades
			for (auto activeTrade : sqlManager.GetActiveTradesFromDb(_symbol))
			{
				activeTrade.ExitPrice = activeTrade.TakeProfit;
				activeTrade.ExitTime = jsonResult["updateTime"];
				activeTrade.Earnings = activeTrade.Amount * activeTrade.ExitPrice;
				activeTrade.WinLoss = activeTrade.Earnings - activeTrade.InvestedAmount;

				sqlManager.UpdateTradeInDb(activeTrade);
			}

			break;
		}

		std::this_thread::sleep_for(chrono::milliseconds(10000));
	}

	logger->WriteInfoEntry("Done.");
}

void Bot::ManualRun(net::io_context *_ioc)
{
	
	vector<void*> handles;

	WebSocketCollection websocketCollection{ *_ioc, BINANCE_HOST, BINANCE_PORT, logger };

	auto fifteenMinuteCandlestickHandle{ websocketCollection.KlineCandleStick(symbols, EIntervals::FIFTEENMINUTES,
		[this](auto _answer)
		{
			logger->WriteInfoEntry(_answer);
			return sqlManager.AddAssetToDb(_answer);
		}) };

	auto oneHourCandlestickHandle{ websocketCollection.KlineCandleStick(symbols, EIntervals::ONEHOUR,
		[this](auto _answer)
		{
			logger->WriteInfoEntry(_answer);
			return sqlManager.AddAssetToDb(_answer);
		}) };

	auto oneDayCandlestickHandle{ websocketCollection.KlineCandleStick(symbols, EIntervals::ONEDAY,
		[this](auto _answer)
		{
			logger->WriteInfoEntry(_answer);
			return sqlManager.AddAssetToDb(_answer);
		}) };

	handles.emplace_back(fifteenMinuteCandlestickHandle);
	handles.emplace_back(oneHourCandlestickHandle);
	handles.emplace_back(oneDayCandlestickHandle);

	clock_t start{ clock() };
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
				_ioc->post([this, &start]()
					{
						auto test{ manager.GetSpotAccountCurrentOrderCountUsage() };

						logger->WriteInfoEntry(test);
						start = clock();
					});
			}
			else
			{
				std::this_thread::sleep_for(chrono::milliseconds(3));
			}
		}
	}
}

double Bot::GetBalanceForCoin(const string& _symbol) const
{
	string accInfo{ manager.GetSpotAccountInformation() };

	auto jsonResultAccInfo{ nlohmann::json::parse(accInfo) };
	auto balances{ jsonResultAccInfo["balances"] };

	for (auto& balance : balances)
	{
		if (balance["asset"] == _symbol)
		{
			const string balanceToReturn{ balance["free"] };
			return stod(balanceToReturn);
		}
	}

	return 0;
}

string Bot::RoundValueToDecimalValue(const double& _valueToRound, const int _decimalPosition) const
{
	const int roundToDecimal{ static_cast<const int>(pow(10, _decimalPosition)) };
	const auto doubleToReturn{ ceill(_valueToRound * roundToDecimal) / roundToDecimal };

	stringstream ss;
	ss << fixed;
	ss.precision(_decimalPosition); // set # places after decimal
	ss << doubleToReturn;
	return ss.str();
}

bool Bot::CreateTradeAndSafeInDb(const string& _symbol, const string& _quantity, const string& _price, const string& _clientOrderId, const ESide _side, const double _takeProfit) const
{
	const auto tradeResponse{ manager.PostSpotAccountNewOrder(_symbol, ETimeInForce::GTC, _quantity, "", _price, _clientOrderId, "", "", ENewOrderResponseType::FULL, _side, EOrderType::LIMIT) };
	if (const auto jsonResultTrade{ nlohmann::json::parse(tradeResponse) }; jsonResultTrade.contains("code"))
	{
		return false;
	}

	const auto trade{ CreateTradeObjectFromJsonToInsert(tradeResponse) };
	sqlManager.AddTradeToDb(trade, _takeProfit);

	return true;
}

/// <summary>
/// Create Profittaker trade for Safetytrade if there is none and only if the safetytrade got filled
///	return the ClientorderId to remove the trade if the initial Profittakertrade got filled
/// </summary>
/// <param name="_symbol"></param>
/// <param name="_tradeNumber"></param>
/// <param name="_takeProfitPrice"></param>
/// <returns></returns>
string Bot::CreateProfitTakerForSafetyTrade(const string& _symbol, const int _tradeNumber, const string& _takeProfitPrice) const
{
	auto result{ manager.GetSpotAccountQueryOrder(_symbol, 0, _symbol + "safetytrade" + to_string(_tradeNumber)) };
	auto jsonResult{ nlohmann::json::parse(result) };

	if (jsonResult["status"] == "FILLED")
	{
		auto resultProfitTrade{ manager.GetSpotAccountQueryOrder(_symbol, 0, _symbol + "safetyprofit" + to_string(_tradeNumber)) };
		auto jsonResultProfit{ nlohmann::json::parse(resultProfitTrade) };
		if (jsonResultProfit.contains("code"))
		{
			CreateTradeAndSafeInDb(_symbol, jsonResult["origQty"], _takeProfitPrice, _symbol + "safetyprofit" + to_string(_tradeNumber), ESide::SELL, stod(_takeProfitPrice));
		}

		return "";
	}
	else
	{
		return _symbol + "safetytrade" + to_string(_tradeNumber);
	}
}

Trade Bot::CreateTradeObjectFromJsonToInsert(const string& _tradeResponse) const
{
	auto jsonResultTrade{ nlohmann::json::parse(_tradeResponse) };

	Trade trade;

	trade.TradeId = jsonResultTrade["orderId"];
	trade.InvestedAmount = config.InitialBuy;
	trade.BotName = "DCA";
	trade.ClientOrderID = jsonResultTrade["clientOrderId"];
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

	const string amount{ jsonResultTrade["origQty"] };
	trade.Amount = stod(amount);

	const auto takeProfitPrice{ trade.EntryPrice + trade.EntryPrice * config.TakeProfit };

	if (takeProfitPrice * trade.Amount > 10)
	{
		trade.TakeProfit = takeProfitPrice;
	}
	else
	{
		trade.TakeProfit = 10 / trade.Amount;
	}

	return trade;
}