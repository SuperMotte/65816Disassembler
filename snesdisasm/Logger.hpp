#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

//shorthand for the lazy
#define LOG_SRC(type, msg) Logger::Instance().log((Logger::LogType::type), (msg), (__FILE__":") + std::to_string(__LINE__))

/*! \brief A Logger to write additional status informations of the program to a file.
 */
class Logger {
  private:
    std::ofstream mLogStream;
    std::string mLastLog;

    void writeLog(const std::string &kindOfMessage, const std::string &message, const std::string &location);
  public:
    enum class LogType {
        ERROR,
        STATE,
        WARNING,
        HINT,
        MESSAGE
    };

    /*! \brief Constructs the logger
     *
     * \param logPath the path to a target file to log to
     */
    Logger(const std::string &logPath = "Verlauf.log");

    /*! \brief Destroys the logger
     *
     * Closes the log file
     */
    ~Logger();

    /*! \brief Returns a singleton reference.
     *
     * The singleton is created when this function is called first and a referebce to the same instance is
     * returned thereafter.
     */
    static Logger &Instance();

    /*! \brief Closes the log file
     *
     * Be aware that further calls to \see log throw.
     */
    void closeLog();

    /*! \brief Writes a log entry
     *
     *  \param LogType is the type of the entry
     *  \param message is the actual text
     *  \param location specifies where the message occured
     *
     *  You may use the LOG_SRC macro to log messages that specify the source file and line number as location.
     */
    void log(LogType type, const std::string &message, const std::string &location);

    /*! \brief Start a new paragraph within the log file
     */
    void logBreak();

    /*! \brief Return the last line as it was written to the log file
     */
    const std::string &lastLog() const;
};

#endif // LOGGER_H
