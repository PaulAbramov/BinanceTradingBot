#pragma once

#include "LoggingFacility.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

class FileLogger final : public LoggingFacility
{
private:
    std::mutex mu;
public:
    virtual void WriteInfoEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        mu.lock();
        LOG(INFO) << _callerFunction << "(): " << _entry << endl;
        mu.unlock();
    }
    virtual void WriteWarnEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        mu.lock();
        LOG(WARNING) << _callerFunction << "(): " << _entry << endl;
        mu.unlock();
    }
    virtual void WriteErrorEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        mu.lock();
        LOG(ERROR) << _callerFunction << "(): " << _entry << endl;
        mu.unlock();
    }
};