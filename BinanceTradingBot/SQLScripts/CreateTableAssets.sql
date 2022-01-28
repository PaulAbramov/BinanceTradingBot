DROP TABLE IF EXISTS [dbo].[Assets];

CREATE TABLE dbo.Assets
	(
	Id int NOT NULL,
	Asset varchar(10) NOT NULL,
	Interval varchar(10) NOT NULL,
	DateTime datetime2(7) NOT NULL,
	CandleOpen decimal(16,8) NOT NULL,
	CandleHigh decimal(16,8) NOT NULL,
	CandleLow decimal(16,8) NOT NULL,
	CandleClose decimal(16,8) NOT NULL
	)

ALTER TABLE Assets DROP COLUMN ID 
ALTER TABLE Assets ADD ID INT IDENTITY(1,1)