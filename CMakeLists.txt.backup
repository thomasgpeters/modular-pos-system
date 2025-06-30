cmake_minimum_required(VERSION 3.10)
project(RestaurantPOS VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags for debugging and optimization
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(PkgConfig REQUIRED)
# cmake

# Find Wt
find_package(wt REQUIRED)
find_package(wthttp)
# pkg_check_modules(WT REQUIRED wt wthttp)

# Alternative if using pkg-config
# pkg_check_modules(WT REQUIRED wt wthttp)


# Source files
set(SOURCES
    src/main.cpp
    src/MenuItem.cpp
    src/Order.cpp
    src/OrderManager.cpp
    src/PaymentProcessor.cpp
    src/KitchenInterface.cpp
    src/RestaurantPOSApp.cpp
)

# Header files (for IDE organization)
set(HEADERS
    ../include/MenuItem.hpp
    ../include/Order.hpp
    ../include/OrderManager.hpp
    ../include/PaymentProcessor.hpp
    ../include/KitchenInterface.hpp
    ../include/RestaurantPOSApp.hpp
)

# Add executable
add_executable(restaurant_pos ${SOURCES} ${HEADERS})

# Link libraries
# Link the libraries to your target
target_link_libraries(restaurant_pos 
    wt
    wthttp
    ${WT_LIBRARIES}  # if using pkg-config
)

# target_link_libraries(restaurant_pos ${WT_LIBRARIES})
target_include_directories(restaurant_pos PRIVATE ${WT_INCLUDE_DIRS})
target_compile_options(restaurant_pos PRIVATE ${WT_CFLAGS_OTHER})

# Set output directory
set_target_properties(restaurant_pos PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Create www directory for static files
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/www)

# Copy any static resources (if they exist)
# file(COPY ${CMAKE_SOURCE_DIR}/www/ DESTINATION ${CMAKE_BINARY_DIR}/www/)

# Install target
install(TARGETS restaurant_pos DESTINATION bin)

# Print build information
message(STATUS "Restaurant POS System v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Wt libraries: ${WT_LIBRARIES}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Optional: Create a debug target
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug mode enabled - additional checks and logging")
endif()

# Optional: Add test executable
# set(TEST_SOURCES
#     ../test/test_pos.cpp
#     ../src/MenuItem.cpp
#     ../src/Order.cpp
#     ../src/OrderManager.cpp
#     ../src/PaymentProcessor.cpp
#     ../src/KitchenInterface.cpp
# )

# Create test executable (optional)
if(BUILD_TESTING OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(test_pos ${TEST_SOURCES} ${HEADERS})
    target_link_libraries(test_pos ${WT_LIBRARIES})
    target_include_directories(test_pos PRIVATE ${WT_INCLUDE_DIRS})
    target_compile_options(test_pos PRIVATE ${WT_CFLAGS_OTHER})
    target_compile_definitions(test_pos PRIVATE UNIT_TESTING)
    
    set_target_properties(test_pos PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    # Add test target
    enable_testing()
    add_test(NAME pos_unit_tests COMMAND test_pos)
    
    message(STATUS "Unit tests enabled - build with 'make test_pos' and run tests with 'make test'")
endif()

# Optional: Create documentation target (if Doxygen is available)
# find_package(Doxygen QUIET)
# if(DOXYGEN_FOUND)
#    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
#    add_custom_target(doc
#        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        COMMENT "Generating API documentation with Doxygen" VERBATIM
#    )
# endif()
