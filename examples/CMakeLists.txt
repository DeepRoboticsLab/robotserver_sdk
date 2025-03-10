# 示例程序的主 CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(robotserver_sdk_examples)

# 查找线程库
find_package(Threads REQUIRED)

# 添加子目录
add_subdirectory(basic)
add_subdirectory(advanced)

# 安装示例目录结构
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/basic
    ${CMAKE_CURRENT_SOURCE_DIR}/advanced
    DESTINATION share/robotserver_sdk/examples
    FILES_MATCHING PATTERN "*.cpp" PATTERN "*.json"
)
