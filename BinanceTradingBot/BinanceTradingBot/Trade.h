#pragma once
#include <string>

struct Trade
{
	long TradeId;
	std::string BotName;
	std::string ClientOrderID;
	double InvestedAmount;
	double EntryPrice;
	time_t EntryTime;
	std::string Asset;
	double Amount;
	double StopLoss;
	double TakeProfit;
	double ExitPrice;
	time_t ExitTime;
	double Earnings;
	double WinLoss;
};
