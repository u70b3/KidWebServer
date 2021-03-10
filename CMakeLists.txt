cmake_minimum_required(VERSION 3.12)

project(ToyWebServer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR on)

add_executable(server
    src/Epoller.cpp
    src/Buffer.cpp
    src/ErrorHandling.cpp
    src/main.cpp
    )

add_executable(client
    test/test.cpp 
    src/ErrorHandling.cpp
    )
