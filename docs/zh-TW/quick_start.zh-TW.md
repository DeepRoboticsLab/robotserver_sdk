# 快速入門指南

本指南將幫助您快速上手使用機器狗 RobotServer SDK，即使您不熟悉 C++ 程式語言。

## 系統要求

在開始使用 SDK 之前，請確保您的系統滿足以下要求：

- 作業系統：Linux、macOS 或 Windows
- C++17 或更高版本的編譯器
- CMake 3.10 或更高版本
- Boost 1.66 或更高版本
- nlohmann/json 庫
- rapidxml 庫

```bash
chmod +x scripts/install_dependencies.sh
./scripts/install_dependencies.sh
```

## 安裝 SDK

### 從原始碼構建

1. 克隆 SDK 代碼庫：

```bash
git clone https://github.com/DeepRoboticsLab/robotserver_sdk.git
cd robotserver_sdk
```

2. 創建構建目錄並編譯：

```bash
mkdir build && cd build
cmake ..
make
```

3. 安裝 SDK（可選）：

```bash
sudo make install / sudo make uninstall
```

## 基本用法

### 完整示例

參考 `examples/basic/basic_example.cpp` 文件，實現了一個簡單的示例，展示如何使用 SDK 連接到機器狗並發送導航任務。

### 1. 包含頭文件

```cpp
#include <robotserver_sdk.h>
```

### 2. 創建 SDK 實例

```cpp
// 創建 SDK 實例
robotserver_sdk::RobotServerSdk sdk;
```

### 3. 連接到機器狗控制系統

```cpp
// 連接到機器狗控制系統
if (!sdk.connect("192.168.1.106", 30000)) {
    std::cerr << "連接失敗!" << std::endl;
    return 1;
}
```

### 4. 獲取實時狀態

```cpp
// 獲取實時狀態
auto status = sdk.request1002_RunTimeState();
std::cout << "當前位置: (" << status.posX << ", " << status.posY << ", " << status.posZ << ")" << std::endl;
```

### 5. 發送導航任務

```cpp
// 創建導航點
auto points = default_navigation_points;

// 發送導航任務
sdk.request1003_StartNavTask(points, [](void(const NavigationResult& navigationResult)) {
    if (navigationResult.errorCode == ErrorCode_Navigation::SUCCESS) {
        std::cout << "導航任務成功完成!" << std::endl;
    } else {
        std::cout << "導航任務失敗, errorStatus: " << static_cast<int>(navigationResult.errorStatus) << std::endl;
    }
});
```

### 6. 查詢導航任務狀態

```cpp
// 查詢任務狀態
auto taskStatus = sdk.request1007_NavTaskState();
std::cout << "任務狀態: " << static_cast<int>(taskStatus.status) << std::endl;
```

### 7. 取消導航任務

```cpp
// 取消導航任務
if (sdk.request1004_CancelNavTask()) {
    std::cout << "導航任務已取消" << std::endl;
} else {
    std::cerr << "導航任務取消失敗" << std::endl;
}
```

### 8. 斷開連接

```cpp
// 斷開連接
sdk.disconnect();
```

## 下一步

- 查看 [SDK 架構概述](architecture.zh-TW.md) 了解 SDK 的整體架構和設計理念
- 查看 [API 參考](api_reference.zh-TW.md) 了解更多 SDK 功能
