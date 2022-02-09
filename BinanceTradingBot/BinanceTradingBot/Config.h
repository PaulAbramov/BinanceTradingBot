#pragma once

struct Config
{
public:
	std::string TaapiSecret;
	std::string ApiKey;
	std::string SecretKey;
	std::string GnuplotPath;

	std::string SqlServer;
	std::string SqlUserName;
	std::string SqlPassword;

	int MaxTrades;
	double TakeProfit;
	double FirstSafetyOrder;
	double SecondSafetyOrder;
	double ThirdSafetyOrder;
	double FourthSafetyOrder;

	double InitialBuy;
	double FirstSafetyOrderAmount;
	double SecondSafetyOrderAmount;
	double ThirdSafetyOrderAmount;
	double FourthSafetyOrderAmount;

	double RsiBuy1DThreshold;
	double RsiBuy1HThreshold;
	double RsiBuy15MThreshold;
	double RsiSell1DThreshold;
	double RsiSell1HThreshold;
	double RsiSell15MThreshold;
};
