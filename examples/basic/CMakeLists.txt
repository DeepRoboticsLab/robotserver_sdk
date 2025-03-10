# basic 示例目录的 CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# 基本示例
add_executable(basic_example basic_example.cpp)
target_link_libraries(basic_example PRIVATE robotserver_sdk Threads::Threads)

# 复制配置文件到构建目录
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/default_navigation_points.json
               ${CMAKE_CURRENT_BINARY_DIR}/default_navigation_points.json COPYONLY)

# 复制配置文件到可执行文件目录
add_custom_command(TARGET basic_example POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/default_navigation_points.json
                   $<TARGET_FILE_DIR:basic_example>/default_navigation_points.json)

# 安装示例
install(TARGETS basic_example
    RUNTIME DESTINATION bin/examples/basic
)

# 安装配置文件
install(FILES
    default_navigation_points.json
    DESTINATION bin/examples/basic
)
