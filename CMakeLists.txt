# ============================================================================
# Fixed CMakeLists.txt for Restaurant POS with Theme System
# ============================================================================

cmake_minimum_required(VERSION 3.16)

# Set policy for FindBoost (fixes the warning)
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

# Project definition
project(RestaurantPOS 
    VERSION 1.0.0
    DESCRIPTION "Restaurant Point of Sale System"
    LANGUAGES CXX
)

# Or for all targets:
add_compile_options(-Wno-deprecated-declarations)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ============================================================================
# Find Dependencies
# ============================================================================

# Find Wt
find_package(Wt REQUIRED) 
if(Wt_FOUND)
    message(STATUS "Found Wt: ${Wt_VERSION}")
else()
    message(FATAL_ERROR "Wt not found! Please install Wt framework.")
endif()

# Find Boost (required by Wt)
find_package(Boost REQUIRED COMPONENTS system filesystem thread)

# Find Threads
find_package(Threads REQUIRED)

# ============================================================================
# Source Files Discovery
# ============================================================================

# Automatically find all source files
file(GLOB_RECURSE HEADER_FILES 
    "include/*.hpp"
    "include/*.h"
)

file(GLOB_RECURSE SOURCE_FILES 
    "src/*.cpp"
    "src/*.cc"
    "src/*.cxx"
)

# Get list lengths correctly
list(LENGTH HEADER_FILES HEADER_COUNT)
list(LENGTH SOURCE_FILES SOURCE_COUNT)

# Print discovered files
message(STATUS "Found ${HEADER_COUNT} header files")
message(STATUS "Found ${SOURCE_COUNT} source files")

# Show some example files if found
if(SOURCE_COUNT GREATER 0)
    list(GET SOURCE_FILES 0 FIRST_SOURCE)
    message(STATUS "Example source file: ${FIRST_SOURCE}")
endif()

# ============================================================================
# Executable Definition
# ============================================================================

# Define the main executable
set(EXECUTABLE_NAME "RestaurantPOS")

# Create executable
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Set target properties
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    OUTPUT_NAME "restaurant_pos"
    VERSION ${PROJECT_VERSION}
)

message(STATUS "Creating executable: ${EXECUTABLE_NAME}")

# Configure the Wt config file
configure_file(
    ${CMAKE_SOURCE_DIR}/config/wt_config.xml.in
    ${CMAKE_BINARY_DIR}/config/wt_config.xml
    COPYONLY
)

# ============================================================================
# Include Directories
# ============================================================================

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/include/services
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/components
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/containers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/factories
    ${CMAKE_CURRENT_SOURCE_DIR}/include/events
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
)

# ============================================================================
# Link Libraries
# ============================================================================

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    Wt::Wt
    Wt::HTTP
    Boost::system
    Boost::filesystem
    Boost::thread
    Threads::Threads
)

# ============================================================================
# Compiler Flags
# ============================================================================

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )
endif()

if(MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        /W4
        $<$<CONFIG:Debug>:/Od /Zi>
        $<$<CONFIG:Release>:/O2>
    )
endif()

# ============================================================================
# Theme System Configuration
# ============================================================================

# Set the correct target name for theme system
set(POS_TARGET_NAME ${EXECUTABLE_NAME})

# Check if target exists
if(TARGET ${POS_TARGET_NAME})
    message(STATUS "Configuring theme system for target: ${POS_TARGET_NAME}")
    
    # Theme CSS files
    set(THEME_CSS_FILES
        theme-framework.css
        pos-dialogs.css
        pos-system.css
        pos-enhanced.css
        responsive.css
        theme-light.css
        theme-dark.css
        theme-warm.css
        theme-cool.css
    )
    
    # Get theme file count
    list(LENGTH THEME_CSS_FILES THEME_COUNT)
    
    # Create necessary directories
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docroot/css)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/resources/css)
    
    # Copy CSS files to build locations
    set(CSS_FILES_FOUND 0)
    foreach(CSS_FILE ${THEME_CSS_FILES})
        set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/css/${CSS_FILE}")
        set(DOCROOT_FILE "${CMAKE_CURRENT_BINARY_DIR}/docroot/css/${CSS_FILE}")
        set(RESOURCES_FILE "${CMAKE_CURRENT_BINARY_DIR}/build/resources/css/${CSS_FILE}")
        
        if(EXISTS ${SOURCE_FILE})
            configure_file(${SOURCE_FILE} ${DOCROOT_FILE} COPYONLY)
            configure_file(${SOURCE_FILE} ${RESOURCES_FILE} COPYONLY)
            math(EXPR CSS_FILES_FOUND "${CSS_FILES_FOUND} + 1")
            message(STATUS "  ✓ ${CSS_FILE}")
        else()
            message(WARNING "  ✗ ${CSS_FILE} not found at ${SOURCE_FILE}")
        endif()
    endforeach()
    
    message(STATUS "Theme CSS files found: ${CSS_FILES_FOUND}/${THEME_COUNT}")
    
    # Add compile definitions for theme paths
    target_compile_definitions(${POS_TARGET_NAME} PRIVATE
        THEME_CSS_PATH="${CMAKE_CURRENT_BINARY_DIR}/docroot/css/"
        THEME_RESOURCES_PATH="${CMAKE_CURRENT_BINARY_DIR}/build/resources/css/"
        THEME_INTERNAL_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/css/"
    )
    
    # Custom target for updating CSS files
    add_custom_target(update_theme_css
        COMMAND ${CMAKE_COMMAND} -E echo "Updating theme CSS files..."
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/docroot/css
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/build/resources/css
        COMMENT "Updating theme CSS files"
    )
    
    # Copy CSS files as part of build
    foreach(CSS_FILE ${THEME_CSS_FILES})
        set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/css/${CSS_FILE}")
        if(EXISTS ${SOURCE_FILE})
            add_custom_command(
                TARGET update_theme_css POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/docroot/css/${CSS_FILE}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/build/resources/css/${CSS_FILE}
                COMMENT "Copying ${CSS_FILE}"
            )
        endif()
    endforeach()
    
    # Make executable depend on CSS updates
    add_dependencies(${POS_TARGET_NAME} update_theme_css)
    
    message(STATUS "Theme system configured successfully")
else()
    message(WARNING "Target '${POS_TARGET_NAME}' not found - theme system disabled")
endif()

# ============================================================================
# Installation
# ============================================================================

install(TARGETS ${EXECUTABLE_NAME}
    RUNTIME DESTINATION bin
    COMPONENT runtime
)

# Install CSS files
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/css")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/css/
        DESTINATION share/${PROJECT_NAME}/css
        COMPONENT themes
        FILES_MATCHING PATTERN "*.css"
    )
endif()

# Install docroot
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docroot/
    DESTINATION share/${PROJECT_NAME}/docroot
    COMPONENT themes
    FILES_MATCHING PATTERN "*.css" PATTERN "*.html"
)

# ============================================================================
# Development Helpers
# ============================================================================

# Add helpful targets
add_custom_target(pos_help
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Restaurant POS Build Targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  ${EXECUTABLE_NAME}       - Build main executable"
    COMMAND ${CMAKE_COMMAND} -E echo "  update_theme_css  - Update theme CSS files"
    COMMAND ${CMAKE_COMMAND} -E echo "  pos_help          - Show this help"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Usage:"
    COMMAND ${CMAKE_COMMAND} -E echo "  make ${EXECUTABLE_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo "  make update_theme_css"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Run executable:"
    COMMAND ${CMAKE_COMMAND} -E echo "  ./${EXECUTABLE_NAME} --docroot=../docroot --http-port=8080"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)

# ============================================================================
# Status Summary
# ============================================================================

message(STATUS "")
message(STATUS "=============== Configuration Summary ===============")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Executable: ${EXECUTABLE_NAME}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source Files: ${SOURCE_COUNT}")
message(STATUS "Header Files: ${HEADER_COUNT}")

if(DEFINED CSS_FILES_FOUND)
    message(STATUS "Theme CSS Files: ${CSS_FILES_FOUND}/${THEME_COUNT}")
endif()

message(STATUS "")
message(STATUS "Build commands:")
message(STATUS "  cd build && cmake .. && make")
message(STATUS "  ./${EXECUTABLE_NAME} --docroot=../docroot --http-port=8080")
message(STATUS "==================================================")
message(STATUS "")

# ============================================================================
# Validation
# ============================================================================

# Check if we have any source files
if(NOT SOURCE_FILES)
    message(WARNING "No source files found! Please check your src/ directory.")
    message(STATUS "Looking for files in: ${CMAKE_CURRENT_SOURCE_DIR}/src/")
endif()

# Check if main function exists (basic check)
if(SOURCE_FILES)
    set(MAIN_FOUND FALSE)
    foreach(SOURCE_FILE ${SOURCE_FILES})
        file(READ ${SOURCE_FILE} SOURCE_CONTENT)
        if(SOURCE_CONTENT MATCHES "int main\\(")
            set(MAIN_FOUND TRUE)
            message(STATUS "Found main() function in: ${SOURCE_FILE}")
            break()
        endif()
    endforeach()
    
    if(NOT MAIN_FOUND)
        message(WARNING "No main() function found in source files!")
        message(STATUS "You may need to create src/main.cpp")
    endif()
endif()
