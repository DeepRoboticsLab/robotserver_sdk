# RobotServer SDK

## Introduction

RobotServer SDK provides a simple and easy-to-use interface for controlling and monitoring the navigation tasks of robot dogs. The SDK encapsulates the underlying protocol and network communication details, allowing developers to focus on implementing business logic.

> WARNING: Please install and use the sdk in your own computer, it is strictly forbidden to install and use the sdk in 106 navagation host or other robot's host.

## Features

- Connect and disconnect communication with the robot dog control system
- 1002 Get real-time status information of the robot dog
- 1003 Send navigation task commands
- 1004 Cancel currently executing navigation tasks
- 1007 Query the execution status of the current navigation task

## System Requirements

- C++17 or higher
- CMake 3.10 or higher
- Boost 1.66 or higher (for network communication)
- nlohmann/json (for JSON parsing)
- rapidxml (for XML parsing)

## Installation

### Install Dependencies

```bash
chmod +x scripts/install_dependencies.sh
./scripts/install_dependencies.sh
```

### Build with CMake

```bash
git clone https://github.com/DeepRoboticsLab/robotserver_sdk.git
cd robotserver_sdk
mkdir build && cd build
cmake ..
make
sudo make install / sudo make uninstall (optional)
./bin/basic_example 192.168.1.106 30000
```

## Quick Start

Refer to the `examples/basic/basic_example.cpp` file, which implements a simple example showing how to use the SDK to connect to the robot dog and send navigation tasks.

1. Refer to the *Jueying X30 Pro Application Manual* to complete the mapping work
2. Use the controller to create a new navigation route and synchronize it to the navigation host, rename it to `default_navigation_points.json`
3. If the sdk is not compiled, relpace the `. /examples/basic/default_navigation_points.json` file; if the sdk is compiled, relpace the `. /build/bin/default_navigation_points.json` file.
4. Run the sample application

```bash
./bin/basic_example 192.168.1.106 30000
```

> Caution: You can refer to the '3.1 Remote Login' section in the 「Jueying X30 Pro Application Manual」 to view the robotserver's log information in the robot_server.XXX.log file (XXX is the date information) in the corresponding date folder under the jy_cog/system/log directory of the navigation host via remote login. 

## Project Structure

```bash
├── CMakeLists.txt                 # Main project build configuration file
├── README.md                      # Multi-language documentation entry point
├── README.en.md                   # English documentation
├── README.zh-CN.md                # Simplified Chinese documentation
├── README.zh-TW.md                # Traditional Chinese documentation
├── docs                           # Documentation directory
│   ├── zh-CN                      # Simplified Chinese documentation
│   │   ├── quick_start.zh-CN.md   # Simplified Chinese quick start guide
│   │   ├── architecture.zh-CN.md  # Simplified Chinese architecture overview
│   │   └── api_reference.zh-CN.md # Simplified Chinese API reference
│   ├── zh-TW                      # Traditional Chinese documentation
│   │   ├── quick_start.zh-TW.md   # Traditional Chinese quick start guide
│   │   ├── architecture.zh-TW.md  # Traditional Chinese architecture overview
│   │   └── api_reference.zh-TW.md # Traditional Chinese API reference
│   └── en                         # English documentation
│       ├── quick_start.zh-en.md   # English quick start guide
│       ├── architecture.zh-en.md  # English architecture overview
│       └── api_reference.zh-en.md # English API reference
├── examples                       # Example code directory
│   └── basic                      # Basic example
│       ├── basic_example.cpp      # Basic example code
│       └── default_navigation_points.json # Default navigation point configuration
├── include                        # Public header directory
│   ├── robotserver_sdk.h          # SDK main header file
│   └── types.h                    # Type definition header file
├── scripts                        # Helper script directory
│   └── install_dependencies.sh    # Dependency installation script
└── src                            # Source code directory
    ├── robotserver_sdk.cpp        # SDK main implementation file
    ├── network                    # Network module
    │   ├── asio_network_model.hpp # Boost.Asio network model header file
    │   └── asio_network_model.cpp # Boost.Asio network model implementation
    └── protocol                   # Protocol module
        ├── messages.hpp           # Message definition header file
        ├── messages.cpp           # Message definition implementation
        ├── serializer.hpp         # Serializer header file
        └── serializer.cpp         # Serializer implementation
```

## Documentation

1. [Quick Start Guide](docs/en/quick_start.en.md) - Quickly get started with the basic functions of the SDK
2. [SDK Architecture Overview](docs/en/architecture.en.md) - Overall architecture and design philosophy of the SDK
3. [API Reference](docs/en/api_reference.en.md) - Detailed API usage instructions and examples

## Technical Features

- Developed based on C++17 standard
- Uses Boost.Asio for efficient network communication
- Supports XML format message interaction
- Provides both synchronous and asynchronous operation modes
- Thread-safe design, supports multi-threaded environments

## Getting Help

If you encounter any problems during use, or have any suggestions and feedback, please contact us through the following ways:

- Submit a GitHub Issue
- Contact Technical Support
    - For mainland China users, please contact technical support for help
    - Overseas users can send email to zhangminjie@deeprobotics.cn for help

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements

Thank you for using RobotServer SDK!
