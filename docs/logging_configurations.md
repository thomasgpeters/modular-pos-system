# Logging Configuration Guide

## Current Configuration: DEBUG Testing

The current `wt_config.xml` is set for **DEBUG testing** to see all application messages.

### Test the DEBUG Configuration:

```bash
cd build
make
./SimpleLoggingApp
```

**Expected Output:**
```
Logging configuration:
  Level: DEBUG
  Directory: logs
  Base filename: app
  Max file size: 1073741824 bytes
  Console output: enabled
[2025-07-18 09:50:15.123] [INFO ] Application starting...
[2025-07-18 09:50:15.124] [DEBUG] Command line arguments count: 1
[2025-07-18 09:50:15.125] [INFO ] Wt server configured, attempting to start...
[2025-07-18 09:50:15.126] [INFO ] Server started successfully on http://localhost:8080
[2025-07-18 09:50:18.375] [INFO ] HelloApplication starting up
[2025-07-18 09:50:18.375] [DEBUG] Session ID: ABC123DEF456
[2025-07-18 09:50:18.376] [INFO ] HelloApplication initialized successfully
[2025-07-18 09:50:18.376] [DEBUG] Using default Wt theme with Bootstrap CSS override
```

When you click the button, you'll see:
```
[2025-07-18 09:50:25.123] [DEBUG] Button clicked - count: 1
[2025-07-18 09:50:25.124] [INFO ] Greeting updated for click count: 1
[2025-07-18 09:50:27.456] [DEBUG] Button clicked - count: 2
[2025-07-18 09:50:27.457] [INFO ] Greeting updated for click count: 2
...
[2025-07-18 09:50:35.789] [DEBUG] Button clicked - count: 5
[2025-07-18 09:50:35.789] [WARN ] Button has been clicked 5 times!
[2025-07-18 09:50:35.790] [INFO ] Greeting updated for click count: 5
```

## Configurations for Different Scenarios

### 1. Production Configuration (Minimal Logging)

**For normal operations, edit `wt_config.xml`:**

```xml
<!-- Change this line -->
<property name="logging.level">ERROR</property>
<!-- Also disable console output -->
<property name="logging.enable-console">false</property>
```

**Result:** Only error messages logged, no console output.

### 2. Staging Configuration (Warnings and Errors)

```xml
<property name="logging.level">WARN</property>
<property name="logging.enable-console">true</property>
```

**Result:** Shows warnings and errors, useful for testing.

### 3. Development Configuration (Info and above)

```xml
<property name="logging.level">INFO</property>
<property name="logging.enable-console">true</property>
```

**Result:** Shows general application flow without debug details.

### 4. Debugging Configuration (Everything)

```xml
<property name="logging.level">DEBUG</property>
<property name="logging.enable-console">true</property>
```

**Result:** Shows all messages including detailed debug information.

### 5. Silent Configuration (No Logging)

```xml
<property name="logging.level">NONE</property>
<property name="logging.enable-console">false</property>
```

**Result:** No application logging at all.

## Environment Variable Override

You can also override the configuration temporarily:

```bash
# Test with different levels without editing XML
export logging.level=ERROR
./SimpleLoggingApp

export logging.level=INFO
./SimpleLoggingApp

export logging.level=NONE
./SimpleLoggingApp
```

## Recommended Settings by Environment

| Environment | Level | Console | Use Case |
|-------------|-------|---------|----------|
| **Development** | `DEBUG` | `true` | See everything during development |
| **Testing** | `INFO` | `true` | Track application flow |
| **Staging** | `WARN` | `true` | Catch issues before production |
| **Production** | `ERROR` | `false` | Only log serious problems |
| **Debugging Issues** | `DEBUG` | `true` | Troubleshoot specific problems |

## Quick Configuration Switches

Create these commands for easy switching:

```bash
# Development mode
export logging.level=DEBUG && ./SimpleLoggingApp

# Production mode  
export logging.level=ERROR && ./SimpleLoggingApp

# Silent mode
export logging.level=NONE && ./SimpleLoggingApp
```

## Log File Analysis

With DEBUG enabled, check the log files:

```bash
# View latest log file
ls -la logs/
tail -f logs/app_*.log

# Search for specific message types
grep "\[ERROR\]" logs/app_*.log
grep "\[WARN\]" logs/app_*.log
grep "Button clicked" logs/app_*.log
```
