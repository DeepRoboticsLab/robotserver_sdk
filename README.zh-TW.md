# RobotServer SDK

## 簡介

RobotServer SDK 提供了一套簡單易用的接口，用於控制和監控機器狗的導航任務。該 SDK 封裝了底層的協議和網絡通信細節，使開發者能夠專注於業務邏輯的實現。

## 功能特性

- 連接和斷開與機器狗控制系統的通信
- 1002 獲取機器狗的實時狀態信息
- 1003 發送導航任務指令
- 1004 取消正在執行的導航任務
- 1007 查詢當前導航任務的執行狀態

## 系統要求

- C++17 或更高版本
- CMake 3.10 或更高版本
- Boost 1.66 或更高版本 (用於網絡通信)
- nlohmann/json (用於JSON解析)
- rapidxml (用於XML解析)

## 安裝

### 安裝依賴

```bash
chmod +x scripts/install_dependencies.sh
./scripts/install_dependencies.sh
```

### 使用 CMake 構建

```bash
git clone https://github.com/DeepRoboticsLab/robotserver_sdk.git
cd robotserver_sdk
mkdir build && cd build
cmake ..
make
sudo make install / sudo make uninstall (optional)
./bin/basic_example 192.168.1.106 30000
```

## 快速開始

參考 `examples/basic/basic_example.cpp` 文件，實現了一個簡單的示例，展示如何使用 SDK 連接到機器狗並發送導航任務。

1. 先建圖參考用戶手冊
2. 手柄新建導航路線同步到導航主機，覆蓋 `./examples/basic/default_navigation_points.json` 文件
3. 運行示例程序

```bash
./bin/basic_example 192.168.1.106 30000
```

## 項目結構

```bash
├── CMakeLists.txt                 # 主項目構建配置文件
├── README.md                      # 多語言文檔入口
├── README.en.md                   # 英文文檔
├── README.zh-CN.md                # 簡體中文文檔
├── README.zh-TW.md                # 繁體中文文檔
├── docs                           # 文檔目錄
│   └── zh-CN                      # 中文文檔
│       └── quick_start.zh-CN.md   # 中文快速入門指南
│       └── architecture.zh-CN.md  # 中文架構概述
│       └── api_reference.zh-CN.md # 中文API參考
├── examples                       # 示例代碼目錄
│   └── basic                      # 基礎示例
│       ├── basic_example.cpp      # 基礎示例代碼
│       └── default_navigation_points.json # 默認導航點配置
├── include                        # 公共頭文件目錄
│   ├── robotserver_sdk.h          # SDK主頭文件
│   └── types.h                    # 類型定義頭文件
├── scripts                        # 輔助腳本目錄
│   └── install_dependencies.sh    # 依賴安裝腳本
└── src                            # 源代碼目錄
    ├── robotserver_sdk.cpp        # SDK主實現文件
    ├── network                    # 網絡模塊
    │   ├── asio_network_model.hpp # Boost.Asio網絡模型頭文件
    │   └── asio_network_model.cpp # Boost.Asio網絡模型實現
    └── protocol                   # 協議模塊
        ├── messages.hpp           # 消息定義頭文件
        ├── messages.cpp           # 消息定義實現
        ├── serializer.hpp         # 序列化器頭文件
        └── serializer.cpp         # 序列化器實現
```

## 文檔目錄

1. [快速入門指南](docs/zh-TW/quick_start.zh-TW.md) - 快速上手使用 SDK 的基本功能
2. [SDK 架構概述](docs/zh-TW/architecture.zh-TW.md) - SDK 的整體架構和設計理念
3. [API 參考](docs/zh-TW/api_reference.zh-TW.md) - 詳細的 API 使用說明和示例

## 技術特點

- 基於 C++17 標準開發
- 使用 Boost.Asio 實現高效的網絡通信
- 支持 XML 格式的消息交互
- 提供同步和異步兩種操作方式
- 線程安全設計，支持多線程環境

## 獲取幫助

如果您在使用過程中遇到任何問題，或者有任何建議和反饋，請通過以下方式聯繫我們：

- 提交 GitHub Issue
- 發送郵件至 support@deeprobotics.cn

## 許可證

本項目採用 MIT 許可證。詳情請參閱 LICENSE 文件。

## 感謝

感謝您使用 RobotServer SDK！
