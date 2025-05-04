# client/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)

# Tìm Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Quick)

# Thiết lập qml resources
qt_add_resources(QML_RESOURCES
    resources.qrc
)

# Tạo executable cho client
add_executable(calculator_client
    main.cpp
    NamedPipeClient.cpp
    ${QML_RESOURCES}

)

# Thiết lập properties cho Qt
set_target_properties(calculator_client PROPERTIES
    AUTOMOC ON  # Meta-Object Compiler
    AUTORCC ON  # Resource Compiler
)

# Link với thư viện shared và Qt
target_link_libraries(calculator_client
    PRIVATE
    shared
    Qt6::Core
    Qt6::Quick
)

# Thêm Windows API cho Named Pipes (chỉ trên Windows)
if(WIN32)
    target_link_libraries(calculator_client PRIVATE ws2_32)
endif()

# Set include directories
target_include_directories(calculator_client
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
