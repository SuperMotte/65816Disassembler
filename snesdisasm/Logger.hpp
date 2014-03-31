#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

//shorthand for the lazy
#define LOG_SRC(type, msg) Logger::Instance().log((Logger::LogType::type), (msg), (__FILE__":") + std::to_string(__LINE__))

class Logger {
private:
    std::ofstream mLogStream;
    std::string mLastLog;
    
    void writeLog(const std::string &message, const std::string& location, const std::string& kindOfMessage);
public:
    enum class LogType{
        ERROR,
        STATE,
        WARNING,
        HINT,
        MESSAGE
    };
    
    Logger(const std::string& logPath = "Verlauf.log");
    ~Logger();
    static Logger& Instance(); // returns a signleton reference

    void closeLog();
    
    void log(LogType type, const std::string& message, const std::string& location);
    void logBreak();
    const std::string& lastLog() const;
};

#endif // LOGGER_H
