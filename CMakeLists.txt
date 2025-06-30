# Restaurant POS System - Modular CMakeLists.txt
# =============================================================================
# Project: Restaurant Point of Sale System
# Version: 2.0.0 (Modular Architecture)
# Description: Enterprise-grade POS system with modular service architecture
# =============================================================================

cmake_minimum_required(VERSION 3.16)
project(RestaurantPOSSystem VERSION 2.0.0)

# =============================================================================
# Project Configuration
# =============================================================================

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# =============================================================================
# Build Options
# =============================================================================

option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_DOCUMENTATION "Build documentation with Doxygen" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis tools" OFF)
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# =============================================================================
# Compiler Configuration
# =============================================================================

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
endif()

# Sanitizers
if(ENABLE_SANITIZERS AND NOT MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined")
endif()

# Coverage
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# =============================================================================
# Find Required Dependencies
# =============================================================================

# Find Wt (Web Toolkit)
find_package(Wt REQUIRED)
if(NOT Wt_FOUND)
    message(FATAL_ERROR "Wt library not found. Please install Wt development libraries.")
endif()

# Find Threads
find_package(Threads REQUIRED)

# Find Boost (required by Wt)
find_package(Boost REQUIRED COMPONENTS system filesystem thread)

# Find OpenSSL (optional, for HTTPS support)
find_package(OpenSSL)
if(OpenSSL_FOUND)
    message(STATUS "OpenSSL found - HTTPS support enabled")
    add_definitions(-DHAVE_OPENSSL)
else()
    message(WARNING "OpenSSL not found - HTTPS support disabled")
endif()

# =============================================================================
# Include Directories
# =============================================================================

include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# =============================================================================
# Source File Collections
# =============================================================================

# Core business logic sources (existing files)
set(BUSINESS_LOGIC_SOURCES
    src/MenuItem.cpp
    src/Order.cpp
    src/OrderManager.cpp
    src/PaymentProcessor.cpp
    src/KitchenInterface.cpp
)

# Core business logic headers
set(BUSINESS_LOGIC_HEADERS
    include/MenuItem.hpp
    include/Order.hpp
    include/OrderManager.hpp
    include/PaymentProcessor.hpp
    include/KitchenInterface.hpp
)

# Event system sources
set(EVENT_SYSTEM_SOURCES
    src/events/EventManager.cpp
    src/events/POSEvents.cpp
)

# Event system headers
set(EVENT_SYSTEM_HEADERS
    include/events/EventManager.hpp
    include/events/POSEvents.hpp
)

# Service layer sources
set(SERVICES_SOURCES
    src/services/POSService.cpp
    src/services/ThemeService.cpp
    src/services/NotificationService.cpp
    src/core/ConfigurationManager.cpp
)

# Service layer headers
set(SERVICES_HEADERS
    include/services/POSService.hpp
    include/services/ThemeService.hpp
    include/services/NotificationService.hpp
    include/core/ConfigurationManager.hpp
)

# UI components sources
set(UI_COMPONENTS_SOURCES
    src/ui/components/MenuDisplay.cpp
    src/ui/components/CurrentOrderDisplay.cpp
    src/ui/components/OrderEntryPanel.cpp
    src/ui/components/OrderStatusPanel.cpp
    src/ui/components/ActiveOrdersDisplay.cpp
    src/ui/components/KitchenStatusDisplay.cpp
    src/ui/components/ThemeSelector.cpp
)

# UI components headers
set(UI_COMPONENTS_HEADERS
    include/ui/components/MenuDisplay.hpp
    include/ui/components/CurrentOrderDisplay.hpp
    include/ui/components/OrderEntryPanel.hpp
    include/ui/components/OrderStatusPanel.hpp
    include/ui/components/ActiveOrdersDisplay.hpp
    include/ui/components/KitchenStatusDisplay.hpp
    include/ui/components/ThemeSelector.hpp
)

# UI dialogs sources
set(UI_DIALOGS_SOURCES
    src/ui/dialogs/PaymentDialog.cpp
    src/ui/dialogs/CategoryPopover.cpp
    src/ui/dialogs/ThemeSelectionDialog.cpp
)

# UI dialogs headers
set(UI_DIALOGS_HEADERS
    include/ui/dialogs/PaymentDialog.hpp
    include/ui/dialogs/CategoryPopover.hpp
    include/ui/dialogs/ThemeSelectionDialog.hpp
)

# UI factory sources
set(UI_FACTORY_SOURCES
    src/ui/factories/UIComponentFactory.cpp
)

# UI factory headers
set(UI_FACTORY_HEADERS
    include/ui/factories/UIComponentFactory.hpp
)

# Utility sources
set(UTILS_SOURCES
    src/utils/UIHelpers.cpp
    src/utils/FormatUtils.cpp
)

# Utility headers
set(UTILS_HEADERS
    include/utils/UIHelpers.hpp
    include/utils/FormatUtils.hpp
)

# Core application sources
set(CORE_SOURCES
    src/core/RestaurantPOSApp.cpp
)

# Core application headers
set(CORE_HEADERS
    include/core/RestaurantPOSApp.hpp
)

# =============================================================================
# Create Static Libraries
# =============================================================================

# Business Logic Library
add_library(pos_business_logic STATIC
    ${BUSINESS_LOGIC_SOURCES}
    ${BUSINESS_LOGIC_HEADERS}
)

target_include_directories(pos_business_logic PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pos_business_logic PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
)

# Event System Library
add_library(pos_events STATIC
    ${EVENT_SYSTEM_SOURCES}
    ${EVENT_SYSTEM_HEADERS}
)

target_include_directories(pos_events PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pos_events PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
)

# Services Library
add_library(pos_services STATIC
    ${SERVICES_SOURCES}
    ${SERVICES_HEADERS}
)

target_include_directories(pos_services PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pos_services PUBLIC
    pos_events
    pos_business_logic
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Utilities Library
add_library(pos_utils STATIC
    ${UTILS_SOURCES}
    ${UTILS_HEADERS}
)

target_include_directories(pos_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pos_utils PUBLIC
    pos_business_logic
    ${WT_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

# UI Components Library
add_library(pos_ui_components STATIC
    ${UI_COMPONENTS_SOURCES}
    ${UI_COMPONENTS_HEADERS}
)

target_include_directories(pos_ui_components PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pos_ui_components PUBLIC
    pos_services
    pos_utils
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# UI Dialogs Library
add_library(pos_ui_dialogs STATIC
    ${UI_DIALOGS_SOURCES}
    ${UI_DIALOGS_HEADERS}
)

target_include_directories(pos_ui_dialogs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pos_ui_dialogs PUBLIC
    pos_services
    pos_utils
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# UI Factory Library
add_library(pos_ui_factory STATIC
    ${UI_FACTORY_SOURCES}
    ${UI_FACTORY_HEADERS}
)

target_include_directories(pos_ui_factory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pos_ui_factory PUBLIC
    pos_ui_components
    pos_ui_dialogs
    pos_services
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# Core Application Library
add_library(pos_core STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

target_include_directories(pos_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pos_core PUBLIC
    pos_ui_factory
    pos_services
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
)

# =============================================================================
# Main Executable
# =============================================================================

add_executable(restaurant_pos
    src/main.cpp
)

target_link_libraries(restaurant_pos PRIVATE
    pos_core
    ${WT_LIBRARY}
    ${WT_HTTP_LIBRARY}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

if(OpenSSL_FOUND)
    target_link_libraries(restaurant_pos PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()

# =============================================================================
# Testing Configuration
# =============================================================================

if(BUILD_TESTS)
    enable_testing()
    
    # Find or download Google Test
    find_package(GTest)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/v1.14.0.zip
        )
        FetchContent_MakeAvailable(googletest)
    endif()
    
    # Test sources
    set(TEST_SOURCES
        tests/test_main.cpp
        tests/test_menu_item.cpp
        tests/test_order.cpp
        tests/test_order_manager.cpp
        tests/test_payment_processor.cpp
        tests/test_kitchen_interface.cpp
        tests/test_event_manager.cpp
        tests/test_pos_service.cpp
        tests/test_configuration_manager.cpp
        tests/test_theme_service.cpp
        tests/test_notification_service.cpp
        tests/test_ui_helpers.cpp
        tests/test_format_utils.cpp
    )
    
    # Create test executable
    add_executable(pos_tests ${TEST_SOURCES})
    
    target_link_libraries(pos_tests PRIVATE
        pos_core
        pos_services
        pos_utils
        gtest
        gtest_main
        ${WT_LIBRARY}
        ${WT_HTTP_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    
    # Register tests
    add_test(NAME pos_unit_tests COMMAND pos_tests)
    
    # Integration tests
    add_executable(pos_integration_tests
        tests/integration/test_full_workflow.cpp
        tests/integration/test_component_integration.cpp
    )
    
    target_link_libraries(pos_integration_tests PRIVATE
        pos_core
        gtest
        gtest_main
        ${WT_LIBRARY}
        ${WT_HTTP_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    
    add_test(NAME pos_integration_tests COMMAND pos_integration_tests)
endif()

# =============================================================================
# Documentation
# =============================================================================

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    else()
        message(WARNING "Doxygen not found, documentation will not be built")
    endif()
endif()

# =============================================================================
# Static Analysis
# =============================================================================

if(ENABLE_STATIC_ANALYSIS)
    # Clang-tidy
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    endif()
    
    # Cppcheck
    find_program(CPPCHECK_EXE NAMES "cppcheck")
    if(CPPCHECK_EXE)
        add_custom_target(cppcheck
            COMMAND ${CPPCHECK_EXE}
            --enable=warning,performance,portability,information,missingInclude
            --std=c++17
            --library=std.cfg
            --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
            --verbose
            --quiet
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
        message(STATUS "cppcheck found: ${CPPCHECK_EXE}")
    endif()
endif()

# =============================================================================
# Installation
# =============================================================================

# Install executable
install(TARGETS restaurant_pos
    RUNTIME DESTINATION bin
)

# Install libraries
install(TARGETS 
    pos_business_logic
    pos_events
    pos_services
    pos_utils
    pos_ui_components
    pos_ui_dialogs
    pos_ui_factory
    pos_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Install configuration files
install(FILES
    pos_config.xml
    DESTINATION etc
)

# Install themes
install(DIRECTORY themes/
    DESTINATION share/restaurant-pos/themes
)

# Install documentation
if(BUILD_DOCUMENTATION)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html/
        DESTINATION share/doc/restaurant-pos
        OPTIONAL
    )
endif()

# =============================================================================
# Development Targets
# =============================================================================

# Format code with clang-format
find_program(CLANG_FORMAT_EXE NAMES "clang-format")
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    )
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
        COMMENT "Formatting source code with clang-format"
    )
    
    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run -Werror ${ALL_SOURCE_FILES}
        COMMENT "Checking source code formatting"
    )
endif()

# Development setup target
add_custom_target(dev_setup
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/logs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/config
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${CMAKE_CURRENT_SOURCE_DIR}/pos_config.xml 
        ${CMAKE_BINARY_DIR}/config/
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/themes
        ${CMAKE_BINARY_DIR}/themes
    COMMENT "Setting up development environment"
)

# Run target for development
add_custom_target(run_dev
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/restaurant_pos 
        --docroot=${CMAKE_CURRENT_SOURCE_DIR}/docroot
        --config=${CMAKE_BINARY_DIR}/config/pos_config.xml
        --http-port=8081
        --threads=4
    DEPENDS restaurant_pos dev_setup
    COMMENT "Running POS system in development mode"
)

# Clean build target
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMENT "Cleaning all build artifacts"
)

# =============================================================================
# Package Configuration
# =============================================================================

set(CPACK_PACKAGE_NAME "restaurant-pos")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Enterprise Restaurant Point of Sale System")
set(CPACK_PACKAGE_VENDOR "Restaurant POS Team")
set(CPACK_PACKAGE_CONTACT "support@restaurant-pos.com")

# Source package
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-src")
set(CPACK_SOURCE_IGNORE_FILES
    "/\\.git/"
    "/build/"
    "/\\.vscode/"
    "/\\.idea/"
    "\\.swp$"
    "\\.orig$"
    "/CMakeLists\\.txt\\.user$"
)

# Binary packages
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    
    # Debian package configuration
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libwt-dev, libboost-all-dev")
    set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    
    # RPM package configuration
    set(CPACK_RPM_PACKAGE_REQUIRES "witty-devel, boost-devel")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Business")
endif()

include(CPack)

# =============================================================================
# Status Summary
# =============================================================================

message(STATUS "=======================================================")
message(STATUS "Restaurant POS System - Modular Architecture v${PROJECT_VERSION}")
message(STATUS "=======================================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "")
message(STATUS "Features:")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build documentation: ${BUILD_DOCUMENTATION}")
message(STATUS "  Static analysis: ${ENABLE_STATIC_ANALYSIS}")
message(STATUS "  Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "  Code coverage: ${ENABLE_COVERAGE}")
message(STATUS "")
message(STATUS "Dependencies:")
message(STATUS "  Wt: ${WT_VERSION}")
message(STATUS "  Boost: ${Boost_VERSION}")
message(STATUS "  OpenSSL: ${OPENSSL_VERSION}")
message(STATUS "")
message(STATUS "Libraries to build:")
message(STATUS "  • pos_business_logic")
message(STATUS "  • pos_events") 
message(STATUS "  • pos_services")
message(STATUS "  • pos_utils")
message(STATUS "  • pos_ui_components")
message(STATUS "  • pos_ui_dialogs")
message(STATUS "  • pos_ui_factory")
message(STATUS "  • pos_core")
message(STATUS "")
message(STATUS "Targets available:")
message(STATUS "  make restaurant_pos  - Build main application")
message(STATUS "  make pos_tests      - Build and run tests")
message(STATUS "  make docs           - Generate documentation")
message(STATUS "  make format         - Format source code")
message(STATUS "  make dev_setup      - Setup development environment")
message(STATUS "  make run_dev        - Run in development mode")
message(STATUS "  make package        - Create installation package")
message(STATUS "=======================================================")

# =============================================================================
# Final Configuration Validation
# =============================================================================

# Validate that all required files exist
set(REQUIRED_FILES
    "src/main.cpp"
    "include/core/RestaurantPOSApp.hpp"
    "src/core/RestaurantPOSApp.cpp"
)

foreach(FILE ${REQUIRED_FILES})
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
        message(WARNING "Required file not found: ${FILE}")
    endif()
endforeach()

# Validate configuration file
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pos_config.xml")
    message(WARNING "Configuration file not found: pos_config.xml")
    message(STATUS "You may need to create a configuration file for proper operation")
endif()

# Validate themes directory
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/themes")
    message(WARNING "Themes directory not found: themes/")
    message(STATUS "You may need to create theme CSS files for proper styling")
endif()
