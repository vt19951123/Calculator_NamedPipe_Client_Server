# server/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)

# Tạo executable cho server
add_executable(calculator_server
    main.cpp
    NamedPipeServer.cpp
)

# Link với thư viện shared
target_link_libraries(calculator_server
    PRIVATE
    shared
)

# Thêm Windows API cho Named Pipes (chỉ trên Windows)
if(WIN32)
    target_link_libraries(calculator_server PRIVATE ws2_32)
endif()

# Set include directories
target_include_directories(calculator_server
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)