# Restaurant POS System - Startup Guide

## Building and Running

### 1. Build the Project
```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
make

# Verify the executable was created
ls -la bin/restaurant_pos
```

### 2. Run the Application
```bash
# From the build directory, run with proper document root
./bin/restaurant_pos --http-port 8081 --docroot ..

# Alternative: Run from project root directory
cd ..
./build/bin/restaurant_pos --http-port 8081 --docroot .
```

### 3. Access the Application
Open your web browser and navigate to:
- **http://localhost:8081**
- **http://127.0.0.1:8081**

## Common Issues and Solutions

### Issue: "Not visible at /"
**Problem:** You're trying to access the wrong URL
**Solution:** The application runs on port 8081, not the default web port 80

### Issue: "Failed to start server"
**Solutions:**
1. **Port already in use:**
   ```bash
   # Use a different port
   ./bin/restaurant_pos --http-port 8082 --docroot .
   ```

2. **Permission issues:**
   ```bash
   # For ports below 1024, use sudo (not recommended for development)
   sudo ./bin/restaurant_pos --http-port 80 --docroot .
   ```

3. **Missing docroot:**
   ```bash
   # Make sure you specify the correct document root
   ./bin/restaurant_pos --http-port 8081 --docroot /full/path/to/project
   ```

### Issue: "Wt libraries not found"
**Solution:** Install Wt development packages
```bash
# Ubuntu/Debian
sudo apt-get install libwt-dev libwthttp-dev

# CentOS/RHEL
sudo yum install wt-devel

# macOS
brew install wt
```

## Command Line Options

```bash
./bin/restaurant_pos [options]

Common options:
  --http-port 8081              # Set HTTP port (default: 8081)
  --docroot .                   # Set document root directory
  --http-address 0.0.0.0        # Bind to all interfaces
  --config wt_config.xml        # Use configuration file
  --help                        # Show all available options
```

## Configuration File Alternative

You can also use the provided configuration file:
```bash
./bin/restaurant_pos --config ../wt_config.xml
```

## Accessing the POS System

Once running successfully, you'll see:
```
==================================================
    Restaurant POS System - Starting Server
==================================================
✓ Server started successfully!
✓ POS System is ready for operations

Access the POS system at:
  Local:    http://localhost:8081
  Network:  http://<your-ip>:8081
==================================================
```

## Project Structure
```
restaurant-pos/
├── build/
│   └── bin/
│       └── restaurant_pos     # Your executable
├── src/                       # Source files
├── include/                   # Header files
├── CMakeLists.txt
├── wt_config.xml
└── README.md
```

## Testing the Application

1. **Start New Order:** Set table number and click "Start New Order"
2. **Add Items:** Click "Add" next to menu items
3. **Send to Kitchen:** Click "Send to Kitchen" when ready
4. **Process Payment:** Click "Process Payment" to complete order
5. **Kitchen Status:** Check the "Kitchen Status" tab for order updates

## Troubleshooting Network Access

If you need to access from other machines on your network:
```bash
# Bind to all interfaces
./bin/restaurant_pos --http-port 8081 --http-address 0.0.0.0 --docroot .

# Then access via your machine's IP address
# http://YOUR_IP_ADDRESS:8081
```

## Development Tips

- Use `--http-port` to avoid conflicts with other services
- The `--docroot` should point to where your static files are located
- For development, usually set docroot to project root (`.` or `..`)
- Check firewall settings if accessing from remote machines
