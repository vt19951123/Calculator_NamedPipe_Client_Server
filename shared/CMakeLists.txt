# shared/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)

# Tạo thư viện interface
add_library(shared INTERFACE)

# Thiết lập include directory
target_include_directories(shared 
    INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)