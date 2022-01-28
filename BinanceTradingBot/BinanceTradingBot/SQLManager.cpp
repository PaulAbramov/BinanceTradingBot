#include "SQLManager.h"

SQLManager::SQLManager(const Logger& _logger, int _symbolAmount) : logger(_logger)
{
	const shared_ptr<SqlConnectionFactory> connectionFactory(new SqlConnectionFactory("localhost@Binance;MARS Connection=True;", "", ""));
	pool = std::make_shared<ConnectionPool<SAConnection>>(_symbolAmount * 2, connectionFactory); // double the size of the symbols, for trading and updating the Db
}

bool SQLManager::AddAssetToDb(const string& _answer) const
{
	auto jsonResult{ nlohmann::json::parse(_answer) };
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
	
	shared_ptr<SAConnection> connection;
	try
	{
		connection = pool->Borrow();
	}
	catch (ConnectionUnavailable&)
	{
		return true;
	}
	
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
			logger->WriteErrorEntry("inner exception");
			logger->WriteErrorEntry(exception.ErrText().GetMultiByteChars());
			pool->Unborrow(connection);
			return false;
		}
	}
	catch (SAException& exception2)
	{
		(&(*connection))->Rollback();
		logger->WriteErrorEntry("outer exception");
		logger->WriteErrorEntry(exception2.ErrText().GetMultiByteChars());
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
		logger->WriteErrorEntry("third exception");
		logger->WriteErrorEntry(exc.ErrText().GetMultiByteChars());
		pool->Unborrow(connection);
		return false;
	}
	
	pool->Unborrow(connection);
	
	return true;
}

void SQLManager::AddTradeToDb(const Trade& _trade, double _takeProfit) const
{
	shared_ptr<SAConnection> connection;
	try
	{
		connection = pool->Borrow();
	}
	catch (ConnectionUnavailable&)
	{
		logger->WriteErrorEntry("Connection in pool unavailable.");
		return;
	}

	try
	{
		SACommand insert(&(*connection), _TSA("INSERT INTO TRADES(OrderID, BotName, ClientOrderID, InvestedAmount, EntryPrice, EntryTime, Asset, Amount, StopLoss, TakeProfit, ExitPrice) VALUES(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11)"));

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
		insert.Param(11).setAsNumeric() = _trade.ExitPrice;

		insert.Execute();
	}
	catch (SAException& exc)
	{
		(&(*connection))->Rollback();
		logger->WriteErrorEntry("third exception");
		logger->WriteErrorEntry(exc.ErrText().GetMultiByteChars());
	}

	pool->Unborrow(connection);
}

void SQLManager::UpdateTradeInDb(const Trade& _trade) const
{
	shared_ptr<SAConnection> connection;
	try
	{
		connection = pool->Borrow();
	}
	catch (ConnectionUnavailable&)
	{
		logger->WriteErrorEntry("Connection in pool unavailable.");
		return;
	}

	try
	{
		SACommand update(&(*connection), _TSA("UPDATE TRADES SET ExitPrice = :1, ExitTime = :2, Earnings = :3, WinLoss = :4 WHERE ClientOrderID = :5"));

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
		logger->WriteErrorEntry("third exception");
		logger->WriteErrorEntry(exc.ErrText().GetMultiByteChars());
	}

	pool->Unborrow(connection);
}

void SQLManager::RemoveTradeFromDb(const string& _clientOrderId) const
{
	shared_ptr<SAConnection> connection;
	try
	{
		connection = pool->Borrow();
	}
	catch (ConnectionUnavailable&)
	{
		logger->WriteErrorEntry("Connection in pool unavailable.");
		return;
	}

	try
	{
		SACommand insert(&(*connection), _TSA("DELETE FROM TRADES WHERE ClientOrderID = :1"));
		insert.Param(1).setAsString() = _TSA(_clientOrderId.c_str());
		insert.Execute();
	}
	catch (SAException& exc)
	{
		(&(*connection))->Rollback();
		logger->WriteErrorEntry("third exception");
		logger->WriteErrorEntry(exc.ErrText().GetMultiByteChars());
	}

}

vector<Trade> SQLManager::GetActiveTradesFromDb(const string& _symbol) const
{
	vector<Trade> activeTrades;
	shared_ptr<SAConnection> connection;
	try
	{
		connection = pool->Borrow();
	}
	catch (ConnectionUnavailable&)
	{
		logger->WriteErrorEntry("Connection in pool unavailable.");
		return activeTrades;
	}

	SACommand select(&(*connection), _TSA("SELECT * FROM TRADES WHERE Asset = :1 AND Exitprice = 0"));
	select.Param(1).setAsString() = _TSA(_symbol.c_str());
	select.Execute();

	try
	{
		while (select.FetchNext())
		{
			time_t exitTime{};
			double earnings{};
			double winLoss{};
			if (!select[12].isNull())
			{
				exitTime = StringToTime(select[12].asString().GetMultiByteChars());
				earnings = select[13].asNumeric();
				winLoss = select[14].asNumeric();
			}

			activeTrades.push_back(
				{
					select[1].asLong(),
					select[2].asString().GetMultiByteChars(),
					select[3].asString().GetMultiByteChars(),
					select[4].asNumeric(),
					select[5].asNumeric(),
					StringToTime(select[6].asString().GetMultiByteChars()),
					select[7].asString().GetMultiByteChars(),
					select[8].asNumeric(),
					select[9].asNumeric(),
					select[10].asNumeric(),
					select[11].asNumeric(),
					exitTime,
					earnings,
					winLoss
				});
		}
	}
	catch (SAException& exception)
	{
		(&(*connection))->Rollback();
		logger->WriteErrorEntry("inner exception");
		logger->WriteErrorEntry(exception.ErrText().GetMultiByteChars());
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