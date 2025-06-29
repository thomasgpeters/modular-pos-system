# Wt Configuration Override Solutions

## Problem
The system-wide Wt configuration `/etc/wt/wt_config.xml` is listening on port 80, overriding your application's port 8081 setting.

## Solution 1: Use Your Local Config File (Recommended)

```bash
# Run with your project's config file explicitly
./build/bin/restaurant_pos --config ./wt_config.xml

# Or from build directory:
cd build
./bin/restaurant_pos --config ../wt_config.xml
```

Your local `wt_config.xml` is already configured for port 8081:
```xml
<http-server>
    <http-listen>127.0.0.1</http-listen>
    <http-port>8081</http-port>
</http-server>
```

## Solution 2: Override with Command Line Arguments

Force the port setting with command line arguments:
```bash
# Command line args should override config files
./build/bin/restaurant_pos --http-port 8081 --http-address 127.0.0.1 --docroot .

# If that doesn't work, be more explicit:
./build/bin/restaurant_pos \
    --http-port 8081 \
    --http-address 127.0.0.1 \
    --docroot . \
    --no-config
```

## Solution 3: Check System Configuration

View the system config that's causing the issue:
```bash
# Check what's in the system config
cat /etc/wt/wt_config.xml

# Look for the http-server section:
# <http-server>
#     <http-port>80</http-port>
# </http-server>
```

## Solution 4: Temporarily Rename System Config

```bash
# Backup and temporarily disable system config
sudo mv /etc/wt/wt_config.xml /etc/wt/wt_config.xml.backup

# Run your app
./build/bin/restaurant_pos --http-port 8081 --docroot .

# Restore system config when done
sudo mv /etc/wt/wt_config.xml.backup /etc/wt/wt_config.xml
```

## Solution 5: Create Custom Config Environment

Create a wrapper script that isolates your config:

```bash
#!/bin/bash
# File: run_pos.sh

# Set environment to ignore system config
export WT_CONFIG_XML=""

# Run with explicit local config
./build/bin/restaurant_pos --config ./wt_config.xml

# Make executable
chmod +x run_pos.sh

# Run
./run_pos.sh
```

## Solution 6: Access on Port 80 (if that's what you want)

If you actually want to access on port 80 (default web port):
```bash
# You'd access at http://localhost/ (no port number needed)
# But you need sudo for ports below 1024
sudo ./build/bin/restaurant_pos --config /etc/wt/wt_config.xml
```

## Recommended Approach

**Use Solution 1** - explicitly specify your local config:

```bash
cd /path/to/your/restaurant-pos
./build/bin/restaurant_pos --config ./wt_config.xml
```

Then access at: **http://localhost:8081**

## Verify Configuration Loading

Add this debug output to see which config is being used:
```bash
# Run with verbose output to see config loading
./build/bin/restaurant_pos --config ./wt_config.xml --verbose

# Or check the startup messages for port information
```

## Update Your Local Config (Optional)

If you want to modify your local config for better development:

```xml
<!-- wt_config.xml - Enhanced for development -->
<server>
    <application-settings>
        <max-request-size>128</max-request-size>
        <session-timeout>600</session-timeout>
        <reload-on-exception>true</reload-on-exception>
    </application-settings>
    
    <http-server>
        <http-listen>0.0.0.0</http-listen>  <!-- Bind to all interfaces -->
        <http-port>8081</http-port>
        <threads>4</threads>
    </http-server>
    
    <properties>
        <property name="approot">.</property>
        <property name="docroot">.</property>
        <property name="resources-dir">./resources</property>
    </properties>
    
    <!-- Development settings -->
    <settings>
        <setting name="debug">true</setting>
        <setting name="behind-reverse-proxy">false</setting>
    </settings>
</server>
```

## Test Your Solution

After applying any solution:

1. **Start the server:**
   ```bash
   ./build/bin/restaurant_pos --config ./wt_config.xml
   ```

2. **Look for this output:**
   ```
   ✓ Server started successfully!
   Access the POS system at:
     Local:    http://localhost:8081
   ```

3. **Access in browser:**
   - http://localhost:8081
   - Should show the Restaurant POS interface

4. **Verify functionality:**
   - Set table number
   - Click "Start New Order"
   - Add menu items
   - Check active orders panel
