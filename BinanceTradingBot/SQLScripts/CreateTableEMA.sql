DROP TABLE IF EXISTS [dbo].[EMAs];

CREATE TABLE dbo.EMAs
	(
	Id int IDENTITY(1,1) PRIMARY KEY,
	AssetId int FOREIGN KEY REFERENCES Assets(Id) unique WITH (IGNORE_DUP_KEY = ON),
	EMA5 decimal(18,8),
	EMA9 decimal(18,8),
	EMA12 decimal(18,8),
	EMA20 decimal(18,8),
	EMA26 decimal(18,8),
	EMA50 decimal(18,8),
	EMA75 decimal(18,8),
	EMA200 decimal(18,8)
	)