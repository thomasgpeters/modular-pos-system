# CMakeLists.txt for Modular Restaurant POS System
cmake_minimum_required(VERSION 3.10)
project(RestaurantPOS VERSION 2.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find required packages
find_package(Wt REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# =================================================================
# Core Business Logic Library (existing components)
# =================================================================
add_library(pos_business_logic
    src/MenuItem.cpp
    src/Order.cpp
    src/OrderManager.cpp
    src/PaymentProcessor.cpp
    src/KitchenInterface.cpp
)

target_include_directories(pos_business_logic PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# =================================================================
# Event System Library
# =================================================================
add_library(pos_events
    src/events/EventManager.cpp
    src/events/POSEvents.cpp
)

target_include_directories(pos_events PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(pos_events
    ${WT_LIBRARY}
)

# =================================================================
# Services Library (NEW - main focus of this implementation)
# =================================================================
add_library(pos_services
    src/services/NotificationService.cpp
    src/services/POSService.cpp
    src/services/ThemeService.cpp
    src/services/ConfigurationManager.cpp
)

target_include_directories(pos_services PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(pos_services
    pos_events
    pos_business_logic
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# =================================================================
# UI Components Library (to be implemented next)
# =================================================================
add_library(pos_ui_components
    # These will be implemented in the next phase
    src/ui/components/OrderEntryPanel.cpp
    src/ui/components/OrderStatusPanel.cpp
    src/ui/components/MenuDisplay.cpp
    src/ui/components/CurrentOrderDisplay.cpp
    src/ui/components/ActiveOrdersDisplay.cpp
    src/ui/components/KitchenStatusDisplay.cpp
    src/ui/components/ThemeSelector.cpp
)

target_include_directories(pos_ui_components PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(pos_ui_components
    pos_services
    pos_events
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# =================================================================
# UI Dialogs Library (to be implemented next)
# =================================================================
add_library(pos_ui_dialogs
    # These will be implemented in the next phase
    src/ui/dialogs/PaymentDialog.cpp
    src/ui/dialogs/CategoryPopover.cpp
    src/ui/dialogs/ThemeSelectionDialog.cpp
)

target_include_directories(pos_ui_dialogs PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(pos_ui_dialogs
    pos_services
    pos_events
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# =================================================================
# UI Factory Library (to be implemented next)
# =================================================================
add_library(pos_ui_factories
    # This will be implemented in the next phase
    src/ui/factories/UIComponentFactory.cpp
)

target_include_directories(pos_ui_factories PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(pos_ui_factories
    pos_ui_components
    pos_ui_dialogs
    pos_services
    ${WT_LIBRARY}
)

# =================================================================
# Core Application Library
# =================================================================
add_library(pos_core
    src/core/RestaurantPOSApp.cpp
)

target_include_directories(pos_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(pos_core
    pos_ui_factories
    pos_ui_components
    pos_services
    pos_events
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# =================================================================
# Utilities Library
# =================================================================
add_library(pos_utils
    src/utils/UIHelpers.cpp
    src/utils/FormatUtils.cpp
)

target_include_directories(pos_utils PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(pos_utils
    ${WT_LIBRARY}
)

# =================================================================
# Main Executable
# =================================================================
add_executable(restaurant_pos
    src/main.cpp
)

target_link_libraries(restaurant_pos
    pos_core
    pos_ui_factories
    pos_ui_components
    pos_ui_dialogs
    pos_services
    pos_events
    pos_business_logic
    pos_utils
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# =================================================================
# Installation
# =================================================================
install(TARGETS restaurant_pos DESTINATION bin)

# Install configuration files
install(FILES wt_config.xml DESTINATION etc)

# Install themes
install(DIRECTORY themes/ DESTINATION share/restaurant-pos/themes)

# =================================================================
# Development Tools
# =================================================================

# Code formatting target (requires clang-format)
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.[ch]pp
        ${CMAKE_SOURCE_DIR}/include/*.[ch]pp
    )
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${ALL_CXX_SOURCE_FILES}
        COMMENT "Running clang-format on source files"
    )
endif()

# Static analysis target (requires cppcheck)
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK} --enable=all --std=c++17 --verbose --quiet
        ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include
        COMMENT "Running cppcheck static analysis"
    )
endif()

# Documentation target (requires doxygen)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# =================================================================
# Testing (optional - requires Google Test)
# =================================================================
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()
    
    # Service Tests
    add_executable(test_services
        tests/services/test_NotificationService.cpp
        tests/services/test_POSService.cpp
        tests/services/test_ThemeService.cpp
        tests/services/test_ConfigurationManager.cpp
    )
    
    target_link_libraries(test_services
        pos_services
        pos_events
        pos_business_logic
        GTest::GTest
        GTest::Main
    )
    
    # Event System Tests
    add_executable(test_events
        tests/events/test_EventManager.cpp
        tests/events/test_POSEvents.cpp
    )
    
    target_link_libraries(test_events
        pos_events
        GTest::GTest
        GTest::Main
    )
    
    # Business Logic Tests
    add_executable(test_business_logic
        tests/business/test_Order.cpp
        tests/business/test_OrderManager.cpp
        tests/business/test_PaymentProcessor.cpp
        tests/business/test_KitchenInterface.cpp
    )
    
    target_link_libraries(test_business_logic
        pos_business_logic
        GTest::GTest
        GTest::Main
    )
    
    # Register tests with CTest
    add_test(NAME ServiceTests COMMAND test_services)
    add_test(NAME EventTests COMMAND test_events)
    add_test(NAME BusinessLogicTests COMMAND test_business_logic)
endif()

# =================================================================
# Build Configuration Summary
# =================================================================
message(STATUS "")
message(STATUS "Restaurant POS System - Modular Architecture v${PROJECT_VERSION}")
message(STATUS "=================================================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Wt version: ${Wt_VERSION}")
message(STATUS "")
message(STATUS "Libraries to build:")
message(STATUS "  ✓ pos_business_logic  - Core business components")
message(STATUS "  ✓ pos_events         - Event system")
message(STATUS "  ✓ pos_services       - Service layer (NEW)")
message(STATUS "  ○ pos_ui_components  - UI components (TODO)")
message(STATUS "  ○ pos_ui_dialogs     - UI dialogs (TODO)")
message(STATUS "  ○ pos_ui_factories   - UI factories (TODO)")
message(STATUS "  ✓ pos_core          - Main application")
message(STATUS "  ✓ pos_utils         - Utilities")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  make restaurant_pos  - Build main executable")
message(STATUS "  make format         - Format code (requires clang-format)")
message(STATUS "  make cppcheck       - Static analysis (requires cppcheck)")
message(STATUS "  make docs           - Generate documentation (requires doxygen)")
if(BUILD_TESTS)
message(STATUS "  make test           - Run unit tests")
endif()
message(STATUS "=================================================================")

# =================================================================
# Compiler-specific settings
# =================================================================
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(restaurant_pos PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -Wno-missing-field-initializers
    )
    
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(restaurant_pos PRIVATE -g -O0)
        target_compile_definitions(restaurant_pos PRIVATE DEBUG)
    else()
        target_compile_options(restaurant_pos PRIVATE -O3 -DNDEBUG)
    endif()
endif()

# =================================================================
# Platform-specific settings
# =================================================================
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(restaurant_pos PRIVATE WIN32_LEAN_AND_MEAN)
elseif(APPLE)
    # macOS-specific settings
    set_target_properties(restaurant_pos PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist.in
    )
elseif(UNIX)
    # Linux-specific settings
    target_link_libraries(restaurant_pos pthread)
endif()

# =================================================================
# Export compile commands for IDE support
# =================================================================
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
