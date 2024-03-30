cmake_minimum_required(VERSION 3.8)
project(EvaluationProject)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the filter directory as a subdirectory
add_subdirectory(filter)

# Add the executable with evaluation.cpp as the source file
add_executable(EvaluationProgram evaluation.cpp)

# Link the filter library to the executable
target_link_libraries(EvaluationProgram filter)
