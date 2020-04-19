#pragma once

#include "LoggingFacility.h"
#include "easylogging++.h"

#include <iostream>

class StandardOutputLogger : public LoggingFacility 
{
public:
    virtual void writeInfoEntry(std::string_view entry) override 
    {
        cout << "[INFO] " << entry << endl;
    }
    virtual void writeWarnEntry(std::string_view entry) override 
    {
        cout << "[WARNING] " << entry << endl;
    }
    virtual void writeErrorEntry(std::string_view entry) override 
    {
        cout << "[ERROR] " << entry << endl;
    }
};