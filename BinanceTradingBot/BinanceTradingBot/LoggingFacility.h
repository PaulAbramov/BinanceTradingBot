#pragma once

#include <memory>
#include <string_view>

using namespace std;

class LoggingFacility
{
public:
    virtual ~LoggingFacility() = default;
    virtual void writeInfoEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) = 0;
    virtual void writeWarnEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) = 0;
    virtual void writeErrorEntry(string_view _entry, const char* _callerFunction = __builtin_FUNCTION()) = 0;
};

using Logger = shared_ptr<LoggingFacility>;
