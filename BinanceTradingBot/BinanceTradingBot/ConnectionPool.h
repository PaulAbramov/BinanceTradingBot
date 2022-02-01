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

public:
	ConnectionPool(size_t _poolSize, const std::shared_ptr<ConnectionFactory>& _factory)
	{
		this->poolSize = _poolSize;
		this->factory = _factory;

		// Fill the pool
		while (this->pool.size() < this->poolSize) 
		{
			this->pool.push_back(this->factory->Create());
		}
	}

	ConnectionPoolStats GetStats()
	{
		std::lock_guard<std::mutex> lock(this->ioMutex);

		// Get stats
		const ConnectionPoolStats stats{ this->pool.size(), this->borrowed.size() };

		return stats;
	}

	std::shared_ptr<T> Borrow()
	{
		// Lock
		std::lock_guard<std::mutex> lock(this->ioMutex);

		// Check for a free connection
		if (this->pool.size() == 0)
		{
			// Are there any crashed connections listed as "borrowed"?
			for (auto it{ this->borrowed.begin() }; it != this->borrowed.end(); ++it)
			{
				const auto isAlive { (*it)->isAlive() };
				const auto isConnected { (*it)->isConnected() };

				if (!isConnected || !isAlive)
				{
					// This connection has been abandoned! Destroy it and create a new connection
					try
					{
						// If we are able to create a new connection, return it
						std::cout << "Creating new connection to replace discarded connection";
						//std::shared_ptr<SAConnection> conn { this->factory->Create() };
						this->borrowed.erase(it);
						//this->borrowed.insert(conn);
						//return std::static_pointer_cast<T>(conn);
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
		//std::shared_ptr<SAConnection>conn{ this->pool.front() };
		this->pool.pop_front();

		// Add it to the borrowed list
		//this->borrowed.insert(conn);

		//return std::static_pointer_cast<T>(conn);
	}

	void Unborrow(std::shared_ptr<T> _connection)
	{
		// Lock
		std::lock_guard<std::mutex> lock(this->ioMutex);

		// Push onto the pool
		//this->pool.push_back(std::static_pointer_cast<SAConnection>(_connection));

		// Unborrow
		this->borrowed.erase(_connection);
	}
};