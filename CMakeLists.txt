cmake_minimum_required(VERSION 3.12)

project(ToyWebServer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR on)

add_executable(server
    src/Epoller.cpp
    src/Server.cpp
    src/ErrorHandling.cpp
    )

add_executable(client
    src/Client.cpp 
    src/ErrorHandling.cpp
    )
