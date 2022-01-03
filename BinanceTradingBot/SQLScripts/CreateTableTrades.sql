DROP TABLE IF EXISTS [dbo].[Trades];

CREATE TABLE dbo.Trades
	(
	Id int NOT NULL,
	OrderID bigint NOT NULL,
	BotName nchar(10) NOT NULL,
	InvestedAmount float(53) NOT NULL,
	EntryPrice float(53) NOT NULL,
	EntryTime datetime2(7) NOT NULL,
	Asset nchar(10) NOT NULL,
	Amount float(53) NOT NULL,
	StopLoss float(53),
	TakeProfit float(53),
	ExitPrice float(53),
	ExitTime datetime2(7),
	Earnings float(53),
	WinLoss float(53)
	)

ALTER TABLE Trades DROP COLUMN ID 
ALTER TABLE Trades ADD ID INT IDENTITY(1,1)