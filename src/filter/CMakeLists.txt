cmake_minimum_required(VERSION 3.8)
project(FilterLibrary)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add library target
add_library(filter STATIC
    bloom/bloom.h
    vector_quotient_filter/vqf_cpp.h
    vector_quotient_filter/vqf_filter.h
    vector_quotient_filter/vqf_precompute.h
    vector_quotient_filter/vqf_filter.cpp  # Assuming this is the main source file for the filter
    hashutil.h
    # Add other source (.cpp) files here
)

# Specify the directories where the compiler should look for the header files
target_include_directories(filter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/bloom
    ${CMAKE_CURRENT_SOURCE_DIR}/vector_quotient_filter
)

# If you have any third-party dependencies, you might need to link them as well
# target_link_libraries(filter some_other_library)
