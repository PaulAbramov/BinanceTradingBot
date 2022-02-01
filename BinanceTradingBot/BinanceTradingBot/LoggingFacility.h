#pragma once

#include <source_location>
#include <memory>
#include <string_view>

class LoggingFacility
{
public:
    virtual ~LoggingFacility() = default;
    virtual void WriteInfoEntry(std::string_view _entry, const std::source_location& _where = std::source_location::current()) = 0;
    virtual void WriteWarnEntry(std::string_view _entry, const std::source_location& _where = std::source_location::current()) = 0;
    virtual void WriteErrorEntry(std::string_view _entry, const std::source_location& _where = std::source_location::current()) = 0;
};

using Logger = std::shared_ptr<LoggingFacility>;
