#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <set>

#include "ConnectionFactory.h"
#include "ConnectionPoolStats.h"

struct ConnectionUnavailable final : std::exception {

	char const* what() const throw() override
	{
		return "Unable to allocate connection";
	}
};

template<class T>
class ConnectionPool
{
private:

protected:
	std::shared_ptr<ConnectionFactory> factory;
	size_t poolSize{};
	std::deque<std::shared_ptr<T>> pool;
	std::set<std::shared_ptr<T>> borrowed;
	std::mutex ioMutex;
	eSAClient saClient;

public:
	ConnectionPool(size_t _poolSize, const std::shared_ptr<ConnectionFactory>& _factory, eSAClient _client)
	{
		poolSize = _poolSize;
		factory = _factory;
		saClient = _client;

		// Fill the pool
		while (pool.size() < poolSize) 
		{
			pool.push_back(factory->Create(_client));
		}
	}

	ConnectionPoolStats GetStats()
	{
		std::lock_guard<std::mutex> lock(ioMutex);

		// Get stats
		const ConnectionPoolStats stats{ pool.size(), borrowed.size() };

		return stats;
	}

	std::shared_ptr<T> Borrow()
	{
		// Lock
		std::lock_guard<std::mutex> lock(ioMutex);

		// Check for a free connection
		if (pool.size() == 0)
		{
			// Are there any crashed connections listed as "borrowed"?
			for (auto it{ borrowed.begin() }; it != borrowed.end(); ++it)
			{
				const auto isAlive { (*it)->isAlive() };
				const auto isConnected { (*it)->isConnected() };

				if (!isConnected || !isAlive)
				{
					// This connection has been abandoned! Destroy it and create a new connection
					try
					{
						// If we are able to create a new connection, return it
						FileLogger::WriteInfoEntry("Creating new connection to replace discarded connection");
						std::shared_ptr<SAConnection> conn { factory->Create(saClient) };
						borrowed.erase(it);
						borrowed.insert(conn);
						return std::static_pointer_cast<T>(conn);
					}
					catch (std::exception& e) 
					{
						// Error creating a replacement connection
						throw ConnectionUnavailable();
					}
				}
			}

			// Nothing available
			throw ConnectionUnavailable();
		}

		// Take one off the front
		std::shared_ptr<SAConnection>conn{ pool.front() };
		pool.pop_front();

		// Add it to the borrowed list
		borrowed.insert(conn);

		return std::static_pointer_cast<T>(conn);
	}

	void Unborrow(std::shared_ptr<T> _connection)
	{
		// Lock
		std::lock_guard<std::mutex> lock(ioMutex);

		// Push onto the pool
		pool.push_back(std::static_pointer_cast<SAConnection>(_connection));

		// Unborrow
		borrowed.erase(_connection);
	}
};