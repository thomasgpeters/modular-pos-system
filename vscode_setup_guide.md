# VS Code Setup Guide for Restaurant POS

## Problem
VS Code launch configuration missing `--docroot` argument causing Wt Server Exception.

## Solution: Configure VS Code Launch and Tasks

### 1. Create .vscode Directory Structure

```bash
# From your project root
mkdir -p .vscode
```

Your project structure should look like:
```
restaurant-pos/
├── .vscode/
│   ├── launch.json       # Debug configurations
│   └── tasks.json        # Build tasks
├── build/
├── src/
├── include/
├── CMakeLists.txt
└── wt_config.xml
```

### 2. Configure launch.json

Create `.vscode/launch.json` with the debug configurations provided above.

**Key points:**
- `--docroot` set to `${workspaceFolder}` (your project root)
- `--http-port 8081` for consistent port usage
- `--config` points to your local `wt_config.xml`
- Uses `lldb` for macOS debugging

### 3. Configure tasks.json

Create `.vscode/tasks.json` with the build tasks provided above.

**Available tasks:**
- **build**: Compile the project using make
- **cmake-configure**: Run CMake configuration
- **clean**: Clean build artifacts
- **rebuild**: Clean and rebuild
- **run-pos-terminal**: Run POS system in terminal

### 4. Using the Configurations

#### Debug the Application
1. Open VS Code in your project root
2. Go to Run and Debug panel (Ctrl+Shift+D / Cmd+Shift+D)
3. Select "Restaurant POS - Debug" from dropdown
4. Press F5 or click "Start Debugging"

#### Build the Application
1. Open Command Palette (Ctrl+Shift+P / Cmd+Shift+P)
2. Type "Tasks: Run Task"
3. Select "build" to compile
4. Or use Ctrl+Shift+B / Cmd+Shift+B for default build

#### Run Without Debugging
1. Select "Restaurant POS - Development Server"
2. Press Ctrl+F5 / Cmd+F5

### 5. Alternative: Quick Fix for Current Issue

If you just want to fix the immediate problem without full VS Code setup:

**Option A: Edit existing launch configuration**
Add the `args` section to your current launch configuration:
```json
"args": [
    "--http-port", "8081",
    "--docroot", "${workspaceFolder}",
    "--config", "${workspaceFolder}/wt_config.xml"
]
```

**Option B: Set working directory and args**
```json
{
    "name": "Debug Restaurant POS",
    "type": "cppdbg",
    "request": "launch",
    "program": "${workspaceFolder}/build/bin/restaurant_pos",
    "args": ["--http-port", "8081", "--docroot", "."],
    "cwd": "${workspaceFolder}",
    "MIMode": "lldb"
}
```

### 6. Verify Setup

After configuration:

1. **Build**: Press Ctrl+Shift+B / Cmd+Shift+B
2. **Debug**: Press F5
3. **Check output**: Look for:
   ```
   ✓ Server started successfully!
   Access the POS system at:
     Local:    http://localhost:8081
   ```
4. **Access**: Open http://localhost:8081 in browser

### 7. Debugging Tips

#### Breakpoints
- Set breakpoints in your C++ code
- Debug through order creation, payment processing, etc.

#### Console Output
- View application output in VS Code's Debug Console
- Monitor Wt server messages and application logs

#### Environment Variables
Add to launch.json if needed:
```json
"environment": [
    {"name": "WT_CONFIG_XML", "value": ""},
    {"name": "DEBUG", "value": "1"}
]
```

### 8. Common Launch Configuration Issues

#### Wrong Program Path
```json
// Make sure this points to your actual executable
"program": "${workspaceFolder}/build/bin/restaurant_pos"
```

#### Missing Arguments
```json
// Always include these minimum args for Wt applications
"args": [
    "--http-port", "8081",
    "--docroot", "${workspaceFolder}"
]
```

#### Wrong Working Directory
```json
// Set to project root where wt_config.xml is located
"cwd": "${workspaceFolder}"
```

### 9. Testing the Configuration

1. **Start debugging** (F5)
2. **Should see startup message** without errors
3. **Browser test**: Navigate to http://localhost:8081
4. **Application test**: 
   - Set table number
   - Start new order
   - Add menu items
   - Verify UI functionality

### 10. Advanced Configuration Options

For production debugging:
```json
{
    "name": "Restaurant POS - Production Debug",
    "args": [
        "--http-port", "80",
        "--docroot", "${workspaceFolder}",
        "--http-address", "0.0.0.0",
        "--threads", "4"
    ],
    "console": "externalTerminal"
}
```

The main fix is ensuring your launch configuration includes the required `--docroot` argument that Wt needs to serve web content!

## Next Step: Enhanced UI Styling

The current UI is quite plain. Consider adding modern POS styling with:
- Professional color scheme (dark headers, branded colors)
- Card-based layout for better visual organization
- Enhanced typography and spacing
- Interactive hover effects and animations
- Restaurant-themed icons and visual elements
