#pragma once

#include <iostream>

#include "LoggingFacility.h"
#include "easylogging++.h"

class StandardOutputLogger final : public LoggingFacility 
{
private:
    std::mutex mu;
public:
    virtual void WriteInfoEntry(string_view _entry, const source_location& _where) override
    {
        mu.lock();
        cout << "[INFO] " << _where.function_name() << "(): " << _entry << endl;
        mu.unlock();
    }
    virtual void WriteWarnEntry(string_view _entry, const source_location& _where) override
    {
        mu.lock();
        cout << "[WARNING] " << _where.function_name() << "(): " << _entry << endl;
        mu.unlock();
    }
    virtual void WriteErrorEntry(string_view _entry, const source_location& _where) override
    {
        mu.lock();
        cout << "[ERROR] " << _where.function_name() << "(): " << _entry << endl;
        mu.unlock();
    }
};