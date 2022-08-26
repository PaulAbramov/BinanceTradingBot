#define ELPP_THREAD_SAFE

#include "Bot.h"

using namespace std;

Bot::Bot(const Config& _config) : manager(_config.ApiKey, _config.SecretKey), sqlManager(symbols.size(), _config)
{
	config = _config;

	maxOrderValue = config.InitialBuy + config.FirstSafetyOrderAmount + config.SecondSafetyOrderAmount + config.ThirdSafetyOrderAmount + config.FourthSafetyOrderAmount;
}

bool Bot::Run()
{
	net::io_context ioc;
	
	ioc.post([this, &ioc]()
	{
		manager.CurlFile();

		//std::thread t([this, &ioc]()
		//	{
		//		DCARun(&ioc);
		//	});
		//t.detach();

		//std::thread t2([this, &ioc]()
		//	{
		//		ManualRun(&ioc);
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
				std::this_thread::sleep_for(chrono::milliseconds(3));
			}
		}
	}

	return EXIT_SUCCESS;
}

void Bot::DCARun(net::io_context *_ioc)
{
	clock_t start{ clock() };
	while (true)
	{
		// 15 seconds to wait bcs of the API
		// Free:	 1 API request / 15 seconds
		// Basic:	 5 API requests / 15 seconds
		// Pro :	30 API requests / 15 seconds
		// Expert : 75 API requests / 15 seconds
		if (((static_cast<float>(clock()) - start) / CLOCKS_PER_SEC) > 5)
		{
			auto result = manager.GetMarketExchangeInformation(symbols);
			auto jsonResult = JsonHelper::ParseStringToJson(result);
			if(jsonResult.empty())
			{
				continue;
			}

			for(auto symbol : jsonResult["symbols"])
			{
				SymbolExchangeInfo info
				{
					symbol["symbol"]
				};

				for(auto filter : symbol["filters"])
				{
					if(filter["filterType"] == "PRICE_FILTER")
					{
						string PriceFilterMinPrice = filter["minPrice"];
						string PriceFilterMaxPrice = filter["maxPrice"];
						string PriceFilterTickSize = filter["tickSize"];

						info.PriceFilterMinPrice = stod(PriceFilterMinPrice);
						info.PriceFilterMaxPrice = stod(PriceFilterMaxPrice);
						info.PriceFilterTickSize = stod(PriceFilterTickSize);

						//ticksize might be 0.01
						//log10 calculates back, which multiplier has to be used to get to 1
						//in our example of ticksize 0.01, this will be the multiplier -2
						//negate it to 2
						info.PriceFilterTickSizeDecimalPosition = -log10(info.PriceFilterTickSize);
					}
					else if(filter["filterType"] == "LOT_SIZE")
					{
						string LotSizeMinQuantity = filter["minQty"];
						string LotSizeMaxQuantity = filter["maxQty"];
						string LotSizeStepSize = filter["stepSize"];

						info.LotSizeMinQuantity = stod(LotSizeMinQuantity);
						info.LotSizeMaxQuantity = stod(LotSizeMaxQuantity);
						info.LotSizeStepSize = stod(LotSizeStepSize);

						info.LotSizeStepSizeDecimalPosition = -log10(info.LotSizeStepSize);
					}
				}

				symbolsInformations.push_back(info);
			}

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
						FileLogger::WriteInfoEntry(std::format("Trade with symbol {} has been completed.", symbol));

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

					FileLogger::WriteInfoEntry(std::format("{} has a open Trade or maxtrades is exceeding.", symbol));

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
					string symbolToCheck = symbol;
					string currency = "BUSD";

					size_t pos = symbolToCheck.find(currency);
					if (pos != std::string::npos)
					{
						// If found then erase it from string
						symbolToCheck.erase(pos, currency.length());
						symbolToCheck += "/";
						symbolToCheck += currency;
					}

					string result;
					string url{ std::format("https://api.taapi.io/rsi?secret={}&exchange=binance&symbol={}&interval={}", config.TaapiSecret, symbolToCheck, eIntervalsToString.at(interval))};

					manager.CurlAPI(url, result);

					auto jsonResult = JsonHelper::ParseStringToJson(result);
					if (jsonResult.empty())
					{
						break;
					}

					if (!jsonResult["value"].is_number_float())
					{
						FileLogger::WriteWarnEntry("did not get value");
						FileLogger::WriteWarnEntry(result);
						break;
					}

					auto rsiValue{ jsonResult["value"] };
					rsiVector.push_back(rsiValue);

					std::this_thread::sleep_for(chrono::milliseconds(3000));
				}

				if (rsiVector.size() < 3)
				{
					continue;
				}

				if (rsiVector[0] < config.RsiBuy15MThreshold && rsiVector[1] < config.RsiBuy1HThreshold && rsiVector[2] < config.RsiBuy1DThreshold)
				{
					FileLogger::WriteWarnEntry(std::format("Buying condition met for {}. Check for coin and order status.", symbol));

					runningTrades.insert(make_pair(symbol, std::async(std::launch::async, [this, &symbol]()
					{
#pragma region initialTrade
						const auto tradeResponse{ manager.PostSpotAccountNewOrder(symbol, ETimeInForce::NONE, "", to_string(config.InitialBuy), "", symbol + "initialTrade", "", "", ENewOrderResponseType::FULL, ESide::BUY, EOrderType::MARKET) };
						const auto trade{ CreateTradeObjectFromJsonToInsert(tradeResponse) };
						sqlManager.AddTradeToDb(trade);
#pragma endregion
						const auto initialBuyPrice{ trade.EntryPrice };
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

						//auto iterator = ranges::find_if(symbolsInformations, [&symbol](SymbolExchangeInfo& _info) { return _info.Symbol == symbol; });
						//if(iterator == symbolsInformations.end())
						//{
						//	return;
						//}

						//const auto firstSafetyOrderAmountInQuantity{ RoundValueToDecimalValue(config.FirstSafetyOrderAmount / firstSafetyOrderPrice, iterator->LotSizeStepSizeDecimalPosition) };
						//const auto secondSafetyOrderAmountInQuantity{ RoundValueToDecimalValue(config.SecondSafetyOrderAmount / secondSafetyOrderPrice, iterator->LotSizeStepSizeDecimalPosition) };
						//const auto thirdSafetyOrderAmountInQuantity{ RoundValueToDecimalValue(config.ThirdSafetyOrderAmount / thirdSafetyOrderPrice, iterator->LotSizeStepSizeDecimalPosition) };
						//const auto fourthSafetyOrderAmountInQuantity{ RoundValueToDecimalValue(config.FourthSafetyOrderAmount / fourthSafetyOrderPrice, iterator->LotSizeStepSizeDecimalPosition) };
						//const auto firstSafetyOrderPriceString{ RoundValueToDecimalValue(firstSafetyOrderPrice, iterator->PriceFilterTickSizeDecimalPosition) };
						//const auto secondSafetyOrderPriceString{ RoundValueToDecimalValue(secondSafetyOrderPrice, iterator->PriceFilterTickSizeDecimalPosition) };
						//const auto thirdSafetyOrderPriceString{ RoundValueToDecimalValue(thirdSafetyOrderPrice, iterator->PriceFilterTickSizeDecimalPosition) };
						//const auto fourthSafetyOrderPriceString{ RoundValueToDecimalValue(fourthSafetyOrderPrice, iterator->PriceFilterTickSizeDecimalPosition) };
						//const auto takeProfitOrderPriceString{ RoundValueToDecimalValue(takeProfitOrderPrice, iterator->PriceFilterTickSizeDecimalPosition) };

						//Takeprofit
						if(!CreateTradeAndSafeInDb(symbol, trade.Amount, takeProfitOrderPrice, symbol + "takeprofit", ESide::SELL, takeProfitOrderPrice))
						{
							FileLogger::WriteErrorEntry("placing takeprofittrade failed.");
						}

						//Safetytrades
						if (!CreateTradeAndSafeInDb(symbol, config.FirstSafetyOrderAmount / firstSafetyOrderPrice, firstSafetyOrderPrice, symbol + "safetytrade1", ESide::BUY, takeProfitOrderPrice))
						{
							FileLogger::WriteErrorEntry("placing safetytrade1 failed.");
						}
						if (!CreateTradeAndSafeInDb(symbol, config.SecondSafetyOrderAmount / secondSafetyOrderPrice, secondSafetyOrderPrice, symbol + "safetytrade2", ESide::BUY, takeProfitOrderPrice))
						{
							FileLogger::WriteErrorEntry("placing safetytrade2 failed.");
						}
						if (!CreateTradeAndSafeInDb(symbol, config.ThirdSafetyOrderAmount / thirdSafetyOrderPrice, thirdSafetyOrderPrice, symbol + "safetytrade3", ESide::BUY, takeProfitOrderPrice))
						{
							FileLogger::WriteErrorEntry("placing safetytrade3 failed.");
						}
						if (!CreateTradeAndSafeInDb(symbol, config.FourthSafetyOrderAmount / fourthSafetyOrderPrice, fourthSafetyOrderPrice, symbol + "safetytrade4", ESide::BUY, takeProfitOrderPrice))
						{
							FileLogger::WriteErrorEntry("placing safetytrade4 failed.");
						}

						FileLogger::WriteWarnEntry(std::format("Placed initialtrade, takeprofit and the safetytrades for symbol: {}", symbol));

						CheckTrades(symbol);
					})));
				}

				if(rsiVector[0] > config.RsiSell15MThreshold && rsiVector[1] > config.RsiSell1HThreshold && rsiVector[2] > config.RsiSell1DThreshold)
				{
					FileLogger::WriteWarnEntry(std::format("Selling condition met for {}. Check for coin and order status", symbol));
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
		auto jsonResult = JsonHelper::ParseStringToJson(result);
		if (jsonResult.empty())
		{
			continue;
		}

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
			//TODO check WinLoss regarding Commission
			for (auto activeTrade : sqlManager.GetActiveTradesFromDb(_symbol))
			{
				activeTrade.ExitPrice = activeTrade.TakeProfit;
				activeTrade.ExitTime = jsonResult["updateTime"];
				activeTrade.Earnings = activeTrade.Amount * activeTrade.ExitPrice;
				activeTrade.WinLoss = activeTrade.Earnings - activeTrade.InvestedAmount;

				sqlManager.UpdateTradeInDb(activeTrade);
			}

			//TODO is it wished here? 
			break;
		}

		std::this_thread::sleep_for(chrono::milliseconds(10000));
	}
}

void Bot::ManualRun(net::io_context *_ioc)
{
	
	vector<void*> handles;

	WebSocketCollection websocketCollection{ *_ioc, BINANCE_HOST, BINANCE_PORT };

	auto fifteenMinuteCandlestickHandle{ websocketCollection.KlineCandleStick(symbols, EIntervals::FIFTEENMINUTES,
		[this](auto _answer)
		{
			FileLogger::WriteInfoEntry(_answer);
			return sqlManager.AddAssetToDb(_answer);
		}) };

	auto oneHourCandlestickHandle{ websocketCollection.KlineCandleStick(symbols, EIntervals::ONEHOUR,
		[this](auto _answer)
		{
			FileLogger::WriteInfoEntry(_answer);
			return sqlManager.AddAssetToDb(_answer);
		}) };

	auto oneDayCandlestickHandle{ websocketCollection.KlineCandleStick(symbols, EIntervals::ONEDAY,
		[this](auto _answer)
		{
			FileLogger::WriteInfoEntry(_answer);
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

						FileLogger::WriteInfoEntry(test);
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
	const string accInfo{ manager.GetSpotAccountInformation() };
	auto jsonResultAccInfo = JsonHelper::ParseStringToJson(accInfo);
	if (jsonResultAccInfo.empty())
	{
		return 0;
	}

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

bool Bot::CreateTradeAndSafeInDb(const string& _symbol, const double _quantity, const double _price, const string& _clientOrderId, const ESide _side, const double _takeProfit) const
{
	const auto iterator = ranges::find_if(symbolsInformations, [&_symbol](const SymbolExchangeInfo _info) { return _info.Symbol == _symbol; });
	if (iterator == symbolsInformations.end())
	{
		return false;
	}

	const auto quantityString{ RoundValueToDecimalValue(_quantity, iterator->LotSizeStepSizeDecimalPosition) };
	const auto priceString{ RoundValueToDecimalValue(_price, iterator->PriceFilterTickSizeDecimalPosition) };

	const auto tradeResponse{ manager.PostSpotAccountNewOrder(_symbol, ETimeInForce::GTC, quantityString, "", priceString, _clientOrderId, "", "", ENewOrderResponseType::FULL, _side, EOrderType::LIMIT) };
	const auto jsonResultTrade = JsonHelper::ParseStringToJson(tradeResponse);
	if (jsonResultTrade.empty())
	{
		return false;
	}

	if (jsonResultTrade.contains("code"))
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
	const auto result{ manager.GetSpotAccountQueryOrder(_symbol, 0, _symbol + "safetytrade" + to_string(_tradeNumber)) };
	auto jsonResult = JsonHelper::ParseStringToJson(result);
	if (jsonResult.empty())
	{
		return "";
	}

	if (jsonResult["status"] == "FILLED")
	{
		const auto resultProfitTrade{ manager.GetSpotAccountQueryOrder(_symbol, 0, _symbol + "safetyprofit" + to_string(_tradeNumber)) };
		const auto jsonResultProfit = JsonHelper::ParseStringToJson(resultProfitTrade);
		if (jsonResultProfit.empty())
		{
			return "";
		}

		if (jsonResultProfit.contains("code"))
		{
			const string quantity = jsonResult["origQty"];
			CreateTradeAndSafeInDb(_symbol, stod(quantity), stod(_takeProfitPrice), _symbol + "safetyprofit" + to_string(_tradeNumber), ESide::SELL, stod(_takeProfitPrice));
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
	Trade trade;

	auto jsonResultTrade = JsonHelper::ParseStringToJson(_tradeResponse);
	if (jsonResultTrade.empty())
	{
		return trade;
	}

	trade.TradeId = jsonResultTrade["orderId"];
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

	trade.CommissionAsset = jsonResultTrade["fills"][0]["commissionAsset"];

	if (!jsonResultTrade["fills"].empty())
	{
		for(auto fill : jsonResultTrade["fills"])
		{
			string commissionString = fill["commission"];
			trade.Commission += stod(commissionString);
		}
	}

	trade.InvestedAmount = trade.EntryPrice * trade.Amount;


	return trade;
}