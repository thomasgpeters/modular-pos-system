# Debugging the Modular POS System

This guide describes how to build and run the Restaurant POS application in debug mode for development and troubleshooting.

## Table of Contents

1. [Building in Debug Mode](#building-in-debug-mode)
2. [Running with Debug Options](#running-with-debug-options)
3. [Logging Configuration](#logging-configuration)
4. [IDE Debugging Setup](#ide-debugging-setup)
5. [Common Debugging Scenarios](#common-debugging-scenarios)
6. [Wt Framework Debugging](#wt-framework-debugging)
7. [Network Debugging](#network-debugging)

---

## Building in Debug Mode

### Using CMake

```bash
# Create a debug build directory
mkdir -p build-debug && cd build-debug

# Configure with debug flags
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
make -j$(nproc)
```

The debug build includes:
- Full symbol information (`-g`)
- No optimization (`-O0`)
- Debug assertions enabled
- AddressSanitizer support (optional)

### Using Makefile

```bash
# Build debug version
make debug

# Run debug version
make run-debug
```

### Manual Compilation with Debug Flags

```bash
g++ -std=c++17 -g -O0 -DDEBUG \
    -fsanitize=address -fsanitize=undefined \
    -o restaurant_pos_debug \
    src/*.cpp src/**/*.cpp \
    $(pkg-config --cflags --libs wt wthttp)
```

### Debug Build Options

| Flag | Purpose |
|------|---------|
| `-g` | Include debug symbols |
| `-O0` | Disable optimization |
| `-DDEBUG` | Enable debug code paths |
| `-fsanitize=address` | Memory error detection |
| `-fsanitize=undefined` | Undefined behavior detection |
| `-Wall -Wextra` | Enable all warnings |

---

## Running with Debug Options

### Basic Debug Run

```bash
# From project root
./build-debug/restaurant_pos \
    --config ./wt_config.xml \
    --docroot . \
    --http-port 8080
```

### Wt Server Debug Options

```bash
./restaurant_pos \
    --config ./wt_config.xml \
    --docroot . \
    --http-port 8080 \
    --accesslog access.log \
    --no-compression \
    --verbose
```

### Available Command Line Options

| Option | Description |
|--------|-------------|
| `--http-port <port>` | HTTP listening port (default: 8080) |
| `--http-address <addr>` | Bind address (default: 0.0.0.0) |
| `--docroot <path>` | Document root for static files |
| `--config <file>` | Wt configuration file |
| `--accesslog <file>` | HTTP access log file |
| `--no-compression` | Disable response compression |
| `--verbose` | Verbose server output |

### Environment Variables for Debugging

```bash
# Enable Wt debug output
export WT_DEBUG=1

# Set log level
export POS_LOG_LEVEL=debug

# Enable core dumps
ulimit -c unlimited

# Run with environment
WT_DEBUG=1 POS_LOG_LEVEL=debug ./restaurant_pos --config ./wt_config.xml
```

---

## Logging Configuration

### Log Levels

The application supports four log levels:

| Level | Description | Use Case |
|-------|-------------|----------|
| `error` | Critical errors only | Production |
| `warn` | Warnings and errors | Production monitoring |
| `info` | General information | Normal development |
| `debug` | Detailed debugging | Troubleshooting |

### Configure via pos_config.xml

```xml
<logging>
    <level>debug</level>           <!-- debug, info, warn, error -->
    <file-logging>true</file-logging>
    <log-directory>./logs</log-directory>
    <max-log-size>10485760</max-log-size>  <!-- 10MB -->
    <max-log-files>5</max-log-files>
</logging>
```

### Configure via wt_config.xml

```xml
<server>
    <application-settings location="*">
        <log-file>wt-debug.log</log-file>
        <log-config>* -info:WebRequest</log-config>
        <debug>true</debug>
    </application-settings>
</server>
```

### Log Configuration Options

| Setting | Description |
|---------|-------------|
| `*` | Log all categories |
| `-info:WebRequest` | Exclude info-level WebRequest logs |
| `+debug` | Include debug messages |
| `wthttp` | HTTP server specific logs |

### Runtime Log Level Change

In the application code, log levels can be changed:

```cpp
#include "utils/Logging.hpp"

// Get logger instance
Logger& logger = Logger::getInstance();

// Log at different levels
logger.debug("Detailed debug information");
logger.info("General information");
logger.warn("Warning message");
logger.error("Error occurred");
```

---

## IDE Debugging Setup

### Visual Studio Code

Create/update `.vscode/launch.json`:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug restaurant_pos",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build-debug/restaurant_pos",
            "args": [
                "--config", "${workspaceFolder}/wt_config.xml",
                "--docroot", "${workspaceFolder}",
                "--http-port", "8080"
            ],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [
                {"name": "WT_DEBUG", "value": "1"},
                {"name": "POS_LOG_LEVEL", "value": "debug"}
            ],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build-debug"
        }
    ]
}
```

Create `.vscode/tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build-debug",
            "type": "shell",
            "command": "cd build-debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j4",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```

### CLion

1. Open project with CMakeLists.txt
2. Go to **Settings → Build, Execution, Deployment → CMake**
3. Add a Debug profile:
   - Build type: `Debug`
   - CMake options: `-DCMAKE_BUILD_TYPE=Debug`
4. Edit Run Configuration:
   - Program arguments: `--config ./wt_config.xml --docroot . --http-port 8080`
   - Working directory: Project root
   - Environment: `WT_DEBUG=1`

### Xcode (macOS)

1. Create Xcode project from CMake:
   ```bash
   mkdir build-xcode && cd build-xcode
   cmake -G Xcode -DCMAKE_BUILD_TYPE=Debug ..
   open restaurant_pos.xcodeproj
   ```

2. Edit Scheme → Run → Arguments:
   - Add: `--config ./wt_config.xml --docroot . --http-port 8080`

3. Edit Scheme → Run → Environment Variables:
   - Add: `WT_DEBUG = 1`

---

## Common Debugging Scenarios

### 1. UI Not Rendering Properly

**Symptoms**: Blank page, missing components, garbled text

**Debug Steps**:
```bash
# Check browser console for JavaScript errors
# Open: http://localhost:8080/pos

# Check server logs
tail -f logs/pos.log

# Verify CSS is being served
curl -I http://localhost:8080/assets/css/base.css
```

**Common Causes**:
- CSS files not in docroot
- Wt progressive-bootstrap issues
- JavaScript errors in browser

### 2. Order Not Saving

**Debug Steps**:
```cpp
// Add debug logging in POSService.cpp
logger.debug("Creating order for table: " + tableId);
logger.debug("Order items count: " + std::to_string(items.size()));
```

```bash
# Run with debug logging
POS_LOG_LEVEL=debug ./restaurant_pos --config ./wt_config.xml
```

### 3. Event System Issues

**Debug Steps**:
```cpp
// In EventManager.cpp, enable event tracing
void EventManager::publish(const std::string& event, const std::any& data) {
    std::cout << "[EVENT] Publishing: " << event << std::endl;
    std::cout << "[EVENT] Subscribers: " << subscribers_[event].size() << std::endl;
    // ... rest of method
}
```

### 4. Memory Issues

**Run with AddressSanitizer**:
```bash
# Build with sanitizers
g++ -std=c++17 -g -fsanitize=address -fsanitize=leak \
    -o restaurant_pos_asan src/*.cpp \
    $(pkg-config --cflags --libs wt wthttp)

# Run - will report memory issues
./restaurant_pos_asan --config ./wt_config.xml
```

### 5. Session/State Issues

**Debug Steps**:
```bash
# Clear browser cookies/cache
# Or use incognito mode

# Check session in logs
grep -i "session" logs/wt.log
```

---

## Wt Framework Debugging

### Enable Wt Internal Debugging

In `wt_config.xml`:
```xml
<server>
    <application-settings location="*">
        <debug>true</debug>
        <log-config>* +debug</log-config>
    </application-settings>
</server>
```

### Debug Widget Hierarchy

```cpp
// In RestaurantPOSApp.cpp
void RestaurantPOSApp::debugWidgetTree(Wt::WWidget* widget, int depth) {
    std::string indent(depth * 2, ' ');
    std::cout << indent << widget->objectName()
              << " [" << typeid(*widget).name() << "]"
              << " visible=" << widget->isVisible()
              << std::endl;

    if (auto container = dynamic_cast<Wt::WContainerWidget*>(widget)) {
        for (auto* child : container->children()) {
            debugWidgetTree(child, depth + 1);
        }
    }
}

// Call from anywhere
debugWidgetTree(root(), 0);
```

### Debug AJAX Requests

In browser Developer Tools:
1. Open Network tab
2. Filter by XHR/Fetch
3. Look for requests to `?wtd=...`
4. Inspect request/response payloads

### Wt Log Categories

| Category | Description |
|----------|-------------|
| `wthttp` | HTTP server operations |
| `Wt` | General Wt framework |
| `Wt.Auth` | Authentication |
| `Wt.Dbo` | Database operations |
| `WebRequest` | Individual HTTP requests |

---

## Network Debugging

### Monitor HTTP Traffic

```bash
# Using curl to test endpoints
curl -v http://localhost:8080/pos

# Check if static files are served
curl -I http://localhost:8080/assets/css/base.css

# Test with specific headers
curl -H "Accept: text/html" http://localhost:8080/pos
```

### Debug with Browser DevTools

1. **Network Tab**: Monitor all requests
2. **Console Tab**: JavaScript errors
3. **Elements Tab**: Inspect DOM structure
4. **Application Tab**: Cookies, session storage

### Check Port Availability

```bash
# Check if port is in use
lsof -i :8080
netstat -tlnp | grep 8080

# Kill process on port
kill $(lsof -t -i:8080)
```

---

## Debugging Checklist

Before reporting an issue, verify:

- [ ] Built with debug flags (`-g -O0`)
- [ ] Log level set to `debug`
- [ ] Browser cache cleared
- [ ] Correct docroot path specified
- [ ] CSS/assets accessible via HTTP
- [ ] No JavaScript console errors
- [ ] Session cookies not corrupted
- [ ] Port not blocked by firewall

---

## Quick Debug Commands

```bash
# Full debug startup
mkdir -p logs && \
WT_DEBUG=1 \
POS_LOG_LEVEL=debug \
./build-debug/restaurant_pos \
    --config ./wt_config.xml \
    --docroot . \
    --http-port 8080 \
    --accesslog logs/access.log \
    2>&1 | tee logs/console.log

# Watch logs in real-time
tail -f logs/*.log

# Clean restart
pkill restaurant_pos; rm -f logs/*.log; make run-debug
```

---

## Getting Help

If issues persist:

1. Check `logs/` directory for error messages
2. Review browser console for JavaScript errors
3. Verify Wt framework is correctly installed: `pkg-config --modversion wt`
4. Consult Wt documentation: https://www.webtoolkit.eu/wt/doc/reference/html/
