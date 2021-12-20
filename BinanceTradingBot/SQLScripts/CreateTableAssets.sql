DROP TABLE IF EXISTS [dbo].[Assets];

CREATE TABLE dbo.Assets
	(
	Id int NOT NULL,
	Asset nchar(10) NOT NULL,
	Interval nchar(10) NOT NULL,
	DateTime datetime2(7) NOT NULL,
	CandleOpen float(53) NOT NULL,
	CandleHigh float(53) NOT NULL,
	CandleLow float(53) NOT NULL,
	CandleClose float(53) NOT NULL
	)

ALTER TABLE Assets DROP COLUMN ID 
ALTER TABLE Assets ADD ID INT IDENTITY(1,1)