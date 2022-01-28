#pragma once

#include "LoggingFacility.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

class FileLogger final : public LoggingFacility
{
private:
    std::mutex mu;
public:
    virtual void WriteInfoEntry(string_view _entry, const source_location& _where) override
    {
        mu.lock();
        //LOG(INFO) << _where.file_name() << "(" << _where.line() << ":" << _where.column() << ") " << _where.function_name() << "(): " << _entry << endl;
        LOG(INFO) << _where.function_name() << "(): " << _entry << endl;
        mu.unlock();
    }
    virtual void WriteWarnEntry(string_view _entry, const source_location& _where) override
    {
        mu.lock();
        LOG(WARNING) << _where.function_name() << "(): " << _entry << endl;
        mu.unlock();
    }
    virtual void WriteErrorEntry(string_view _entry, const source_location& _where) override
    {
        mu.lock();
        LOG(ERROR) << _where.function_name() << "(): " << _entry << endl;
        mu.unlock();
    }
};