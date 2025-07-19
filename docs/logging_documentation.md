# C++ Wt Advanced Logging Framework

## Overview

This is a thread-safe, configurable logging framework designed for C++ Wt web applications. It provides file-based logging with automatic rotation, multiple log levels, and flexible configuration through both XML files and environment variables.

## Features

- **Thread-Safe**: Uses mutex protection for concurrent access
- **Configurable Log Levels**: DEBUG, INFO, WARN, ERROR, NONE
- **Automatic File Rotation**: Creates new log files when size exceeds configured limit (default: 1GB)
- **Timestamped Entries**: Precise timestamps with milliseconds
- **Flexible Configuration**: XML configuration file with environment variable overrides
- **Append-Only Logging**: Never overwrites existing log data
- **Console Output Control**: Can enable/disable console output
- **Singleton Pattern**: Single instance across your application

## Quick Start

### 1. Basic Usage

```cpp
#include "Logger.h"  // Include the logger

int main() {
    Logger& logger = Logger::getInstance();
    
    logger.info("Application started");
    logger.debug("Debug information");
    logger.warn("Warning message");
    logger.error("Error occurred");
    
    return 0;
}
```

### 2. Configuration File

Create a `wt_config.xml` file in your project root:

```xml
<?xml version="1.0" encoding="utf-8"?>
<server>
    <application-settings location="*">
        <properties>
            <!-- Logging Configuration -->
            <property name="logging.level">DEBUG</property>
            <property name="logging.directory">logs</property>
            <property name="logging.base-filename">app</property>
            <property name="logging.max-file-size">1073741824</property>
            <property name="logging.enable-console">true</property>
        </properties>
    </application-settings>
    
    <!-- HTTP Configuration -->
    <http-address>0.0.0.0</http-address>
    <http-port>8080</http-port>
</server>
```

### 3. Environment Variable Override

```bash
export logging.level=DEBUG
./your_application
```

## Configuration Reference

Structured as a monolithic application while keeping all functionality unchanged. Let me create the separate files:Perfect! I've successfully split your monolithic application into modular files while keeping all functionality unchanged. Here's the new structure:

## File Structure

### 1. **logging.hpp** - Header File
- Contains `LogLevel` enum and `Logger` class declarations
- Header guards to prevent multiple inclusions
- All public and private method declarations
- Member variable declarations

### 2. **logging.cpp** - Implementation File
- All Logger method implementations
- Includes only necessary headers for implementation
- Contains all the logging logic, file handling, configuration parsing

### 3. **main.cpp** - Main Application File
- Wt application class (`HelloApplication`)
- Application factory function
- Main function with server setup
- Includes `logging.hpp` to use the Logger

### 4. **CMakeLists.txt** - Updated Build Configuration
- Now compiles multiple source files
- Same build options and customizations

## Usage

### Build and Run:
```bash
# Same build process as before
mkdir build
cd build
cmake ..
make

# Run the application
./SimpleLoggingApp
```

## Benefits of Modular Structure

### **Maintainability**
- Easier to find and modify specific functionality
- Clear separation of concerns
- Reduced compilation time (only recompile changed files)

### **Reusability**
- Logger can be easily included in other projects
- Just copy `logging.hpp` and `logging.cpp`
- Clean API in header file

### **Development**
- Multiple developers can work on different files
- Header file serves as documentation
- Cleaner include dependencies

## Integration in Other Projects

To use the logging system in other C++ projects:

```cpp
// Just include the header
#include "logging.hpp"

int main() {
    Logger& logger = Logger::getInstance();
    logger.info("My application started");
    return 0;
}
```

Then compile with:
```cmake
add_executable(my_app my_main.cpp logging.cpp)
```

## File Responsibilities

| File | Purpose | Contains |
|------|---------|----------|
| `logging.hpp` | Interface | Declarations, enums, API |
| `logging.cpp` | Implementation | All logging logic, file I/O |
| `main.cpp` | Application | Wt app, UI, main function |
| `wt_config.xml` | Configuration | Settings and properties |

All functionality remains exactly the same - configuration system, file rotation, thread safety, environment variable support, and all logging features work identically to the original monolithic version!

### Configuration Properties

| Property | Default | Description |
|----------|---------|-------------|
| `logging.level` | `INFO` | Log level: DEBUG, INFO, WARN, ERROR, NONE |
| `logging.directory` | `logs` | Directory where log files are stored |
| `logging.base-filename` | `app` | Base name for log files |
| `logging.max-file-size` | `1073741824` | Maximum file size in bytes (1GB) |
| `logging.enable-console` | `true` | Enable/disable console output |

### Configuration Priority

1. **Environment Variables** (highest priority)
2. **XML Configuration File** (`wt_config.xml`)
3. **Default Values** (lowest priority)

### Log Levels

| Level | Numeric Value | Description |
|-------|---------------|-------------|
| `NONE` | 0 | No logging output |
| `ERROR` | 1 | Error messages only |
| `WARN` | 2 | Warnings and errors |
| `INFO` | 3 | Information, warnings, and errors |
| `DEBUG` | 4 | All messages including debug info |

## API Reference

### Core Methods

#### `Logger::getInstance()`
Returns the singleton Logger instance.

```cpp
Logger& logger = Logger::getInstance();
```

#### Logging Methods

```cpp
// Generic logging with level parameter
logger.log(LogLevel::DEBUG, "Debug message");

// Convenience methods
logger.debug("Debug information");
logger.info("General information");
logger.warn("Warning message");
logger.error("Error occurred");
```

#### Configuration Methods

```cpp
// Set log level programmatically
logger.setLogLevel(LogLevel::DEBUG);
logger.setLogLevel("DEBUG");

// Get current log level
LogLevel level = logger.getLogLevel();
std::string levelStr = logger.getCurrentLevelString();
```

## File Structure

### Log File Naming

Log files are automatically named with timestamps:
```
logs/
├── app_20250717_143022.log
├── app_20250717_150133.log
└── app_20250717_153045.log
```

### Log Entry Format

```
[2025-07-17 14:30:22.123] [INFO ] Application started
[2025-07-17 14:30:22.124] [DEBUG] Session ID: abc123def456
[2025-07-17 14:30:25.567] [WARN ] Button clicked 5 times
[2025-07-17 14:30:28.890] [ERROR] Database connection failed
```

## Integration Guide

### 1. Add to Existing Project

Copy the Logger class definition and implementation to your project:

```cpp
// Include these headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <filesystem>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <map>
#include <regex>

// Add the Logger class definition and implementation
// (Copy from the provided source code)
```

### 2. CMakeLists.txt Configuration

```cmake
# Add to your CMakeLists.txt
target_compile_options(${PROJECT_NAME} PRIVATE 
    -Wall 
    -Wextra 
    -Wno-deprecated-declarations  # Suppress Wt library warnings
)

# Copy configuration file to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/wt_config.xml
        ${CMAKE_CURRENT_BINARY_DIR}/wt_config.xml
    COMMENT "Copying wt_config.xml to build directory"
)

# Create logs directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory logs
    COMMENT "Creating logs directory"
)
```

### 3. Usage Examples

### Basic Application Logging

```cpp
int main(int argc, char** argv) {
    Logger& logger = Logger::getInstance();
    
    logger.info("Application starting...");
    
    try {
        // Your application logic
        logger.debug("Processing user request");
        
        // Simulate some work
        if (someCondition) {
            logger.warn("Unusual condition detected");
        }
        
        logger.info("Request processed successfully");
        
    } catch (const std::exception& e) {
        logger.error("Exception caught: " + std::string(e.what()));
        return 1;
    }
    
    logger.info("Application terminated normally");
    return 0;
}
```

### In Wt Applications

```cpp
class MyWtApplication : public Wt::WApplication {
public:
    MyWtApplication(const Wt::WEnvironment& env) : WApplication(env) {
        Logger& logger = Logger::getInstance();
        logger.info("Application session started: " + sessionId());
        
        // Your application code here
    }
};
```


### 4. Production vs Development Configuration

**Development (`wt_config.xml`):**
```xml
<property name="logging.level">DEBUG</property>
<property name="logging.enable-console">true</property>
```

**Production (`wt_config.xml`):**
```xml
<property name="logging.level">WARN</property>
<property name="logging.enable-console">false</property>
```

### 5. Environment-Based Configuration

```bash
# Development
export logging.level=DEBUG
./app

# Testing
export logging.level=INFO
./app

# Production
export logging.level=ERROR
./app

# Debugging production issues
export logging.level=DEBUG
./app
```

## 6. Performance Considerations

- **File I/O**: Logging writes are buffered and flushed immediately
- **Thread Safety**: Mutex locking may introduce slight overhead in highly concurrent scenarios
- **Memory Usage**: Minimal memory footprint with string-based configuration cache
- **File Rotation**: Automatic rotation prevents disk space issues

## Troubleshooting

### Common Issues

#### 1. Log Files Not Created
**Problem**: No log files appear in the logs directory.
**Solution**: 
- Check file permissions for the logs directory
- Verify the application has write access to the current directory
- Check if logging level is set to NONE

#### 2. Configuration Not Loading
**Problem**: Settings from `wt_config.xml` are ignored.
**Solution**:
- Ensure `wt_config.xml` is in the same directory as the executable
- Verify XML syntax is correct
- Check console output for configuration loading messages

#### 3. Console Output Missing
**Problem**: No logging output appears in console.
**Solution**:
- Check `logging.enable-console` property is set to `true`
- Verify log level allows the messages you expect to see

### Debug Mode

Enable maximum verbosity for troubleshooting:

```bash
export logging.level=DEBUG
export logging.enable-console=true
./your_application
```

### 7. Best Practices

### 1. Log Level Guidelines

- **DEBUG**: Detailed information for diagnosing problems
- **INFO**: General information about program execution
- **WARN**: Something unexpected happened, but the application can continue
- **ERROR**: A serious problem occurred

### 2. Message Format

```cpp
// Good: Descriptive and contextual
logger.info("User login successful: username=" + username + ", session=" + sessionId);

// Avoid: Vague messages
logger.info("Success");
```

### 3. Error Handling

```cpp
try {
    // Risky operation
} catch (const std::exception& e) {
    logger.error("Database operation failed: " + std::string(e.what()) + 
                 ", table=" + tableName + ", operation=" + operation);
    throw; // Re-throw if needed
}
```

### 4. Performance Sensitive Code

```cpp
// Check log level before expensive string operations
if (logger.getLogLevel() >= LogLevel::DEBUG) {
    logger.debug("Expensive debug info: " + generateExpensiveDebugInfo());
}
```

Let me update the configuration to set DEBUG logging level and then show you how to reduce logging for normal operations:Perfect! I've configured the application for DEBUG testing and created a comprehensive logging configuration guide. Here's what to test:

## Current Setup: DEBUG Mode

The application is now configured for **maximum verbosity** to test all logging features.

### Test the DEBUG Configuration:

```bash
cd build
make
./SimpleLoggingApp
```

### What You'll See:

**1. Startup Messages:**
```
[2025-07-18 09:50:15.123] [INFO ] Application starting...
[2025-07-18 09:50:15.124] [DEBUG] Command line arguments count: 1
[2025-07-18 09:50:15.125] [INFO ] Wt server configured, attempting to start...
[2025-07-18 09:50:18.375] [DEBUG] Session ID: ABC123DEF456
[2025-07-18 09:50:18.376] [DEBUG] Using default Wt theme with Bootstrap CSS override
```

**2. Button Click Testing:**
- Click the button multiple times to see:
  - `[DEBUG]` messages for each click
  - `[INFO ]` messages for updates  
  - `[WARN ]` message at 5 clicks
  - `[ERROR]` messages after 10 clicks

**3. File Logging:**
- Check `logs/app_YYYYMMDD_HHMMSS.log` for all messages
- Both console and file will show the same content

## For Normal Operations:

After testing, change the logging level for production:

### Quick Environment Override:
```bash
# Minimal logging for normal operations
export logging.level=ERROR
./SimpleLoggingApp

# Or completely silent
export logging.level=NONE  
./SimpleLoggingApp
```

### Permanent Configuration:
Edit `wt_config.xml` and change:
```xml
<property name="logging.level">ERROR</property>  <!-- Only errors -->
<property name="logging.enable-console">false</property>  <!-- No console noise -->
```

This gives you the flexibility to:
- **Debug with full verbosity** when developing/troubleshooting
- **Run quietly in production** with minimal logging overhead
- **Switch configurations easily** via environment variables

Test it now with DEBUG to see all the logging features in action!


## License and Support

This logging framework is designed to be modular and reusable across C++ Wt projects. Feel free to modify and extend it according to your specific requirements.

For questions or issues, refer to the source code comments and this documentation.