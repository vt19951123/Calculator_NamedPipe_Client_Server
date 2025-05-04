cmake_minimum_required(VERSION 3.14)
project(Calculator_NamedPipe_Client_Server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Thêm các thư mục con vào dự án
add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)
