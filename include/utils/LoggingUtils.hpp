#ifndef LOGGINGUTILS_HPP
#define LOGGINGUTILS_HPP

#include <string>
#include <sstream>

// ============================================================================
// Logging Utility Functions
// ============================================================================

/**
 * @brief Utility class for logging-related string conversions and formatting
 */
class LoggingUtils {
public:
    /**
     * @brief Convert boolean to string
     * @param value Boolean value to convert
     * @return "true" or "false"
     */
    static std::string boolToString(bool value) {
        return value ? "true" : "false";
    }
    
    /**
     * @brief Convert any numeric type to string
     * @param value Numeric value to convert
     * @return String representation
     */
    template<typename T>
    static std::string toString(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
    
    /**
     * @brief Format log message with key-value pairs
     * @param key The key/label
     * @param value The value (any type)
     * @return Formatted string "key: value"
     */
    template<typename T>
    static std::string formatKeyValue(const std::string& key, const T& value) {
        std::ostringstream oss;
        oss << key << ": " << value;
        return oss.str();
    }
    
    /**
     * @brief Format log message with key-boolean value pair
     * @param key The key/label
     * @param value Boolean value
     * @return Formatted string "key: true/false"
     */
    static std::string formatKeyValue(const std::string& key, bool value) {
        return key + ": " + boolToString(value);
    }
    
    /**
     * @brief Format multiple key-value pairs into a single string
     * @param prefix Optional prefix for the line
     * @param keyValues Pairs of keys and values
     * @return Formatted string
     */
    template<typename... Args>
    static std::string formatMultiple(const std::string& prefix, Args... keyValues) {
        std::ostringstream oss;
        if (!prefix.empty()) {
            oss << prefix << " - ";
        }
        formatMultipleHelper(oss, keyValues...);
        return oss.str();
    }
    
    /**
     * @brief Create a formatted status line
     * @param operation The operation name
     * @param status Status (success/error/etc)
     * @param details Optional details
     * @return Formatted status string
     */
    static std::string formatStatus(const std::string& operation, 
                                   const std::string& status, 
                                   const std::string& details = "") {
        std::ostringstream oss;
        oss << "[" << operation << "] " << status;
        if (!details.empty()) {
            oss << " - " << details;
        }
        return oss.str();
    }
    
    /**
     * @brief Create an error message with context
     * @param component Component name
     * @param operation Operation that failed
     * @param error Error description
     * @return Formatted error string
     */
    static std::string formatError(const std::string& component,
                                  const std::string& operation,
                                  const std::string& error) {
        std::ostringstream oss;
        oss << "[" << component << "] ERROR in " << operation << ": " << error;
        return oss.str();
    }

private:
    // Helper for recursive template parameter processing
    template<typename T>
    static void formatMultipleHelper(std::ostringstream& oss, const std::string& key, const T& value) {
        oss << key << ": " << value;
    }
    
    template<typename T, typename... Args>
    static void formatMultipleHelper(std::ostringstream& oss, const std::string& key, const T& value, Args... rest) {
        oss << key << ": " << value << ", ";
        formatMultipleHelper(oss, rest...);
    }
};

// ============================================================================
// Convenient Macros for Common Logging Patterns
// ============================================================================

/**
 * @brief Log a key-value pair with the logger
 * @param logger_ref Logger instance
 * @param level_method Log level method (info, debug, warn, error)
 * @param key_name Key name
 * @param key_value Value to log
 */
#define LOG_KEY_VALUE(logger_ref, level_method, key_name, key_value) \
    logger_ref.level_method(LoggingUtils::formatKeyValue(key_name, key_value))

/**
 * @brief Log a boolean configuration value
 * @param logger_ref Logger instance
 * @param level_method Log level method
 * @param config_key Configuration key
 * @param config_value Boolean value
 */
#define LOG_CONFIG_BOOL(logger_ref, level_method, config_key, config_value) \
    logger_ref.level_method("  - " + LoggingUtils::formatKeyValue(config_key, config_value))

/**
 * @brief Log a string configuration value
 * @param logger_ref Logger instance
 * @param level_method Log level method
 * @param config_key Configuration key
 * @param config_value String value
 */
#define LOG_CONFIG_STRING(logger_ref, level_method, config_key, config_value) \
    logger_ref.level_method("  - " + LoggingUtils::formatKeyValue(config_key, config_value))

/**
 * @brief Log an operation status
 * @param logger_ref Logger instance
 * @param operation_name Operation name
 * @param is_success Whether operation succeeded
 */
#define LOG_OPERATION_STATUS(logger_ref, operation_name, is_success) \
    logger_ref.info(LoggingUtils::formatStatus(operation_name, is_success ? "SUCCESS" : "FAILED"))

/**
 * @brief Log an error with component context
 * @param logger_ref Logger instance
 * @param component_name Component name
 * @param operation_name Operation that failed
 * @param error_msg Error message
 */
#define LOG_COMPONENT_ERROR(logger_ref, component_name, operation_name, error_msg) \
    logger_ref.error(LoggingUtils::formatError(component_name, operation_name, error_msg))

#endif // LOGGINGUTILS_HPP
