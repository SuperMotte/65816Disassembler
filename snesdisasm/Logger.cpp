#include "Logger.hpp"

using namespace std;

Logger::Logger(const std::string &logPath) {
    mLogStream.open(logPath, std::ios::out);
}

Logger::~Logger() {
    closeLog();
}

Logger &Logger::Instance() {
    static Logger logger;

    return logger;
}

void Logger::closeLog() {
    mLogStream.close();
}

void Logger::writeLog(const std::string &kindOfMessage, const std::string &message, const std::string &location) {
    mLastLog = kindOfMessage + message + " IN " + location;
    mLogStream << mLastLog << std::endl;
}

void Logger::log(Logger::LogType type, const string &message, const string &location) {
    switch(type) {
    case Logger::LogType::ERROR:
        writeLog("FEHLER : ", message, location);
        break;
    case Logger::LogType::STATE:
        writeLog("STATUS : ", message, location);
        break;
    case Logger::LogType::WARNING:
        writeLog("WARNUNG: ", message , location);
        break;
    case Logger::LogType::HINT:
        writeLog("HINWEIS: ", message , location);
        break;
    case Logger::LogType::MESSAGE:
        mLogStream << message << std::endl;
        break;
    }
}

//Einsatz um etwas voneinander deutlich abzutrennen (Beispiel nach ProgrammEnde um vom nächsten Start abgetrennt zu sein)
void Logger::logBreak() {
    mLastLog.clear();
    mLastLog.resize(256);
    mLastLog.assign(253, '-');
    mLogStream << mLastLog << std::endl;
}

const std::string &Logger::lastLog() const {
    return mLastLog;
}

/*
Initialization
begin
{$IF OverwriteLog}
  FLogRWop := SDL_RWFromFile('Verlauf.Log', 'w+t'); //Der Log wird überschrieben
{$ELSE}
  FLogRWop := SDL_RWFromFile('Verlauf.Log', 'at' ); //Der Log wird erweitert
{$IFEND}
  LogBreak;
  Log     ('***** ProgrammStart *****'                             );
  Log     ('Uhrzeit: '+TimeToStr(Time)+' / Datum: '+DateToStr(Date));
  Log     ('Version: '+VersionName                                 );
  Log     ('Programmpfad: "'+ExtractFilePath(ParamStr(0))+'"'      );
end;

Finalization
begin
  Log     ('***** ProgrammEnde  *****'                             );
  Log     ('Uhrzeit: '+TimeToStr(Time)+' Datum: '+DateToStr(Date  ));
  LogBreak;
end;
*/
