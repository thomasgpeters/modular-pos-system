# ============================================================================
# Restaurant POS CMakeLists.txt - Enhanced CSS Asset Management
# ============================================================================

cmake_minimum_required(VERSION 3.16)
project(restaurant_pos)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(WT REQUIRED wt wthttp)

# ============================================================================
# ASSET MANAGEMENT CONFIGURATION
# ============================================================================

# Set up asset directories
set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_BUILD_DIR "${CMAKE_BINARY_DIR}/assets")

# CSS specific directories
set(CSS_SOURCE_DIR "${ASSETS_SOURCE_DIR}/css")
set(CSS_BUILD_DIR "${ASSETS_BUILD_DIR}/css")

# ============================================================================
# CSS FILE COPYING FUNCTION
# ============================================================================

# Function to copy CSS files preserving directory structure
function(copy_css_files source_dir dest_dir)
    # Create destination directory
    file(MAKE_DIRECTORY ${dest_dir})
    
    # Get all CSS files recursively
    file(GLOB_RECURSE CSS_FILES 
        "${source_dir}/*.css"
        "${source_dir}/*.scss"  # In case you add SCSS later
    )
    
    foreach(css_file ${CSS_FILES})
        # Get relative path from source directory
        file(RELATIVE_PATH rel_path ${source_dir} ${css_file})
        
        # Set destination path
        set(dest_file "${dest_dir}/${rel_path}")
        
        # Get directory of destination file
        get_filename_component(dest_file_dir ${dest_file} DIRECTORY)
        
        # Create destination directory if it doesn't exist
        file(MAKE_DIRECTORY ${dest_file_dir})
        
        # Copy file
        configure_file(${css_file} ${dest_file} COPYONLY)
        
        message(STATUS "Copying CSS: ${rel_path}")
    endforeach()
endfunction()

# ============================================================================
# COPY CSS ASSETS TO BUILD DIRECTORY
# ============================================================================

# Copy all CSS files to build directory
copy_css_files(${CSS_SOURCE_DIR} ${CSS_BUILD_DIR})

# ============================================================================
# CREATE CSS FILE LISTS FOR TRACKING
# ============================================================================

# Create lists of CSS files for dependency tracking
file(GLOB_RECURSE THEME_CSS_FILES "${CSS_SOURCE_DIR}/themes/*.css")
file(GLOB_RECURSE COMPONENT_CSS_FILES "${CSS_SOURCE_DIR}/components/*.css")
file(GLOB FRAMEWORK_CSS_FILES "${CSS_SOURCE_DIR}/*.css")

# Print CSS file summary
list(LENGTH THEME_CSS_FILES theme_count)
list(LENGTH COMPONENT_CSS_FILES component_count)
list(LENGTH FRAMEWORK_CSS_FILES framework_count)

message(STATUS "CSS Asset Summary:")
message(STATUS "  Framework CSS files: ${framework_count}")
message(STATUS "  Theme CSS files: ${theme_count}")
message(STATUS "  Component CSS files: ${component_count}")

# ============================================================================
# CUSTOM TARGET FOR CSS UPDATES
# ============================================================================

# Create a custom target to update CSS files during development
add_custom_target(copy_css_assets
    COMMAND ${CMAKE_COMMAND} -E echo "Updating CSS assets..."
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CSS_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CSS_BUILD_DIR}/themes
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CSS_BUILD_DIR}/components
    
    # Copy framework CSS files
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CSS_SOURCE_DIR}/themes ${CSS_BUILD_DIR}/themes
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CSS_SOURCE_DIR}/components ${CSS_BUILD_DIR}/components
    
    # Copy individual framework files
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CSS_SOURCE_DIR}/*.css ${CSS_BUILD_DIR}/
    
    COMMENT "Copying CSS assets to build directory"
    VERBATIM
)

# ============================================================================
# SOURCE FILES
# ============================================================================

# Define source files
set(SOURCES
    src/main.cpp
    src/core/RestaurantPOSApp.cpp
    src/events/POSEvents.cpp
    
    # UI Components
    src/ui/components/ActiveOrdersDisplay.cpp
    src/ui/components/CurrentOrderDisplay.cpp
    src/ui/components/MenuDisplay.cpp
    src/ui/components/OrderEntryPanel.cpp
    
    # UI Containers
    src/ui/containers/POSModeContainer.cpp
    src/ui/containers/KitchenModeContainer.cpp
    
    # Add other source files as needed...
)

# Define header files for IDE integration
set(HEADERS
    include/core/RestaurantPOSApp.hpp
    include/events/EventManager.hpp
    include/events/POSEvents.hpp
    include/services/POSService.hpp
    include/services/ThemeService.hpp
    
    # UI Component headers
    include/ui/components/ActiveOrdersDisplay.hpp
    include/ui/components/CurrentOrderDisplay.hpp
    include/ui/components/MenuDisplay.hpp
    include/ui/components/OrderEntryPanel.hpp
    
    # UI Container headers
    include/ui/containers/POSModeContainer.hpp
    include/ui/containers/KitchenModeContainer.hpp
    
    # Add other headers as needed...
)

# ============================================================================
# EXECUTABLE CONFIGURATION
# ============================================================================

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${WT_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    ${WT_LIBRARIES}
)

# Set compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE 
    ${WT_CFLAGS_OTHER}
    -Wall 
    -Wextra
)

# ============================================================================
# MAKE CSS COPYING PART OF BUILD PROCESS
# ============================================================================

# Make CSS copying a dependency of the main executable
add_dependencies(${PROJECT_NAME} copy_css_assets)

# Ensure CSS files are copied before building
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Ensuring CSS assets are up to date..."
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target copy_css_assets
)

# ============================================================================
# DEVELOPMENT CONVENIENCE TARGETS
# ============================================================================

# Create convenience target for quick CSS updates
add_custom_target(update-css
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target copy_css_assets
    COMMENT "Quick CSS update for development"
)

# Create target to clean CSS assets
add_custom_target(clean-css
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CSS_BUILD_DIR}
    COMMENT "Cleaning CSS assets from build directory"
)

# ============================================================================
# DOCROOT CONFIGURATION FOR WT APPLICATION
# ============================================================================

# Set docroot to build directory for development
set(WT_DOCROOT "${CMAKE_BINARY_DIR}")

# Create a configuration header for docroot path
configure_file(
    "${CMAKE_SOURCE_DIR}/include/config/DocRootConfig.hpp.in"
    "${CMAKE_BINARY_DIR}/include/config/DocRootConfig.hpp"
    @ONLY
)

# Add the build include directory for generated headers
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_BINARY_DIR}/include
)

# ============================================================================
# INSTALL CONFIGURATION (for production deployment)
# ============================================================================

# Install executable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install CSS assets
install(DIRECTORY ${CSS_BUILD_DIR}/
    DESTINATION share/${PROJECT_NAME}/assets/css
    FILES_MATCHING PATTERN "*.css"
)

# ============================================================================
# DEVELOPMENT HELPER MESSAGES
# ============================================================================

message(STATUS "=== Restaurant POS Build Configuration ===")
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "CSS source: ${CSS_SOURCE_DIR}")
message(STATUS "CSS build: ${CSS_BUILD_DIR}")
message(STATUS "Wt docroot: ${WT_DOCROOT}")
message(STATUS "")
message(STATUS "Development commands:")
message(STATUS "  make update-css    # Update CSS files only")
message(STATUS "  make clean-css     # Clean CSS build files")
message(STATUS "  make copy_css_assets # Full CSS asset copy")
message(STATUS "===============================================")

# ============================================================================
# VERIFICATION
# ============================================================================

# Verify that CSS source directory exists
if(NOT EXISTS ${CSS_SOURCE_DIR})
    message(WARNING "CSS source directory does not exist: ${CSS_SOURCE_DIR}")
    message(WARNING "Please create the CSS directory structure")
endif()

# Create docroot config verification
if(EXISTS ${CSS_SOURCE_DIR})
    message(STATUS "✓ CSS source directory found")
    
    # Check for theme files
    file(GLOB theme_check "${CSS_SOURCE_DIR}/themes/*.css")
    if(theme_check)
        message(STATUS "✓ Theme CSS files found")
    else()
        message(STATUS "! No theme CSS files found in themes/ directory")
    endif()
    
    # Check for component files  
    file(GLOB component_check "${CSS_SOURCE_DIR}/components/*.css")
    if(component_check)
        message(STATUS "✓ Component CSS files found")
    else()
        message(STATUS "! No component CSS files found in components/ directory")
    endif()
endif()
