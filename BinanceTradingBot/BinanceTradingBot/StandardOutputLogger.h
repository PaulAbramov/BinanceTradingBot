#pragma once

#include <iostream>

#include "LoggingFacility.h"
#include "easylogging++.h"

class StandardOutputLogger final : public LoggingFacility 
{
private:
    std::mutex mu;
public:
    virtual void WriteInfoEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        mu.lock();
        cout << "[INFO] " << _callerFunction << "(): " << _entry << endl;
        mu.unlock();
    }
    virtual void WriteWarnEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        mu.lock();
        cout << "[WARNING] " << _callerFunction << "(): " << _entry << endl;
        mu.unlock();
    }
    virtual void WriteErrorEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        mu.lock();
        cout << "[ERROR] " << _callerFunction << "(): " << _entry << endl;
        mu.unlock();
    }
};