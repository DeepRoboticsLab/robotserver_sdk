# Quick Start Guide

This guide will help you quickly get started with the Robot Dog RobotServer SDK, even if you are not familiar with the C++ programming language.

## System Requirements

Before you start using the SDK, please ensure your system meets the following requirements:

- Operating System: Linux, macOS, or Windows
- C++17 or higher compiler
- CMake 3.10 or higher
- Boost 1.66 or higher
- nlohmann/json library
- rapidxml library

```bash
chmod +x scripts/install_dependencies.sh
./scripts/install_dependencies.sh
```

## Installing the SDK

### Building from Source

1. Clone the SDK repository:

```bash
git clone https://github.com/DeepRoboticsLab/robotserver_sdk.git
cd robotserver_sdk
```

2. Create a build directory and compile:

```bash
mkdir build && cd build
cmake ..
make
```

3. Install the SDK (optional):

```bash
sudo make install / sudo make uninstall
```

## Basic Usage

### Complete Example

Refer to the `examples/basic/basic_example.cpp` file, which implements a simple example showing how to use the SDK to connect to the robot dog and send navigation tasks.

### 1. Include Header File

```cpp
#include <robotserver_sdk.h>
```

### 2. Create SDK Instance

```cpp
// Create SDK instance
robotserver_sdk::RobotServerSdk sdk;
```

### 3. Connect to the Robot Dog Control System

```cpp
// Connect to the robot dog control system
if (!sdk.connect("192.168.1.106", 30000)) {
    std::cerr << "Connection failed!" << std::endl;
    return 1;
}
```

### 4. Get Real-time Status

```cpp
// Get real-time status
auto status = sdk.request1002_RunTimeState();
std::cout << "Current position: (" << status.posX << ", " << status.posY << ", " << status.posZ << ")" << std::endl;
```

### 5. Send Navigation Task

```cpp
// Create navigation points
auto points = default_navigation_points;

// Send navigation task
sdk.request1003_StartNavTask(points, [](void(const NavigationResult& navigationResult)) {
    if (navigationResult.errorCode == ErrorCode_Navigation::SUCCESS) {
        std::cout << "Navigation task completed successfully!" << std::endl;
    } else {
        std::cout << "Navigation task failed, errorStatus: " << static_cast<int>(navigationResult.errorStatus) << std::endl;
    }
});
```

### 6. Query Navigation Task Status

```cpp
// Query task status
auto taskStatus = sdk.request1007_NavTaskState();
std::cout << "Task status: " << static_cast<int>(taskStatus.status) << std::endl;
```

### 7. Cancel Navigation Task

```cpp
// Cancel navigation task
if (sdk.request1004_CancelNavTask()) {
    std::cout << "Navigation task cancelled" << std::endl;
} else {
    std::cerr << "Failed to cancel navigation task" << std::endl;
}
```

### 8. Disconnect

```cpp
// Disconnect
sdk.disconnect();
```

## Next Steps

- Check out the [SDK Architecture Overview](architecture.en.md) to understand the overall architecture and design philosophy of the SDK
- Check out the [API Reference](api_reference.en.md) to learn more about SDK functions
