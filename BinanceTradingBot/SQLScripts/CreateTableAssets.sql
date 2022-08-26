DROP TABLE IF EXISTS [dbo].[SMAs];
DROP TABLE IF EXISTS [dbo].[EMAs];
DROP TABLE IF EXISTS [dbo].[ATRs];
DROP TABLE IF EXISTS [dbo].[Assets];

CREATE TABLE dbo.Assets
	(
	Id int IDENTITY(1,1) PRIMARY KEY,
	Asset varchar(10) NOT NULL,
	Interval varchar(10) NOT NULL,
	OpenTime datetime2(7) NOT NULL,
	CandleOpen decimal(28,8) NOT NULL,
	CandleHigh decimal(28,8) NOT NULL,
	CandleLow decimal(28,8) NOT NULL,
	CandleClose decimal(28,8) NOT NULL,
	CloseTime datetime2(7) NOT NULL,
	Volume decimal(28,8) NOT NULL,
	QuoteAssetVolume decimal(28,8) NOT NULL,
	Trades int NOT NULL,
	TakerBuyBaseAssetVolume decimal(28,8) NOT NULL,
	TakerBuyQuoteAssetVolume decimal(28,8) NOT NULL,
	)