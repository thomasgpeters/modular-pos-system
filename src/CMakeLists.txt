cmake_minimum_required(VERSION 3.16)
project(RestaurantPOS)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Wt library
find_package(Wt REQUIRED Wt HTTP)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Core required files that must exist
set(CORE_SOURCES
    src/main.cpp
    src/core/RestaurantPOSApp.cpp
    src/services/POSService.cpp
    src/events/EventManager.cpp
    src/events/POSEvents.cpp
    src/MenuItem.cpp
    src/Order.cpp
    src/OrderManager.cpp
    src/PaymentProcessor.cpp
    src/KitchenInterface.cpp
)

# Start with core sources
set(ALL_SOURCES ${CORE_SOURCES})

# Check for optional files and add them if they exist
set(OPTIONAL_FILES
    src/core/ConfigurationManager.cpp
    src/services/ThemeService.cpp
    src/services/NotificationService.cpp
    src/ui/components/OrderEntryPanel.cpp
    src/ui/components/OrderStatusPanel.cpp
    src/ui/components/MenuDisplay.cpp
    src/ui/components/CurrentOrderDisplay.cpp
    src/ui/components/ActiveOrdersDisplay.cpp
    src/ui/components/KitchenStatusDisplay.cpp
    src/ui/components/ThemeSelector.cpp
    src/ui/dialogs/PaymentDialog.cpp
    src/ui/dialogs/CategoryPopover.cpp
    src/ui/dialogs/ThemeSelectionDialog.cpp
    src/ui/factories/UIComponentFactory.cpp
    src/utils/UIHelpers.cpp
)

# Add optional files if they exist
foreach(source_file ${OPTIONAL_FILES})
    if(EXISTS "${CMAKE_SOURCE_DIR}/${source_file}")
        list(APPEND ALL_SOURCES ${source_file})
        message(STATUS "Including: ${source_file}")
    endif()
endforeach()

# Print what we're building
message(STATUS "Building Restaurant POS with core functionality")
message(STATUS "Core files: 10")

# Create executable
add_executable(restaurant-pos ${ALL_SOURCES})

# Link libraries
target_link_libraries(restaurant-pos Wt::Wt Wt::HTTP)

# Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(restaurant-pos PRIVATE 
        -Wall 
        -Wextra 
        -Wno-deprecated-declarations
        -Wno-unused-parameter
        -Wno-unused-variable
    )
endif()

# Set output directory
set_target_properties(restaurant-pos PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Create config file if template exists
if(EXISTS "${CMAKE_SOURCE_DIR}/wt_config.xml.in")
    configure_file(
        ${CMAKE_SOURCE_DIR}/wt_config.xml.in
        ${CMAKE_BINARY_DIR}/wt_config.xml
        @ONLY
    )
endif()

message(STATUS "CMake configuration complete")

# Installation
install(TARGETS restaurant-pos DESTINATION bin)
