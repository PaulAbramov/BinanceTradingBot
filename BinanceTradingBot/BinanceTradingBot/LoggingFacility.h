#pragma once

#include <memory>
#include <string_view>

using namespace std;

class LoggingFacility
{
public:
    virtual ~LoggingFacility() = default;
    virtual void writeInfoEntry(string_view entry) = 0;
    virtual void writeWarnEntry(string_view entry) = 0;
    virtual void writeErrorEntry(string_view entry) = 0;
};

using Logger = shared_ptr<LoggingFacility>;
