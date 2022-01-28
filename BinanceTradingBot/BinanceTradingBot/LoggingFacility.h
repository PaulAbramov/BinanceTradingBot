#pragma once

#include <source_location>
#include <memory>
#include <string_view>

using namespace std;

class LoggingFacility
{
public:
    virtual ~LoggingFacility() = default;
    virtual void WriteInfoEntry(string_view _entry, const source_location& _where = source_location::current()) = 0;
    virtual void WriteWarnEntry(string_view _entry, const source_location& _where = source_location::current()) = 0;
    virtual void WriteErrorEntry(string_view _entry, const source_location& _where = source_location::current()) = 0;
};

using Logger = shared_ptr<LoggingFacility>;
