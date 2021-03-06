cmake_minimum_required(VERSION 3.12)
project(ToyWebServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(server
    src/server.cpp
    src/error_handling.cpp
    )

add_executable(client
    src/client.cpp 
    src/error_handling.cpp
    )

