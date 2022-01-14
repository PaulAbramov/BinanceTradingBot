DROP TABLE IF EXISTS [dbo].[Trades];

CREATE TABLE dbo.Trades
	(
	Id int NOT NULL,
	OrderID bigint NOT NULL,
	BotName varchar(10) NOT NULL,
	InvestedAmount decimal(16,8) NOT NULL,
	EntryPrice decimal(16,8) NOT NULL,
	EntryTime datetime2(7) NOT NULL,
	Asset varchar(10) NOT NULL,
	Amount decimal(16,8) NOT NULL,
	StopLoss decimal(16,8),
	TakeProfit decimal(16,8),
	ExitPrice decimal(16,8),
	ExitTime datetime2(7),
	Earnings decimal(16,8),
	WinLoss decimal(16,8)
	)

ALTER TABLE Trades DROP COLUMN ID 
ALTER TABLE Trades ADD ID INT IDENTITY(1,1)