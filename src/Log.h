#ifndef LOG_H
#define LOG_H

#include <iostream>

/*
  USAGE:
    The functions provided returns standard output/error streams depending
    on the log level specified at startup. Example usage:

    logWarning() << "WARNING WARNING" << std::endl;

    You can create custom logger objects if you want different output
    streams. You can then invoke these objects with function syntax:

    Logger loggg (LogLevel::INFO, std::cerr);
    loggg() << "HEY" << std::endl;

    Use the command line flag "-log <logtype>" to print logtypes higher
    than error. For example, if you specify "-log info", then the program
    will print out info, and all lower prioritized logtypes (warning+error).
 */

enum class LogType {
    ERROR,
    WARNING,
    INFO,
    DEBUG,    
};

class Logger {
public:
    Logger();
    Logger(LogType lt, std::ostream& out);
    std::ostream& operator()();
private:
    LogType logType;
    std::ostream& outStream;
    bool shouldLog;
};

std::ostream& logError();
std::ostream& logWarning();
std::ostream& logInfo();
std::ostream& logDebug();

#endif
