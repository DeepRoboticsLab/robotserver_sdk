# RobotServer SDK

## 简介

RobotServer SDK 提供了一套简单易用的接口，用于控制和监控机器狗的导航任务。该 SDK 封装了底层的协议和网络通信细节，使开发者能够专注于业务逻辑的实现。

## 功能特性

- 连接和断开与机器狗控制系统的通信
- 1002 获取机器狗的实时状态信息
- 1003 发送导航任务指令
- 1004 取消正在执行的导航任务
- 1007 查询当前导航任务的执行状态

## 系统要求

- C++17 或更高版本
- CMake 3.10 或更高版本
- Boost 1.66 或更高版本 (用于网络通信)
- nlohmann/json (用于JSON解析)
- rapidxml (用于XML解析)

## 安装

### 安装依赖

```bash
chmod +x scripts/install_dependencies.sh
./scripts/install_dependencies.sh
```

### 使用 CMake 构建

```bash
git clone https://github.com/DeepRoboticsLab/robotserver_sdk.git
cd robotserver_sdk
mkdir build && cd build
cmake ..
make
sudo make install / sudo make uninstall (optional)
./bin/basic_example 192.168.1.106 30000
```

## 快速开始

参考 `examples/basic/basic_example.cpp` 文件，实现了一个简单的示例，展示如何使用 SDK 连接到机器狗并发送导航任务。

1. 先建图，参考用户手册
2. 手柄新建导航路线同步到导航主机，覆盖 `./examples/basic/default_navigation_points.json` 文件
3. 运行示例程序

```bash
./bin/basic_example 192.168.1.106 30000
```

## 项目结构

```bash
├── CMakeLists.txt                 # 主项目构建配置文件
├── README.md                      # 多语言文档入口
├── README.en.md                   # 英文文档
├── README.zh-CN.md                # 简体中文文档
├── README.zh-TW.md                # 繁体中文文档
├── docs                           # 文档目录
│   └── zh-CN                      # 中文文档
│       └── quick_start.zh-CN.md   # 中文快速入门指南
│       └── architecture.zh-CN.md  # 中文架构概述
│       └── api_reference.zh-CN.md # 中文API参考
├── examples                       # 示例代码目录
│   └── basic                      # 基础示例
│       ├── basic_example.cpp      # 基础示例代码
│       └── default_navigation_points.json # 默认导航点配置
├── include                        # 公共头文件目录
│   ├── robotserver_sdk.h          # SDK主头文件
│   └── types.h                    # 类型定义头文件
├── scripts                        # 辅助脚本目录
│   └── install_dependencies.sh    # 依赖安装脚本
└── src                            # 源代码目录
    ├── robotserver_sdk.cpp        # SDK主实现文件
    ├── network                    # 网络模块
    │   ├── asio_network_model.hpp # Boost.Asio网络模型头文件
    │   └── asio_network_model.cpp # Boost.Asio网络模型实现
    └── protocol                   # 协议模块
        ├── messages.hpp           # 消息定义头文件
        ├── messages.cpp           # 消息定义实现
        ├── serializer.hpp         # 序列化器头文件
        └── serializer.cpp         # 序列化器实现
```

## 文档目录

1. [快速入门指南](docs/zh-CN/quick_start.zh-CN.md) - 快速上手使用 SDK 的基本功能
2. [SDK 架构概述](docs/zh-CN/architecture.zh-CN.md) - SDK 的整体架构和设计理念
3. [API 参考](docs/zh-CN/api_reference.zh-CN.md) - 详细的 API 使用说明和示例

## 技术特点

- 基于 C++17 标准开发
- 使用 Boost.Asio 实现高效的网络通信
- 支持 XML 格式的消息交互
- 提供同步和异步两种操作方式
- 线程安全设计，支持多线程环境

## 获取帮助

如果您在使用过程中遇到任何问题，或者有任何建议和反馈，请通过以下方式联系我们：

- 提交 GitHub Issue
- 发送邮件至 support@deeprobotics.cn

## 许可证

本项目采用 MIT 许可证。详情请参阅 LICENSE 文件。

## 感谢

感谢您使用 RobotServer SDK！
