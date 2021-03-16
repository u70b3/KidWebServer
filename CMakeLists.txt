cmake_minimum_required(VERSION 3.12)

project(ToyWebServer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR on)


add_executable(${PROJECT_NAME}
    src/Epoller.cpp
    src/Buffer.cpp
    src/ErrorHandling.cpp
    src/main.cpp
    )

add_executable(client
    test/test.cpp 
    src/ErrorHandling.cpp
    )

target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
