cmake_minimum_required(VERSION 3.10)
project(robotserver_sdk VERSION 0.1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置构建类型
set(CMAKE_BUILD_TYPE Release)

# 为 C++ 编译器添加警告选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找依赖包
find_package(Boost 1.66 REQUIRED COMPONENTS system thread)
find_package(nlohmann_json REQUIRED)
find_package(RapidXML QUIET)

# 如果没有找到RapidXML，则使用内部提供的版本
if(NOT RapidXML_FOUND)
    message(STATUS "Using bundled RapidXML")
    set(RAPIDXML_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidxml)
endif()

# 设置包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
    ${RAPIDXML_INCLUDE_DIR}
)

# 收集源文件
file(GLOB_RECURSE SDK_SOURCES
    "src/*.cpp"
)

# 创建库目标
add_library(${PROJECT_NAME} SHARED ${SDK_SOURCES})

# 设置库的属性
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "include/robotserver_sdk.h;include/types.h"
)

# 链接依赖库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${Boost_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# 安装目标
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

# 安装头文件
install(DIRECTORY include/
    DESTINATION include
)

# 添加示例子目录
add_subdirectory(examples)

# 启用测试
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 生成包配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# 添加卸载支持
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

# 输出配置信息
message(STATUS "")
message(STATUS "RobotServer SDK Configuration:")
message(STATUS "  Version:           ${PROJECT_VERSION}")
message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  C++ Compiler:      ${CMAKE_CXX_COMPILER}")
message(STATUS "  C++ flags:         ${CMAKE_CXX_FLAGS}")
message(STATUS "  Boost version:     ${Boost_VERSION}")
message(STATUS "")
