# API Reference

This document provides a detailed API reference for the Robot Dog RobotServer SDK, including descriptions of all public classes, methods, enumerations, and data structures.

## Table of Contents

- [RobotServerSdk Class](#RobotServerSdk-Class)
- [Data Types](#Data-Types)
- [Enumeration Types](#Enumeration-Types)
- [Callback Functions](#Callback-Functions)

## RobotServerSdk Class

`RobotServerSdk` is the main interface class of the SDK, providing all functionality for communicating with the robot dog control system.

### Constructors and Destructors

```cpp
/**
 * @brief Create a RobotServerSdk instance
 */
RobotServerSdk();

/**
 * @brief Destroy the RobotServerSdk instance, release all resources
 */
~RobotServerSdk();
```

### Connection Management

```cpp
/**
 * @brief Connect to the robot dog control system
 * @param host Host address (IP or domain name)
 * @param port Port number
 * @return Returns true if the connection request was successfully sent; otherwise returns false
 */
bool connect(const std::string& host, uint16_t port);

/**
 * @brief Disconnect from the robot dog control system
 */
void disconnect();

/**
 * @brief Check if connected to the robot dog control system
 * @return Returns true if connected; otherwise returns false
 */
bool isConnected() const;
```

### Real-time Status

```cpp
/**
 * @brief Get the robot dog's real-time status (synchronous method)
 * @return Contains real-time status such as position, speed, angle, battery level, etc.
 */
RealTimeStatus request1002_RunTimeState();
```

### Navigation Tasks

```cpp
/**
 * @brief Start a navigation task (asynchronous method)
 * @param navigation_points List of navigation points
 * @param navigationResultCallback Result callback function
 * @note After the navigation task is completed, the result will be returned through the callback function;
 *       the callback function is called in the IO thread and should not perform long-duration operations
 */
void request1003_StartNavTask(
    const std::vector<NavigationPoint>& navigation_points,
    NavigationResultCallback navigationResultCallback);

/**
 * @brief Cancel a navigation task (synchronous method)
 * @return Returns true if cancellation is successful; otherwise returns false
 */
bool request1004_CancelNavTask();

/**
 * @brief Query navigation task status (synchronous method)
 * @return Contains task status and error code
 */
TaskStatusResult request1007_NavTaskState();
```

### Version Information

```cpp
/**
 * @brief Get SDK version information
 * @return SDK version string
 */
static std::string getVersion();
```

## Data Types

### NavigationPoint

```cpp
/**
 * @brief 1003 Navigation Point
 */
struct NavigationPoint {
    int mapId = 0;       ///< Map ID
    int value = 0;       ///< Point value
    double posX = 0.0;   ///< X coordinate
    double posY = 0.0;   ///< Y coordinate
    double posZ = 0.0;   ///< Z coordinate
    double angleYaw = 0.0;///< Yaw angle
    int pointInfo = 0;   ///< Point information
    int gait = 0;        ///< Gait
    int speed = 0;       ///< Speed
    int manner = 0;      ///< Manner
    int obsMode = 0;     ///< Obstacle mode
    int navMode = 0;     ///< Navigation mode
    int terrain = 0;     ///< Terrain
    int posture = 0;     ///< Posture
};
```

### RealTimeStatus

```cpp
/**
 * @brief 1002 Get robot dog real-time status
 */
struct RealTimeStatus {
    int motionState = 0;                ///< Motion state
    double posX = 0.0;                  ///< Position X
    double posY = 0.0;                  ///< Position Y
    double posZ = 0.0;                  ///< Position Z
    double angleYaw = 0.0;              ///< Angle Yaw
    double roll = 0.0;                  ///< Angle Roll
    double pitch = 0.0;                 ///< Angle Pitch
    double yaw = 0.0;                   ///< Angle Yaw
    double speed = 0.0;                 ///< Speed
    double curOdom = 0.0;               ///< Current odometry
    double sumOdom = 0.0;               ///< Accumulated odometry
    uint64_t curRuntime = 0;            ///< Current runtime
    uint64_t sumRuntime = 0;            ///< Accumulated runtime
    double res = 0.0;                   ///< Response time
    double x0 = 0.0;                    ///< Coordinate X0
    double y0 = 0.0;                    ///< Coordinate Y0
    int h = 0;                          ///< Height
    int electricity = 0;                ///< Battery level
    int location = 0;                   ///< Location  Normal positioning=0, Positioning lost=1
    int RTKState = 0;                   ///< RTK state
    int onDockState = 0;                ///< On dock state
    int gaitState = 0;                  ///< Gait state
    int motorState = 0;                 ///< Motor state
    int chargeState = 0;                ///< Charging state
    int controlMode = 0;                ///< Control mode
    int mapUpdateState = 0;             ///< Map update state
};
```

### NavigationResult

```cpp
/**
 * @brief 1003 Navigation task result
 */
struct NavigationResult {
    int value = 0;                                                  ///< Navigation task target point number, corresponding to the issued navigation task request
    ErrorCode_Navigation errorCode = ErrorCode_Navigation::SUCCESS; ///< Error code 0:Success; 1:Failure; 2:Cancelled
    ErrorStatus errorStatus = ErrorStatus::DEFAULT;                 ///< Error status code; Specific reason for navigation task failure
};
```

### TaskStatusResult

```cpp
/**
 * @brief 1007 Task status query result
 */
struct TaskStatusResult {
    int value = 0;                                                      ///< Navigation task target point number, corresponding to the issued navigation task request
    NavigationStatus status = NavigationStatus::COMPLETED;              ///< Navigation status:  0:Completed; 1:Executing; 2:Failed
    ErrorCode_QueryStatus errorCode = ErrorCode_QueryStatus::COMPLETED; ///< Error code:   0:Success; 1:Executing; 2:Failed
};
```

## Enumeration Types

### ErrorCode_Navigation

```cpp
/**
 * @brief 1003 Navigation task response ErrorCode enumeration
 */
enum class ErrorCode_Navigation {
    SUCCESS = 0,      ///< Operation successful
    FAILURE = 1,      ///< Operation failed
    CANCELLED = 2,    ///< Operation cancelled

    INVALID_PARAM = 3,///< Invalid parameter
    NOT_CONNECTED = 4 ///< Not connected
};
```

### ErrorStatus_Navigation

```cpp
/**
 * @brief 1003 Navigation task response ErrorStatus enumeration
 */
enum class ErrorStatus_Navigation {
    DEFAULT = 0,                                                                 ///< Default value
    SINGLE_POINT_INSPECTION_TASK_CANCELLED = 8962,                               ///< Single point inspection task cancelled
    SINGLE_POINT_INSPECTION_TASK_COMPLETED = 8960,                               ///< Single point inspection task completed
    MOTION_STATE_EXCEPTION_FAILED = 41729,                                       ///< Motion state exception, task failed (soft emergency stop, fall)
    LOW_POWER_FAILED = 41730,                                                    ///< Low power, task failed
    MOTOR_OVER_TEMPERATURE_EXCEPTION_FAILED = 41731,                             ///< Motor over temperature exception, task failed
    USING_CHARGER_CHARGING_FAILED = 41732,                                       ///< Using charger charging, task failed
    NAVIGATION_PROCESS_NOT_STARTED_FAILED = 41745,                               ///< Navigation process not started, cannot issue task
    NAVIGATION_MODULE_COMMUNICATION_EXCEPTION_FAILED = 41746,                    ///< Navigation module communication exception, cannot issue task
    POSITION_STATE_CONTINUOUSLY_EXCEPTION_FAILED = 41747,                        ///< Position state continuously exception (over 30s)
    TERRAIN_MODULE_STATE_EXCEPTION_FAILED = 41748,                               ///< Terrain module state exception
    STAND_UP_FAILED = 41761,                                                     ///< Send stand up failed
    EXECUTE_STAND_UP_FAILED = 41762,                                             ///< Execute stand up failed
    SWITCH_FORCE_CONTROL_FAILED = 41763,                                         ///< Switch force control failed
    SWITCH_WALKING_MODE_FAILED = 41764,                                          ///< Switch walking mode failed
    PUP_FAILED = 41765,                                                          ///< Lie down failed
    SOFT_EMERGENCY_STOP_FAILED = 41766,                                          ///< Soft emergency stop
    SWITCH_GAIT_FAILED = 41767,                                                  ///< Switch gait failed
    SWITCH_NAVIGATION_MODE_FAILED = 41768,                                       ///< Switch navigation mode failed
    SWITCH_MANUAL_MODE_FAILED = 41769,                                           ///< Switch manual mode failed
    SWITCH_NORMAL_OR_CRAWL_HEIGHT_STATE_FAILED = 41770,                          ///< Switch normal/crawl height state failed
    SWITCH_STOP_AVOIDANCE_MODULE_SPEED_INPUT_SOURCE_FAILED = 41777,              ///< Switch stop avoidance module speed input source failed
    SET_TERRAIN_MAP_PARAMETER_FAILED = 41778,                                    ///< Set terrain map parameter failed
    CURRENTLY_EXECUTING_TASK_FAILED = 41793,                                     ///< Currently executing task, issuing new task failed
    SCHEDULE_EXIT_SELF_CHARGING_FAILED = 41794,                                  ///< Schedule exit self charging failed
    EXIT_SELF_CHARGING_EXECUTION_FAILED = 41795,                                 ///< Exit self charging execution failed
    SCHEDULE_ENTER_SELF_CHARGING_FAILED = 41796,                                 ///< Schedule enter self charging failed
    ENTER_SELF_CHARGING_EXECUTION_FAILED = 41797,                                ///< Enter self charging execution failed
    EXIT_PILE_RELOCATION_FAILED = 41798,                                         ///< Exit pile relocation failed
    OPEN_ACCUMULATION_FRAME_FAILED = 41799,                                      ///< Open accumulation frame failed
    CLOSE_ACCUMULATION_FRAME_FAILED = 41800,                                     ///< Close accumulation frame failed
    SWITCH_MAP_FAILED = 41801,                                                   ///< Switch map failed
    EXIST_UPPER_MACHINE_CONNECTION_DISCONNECTED_AUTO_STOP_TASK_FAILED = 41802,   ///< Exist upper machine connection disconnected, auto stop task
    STOP_AVOIDANCE_MODULE_STATE_EXCEPTION_FAILED = 41803,                        ///< Stop avoidance module state exception, navigation failed
    NAVIGATION_GLOBAL_PLANNING_FAILED = 41804,                                   ///< Navigation global planning failed
    NAVIGATION_CONTINUOUS_NAVIGATION_SPEED_NOT_REFRESHED_FAILED = 41805,         ///< Navigation continuous navigation speed not refreshed, navigation failed
    SELF_CHARGING_PROCESS_FAILED = 41806,                                        ///< Self charging process, issuing task failed
    RELOCATION_FAILED = 41881,                                                   ///< Relocation failed
    PROCESS_MANUAL_RESTART_STOP_TASK_FAILED = 41983                              ///< Process manual restart, stop task
};
```

### ErrorCode_QueryStatus

```cpp
/**
 * @brief 1007 Task status query ErrorCode enumeration
 */
enum class ErrorCode_QueryStatus {
    COMPLETED = 0,          ///< Task completed
    EXECUTING = 1,          ///< Task executing
    FAILED = -1,            ///< Cannot execute

    INVALID_RESPONSE = 2,   ///< Invalid response
    TIMEOUT = 3,            ///< Timeout
    NOT_CONNECTED = 4,      ///< Not connected
    UNKNOWN_ERROR = 5       ///< Unknown error
};
```

### Status_QueryStatus

```cpp
/**
 * @brief 1007 Task status query status enumeration
 */
enum class Status_QueryStatus {
    COMPLETED = 0,    ///< Task completed
    EXECUTING = 1,    ///< Task executing
    FAILED = -1       ///< Cannot execute
};
```

### ErrorCode_RealTimeStatus

```cpp
/**
 * @brief 1002 Real-time status query ErrorCode enumeration
 */
enum class ErrorCode_RealTimeStatus {
    SUCCESS = 0,            ///< Operation successful

    INVALID_RESPONSE = 1,   ///< Invalid response
    TIMEOUT = 2,            ///< Timeout
    NOT_CONNECTED = 3,      ///< Not connected
    UNKNOWN_ERROR = 4       ///< Unknown error
};
```

### MessageType

```cpp
/**
 * @brief Message type enumeration
 */
enum class MessageType {
    UNKNOWN = 0,
    GET_REAL_TIME_STATUS_REQ,       // type=1002, command=1
    GET_REAL_TIME_STATUS_RESP,      // type=1002, command=1
    NAVIGATION_TASK_REQ,            // type=1003, command=1
    NAVIGATION_TASK_RESP,           // type=1003, command=1
    CANCEL_TASK_REQ,                // type=1004, command=1
    CANCEL_TASK_RESP,               // type=1004, command=1
    QUERY_STATUS_REQ,               // type=1007, command=1
    QUERY_STATUS_RESP               // type=1007, command=1
};
```

## Callback Functions

### NavigationResultCallback

```cpp
/**
 * @brief Navigation result callback function type
 * @param result Navigation result
 */
using NavigationResultCallback = std::function<void(const NavigationResult& result)>;
```

## Usage Examples

Refer to the `examples/basic/basic_example.cpp` file, which implements a simple example showing how to use the SDK to connect to the robot dog and send navigation tasks.

## Error Handling

The SDK uses `ErrorCode` enumerations to represent operation results. Each synchronous method returns a structure containing the result and error code, and each asynchronous method provides the result and error code in the callback function.

## Thread Safety

All public APIs of the SDK are thread-safe and can be called simultaneously from multiple threads. Callback functions are executed in the IO thread and should not perform long-duration operations.

## Version History

| Version | Release Date | Major Changes |
|---------|--------------|---------------|
| 0.1.0   | 2025-03-07   | Initial version |

## Next Steps

- Check out the [Quick Start Guide](quick_start.en.md) to understand the overall architecture and design philosophy of the SDK
- Check out the [SDK Architecture Overview](architecture.en.md) to understand the overall architecture and design philosophy of the SDK
