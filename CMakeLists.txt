cmake_minimum_required(VERSION 3.16)
project(RestaurantPOS_Enhanced)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Wt library (using your working approach)
find_package(Wt REQUIRED Wt HTTP)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

#==============================================================================
# Enhanced Core Files (Mode Switching Architecture)
#==============================================================================

# Core required files that must exist for enhanced version
set(CORE_SOURCES
    src/main.cpp
    
    # Enhanced main application
    src/core/RestaurantPOSApp.cpp
    
    # Core services
    src/services/POSService.cpp
    src/events/EventManager.cpp
    src/events/POSEvents.cpp
    
    # Data models
    src/MenuItem.cpp
    src/Order.cpp
    src/OrderManager.cpp
    src/PaymentProcessor.cpp
    src/KitchenInterface.cpp
    
    # Enhanced mode containers (core to enhanced functionality)
    src/ui/containers/POSModeContainer.cpp
    src/ui/containers/KitchenModeContainer.cpp
    
    # Enhanced common components (core to enhanced functionality)
    src/ui/components/CommonHeader.cpp
    src/ui/components/CommonFooter.cpp
    src/ui/components/ModeSelector.cpp
)

# Start with enhanced core sources
set(ALL_SOURCES ${CORE_SOURCES})

#==============================================================================
# Enhanced Optional Components
#==============================================================================

# Enhanced optional files (will be included if they exist)
set(ENHANCED_OPTIONAL_FILES
    # Enhanced services
    src/core/ConfigurationManager.cpp
    src/services/ThemeService.cpp
    src/services/NotificationService.cpp
    
    # Enhanced UI components
    src/ui/components/OrderEntryPanel.cpp
    src/ui/components/OrderStatusPanel.cpp
    src/ui/components/MenuDisplay.cpp
    src/ui/components/CurrentOrderDisplay.cpp
    src/ui/components/ActiveOrdersDisplay.cpp
    src/ui/components/KitchenStatusDisplay.cpp
    src/ui/components/ThemeSelector.cpp
    
    # Enhanced dialogs
    src/ui/dialogs/PaymentDialog.cpp
    src/ui/dialogs/CategoryPopover.cpp
    src/ui/dialogs/ThemeSelectionDialog.cpp
    
    # Enhanced factories and utilities
    src/ui/factories/UIComponentFactory.cpp
    src/utils/UIHelpers.cpp
    src/utils/FormatUtils.cpp
    src/utils/CSSLoader.cpp
    
    # Enhanced layout managers
    src/ui/layouts/POSLayoutManager.cpp
    src/ui/layouts/KitchenLayoutManager.cpp
    src/ui/layouts/ResponsiveLayoutManager.cpp
    
    # Additional enhanced components
    src/ui/components/OrderSummaryWidget.cpp
    src/ui/components/MenuCategoryWidget.cpp
    src/ui/components/MenuItemWidget.cpp
    src/ui/components/OrderItemWidget.cpp
    src/ui/components/CustomerInfoWidget.cpp
    
    # Enhanced data models
    src/models/Customer.cpp
    src/models/Payment.cpp
    src/models/KitchenTicket.cpp
    src/models/Table.cpp
    
    # Core utilities
    src/core/DatabaseManager.cpp
    src/core/Logger.cpp
    src/core/ValidationEngine.cpp
    
    # Additional services
    src/services/MenuService.cpp
    src/services/OrderService.cpp
    src/services/PaymentService.cpp
    src/services/ReportingService.cpp
    
    # Utility classes
    src/utils/StringUtils.cpp
    src/utils/DateTimeUtils.cpp
    src/utils/CurrencyUtils.cpp
    src/utils/FileUtils.cpp
    src/utils/NetworkUtils.cpp
)

#==============================================================================
# Check for Optional Files and Add Them
#==============================================================================

# Add enhanced optional files if they exist
set(INCLUDED_COUNT 0)
set(MISSING_COUNT 0)

foreach(source_file ${ENHANCED_OPTIONAL_FILES})
    if(EXISTS "${CMAKE_SOURCE_DIR}/${source_file}")
        list(APPEND ALL_SOURCES ${source_file})
        message(STATUS "✓ Including: ${source_file}")
        math(EXPR INCLUDED_COUNT "${INCLUDED_COUNT} + 1")
    else()
        message(STATUS "⚠ Missing (optional): ${source_file}")
        math(EXPR MISSING_COUNT "${MISSING_COUNT} + 1")
    endif()
endforeach()

#==============================================================================
# Build Status Information
#==============================================================================

list(LENGTH CORE_SOURCES CORE_COUNT)
list(LENGTH ALL_SOURCES TOTAL_COUNT)

message(STATUS "")
message(STATUS "═══════════════════════════════════════════════")
message(STATUS "  Enhanced Restaurant POS Build Configuration")
message(STATUS "═══════════════════════════════════════════════")
message(STATUS "Core files (required): ${CORE_COUNT}")
message(STATUS "Optional files found: ${INCLUDED_COUNT}")
message(STATUS "Optional files missing: ${MISSING_COUNT}")
message(STATUS "Total files building: ${TOTAL_COUNT}")
message(STATUS "")
message(STATUS "Enhanced Features:")
message(STATUS "  ✓ Mode Switching (POS/Kitchen)")
message(STATUS "  ✓ Common Header/Footer")
message(STATUS "  ✓ Bootstrap Theme Support")
message(STATUS "  ✓ Event-Driven Architecture")
message(STATUS "═══════════════════════════════════════════════")

#==============================================================================
# Create Executable
#==============================================================================

# Create executable with enhanced name
add_executable(restaurant-pos-app ${ALL_SOURCES})

# Link libraries (using your working approach)
target_link_libraries(restaurant-pos-app Wt::Wt Wt::HTTP)

#==============================================================================
# Compiler Configuration
#==============================================================================

# Compiler flags (using your working approach + enhanced flags)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(restaurant-pos-app PRIVATE 
        -Wall 
        -Wextra 
        -Wno-deprecated-declarations
        -Wno-unused-parameter
        -Wno-unused-variable
    )
endif()

# Enhanced compiler definitions
target_compile_definitions(restaurant-pos-app PRIVATE
    PROJECT_VERSION="3.0.0"
    PROJECT_NAME="RestaurantPOS"
    WT_ENHANCED_MODE_SUPPORT
    BOOTSTRAP_THEME_SUPPORT
)

# Debug-specific definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(restaurant-pos-app PRIVATE
        DEBUG_MODE
        VERBOSE_LOGGING
        EVENT_DEBUGGING
    )
endif()

#==============================================================================
# Output Configuration
#==============================================================================

# Set output directory (using your working approach)
set_target_properties(restaurant-pos-app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

#==============================================================================
# Configuration Files
#==============================================================================

# Enhanced config file handling
if(EXISTS "${CMAKE_SOURCE_DIR}/wt_config.xml.in")
    configure_file(
        ${CMAKE_SOURCE_DIR}/wt_config.xml.in
        ${CMAKE_BINARY_DIR}/wt_config.xml
        @ONLY
    )
elseif(EXISTS "${CMAKE_SOURCE_DIR}/wt_config.xml")
    # Copy existing config file
    configure_file(
        ${CMAKE_SOURCE_DIR}/wt_config.xml
        ${CMAKE_BINARY_DIR}/wt_config.xml
        COPYONLY
    )
    message(STATUS "✓ Using existing wt_config.xml")
endif()

# Enhanced POS config file
if(EXISTS "${CMAKE_SOURCE_DIR}/pos_config.xml.in")
    configure_file(
        ${CMAKE_SOURCE_DIR}/pos_config.xml.in
        ${CMAKE_BINARY_DIR}/pos_config.xml
        @ONLY
    )
elseif(EXISTS "${CMAKE_SOURCE_DIR}/pos_config.xml")
    # Copy existing POS config file
    configure_file(
        ${CMAKE_SOURCE_DIR}/pos_config.xml
        ${CMAKE_BINARY_DIR}/pos_config.xml
        COPYONLY
    )
    message(STATUS "✓ Using existing pos_config.xml")
endif()

#==============================================================================
# Resource Management
#==============================================================================

# Create enhanced resource directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/resources)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/resources/css)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/resources/js)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/resources/images)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/config)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/themes)

# Copy resource files if they exist
if(EXISTS ${CMAKE_SOURCE_DIR}/resources)
    file(COPY ${CMAKE_SOURCE_DIR}/resources/ 
         DESTINATION ${CMAKE_BINARY_DIR}/bin/resources/)
    message(STATUS "✓ Copied resources directory")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/www)
    file(COPY ${CMAKE_SOURCE_DIR}/www/ 
         DESTINATION ${CMAKE_BINARY_DIR}/bin/www/)
    message(STATUS "✓ Copied www directory")
endif()

# Copy configuration files to bin directory
if(EXISTS ${CMAKE_BINARY_DIR}/wt_config.xml)
    file(COPY ${CMAKE_BINARY_DIR}/wt_config.xml
         DESTINATION ${CMAKE_BINARY_DIR}/bin/)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/pos_config.xml)
    file(COPY ${CMAKE_BINARY_DIR}/pos_config.xml
         DESTINATION ${CMAKE_BINARY_DIR}/bin/)
endif()

#==============================================================================
# Enhanced Custom Targets
#==============================================================================

# Enhanced run target (using your working port configuration)
add_custom_target(run
    COMMAND restaurant-pos-app --config-file=wt_config.xml --http-address=0.0.0.0 --http-port=8081 --docroot=.
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEPENDS restaurant-app
    COMMENT "Running Enhanced Restaurant POS System on http://localhost:8081"
)

# Development run target with debug output
add_custom_target(run-dev
    COMMAND restaurant-pos-app --config-file=wt_config.xml --http-address=localhost --http-port=8081 --docroot=. --accesslog=-
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEPENDS restaurant-pos-app
    COMMENT "Running Enhanced Restaurant POS System in development mode"
)

# Clean run target (fresh start)
add_custom_target(run-clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target restaurant-pos-app
    COMMAND restaurant-pos-app --config-file=wt_config.xml --http-address=localhost --http-port=8081 --docroot=.
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMENT "Clean build and run Enhanced Restaurant POS System"
)

# Theme development target
add_custom_target(run-themes
    COMMAND restaurant-pos-app --config-file=wt_config.xml --http-address=localhost --http-port=8081 --docroot=. --resource-dir=themes
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEPENDS restaurant-pos-app
    COMMENT "Running with theme development support"
)

#==============================================================================
# Installation Rules
#==============================================================================

# Install the enhanced executable
install(TARGETS restaurant-pos-app DESTINATION bin)

# Install configuration files
install(FILES 
    ${CMAKE_BINARY_DIR}/wt_config.xml
    ${CMAKE_BINARY_DIR}/pos_config.xml
    DESTINATION etc/restaurant-pos-app
    OPTIONAL
)

# Install resource directories
if(EXISTS ${CMAKE_BINARY_DIR}/bin/resources)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/resources/
            DESTINATION share/restaurant-pos-app/resources)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/bin/www)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/www/
            DESTINATION share/restaurant-pos-app/www)
endif()

#==============================================================================
# Final Status
#==============================================================================

message(STATUS "")
message(STATUS "Enhanced CMake configuration complete!")
message(STATUS "")
message(STATUS "Build commands:")
message(STATUS "  make                    - Build enhanced POS system")
message(STATUS "  make run                - Build and run on http://localhost:8081")
message(STATUS "  make run-dev            - Build and run in development mode")
message(STATUS "  make run-clean          - Clean build and run")
message(STATUS "  make run-themes         - Run with theme development support")
message(STATUS "  make pos-help           - Show detailed help")
message(STATUS "")
message(STATUS "The enhanced system will use your existing wt_config.xml")
message(STATUS "and will run on port 8081 as configured.")
message(STATUS "")

#==============================================================================
# Helpful Target Information
#==============================================================================

# Add a help target (using pos-help to avoid reserved name conflict)
add_custom_target(pos-help
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Enhanced Restaurant POS System - Available Targets:"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "  make                    Build the enhanced POS system"
    COMMAND ${CMAKE_COMMAND} -E echo "  make run                Run on http://localhost:8081"
    COMMAND ${CMAKE_COMMAND} -E echo "  make run-dev            Run in development mode"
    COMMAND ${CMAKE_COMMAND} -E echo "  make run-clean          Clean build and run"
    COMMAND ${CMAKE_COMMAND} -E echo "  make run-themes         Run with theme support"
    COMMAND ${CMAKE_COMMAND} -E echo "  make install            Install system files"
    COMMAND ${CMAKE_COMMAND} -E echo "  make pos-help           Show this help"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Enhanced Features:"
    COMMAND ${CMAKE_COMMAND} -E echo "  • POS Mode / Kitchen Mode switching"
    COMMAND ${CMAKE_COMMAND} -E echo "  • Common header with theme controls"
    COMMAND ${CMAKE_COMMAND} -E echo "  • Bootstrap theme framework"
    COMMAND ${CMAKE_COMMAND} -E echo "  • Event-driven component architecture"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)
