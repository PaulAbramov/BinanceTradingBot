#pragma once

#include <iostream>

#include "LoggingFacility.h"
#include "easylogging++.h"

class StandardOutputLogger final : public LoggingFacility 
{
public:
    virtual void WriteInfoEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        cout << "[INFO] " << _callerFunction << "(): " << _entry << endl;
    }
    virtual void WriteWarnEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        cout << "[WARNING] " << _callerFunction << "(): " << _entry << endl;
    }
    virtual void WriteErrorEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        cout << "[ERROR] " << _callerFunction << "(): " << _entry << endl;
    }
};