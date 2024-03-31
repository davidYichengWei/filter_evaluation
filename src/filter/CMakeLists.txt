cmake_minimum_required(VERSION 3.8)
project(FilterLibrary)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check for compiler support of various instruction sets
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-mavx" COMPILER_SUPPORTS_AVX)
CHECK_CXX_COMPILER_FLAG("-mavx2" COMPILER_SUPPORTS_AVX2)
CHECK_CXX_COMPILER_FLAG("-mbmi2" COMPILER_SUPPORTS_BMI2)
CHECK_CXX_COMPILER_FLAG("-mbmi" COMPILER_SUPPORTS_BMI1)

# Define the filter library
add_library(filter STATIC
    bloom/bloom.h
    vector_quotient_filter/vqf_cpp.h
    vector_quotient_filter/vqf_filter.h
    vector_quotient_filter/vqf_precompute.h
    vector_quotient_filter/vqf_filter.cpp
    hashutil.h
)

# Apply compiler flags for supported instruction sets to the filter library
# Use PUBLIC to apply the options to both the library itself and the targets that link against the library
if(COMPILER_SUPPORTS_AVX)
  target_compile_options(filter PUBLIC -mavx)
endif()

if(COMPILER_SUPPORTS_AVX2)
  target_compile_options(filter PUBLIC -mavx2)
endif()

if(COMPILER_SUPPORTS_BMI2)
  target_compile_options(filter PUBLIC -mbmi2)
endif()

if(COMPILER_SUPPORTS_BMI1)
  target_compile_options(filter PUBLIC -mbmi)
endif()

# Specify include directories for the filter library
target_include_directories(filter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/bloom
    ${CMAKE_CURRENT_SOURCE_DIR}/vector_quotient_filter
)
