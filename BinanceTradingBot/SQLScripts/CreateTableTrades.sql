DROP TABLE IF EXISTS [dbo].[Trades];

CREATE TABLE dbo.Trades
	(
	Id int IDENTITY(1,1) PRIMARY KEY,
	OrderID bigint NOT NULL,
	BotName varchar(10) NOT NULL,
	ClientOrderID varchar(25) NOT NULL,
	InvestedAmount decimal(16,8) NOT NULL,
	EntryPrice decimal(16,8) NOT NULL,
	EntryTime datetime2(7) NOT NULL,
	Asset varchar(10) NOT NULL,
	Amount decimal(16,8) NOT NULL,
	StopLoss decimal(16,8),
	TakeProfit decimal(16,8),
	Commission decimal(16,8),
    CommissionAsset varchar(10) NOT NULL,
	ExitPrice decimal(16,8),
	ExitTime datetime2(7),
	Earnings decimal(16,8),
	WinLoss decimal(16,8)
	)