#pragma once

#include <iostream>

#include "LoggingFacility.h"
#include "easylogging++.h"

class StandardOutputLogger final : public LoggingFacility 
{
private:
    std::mutex mu;
public:
    virtual void WriteInfoEntry(std::string_view _entry, const std::source_location& _where) override
    {
        mu.lock();
        std::cout << "[INFO] " << _where.function_name() << "(): " << _entry << std::endl;
        mu.unlock();
    }
    virtual void WriteWarnEntry(std::string_view _entry, const std::source_location& _where) override
    {
        mu.lock();
        std::cout << "[WARNING] " << _where.function_name() << "(): " << _entry << std::endl;
        mu.unlock();
    }
    virtual void WriteErrorEntry(std::string_view _entry, const std::source_location& _where) override
    {
        mu.lock();
        std::cout << "[ERROR] " << _where.function_name() << "(): " << _entry << std::endl;
        mu.unlock();
    }
};