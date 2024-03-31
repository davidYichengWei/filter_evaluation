cmake_minimum_required(VERSION 3.8)
project(EvaluationProject)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find pkg-config first
find_package(PkgConfig REQUIRED)

# Use pkg-config to find libpqxx
pkg_check_modules(LIBPQXX REQUIRED libpqxx)

# Include the filter directory as a subdirectory
add_subdirectory(filter)

# Add the executable with evaluation.cpp as the source file
add_executable(EvaluationProgram evaluation.cpp)

# Include directories for libpqxx
target_include_directories(EvaluationProgram PRIVATE ${LIBPQXX_INCLUDE_DIRS})

# Link the filter library and libpqxx to the executable
target_link_libraries(EvaluationProgram filter ${LIBPQXX_LIBRARIES})

# If libpqxx requires any compiler flags (unlikely in this case)
target_compile_options(EvaluationProgram PRIVATE ${LIBPQXX_CFLAGS_OTHER})
