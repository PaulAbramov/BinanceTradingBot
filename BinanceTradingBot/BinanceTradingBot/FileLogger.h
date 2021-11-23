#pragma once

#include "LoggingFacility.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

class FileLogger : public LoggingFacility
{
public:
    virtual void writeInfoEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        LOG(INFO) << _callerFunction << "(): " << _entry << endl;
    }
    virtual void writeWarnEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        LOG(WARNING) << _callerFunction << "(): " << _entry << endl;
    }
    virtual void writeErrorEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        LOG(ERROR) << _callerFunction << "(): " << _entry << endl;
    }
};