#pragma once

#include "LoggingFacility.h"
#include "easylogging++.h"

#include <iostream>

class StandardOutputLogger : public LoggingFacility 
{
public:
    virtual void writeInfoEntry(string_view _entry) override 
    {
        cout << "[INFO] " << _entry << endl;
    }
    virtual void writeWarnEntry(string_view _entry) override 
    {
        cout << "[WARNING] " << _entry << endl;
    }
    virtual void writeErrorEntry(string_view _entry) override 
    {
        cout << "[ERROR] " << _entry << endl;
    }
};