DROP TABLE IF EXISTS [dbo].[SMAs];

CREATE TABLE dbo.SMAs
	(
	Id int IDENTITY(1,1) PRIMARY KEY,
	AssetId int FOREIGN KEY REFERENCES Assets(Id) unique WITH (IGNORE_DUP_KEY = ON),
	SMA5 decimal(18,8),
	SMA9 decimal(18,8),
	SMA12 decimal(18,8),
	SMA20 decimal(18,8),
	SMA26 decimal(18,8),
	SMA50 decimal(18,8),
	SMA75 decimal(18,8),
	SMA200 decimal(18,8)
	)