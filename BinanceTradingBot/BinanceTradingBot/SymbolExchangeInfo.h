#pragma once
#include <string>

struct SymbolExchangeInfo
{
	std::string Symbol;

	double PriceFilterMinPrice;
	double PriceFilterMaxPrice;
	double PriceFilterTickSize;
	double PriceFilterTickSizeDecimalPosition;

	double LotSizeMinQuantity;
	double LotSizeMaxQuantity;
	double LotSizeStepSize;
	double LotSizeStepSizeDecimalPosition;
};
