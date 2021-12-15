#pragma once

#include "LoggingFacility.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

class FileLogger final : public LoggingFacility
{
public:
    virtual void WriteInfoEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        LOG(INFO) << _callerFunction << "(): " << _entry << endl;
    }
    virtual void WriteWarnEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        LOG(WARNING) << _callerFunction << "(): " << _entry << endl;
    }
    virtual void WriteErrorEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        LOG(ERROR) << _callerFunction << "(): " << _entry << endl;
    }
};