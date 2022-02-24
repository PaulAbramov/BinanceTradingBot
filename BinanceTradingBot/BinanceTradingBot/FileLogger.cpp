#include "FileLogger.h"

void FileLogger::WriteInfoEntry(const std::string _entry, const std::source_location& _where)
{
    static std::mutex mu;
    mu.lock();
    //LOG(INFO) << _where.file_name() << "(" << _where.line() << ":" << _where.column() << ") " << _where.function_name() << "(): " << _entry << endl;
    LOG(INFO) << _where.function_name() << "(): " << _entry << std::endl;
    mu.unlock();
}

void FileLogger::WriteWarnEntry(const std::string _entry, const std::source_location& _where)
{
    static std::mutex mu;
    mu.lock();
    LOG(WARNING) << _where.function_name() << "(): " << _entry << std::endl;
    mu.unlock();
}

void FileLogger::WriteErrorEntry(const std::string _entry, const std::source_location& _where)
{
    static std::mutex mu;
    mu.lock();
    LOG(ERROR) << _where.function_name() << "(): " << _entry << std::endl;
    mu.unlock();
}