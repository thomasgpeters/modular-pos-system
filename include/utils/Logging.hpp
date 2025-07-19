#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>
#include <mutex>
#include <fstream>
#include <map>

// ============================================================================
// Logging Framework Header
// ============================================================================

enum class LogLevel {
    NONE = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4
};

class Logger {
public:
    static Logger& getInstance();
    
    void setLogLevel(LogLevel level);
    void setLogLevel(const std::string& levelStr);
    LogLevel getLogLevel() const;
    std::string getCurrentLevelString() const;
    
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    
    void initializeFromConfiguration();
    
private:
    Logger();
    ~Logger();
    
    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void writeToFile(const std::string& logEntry);
    void rotateLogFile();
    std::string getCurrentTimestamp();
    std::string levelToString(LogLevel level) const;
    LogLevel stringToLevel(const std::string& levelStr);
    std::string getCurrentLogFileName();
    
    // Configuration methods
    std::string readConfigProperty(const std::string& propertyName, const std::string& defaultValue = "");
    void loadConfigurationFromFile();
    void loadConfigurationFromEnvironment();
    
    // Member variables
    LogLevel currentLevel_;
    std::string logDirectory_;
    std::string baseFileName_;
    std::size_t maxFileSize_;
    mutable std::mutex logMutex_;
    std::ofstream logFile_;
    std::string currentFileName_;
    bool enableConsole_;
    
    // Configuration cache
    std::map<std::string, std::string> configProperties_;
};

#endif // LOGGING_HPP
