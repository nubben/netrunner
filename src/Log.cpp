#include "Log.h"

#include "CommandLineParams.h"
#include <streambuf>
#include <map>
#include "StringUtils.h"

namespace {
    class NoStream : public std::streambuf {
    public:
        int overflow(int c) { return c; }
    };

    NoStream noStream;
    std::ostream nothing (&noStream);
    std::map<LogType, Logger> loggerCache;

    
    std::ostream& log(LogType lt) {
        auto cacheEntry = loggerCache.find(lt);
        if (cacheEntry == loggerCache.end()) {
            auto& outStream = ((lt == LogType::ERROR) ? std::cerr : std::cout);
            auto newEntry = std::make_pair(lt, Logger(lt, outStream));
            loggerCache.insert(newEntry);
        }
        return loggerCache[lt]();
}
}

Logger::Logger() :
    logType(LogType::DEBUG),
    outStream(std::cout),
    shouldLog(true) {
}

Logger::Logger(LogType lt, std::ostream& out) : 
    logType(lt), 
    outStream(out), 
    shouldLog(false) {
    std::string specifiedLog = toLowercase(getRelativeCLParam("-log", 1));
    LogType logLevel = LogType::ERROR;
    if (specifiedLog == "warning") {
        logLevel = LogType::WARNING;
    } else if (specifiedLog == "info") {
        logLevel = LogType::INFO;
    } else if (specifiedLog == "debug") {
        logLevel = LogType::DEBUG;
    }

    if (lt <= logLevel) {
        shouldLog = true;
    }
}

std::ostream& Logger::operator()() {
    if (shouldLog) {
        return outStream;
    }
    return nothing;
}

std::ostream& logError() {
    return log(LogType::ERROR);
}

std::ostream& logWarning() {
    return log(LogType::WARNING);
}

std::ostream& logInfo() {
    return log(LogType::INFO);
}

std::ostream& logDebug() {
    return log(LogType::DEBUG);
}
