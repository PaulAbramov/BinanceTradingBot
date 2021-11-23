#pragma once

#include <iostream>

#include "LoggingFacility.h"
#include "easylogging++.h"

class StandardOutputLogger : public LoggingFacility 
{
public:
    virtual void writeInfoEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        cout << "[INFO] " << _callerFunction << "(): " << _entry << endl;
    }
    virtual void writeWarnEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        cout << "[WARNING] " << _callerFunction << "(): " << _entry << endl;
    }
    virtual void writeErrorEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) override
    {
        cout << "[ERROR] " << _callerFunction << "(): " << _entry << endl;
    }
};