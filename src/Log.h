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
// I'd like logging macro(s) that auto include __FILE__ and __LINE__ for where the output is coming from (would be cool if we could include function/method name (and class if method)) - odilitime 17/08/02
enum class LogType {
    ERROR, // issue
    WARNING, // potential issue
    NOTICE, // basic user messages, Default include NOTICE, WARNING, ERROR
    INFO, // basic 10k ft overview of what's going on
    DBUG, // can't be DEBUG in clang
    CALLLIST // track each function call entrance and exit
};
// network logging should be separate

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

// can we have some sort of macro to make these noOPs if compile as such
// no need to compile in all the debug logging calls in release
std::ostream& logError();
std::ostream& logWarning();
std::ostream& logNotice();
std::ostream& logInfo();
std::ostream& logDebug();

#endif
