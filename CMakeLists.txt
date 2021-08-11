cmake_minimum_required(VERSION 3.12)

project(KidWebServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR on)

# 编译 ./src/ 下所有文件
aux_source_directory(./src/ SRC_MAIN)

# 编译 ./test/ 下所有文件
aux_source_directory(./test/ TEST_MAIN)

# src 目录生成 KidWebServer 可执行文件
add_executable(
    ${PROJECT_NAME}
    ${SRC_MAIN}
    )

target_link_libraries(
    ${PROJECT_NAME} 
    pthread
    )


# test 目录生成测试可执行文件
add_executable(
    webserver_test
    ./test/unit_test/webserver_test.cpp
    )

add_executable(
    thread_pool_test
    ./test/unit_test/thread_pool_test.cpp
    )
target_link_libraries(
    thread_pool_test
    pthread
    )
