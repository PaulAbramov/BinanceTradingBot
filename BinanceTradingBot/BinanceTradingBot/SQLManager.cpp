#include "SQLManager.h"

using namespace std;

SQLManager::SQLManager(int _symbolAmount, const Config& _config)
{
	//const shared_ptr<SqlConnectionFactory> connectionFactory(new SqlConnectionFactory("localhost@Binance;MARS Connection=True;", "", ""));
	const shared_ptr<SqlConnectionFactory> connectionFactory(new SqlConnectionFactory(_config.SqlServer, _config.SqlUserName, _config.SqlPassword));

	constexpr char dllPath[] = "SuppressMessageBox.dll";

	//Load the dll file
	const HMODULE hModule = LoadLibraryA(dllPath);

	//If hModule is null, then the dll wasn't loaded.
	//An error message will be printed out to the console.
	if (!hModule) 
	{
		FileLogger::WriteWarnEntry("Couldn't load 'SuppressMessageBox.dll'!");
		return;
	}

	// there is a popup box here, but we suppress it
	pool = std::make_shared<ConnectionPool<SAConnection>>(_symbolAmount * 2, connectionFactory, SA_MySQL_Client); // double the size of the symbols, for trading and updating the Db

	if(pool->GetStats().PoolSize > 0)
	{
		FileLogger::WriteInfoEntry("Successfully created SQLPool");
	}

	//remove the .dll, we dont need it anymore
	FreeLibrary(hModule);
}

bool SQLManager::AddAssetToDb(const string& _answer) const
{
	auto jsonResult = JsonHelper::ParseStringToJson(_answer);
	if (jsonResult.empty())
	{
		return false;
	}

	const shared_ptr<SAConnection> connection = BorrowConnection();
	if (connection == nullptr)
	{
		return false;
	}

	if (jsonResult.contains("data"))
	{
		jsonResult = jsonResult["data"];
	}
	
	const string symbol{ jsonResult["s"] };
	const string interval{ jsonResult["k"]["i"] };
	const time_t candleCloseTime{ jsonResult["k"]["T"] };
	const string open{ jsonResult["k"]["o"] };
	const string high{ jsonResult["k"]["h"] };
	const string low{ jsonResult["k"]["l"] };
	const string close{ jsonResult["k"]["c"] };
	
	const string dateTime{ LongToString(candleCloseTime) };
	
	bool existingEntry{};
	try
	{
		SACommand select(&(*connection), _TSA("SELECT ASSET, DATETIME FROM ASSETS WHERE DateTime = :1 AND Asset = :2"));
	
		select.Param(1).setAsDateTime() = _TSA(dateTime.c_str());
		select.Param(2).setAsString() = _TSA(symbol.c_str());
		select.Execute();
	
		try
		{
			while (select.FetchNext())
			{
				string getSymbol{ select[1].asString().GetMultiByteChars() };
				getSymbol.erase(ranges::remove(getSymbol, ' ').begin(), getSymbol.end());
				if (getSymbol == symbol)
				{
					existingEntry = true;
	
					SACommand update(&(*connection), _TSA("UPDATE ASSETS SET CandleHigh = :1, CandleLow = :2, CandleClose = :3 WHERE DateTime = :4 AND Asset = :5; "));
	
					update.Param(1).setAsDouble() = stod(high);
					update.Param(2).setAsDouble() = stod(low);
					update.Param(3).setAsDouble() = stod(close);
					update.Param(4).setAsDateTime() = _TSA(dateTime.c_str());
					update.Param(5).setAsString() = _TSA(symbol.c_str());
					update.Execute();
				}
	
				break;
			}
		}
		catch (SAException& exception)
		{
			(&(*connection))->Rollback();
			FileLogger::WriteErrorEntry("inner exception");
			FileLogger::WriteErrorEntry(exception.ErrText().GetMultiByteChars());
			pool->Unborrow(connection);
			return false;
		}
	}
	catch (SAException& exception2)
	{
		(&(*connection))->Rollback();
		FileLogger::WriteErrorEntry("outer exception");
		FileLogger::WriteErrorEntry(exception2.ErrText().GetMultiByteChars());
		pool->Unborrow(connection);
		return false;
	}
	
	// Insert Asset
	try
	{
		if (!existingEntry)
		{
			SACommand insert(&(*connection), _TSA("INSERT INTO ASSETS(Asset, Interval, DateTime, CandleOpen, CandleHigh, CandleLow, CandleClose) VALUES(:1, :2, :3, :4, :5, :6, :7)"));
	
			insert.Param(1).setAsString() = _TSA(symbol.c_str());
			insert.Param(2).setAsString() = _TSA(interval.c_str());
			insert.Param(3).setAsDateTime() = _TSA(dateTime.c_str());
			insert.Param(4).setAsDouble() = stod(open);
			insert.Param(5).setAsDouble() = stod(high);
			insert.Param(6).setAsDouble() = stod(low);
			insert.Param(7).setAsDouble() = stod(close);
	
			insert.Execute();
		}
	}
	catch (SAException& exc)
	{
		(&(*connection))->Rollback();
		FileLogger::WriteErrorEntry("third exception");
		FileLogger::WriteErrorEntry(exc.ErrText().GetMultiByteChars());
		pool->Unborrow(connection);
		return false;
	}
	
	pool->Unborrow(connection);
	return true;
}

bool SQLManager::AddTradeToDb(const Trade& _trade, double _takeProfit) const
{
	const shared_ptr<SAConnection> connection = BorrowConnection();
	if (connection == nullptr)
	{
		return false;
	}
	
	try
	{
		SACommand insert(&(*connection), _TSA("INSERT INTO Trades(OrderID, BotName, ClientOrderID, InvestedAmount, EntryPrice, EntryTime, Asset, Amount, StopLoss, TakeProfit, ExitPrice) VALUES(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11)"));
	
		const int64_t now{ std::chrono::duration_cast<chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() };
	
		insert.Param(1).setAsLong() = _trade.TradeId;
		insert.Param(2).setAsString() = _TSA(_trade.BotName.c_str());
		insert.Param(3).setAsString() = _TSA(_trade.ClientOrderID.c_str());
		insert.Param(4).setAsNumeric() = _trade.InvestedAmount;
		insert.Param(5).setAsNumeric() = _trade.EntryPrice;
		insert.Param(6).setAsDateTime() = _TSA(LongToString(now).c_str());
		insert.Param(7).setAsString() = _TSA(_trade.Asset.c_str());
		insert.Param(8).setAsNumeric() = _trade.Amount;
		insert.Param(9).setAsNumeric() = _trade.StopLoss;
		insert.Param(10).setAsNumeric() = _takeProfit == 0 ? _trade.TakeProfit : _takeProfit;
		insert.Param(11).setAsNumeric() = _trade.Commission;
		insert.Param(12).setAsString() = _TSA(_trade.CommissionAsset.c_str());
		insert.Param(13).setAsNumeric() = _trade.ExitPrice;
	
		insert.Execute();
	}
	catch (SAException& exc)
	{
		(&(*connection))->Rollback();
		FileLogger::WriteErrorEntry("third exception");
		FileLogger::WriteErrorEntry(exc.ErrText().GetMultiByteChars());
		pool->Unborrow(connection);
		return false;
	}
	
	pool->Unborrow(connection);
	return true;
}

bool SQLManager::UpdateTradeInDb(const Trade& _trade) const
{
	const shared_ptr<SAConnection> connection = BorrowConnection();
	if (connection == nullptr)
	{
		return false;
	}
	
	try
	{
		SACommand update(&(*connection), _TSA("UPDATE Trades SET ExitPrice = :1, ExitTime = :2, Earnings = :3, WinLoss = :4 WHERE ClientOrderID = :5"));
	
		update.Param(1).setAsDouble() = _trade.ExitPrice;
		update.Param(2).setAsDateTime() = _TSA(LongToString(_trade.ExitTime).c_str());
		update.Param(3).setAsDouble() = _trade.Earnings;
		update.Param(4).setAsDouble() = _trade.WinLoss;
		update.Param(5).setAsString() = _TSA(_trade.ClientOrderID.c_str());
	
		update.Execute();
	}
	catch (SAException& exc)
	{
		(&(*connection))->Rollback();
		FileLogger::WriteErrorEntry("third exception");
		FileLogger::WriteErrorEntry(exc.ErrText().GetMultiByteChars());
		pool->Unborrow(connection);
		return false;
	}
	
	pool->Unborrow(connection);
	return true;
}

bool SQLManager::RemoveTradeFromDb(const string& _clientOrderId) const
{
	const shared_ptr<SAConnection> connection = BorrowConnection();
	if (connection == nullptr)
	{
		return false;
	}
	
	try
	{
		SACommand insert(&(*connection), _TSA("DELETE FROM Trades WHERE ClientOrderID = :1"));
		insert.Param(1).setAsString() = _TSA(_clientOrderId.c_str());
		insert.Execute();
	}
	catch (SAException& exc)
	{
		(&(*connection))->Rollback();
		FileLogger::WriteErrorEntry("third exception");
		FileLogger::WriteErrorEntry(exc.ErrText().GetMultiByteChars());
		pool->Unborrow(connection);
		return false;
	}

	pool->Unborrow(connection);
	return true;
}

vector<Trade> SQLManager::GetActiveTradesFromDb(const string& _symbol) const
{
	vector<Trade> activeTrades;
	const shared_ptr<SAConnection> connection = BorrowConnection();
	if(connection == nullptr)
	{
		return activeTrades;
	}
	
	try
	{
		SACommand select(&(*connection), _TSA("SELECT * FROM Trades WHERE Asset = :1 AND Exitprice = 0"));
		select.Param(1).setAsString() = _TSA(_symbol.c_str());
		select.Execute();

		while (select.FetchNext())
		{
			time_t exitTime{};
			double earnings{};
			double winLoss{};
			if (!select[15].isNull())
			{
				exitTime = StringToTime(select[15].asString().GetMultiByteChars());
				earnings = select[16].asNumeric();
				winLoss = select[17].asNumeric();
			}
	
			activeTrades.push_back(
				{
					select[2].asLong(),
					select[3].asString().GetMultiByteChars(),
					select[4].asString().GetMultiByteChars(),
					select[5].asNumeric(),
					select[6].asNumeric(),
					StringToTime(select[7].asString().GetMultiByteChars()),
					select[8].asString().GetMultiByteChars(),
					select[9].asNumeric(),
					select[10].asNumeric(),
					select[11].asNumeric(),
					select[12].asNumeric(),
					select[13].asString().GetMultiByteChars(),
					select[14].asNumeric(),
					exitTime,
					earnings,
					winLoss
				});
		}
	}
	catch (SAException& exception)
	{
		(&(*connection))->Rollback();
		FileLogger::WriteErrorEntry("inner exception");
		FileLogger::WriteErrorEntry(exception.ErrText().GetMultiByteChars());
		pool->Unborrow(connection);
		return activeTrades;
	}
	
	pool->Unborrow(connection);
	return activeTrades;
}

string SQLManager::LongToString(const int64_t _longDate) const
{
	char buff[128];
	
	const std::chrono::duration<int64_t, std::milli> duration(_longDate);
	const auto timePoint{ std::chrono::system_clock::time_point(duration_cast<std::chrono::system_clock::duration>(duration)) };
	const std::time_t inTimeT{ std::chrono::system_clock::to_time_t(timePoint) };
	struct tm newtime {};
	localtime_s(&newtime, &inTimeT);
	strftime(buff, 128, "%Y-%m-%d %H:%M:%S", &newtime);
	std::string resDate(buff);
	
	return resDate;
}

time_t SQLManager::StringToTime(string _dateString) const
{
	ranges::replace(_dateString, 'T', ' ');
	std::tm tmTime{ to_tm(boost::posix_time::time_from_string(_dateString)) };
	
	return mktime(&tmTime);
}

shared_ptr<SAConnection> SQLManager::BorrowConnection() const
{
	try
	{
		return pool->Borrow();
	}
	catch (ConnectionUnavailable&)
	{
		FileLogger::WriteErrorEntry("Connection in pool unavailable.");
		return nullptr;
	}
}
