cmake_minimum_required(VERSION 3.10)
project(julia_set_zoom CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include SoftPosit headers
include_directories(${SOFTPOSIT_INCLUDE_DIRS})

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Build the posit implementation
add_executable(julia_set_posit julia_set_posit.cpp)
target_link_libraries(julia_set_posit ${SOFTPOSIT_LIBRARIES} m)

# Build the float implementation (doesn't require SoftPosit)
add_executable(julia_set_float julia_set_float.cpp)
target_link_libraries(julia_set_float m)

# Build the comparison program
add_executable(julia_set_comparison julia_set_comparison.cpp)

# Link dynamically if shared library is available
if(EXISTS "${SOFTPOSIT_SHARED_LIBRARIES}")
    add_executable(julia_set_posit_dynamic julia_set_posit.cpp)
    target_link_libraries(julia_set_posit_dynamic ${SOFTPOSIT_SHARED_LIBRARIES} m)
endif()

# Install 
install(TARGETS 
    julia_set_posit 
    julia_set_float 
    julia_set_comparison
    DESTINATION bin)

# Copy README to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README.md
               ${CMAKE_CURRENT_BINARY_DIR}/README.md
               COPYONLY) 