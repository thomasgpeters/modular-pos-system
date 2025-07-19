# ============================================================================
# Restaurant POS CMakeLists.txt - Explicit CSS Asset Management
# ============================================================================

cmake_minimum_required(VERSION 3.16)

# Set policy for Boost module changes
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

project(restaurant_pos)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Wt
find_package(Wt REQUIRED COMPONENTS wthttp)
if(Wt_FOUND)
    message(STATUS "Found Wt: ${Wt_VERSION}")
else()
    message(FATAL_ERROR "Wt not found! Please install Wt framework.")
endif()

# ============================================================================
# ASSET DIRECTORIES
# ============================================================================

set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_BUILD_DIR "${CMAKE_BINARY_DIR}/assets")
set(CSS_SOURCE_DIR "${ASSETS_SOURCE_DIR}/css")
set(CSS_BUILD_DIR "${ASSETS_BUILD_DIR}/css")

# ============================================================================
# EXPLICIT CSS FILE DEFINITIONS
# ============================================================================

# Framework/Base CSS Files
set(FRAMEWORK_CSS_FILES
    base.css
    bootstrap-custom.css
    pos-layout.css
    typography.css
    utilities.css
)

# Theme CSS Files
set(THEME_CSS_FILES
    themes/dark-theme.css
    themes/light-theme.css
    themes/colorful-theme.css
    themes/restaurant-theme.css
    themes/high-contrast.css
)

# Component CSS Files
set(COMPONENT_CSS_FILES
    components/pos-components.css
    components/menu-components.css
    components/order-components.css
    components/kitchen-components.css
    components/payment-components.css
    components/modal-components.css
    components/button-components.css
    components/table-components.css
    components/form-components.css
)

# Print CSS file summary
list(LENGTH FRAMEWORK_CSS_FILES framework_count)
list(LENGTH THEME_CSS_FILES theme_count)
list(LENGTH COMPONENT_CSS_FILES component_count)

message(STATUS "CSS Asset Summary:")
message(STATUS "  Framework CSS files: ${framework_count}")
message(STATUS "  Theme CSS files: ${theme_count}")
message(STATUS "  Component CSS files: ${component_count}")

# ============================================================================
# CSS FILE COPYING FUNCTIONS
# ============================================================================

# Function to copy a single CSS file with error checking
function(copy_single_css_file source_file dest_file)
    get_filename_component(dest_dir ${dest_file} DIRECTORY)
    file(MAKE_DIRECTORY ${dest_dir})
    
    if(EXISTS ${source_file})
        configure_file(${source_file} ${dest_file} COPYONLY)
        message(STATUS "✓ Copied: ${source_file} -> ${dest_file}")
    else()
        message(WARNING "✗ CSS file not found: ${source_file}")
    endif()
endfunction()

# Function to copy framework CSS files
function(copy_framework_css_files)
    message(STATUS "Copying framework CSS files...")
    foreach(css_file ${FRAMEWORK_CSS_FILES})
        set(source_path "${CSS_SOURCE_DIR}/${css_file}")
        set(dest_path "${CSS_BUILD_DIR}/${css_file}")
        copy_single_css_file(${source_path} ${dest_path})
    endforeach()
endfunction()

# Function to copy theme CSS files
function(copy_theme_css_files)
    message(STATUS "Copying theme CSS files...")
    foreach(css_file ${THEME_CSS_FILES})
        set(source_path "${CSS_SOURCE_DIR}/${css_file}")
        set(dest_path "${CSS_BUILD_DIR}/${css_file}")
        copy_single_css_file(${source_path} ${dest_path})
    endforeach()
endfunction()

# Function to copy component CSS files
function(copy_component_css_files)
    message(STATUS "Copying component CSS files...")
    foreach(css_file ${COMPONENT_CSS_FILES})
        set(source_path "${CSS_SOURCE_DIR}/${css_file}")
        set(dest_path "${CSS_BUILD_DIR}/${css_file}")
        copy_single_css_file(${source_path} ${dest_path})
    endforeach()
endfunction()

# ============================================================================
# COPY ALL CSS FILES AT CONFIGURE TIME
# ============================================================================

# Create necessary directories
file(MAKE_DIRECTORY ${CSS_BUILD_DIR})
file(MAKE_DIRECTORY ${CSS_BUILD_DIR}/themes)
file(MAKE_DIRECTORY ${CSS_BUILD_DIR}/components)

# Copy all CSS files
copy_framework_css_files()
copy_theme_css_files()
copy_component_css_files()

# ============================================================================
# CUSTOM TARGETS FOR CSS MANAGEMENT
# ============================================================================

# Create individual custom commands for each CSS file type
add_custom_target(copy_framework_css
    COMMENT "Copying framework CSS files"
)

add_custom_target(copy_theme_css
    COMMENT "Copying theme CSS files"
)

add_custom_target(copy_component_css
    COMMENT "Copying component CSS files"
)

# Add custom commands for framework CSS files
foreach(css_file ${FRAMEWORK_CSS_FILES})
    set(source_path "${CSS_SOURCE_DIR}/${css_file}")
    set(dest_path "${CSS_BUILD_DIR}/${css_file}")
    
    add_custom_command(
        TARGET copy_framework_css
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${source_path}
            ${dest_path}
        COMMENT "Updating framework CSS: ${css_file}"
        DEPENDS ${source_path}
    )
endforeach()

# Add custom commands for theme CSS files
foreach(css_file ${THEME_CSS_FILES})
    set(source_path "${CSS_SOURCE_DIR}/${css_file}")
    set(dest_path "${CSS_BUILD_DIR}/${css_file}")
    
    add_custom_command(
        TARGET copy_theme_css
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${source_path}
            ${dest_path}
        COMMENT "Updating theme CSS: ${css_file}"
        DEPENDS ${source_path}
    )
endforeach()

# Add custom commands for component CSS files
foreach(css_file ${COMPONENT_CSS_FILES})
    set(source_path "${CSS_SOURCE_DIR}/${css_file}")
    set(dest_path "${CSS_BUILD_DIR}/${css_file}")
    
    add_custom_command(
        TARGET copy_component_css
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${source_path}
            ${dest_path}
        COMMENT "Updating component CSS: ${css_file}"
        DEPENDS ${source_path}
    )
endforeach()

# Main CSS copy target that depends on all others
add_custom_target(copy_css_assets
    DEPENDS copy_framework_css copy_theme_css copy_component_css
    COMMENT "Copying all CSS assets"
)

# Process the wt_config template file
configure_file(
  ${CMAKE_SOURCE_DIR}/config/wt_config.xml.in   # Input template
  ${CMAKE_BINARY_DIR}/config/wt_config.xml      # Output file
  @ONLY                                         # Replace only @VARIABLE@ placeholders
)

# ============================================================================
# SOURCE FILES
# ============================================================================

set(SOURCES

    # Main
    src/main.cpp

    # Data Model
    src/Employee.cpp
    src/KitchenInterface.cpp
    src/MenuItem.cpp
    src/Order.cpp
    src/OrderManager.cpp
    src/PaymentProcessor.cpp

    # API
    src/api/APIClient.cpp
    src/api/APIConfiguration.cpp

    # Core
    src/core/RestaurantPOSApp.cpp
    src/core/ConfigurationManager.cpp

    # Events
    src/events/EventManager.cpp
    src/events/POSEvents.cpp
    
    # Services
    src/services/EnhancedPOSService.cpp
    src/services/NotificationService.cpp
    src/services/POSService.cpp
    src/services/ThemeService.cpp

    # UI Components
    src/ui/components/ActiveOrdersDisplay.cpp
    src/ui/components/CommonFooter.cpp
    src/ui/components/CommonHeader.cpp
    src/ui/components/CurrentOrderDisplay.cpp
    src/ui/components/KitchenStatusDisplay.cpp
    src/ui/components/MenuDisplay.cpp
    src/ui/components/ModeSelector.cpp
    src/ui/components/OrderEntryPanel.cpp
    src/ui/components/OrderStatusPanel.cpp
    src/ui/components/ThemeSelector.cpp
    
    # UI Containers
    src/ui/containers/POSModeContainer.cpp
    src/ui/containers/KitchenModeContainer.cpp

    # UI Dialogs
    src/ui/dialogs/CategoryPopover.cpp
    src/ui/dialogs/PaymentDialog.cpp
    src/ui/dialogs/ThemeSelectionDialog.cpp

    # UI Factories
    src/ui/factories/UIComponentFactory.cpp

    # Utilities
    src/utils/CSSLoader.cpp
    src/utils/FormatUtils.cpp
    src/utils/Logging.cpp
    src/utils/UIHelpers.cpp
)

set(HEADERS

    # Data Model
    include/Employee.hpp
    include/KitchenInterface.hpp
    include/MenuItem.hpp
    include/Order.hpp
    include/OrderManager.hpp
    include/PaymentProcessor.hpp

    # API
    include/api/APIClient.hpp
    include/api/APIConfiguration.hpp
    include/api/APIRepository.hpp
    include/api/APIServiceFactory.hpp

    # API Repositories
    include/api/repositories/EmployeeRepository.hpp
    include/api/repositories/MenuItemRepository.hpp
    include/api/repositories/OrderRepository.hpp

    # Core
    include/core/RestaurantPOSApp.hpp
    include/core/ConfigurationManager.hpp

    # Events
    include/events/EventManager.hpp
    include/events/POSEvents.hpp
    
    # Services
    include/services/EnhancedPOSService.hpp
    include/services/NotificationService.hpp
    include/services/POSService.hpp
    include/services/ThemeService.hpp

    # UI Components
    include/ui/components/ActiveOrdersDisplay.hpp
    include/ui/components/CommonFooter.hpp
    include/ui/components/CommonHeader.hpp
    include/ui/components/CurrentOrderDisplay.hpp
    include/ui/components/KitchenStatusDisplay.hpp
    include/ui/components/MenuDisplay.hpp
    include/ui/components/ModeSelector.hpp
    include/ui/components/OrderEntryPanel.hpp
    include/ui/components/OrderStatusPanel.hpp
    include/ui/components/ThemeSelector.hpp
    
    # UI Containers
    include/ui/containers/POSModeContainer.hpp
    include/ui/containers/KitchenModeContainer.hpp

    # UI Dialogs
    include/ui/dialogs/CategoryPopover.hpp
    include/ui/dialogs/PaymentDialog.hpp
    include/ui/dialogs/ThemeSelectionDialog.hpp

    # UI Factories
    include/ui/factories/UIComponentFactory.hpp

    # Utilities
    include/utils/CSSLoader.hpp
    include/utils/FormatUtils.hpp
    include/utils/LoggingUtils.hpp
    include/utils/Logging.hpp
    include/utils/UIHelpers.hpp
)

# ============================================================================
# EXECUTABLE CONFIGURATION
# ============================================================================

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} 
    Wt::Wt
    Wt::HTTP
    Threads::Threads
)

target_compile_options(${PROJECT_NAME} PRIVATE 
    -Wall 
    -Wextra
    -Wno-deprecated-declarations
    -Wno-unused-parameter

)

# ============================================================================
# BUILD DEPENDENCIES
# ============================================================================

# Make CSS copying a dependency of the main executable
add_dependencies(${PROJECT_NAME} copy_css_assets)

# ============================================================================
# DEVELOPMENT CONVENIENCE TARGETS
# ============================================================================

# Individual update targets for development
add_custom_target(update-framework-css
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target copy_framework_css
    COMMENT "Update framework CSS files only"
)

add_custom_target(update-theme-css
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target copy_theme_css
    COMMENT "Update theme CSS files only"
)

add_custom_target(update-component-css
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target copy_component_css
    COMMENT "Update component CSS files only"
)

add_custom_target(update-all-css
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target copy_css_assets
    COMMENT "Update all CSS files"
)

# Clean CSS target
add_custom_target(clean-css
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CSS_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CSS_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CSS_BUILD_DIR}/themes
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CSS_BUILD_DIR}/components
    COMMENT "Clean and recreate CSS directories"
)

# ============================================================================
# DOCROOT CONFIGURATION
# ============================================================================

set(WT_DOCROOT "${CMAKE_BINARY_DIR}")

# Create configuration header for docroot path
configure_file(
    "${CMAKE_SOURCE_DIR}/include/config/DocRootConfig.hpp.in"
    "${CMAKE_BINARY_DIR}/include/config/DocRootConfig.hpp"
    @ONLY
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_BINARY_DIR}/include
)

# ============================================================================
# INSTALL CONFIGURATION
# ============================================================================

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install CSS assets explicitly
foreach(css_file ${FRAMEWORK_CSS_FILES})
    install(FILES ${CSS_BUILD_DIR}/${css_file}
        DESTINATION share/${PROJECT_NAME}/assets/css
    )
endforeach()

foreach(css_file ${THEME_CSS_FILES})
    install(FILES ${CSS_BUILD_DIR}/${css_file}
        DESTINATION share/${PROJECT_NAME}/assets/css
    )
endforeach()

foreach(css_file ${COMPONENT_CSS_FILES})
    install(FILES ${CSS_BUILD_DIR}/${css_file}
        DESTINATION share/${PROJECT_NAME}/assets/css
    )
endforeach()

# ============================================================================
# VERIFICATION AND HELPER MESSAGES
# ============================================================================

message(STATUS "=== Restaurant POS Build Configuration ===")
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "CSS source: ${CSS_SOURCE_DIR}")
message(STATUS "CSS build: ${CSS_BUILD_DIR}")
message(STATUS "")
message(STATUS "Development commands:")
message(STATUS "  make update-framework-css  # Update framework CSS only")
message(STATUS "  make update-theme-css      # Update theme CSS only")
message(STATUS "  make update-component-css  # Update component CSS only")
message(STATUS "  make update-all-css        # Update all CSS files")
message(STATUS "  make clean-css             # Clean CSS build files")
message(STATUS "===============================================")

# Verify CSS source directory exists
if(NOT EXISTS ${CSS_SOURCE_DIR})
    message(WARNING "CSS source directory does not exist: ${CSS_SOURCE_DIR}")
    message(STATUS "Creating CSS directory structure...")
    file(MAKE_DIRECTORY ${CSS_SOURCE_DIR})
    file(MAKE_DIRECTORY ${CSS_SOURCE_DIR}/themes)
    file(MAKE_DIRECTORY ${CSS_SOURCE_DIR}/components)
endif()

# Check for missing CSS files and create placeholders
foreach(css_file ${FRAMEWORK_CSS_FILES})
    set(source_path "${CSS_SOURCE_DIR}/${css_file}")
    if(NOT EXISTS ${source_path})
        message(STATUS "Creating placeholder: ${css_file}")
        file(WRITE ${source_path} "/* ${css_file} - Add your styles here */\n")
    endif()
endforeach()

foreach(css_file ${THEME_CSS_FILES})
    set(source_path "${CSS_SOURCE_DIR}/${css_file}")
    if(NOT EXISTS ${source_path})
        message(STATUS "Creating placeholder: ${css_file}")
        file(WRITE ${source_path} "/* ${css_file} - Add your theme styles here */\n")
    endif()
endforeach()

foreach(css_file ${COMPONENT_CSS_FILES})
    set(source_path "${CSS_SOURCE_DIR}/${css_file}")
    if(NOT EXISTS ${source_path})
        message(STATUS "Creating placeholder: ${css_file}")
        file(WRITE ${source_path} "/* ${css_file} - Add your component styles here */\n")
    endif()
endforeach()

message(STATUS "✓ CSS file structure verified/created")

# ============================================================================
# LOGGING MESSAGES / LEVELS
# ============================================================================

# Create logs directory during build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory logs
    COMMENT "Creating logs directory"
)

