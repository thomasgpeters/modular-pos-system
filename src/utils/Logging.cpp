#include "../../include/utils/Logging.hpp"

#include <iostream>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <regex>

// ============================================================================
// Logger Implementation
// ============================================================================

Logger::Logger() 
    : currentLevel_(LogLevel::INFO)
    , logDirectory_("logs")
    , baseFileName_("app")
    , maxFileSize_(1073741824) // 1GB in bytes
    , enableConsole_(true)
{
    // Create logs directory if it doesn't exist
    std::filesystem::create_directories(logDirectory_);
    
    // Initialize from configuration sources
    initializeFromConfiguration();
    
    // Open initial log file
    currentFileName_ = getCurrentLogFileName();
    logFile_.open(currentFileName_, std::ios::app);
    
    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << currentFileName_ << std::endl;
    }
}

Logger::~Logger() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex_);
    currentLevel_ = level;
}

void Logger::setLogLevel(const std::string& levelStr) {
    setLogLevel(stringToLevel(levelStr));
}

LogLevel Logger::getLogLevel() const {
    std::lock_guard<std::mutex> lock(logMutex_);
    return currentLevel_;
}

std::string Logger::getCurrentLevelString() const {
    std::lock_guard<std::mutex> lock(logMutex_);
    return levelToString(currentLevel_);
}

void Logger::initializeFromConfiguration() {
    // Load configuration from file first, then environment variables
    loadConfigurationFromFile();
    loadConfigurationFromEnvironment();
    
    // Apply configuration
    std::string levelStr = readConfigProperty("logging.level", "INFO");
    setLogLevel(levelStr);
    
    logDirectory_ = readConfigProperty("logging.directory", "logs");
    baseFileName_ = readConfigProperty("logging.base-filename", "app");
    
    std::string maxSizeStr = readConfigProperty("logging.max-file-size", "1073741824");
    maxFileSize_ = std::stoull(maxSizeStr);
    
    std::string consoleStr = readConfigProperty("logging.enable-console", "true");
    enableConsole_ = (consoleStr == "true" || consoleStr == "1");
    
    std::cout << "Logging configuration:" << std::endl;
    std::cout << "  Level: " << levelStr << std::endl;
    std::cout << "  Directory: " << logDirectory_ << std::endl;
    std::cout << "  Base filename: " << baseFileName_ << std::endl;
    std::cout << "  Max file size: " << maxFileSize_ << " bytes" << std::endl;
    std::cout << "  Console output: " << (enableConsole_ ? "enabled" : "disabled") << std::endl;
}

void Logger::loadConfigurationFromFile() {
    const std::string configFile = "wt_config.xml";
    
    if (!std::filesystem::exists(configFile)) {
        std::cout << "Configuration file '" << configFile << "' not found. Using defaults." << std::endl;
        return;
    }
    
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cout << "Could not open configuration file: " << configFile << std::endl;
        return;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Simple XML property extraction using regex
    std::regex propertyRegex("<property\\s+name\\s*=\\s*\"([^\"]+)\"\\s*>([^<]+)</property>");
    std::smatch match;
    
    std::string::const_iterator start = content.cbegin();
    while (std::regex_search(start, content.cend(), match, propertyRegex)) {
        std::string name = match[1].str();
        std::string value = match[2].str();
        
        // Trim whitespace
        value.erase(0, value.find_first_not_of(" \t\n\r"));
        value.erase(value.find_last_not_of(" \t\n\r") + 1);
        
        configProperties_[name] = value;
        start = match.suffix().first;
    }
    
    std::cout << "Loaded " << configProperties_.size() << " properties from " << configFile << std::endl;
}

void Logger::loadConfigurationFromEnvironment() {
    // Environment variables override config file settings
    const char* envLevel = std::getenv("logging.level");
    if (envLevel != nullptr) {
        configProperties_["logging.level"] = std::string(envLevel);
        std::cout << "Log level overridden by environment variable: " << envLevel << std::endl;
    }
    
    const char* envDir = std::getenv("logging.directory");
    if (envDir != nullptr) {
        configProperties_["logging.directory"] = std::string(envDir);
    }
}

std::string Logger::readConfigProperty(const std::string& propertyName, const std::string& defaultValue) {
    auto it = configProperties_.find(propertyName);
    if (it != configProperties_.end()) {
        return it->second;
    }
    return defaultValue;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    // Check if this level should be logged
    if (currentLevel_ == LogLevel::NONE || level > currentLevel_) {
        return;
    }
    
    // Create log entry
    std::stringstream logEntry;
    logEntry << "[" << getCurrentTimestamp() << "] "
             << "[" << levelToString(level) << "] "
             << message << std::endl;
    
    // Write to file
    writeToFile(logEntry.str());
    
    // Output to console if enabled
    if (enableConsole_) {
        std::cout << logEntry.str();
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::writeToFile(const std::string& logEntry) {
    if (!logFile_.is_open()) {
        return;
    }
    
    // Check if file size exceeds limit
    logFile_.seekp(0, std::ios::end);
    std::size_t currentSize = logFile_.tellp();
    
    if (currentSize >= maxFileSize_) {
        rotateLogFile();
    }
    
    // Write the log entry
    logFile_ << logEntry;
    logFile_.flush();
}

void Logger::rotateLogFile() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
    
    // Generate new filename with timestamp
    currentFileName_ = getCurrentLogFileName();
    
    // Open new file
    logFile_.open(currentFileName_, std::ios::app);
    
    if (logFile_.is_open()) {
        info("Log file rotated to: " + currentFileName_);
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::NONE:  return "NONE ";
        default: return "UNKNOWN";
    }
}

LogLevel Logger::stringToLevel(const std::string& levelStr) {
    std::string upper = levelStr;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "DEBUG") return LogLevel::DEBUG;
    if (upper == "INFO")  return LogLevel::INFO;
    if (upper == "WARN")  return LogLevel::WARN;
    if (upper == "ERROR") return LogLevel::ERROR;
    if (upper == "NONE")  return LogLevel::NONE;
    
    return LogLevel::INFO; // Default fallback
}

std::string Logger::getCurrentLogFileName() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << logDirectory_ << "/" << baseFileName_ << "_"
       << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S")
       << ".log";
    
    return ss.str();
}
