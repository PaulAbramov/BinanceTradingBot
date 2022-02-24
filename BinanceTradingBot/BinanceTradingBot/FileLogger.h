#pragma once

#include <easylogging++.h>
#include <mutex>
#include <source_location>

class FileLogger
{
private:
public:
    static void WriteInfoEntry(std::string _entry, const std::source_location& _where = std::source_location::current());
    static void WriteWarnEntry(std::string _entry, const std::source_location& _where = std::source_location::current());
    static void WriteErrorEntry(std::string _entry, const std::source_location& _where = std::source_location::current());
};
