# Enhanced POS Migration Guide

This guide helps you migrate from your current working POS system to the enhanced version with mode switching.

## 🔄 Migration Steps

### 1. Backup Your Current System
```bash
# Backup your current working directory
cp -r your-current-pos-project your-current-pos-project-backup
```

### 2. Update CMakeLists.txt
Replace your current `CMakeLists.txt` with the enhanced version I provided. The new version:
- ✅ Uses your working `find_package(Wt REQUIRED Wt HTTP)` approach
- ✅ Maintains your successful configuration file handling
- ✅ Keeps your compiler flags and output directory structure
- ✅ Adds support for the new enhanced components

### 3. Create Required Enhanced Files

You need to create these core files for the enhanced functionality:

**Main Application (Enhanced):**
```
src/core/RestaurantPOSApp_Enhanced.cpp
include/core/RestaurantPOSApp_Enhanced.hpp
```

**Mode Containers:**
```
src/ui/containers/POSModeContainer.cpp
src/ui/containers/KitchenModeContainer.cpp
include/ui/containers/POSModeContainer.hpp
include/ui/containers/KitchenModeContainer.hpp
```

**Common Components:**
```
src/ui/components/CommonHeader.cpp
src/ui/components/CommonFooter.cpp
src/ui/components/ModeSelector.cpp
include/ui/components/CommonHeader.hpp
include/ui/components/CommonFooter.hpp
include/ui/components/ModeSelector.hpp
```

### 4. Update main.cpp

Your `main.cpp` should create the enhanced application:

```cpp
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include "core/RestaurantPOSApp_Enhanced.hpp"

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<RestaurantPOSApp>(env);
}

int main(int argc, char **argv) {
    try {
        Wt::WServer server(argc, argv);
        server.addEntryPoint(Wt::EntryPointType::Application, createApplication);
        server.run();
    } catch (Wt::WServer::Exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
}
```

### 5. Build and Test

```bash
# Create build directory
mkdir build && cd build

# Configure (using your working Wt installation)
cmake ..

# Build
make

# The system will show you what files are missing:
# ✓ Including: src/services/POSService.cpp
# ⚠ Missing (optional): src/ui/layouts/POSLayoutManager.cpp
```

### 6. Start with Minimal Implementation

You can start with just the core required files and add optional components later. The enhanced CMakeLists.txt will build successfully with missing optional files.

**Minimal file set to get started:**
1. Your existing core files (Order.cpp, MenuItem.cpp, etc.)
2. RestaurantPOSApp_Enhanced.cpp (enhanced main app)
3. POSModeContainer.cpp (basic POS mode)
4. KitchenModeContainer.cpp (basic kitchen mode)
5. CommonHeader.cpp (mode switching header)
6. ModeSelector.cpp (mode toggle component)

## 🎯 Key Benefits of Enhanced Version

### Immediate Improvements:
- **Better UX**: Clear separation between POS and Kitchen workflows
- **Focused Interface**: Users see only relevant components for their current mode
- **Professional Look**: Common header/footer with theme controls

### Future-Ready Architecture:
- **Modular Design**: Easy to add new features
- **Event-Driven**: Components communicate through events
- **Theme Support**: Built-in theme switching capability
- **Responsive**: Adapts to different screen sizes

## 🔧 Configuration

Your existing `wt_config.xml` and `pos_config.xml` files will work with the enhanced version. The system runs on **port 8081** as configured in your current setup.

## 🚀 Running the Enhanced System

```bash
# Standard run (uses your existing config)
make run

# Development mode with debugging
make run-dev

# Help with all available targets
make pos-help
```

## 📱 Using the Enhanced Interface

1. **Access**: Visit `http://localhost:8081`
2. **Mode Switching**: Use the header buttons to switch between:
   - **📋 POS Mode**: Order taking and management
   - **👨‍🍳 Kitchen Mode**: Order preparation and status
3. **Theme Controls**: Use the dropdown in the header to change themes
4. **Workflow**: 
   - POS staff focus on creating/editing orders
   - Kitchen staff focus on order preparation and completion

## 🔍 Troubleshooting

### Build Issues:
- The CMakeLists.txt will tell you exactly which files are missing
- Start with core files, add optional components gradually
- All your existing components will still work

### Runtime Issues:
- Check the console output for detailed error messages
- Verify your `wt_config.xml` file is in the correct location
- Ensure port 8081 is available

The enhanced system maintains backward compatibility while providing a much better user experience for restaurant operations!