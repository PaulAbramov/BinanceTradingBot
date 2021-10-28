#pragma once

#include "LoggingFacility.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

class FileLogger : public LoggingFacility
{
public:
    virtual void writeInfoEntry(string_view _entry) override 
    {
        LOG(INFO) << _entry << endl;
    }
    virtual void writeWarnEntry(string_view _entry) override 
    {
        LOG(WARNING) << _entry << endl;
    }
    virtual void writeErrorEntry(string_view _entry) override 
    {
        LOG(ERROR) << _entry << endl;
    }
};