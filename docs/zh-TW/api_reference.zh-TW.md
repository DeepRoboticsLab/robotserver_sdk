# API 參考

本文檔提供機器狗 RobotServer SDK 的詳細 API 參考，包括所有公共類、方法、枚舉和數據結構的說明。

## 目錄

- [RobotServerSdk 類](#RobotServerSdk-類)
- [數據類型](#數據類型)
- [枚舉類型](#枚舉類型)
- [回調函數](#回調函數)

## RobotServerSdk 類

`RobotServerSdk` 是 SDK 的主要接口類，提供與機器狗控制系統通信的所有功能。

### 構造函數和析構函數

```cpp
/**
 * @brief 創建 RobotServerSdk 實例
 */
RobotServerSdk();

/**
 * @brief 銷毀 RobotServerSdk 實例，釋放所有資源
 */
~RobotServerSdk();
```

### 連接管理

```cpp
/**
 * @brief 連接到機器狗控制系統
 * @param host 主機地址（IP 或域名）
 * @param port 端口號
 * @return 如果連接請求已成功發送，則返回 true；否則返回 false
 */
bool connect(const std::string& host, uint16_t port);

/**
 * @brief 斷開與機器狗控制系統的連接
 */
void disconnect();

/**
 * @brief 檢查是否已連接到機器狗控制系統
 * @return 如果已連接，則返回 true；否則返回 false
 */
bool isConnected() const;
```

### 實時狀態

```cpp
/**
 * @brief 獲取機器狗實時狀態（同步方法）
 * @return 包含實時狀態如位置、速度、角度、電量等
 */
RealTimeStatus request1002_RunTimeState();
```

### 導航任務

```cpp
/**
 * @brief 開始導航任務（異步方法）
 * @param navigation_points 導航點列表
 * @param navigationResultCallback 結果回調函數
 * @note 導航任務完成後，會通過回調函數返回結果; 回調函數在IO線程中調用，不應執行長時間操作
 */
void request1003_StartNavTask(
    const std::vector<NavigationPoint>& navigation_points,
    NavigationResultCallback navigationResultCallback);

/**
 * @brief 取消導航任務（同步方法）
 * @return 如果取消成功，則返回 true；否則返回 false
 */
bool request1004_CancelNavTask();

/**
 * @brief 查詢導航任務狀態（同步方法）
 * @return 包含任務狀態和錯誤碼
 */
TaskStatusResult request1007_NavTaskState();
```

### 版本信息

```cpp
/**
 * @brief 獲取 SDK 版本信息
 * @return SDK 版本字符串
 */
static std::string getVersion();
```

## 數據類型

### NavigationPoint

```cpp
/**
 * @brief 1003 導航點
 */
struct NavigationPoint {
    int mapId = 0;       ///< 地圖ID
    int value = 0;       ///< 點值
    double posX = 0.0;   ///< X坐標
    double posY = 0.0;   ///< Y坐標
    double posZ = 0.0;   ///< Z坐標
    double angleYaw = 0.0;///< Yaw角度
    int pointInfo = 0;   ///< 點信息
    int gait = 0;        ///< 步態
    int speed = 0;       ///< 速度
    int manner = 0;      ///< 方式
    int obsMode = 0;     ///< 障礙物模式
    int navMode = 0;     ///< 導航模式
    int terrain = 0;     ///< 地形
    int posture = 0;     ///< 姿態
};
```

### RealTimeStatus

```cpp
/**
 * @brief 1002 獲取機器狗實時狀態
 */
struct RealTimeStatus {
    int motionState = 0;                ///< 運動狀態
    double posX = 0.0;                  ///< 位置X
    double posY = 0.0;                  ///< 位置Y
    double posZ = 0.0;                  ///< 位置Z
    double angleYaw = 0.0;              ///< 角度Yaw
    double roll = 0.0;                  ///< 角度Roll
    double pitch = 0.0;                 ///< 角度Pitch
    double yaw = 0.0;                   ///< 角度Yaw
    double speed = 0.0;                 ///< 速度
    double curOdom = 0.0;               ///< 當前里程
    double sumOdom = 0.0;               ///< 累計里程
    uint64_t curRuntime = 0;            ///< 當前運行時間
    uint64_t sumRuntime = 0;            ///< 累計運行時間
    double res = 0.0;                   ///< 響應時間
    double x0 = 0.0;                    ///< 坐標X0
    double y0 = 0.0;                    ///< 坐標Y0
    int h = 0;                          ///< 高度
    int electricity = 0;                ///< 電量
    int location = 0;                   ///< 位置  定位正常=0, 定位丟失=1
    int RTKState = 0;                   ///< RTK狀態
    int onDockState = 0;                ///< 上岸狀態
    int gaitState = 0;                  ///< 步態狀態
    int motorState = 0;                 ///< 電機狀態
    int chargeState = 0;                ///< 充電狀態
    int controlMode = 0;                ///< 控制模式
    int mapUpdateState = 0;             ///< 地圖更新狀態
};
```

### NavigationResult

```cpp
/**
 * @brief 1003 導航任務結果
 */
struct NavigationResult {
    int value = 0;                                                  ///< 導航任務目標點編號，與下發導航任務請求對應
    ErrorCode_Navigation errorCode = ErrorCode_Navigation::SUCCESS; ///< 錯誤碼 0:成功; 1:失敗; 2:取消
    ErrorStatus errorStatus = ErrorStatus::DEFAULT;                 ///< 錯誤狀態碼; 導航任務失敗的具體原因
};
```

### TaskStatusResult

```cpp
/**
 * @brief 1007 任務狀態查詢結果
 */
struct TaskStatusResult {
    int value = 0;                                                      ///< 導航任務目標點編號，與下發導航任務請求對應
    NavigationStatus status = NavigationStatus::COMPLETED;              ///< 導航狀態:  0:已完成; 1:執行中; 2:失敗
    ErrorCode_QueryStatus errorCode = ErrorCode_QueryStatus::COMPLETED; ///< 錯誤碼:   0:成功; 1:執行中; 2:失敗
};
```

## 枚舉類型

### ErrorCode_Navigation

```cpp
/**
 * @brief 1003 導航任務響應ErrorCode枚舉
 */
enum class ErrorCode_Navigation {
    SUCCESS = 0,      ///< 操作成功
    FAILURE = 1,      ///< 操作失敗
    CANCELLED = 2,    ///< 操作被取消

    INVALID_PARAM = 3,///< 無效參數
    NOT_CONNECTED = 4 ///< 未連接
};
```

### ErrorStatus_Navigation

```cpp
/**
 * @brief 1003 導航任務響應ErrorStatus枚舉
 */
enum class ErrorStatus_Navigation {
    DEFAULT = 0,                                                                 ///< 默認值
    SINGLE_POINT_INSPECTION_TASK_CANCELLED = 8962,                               ///< 單點巡檢任務被取消
    SINGLE_POINT_INSPECTION_TASK_COMPLETED = 8960,                               ///< 單點巡檢任務執行完成
    MOTION_STATE_EXCEPTION_FAILED = 41729,                                       ///< 運動狀態異常，任務失敗 (軟急停、摔倒)
    LOW_POWER_FAILED = 41730,                                                    ///< 電量過低，任務失敗
    MOTOR_OVER_TEMPERATURE_EXCEPTION_FAILED = 41731,                             ///< 電機過溫異常，任務失敗
    USING_CHARGER_CHARGING_FAILED = 41732,                                       ///< 正在使用充電器充電，任務失敗
    NAVIGATION_PROCESS_NOT_STARTED_FAILED = 41745,                               ///< 導航進程未啟動，無法下發任務
    NAVIGATION_MODULE_COMMUNICATION_EXCEPTION_FAILED = 41746,                    ///< 導航模塊通訊異常，無法下發任務
    POSITION_STATE_CONTINUOUSLY_EXCEPTION_FAILED = 41747,                        ///< 定位狀態持續異常 (超過 30s)
    TERRAIN_MODULE_STATE_EXCEPTION_FAILED = 41748,                               ///< 地形模塊狀態異常
    STAND_UP_FAILED = 41761,                                                     ///< 發送起立失敗
    EXECUTE_STAND_UP_FAILED = 41762,                                             ///< 執行起立失敗
    SWITCH_FORCE_CONTROL_FAILED = 41763,                                         ///< 切換力控失敗
    SWITCH_WALKING_MODE_FAILED = 41764,                                          ///< 切換行走模式失敗
    PUP_FAILED = 41765,                                                          ///< 趴下失敗
    SOFT_EMERGENCY_STOP_FAILED = 41766,                                          ///< 被軟急停
    SWITCH_GAIT_FAILED = 41767,                                                  ///< 切換步態失敗
    SWITCH_NAVIGATION_MODE_FAILED = 41768,                                       ///< 切換導航模式失敗
    SWITCH_MANUAL_MODE_FAILED = 41769,                                           ///< 切換手動模式失敗
    SWITCH_NORMAL_OR_CRAWL_HEIGHT_STATE_FAILED = 41770,                          ///< 切換正常 / 匍匐身高狀態失敗
    SWITCH_STOP_AVOIDANCE_MODULE_SPEED_INPUT_SOURCE_FAILED = 41777,              ///< 切換停避障模塊的速度輸入源失敗
    SET_TERRAIN_MAP_PARAMETER_FAILED = 41778,                                    ///< 設置地形圖參數失敗
    CURRENTLY_EXECUTING_TASK_FAILED = 41793,                                     ///< 當前正在執行任務，下發新任務失敗
    SCHEDULE_EXIT_SELF_CHARGING_FAILED = 41794,                                  ///< 調度退出自主充電失敗
    EXIT_SELF_CHARGING_EXECUTION_FAILED = 41795,                                 ///< 退出自主充電執行失敗
    SCHEDULE_ENTER_SELF_CHARGING_FAILED = 41796,                                 ///< 調度進入自主充電失敗
    ENTER_SELF_CHARGING_EXECUTION_FAILED = 41797,                                ///< 進入自主充電執行失敗
    EXIT_PILE_RELOCATION_FAILED = 41798,                                         ///< 退樁後重定位失敗
    OPEN_ACCUMULATION_FRAME_FAILED = 41799,                                      ///< 開啟累積幀失敗
    CLOSE_ACCUMULATION_FRAME_FAILED = 41800,                                     ///< 關閉累積幀失敗
    SWITCH_MAP_FAILED = 41801,                                                   ///< 切換地圖失敗
    EXIST_UPPER_MACHINE_CONNECTION_DISCONNECTED_AUTO_STOP_TASK_FAILED = 41802,   ///< 存在上位機連接斷開，自動停止任務
    STOP_AVOIDANCE_MODULE_STATE_EXCEPTION_FAILED = 41803,                        ///< 持續停障異常，導航失敗
    NAVIGATION_GLOBAL_PLANNING_FAILED = 41804,                                   ///< 導航全局規劃失敗
    NAVIGATION_CONTINUOUS_NAVIGATION_SPEED_NOT_REFRESHED_FAILED = 41805,         ///< 持續導航速度未刷新，導航失敗
    SELF_CHARGING_PROCESS_FAILED = 41806,                                        ///< 自主充電流程中，下發任務失敗
    RELOCATION_FAILED = 41881,                                                   ///< 重定位失敗
    PROCESS_MANUAL_RESTART_STOP_TASK_FAILED = 41983                              ///< 進程手動重啟中，停止任務
};
```

### ErrorCode_QueryStatus

```cpp
/**
 * @brief 1007 任務狀態查詢ErrorCode枚舉
 */
enum class ErrorCode_QueryStatus {
    COMPLETED = 0,          ///< 任務已完成
    EXECUTING = 1,          ///< 任務執行中
    FAILED = -1,            ///< 無法執行

    INVALID_RESPONSE = 2,   ///< 無效響應
    TIMEOUT = 3,            ///< 超時
    NOT_CONNECTED = 4,      ///< 未連接
    UNKNOWN_ERROR = 5       ///< 未知錯誤
};
```

### Status_QueryStatus

```cpp
/**
 * @brief 1007 任務狀態查詢status枚舉
 */
enum class Status_QueryStatus {
    COMPLETED = 0,    ///< 任務已完成
    EXECUTING = 1,    ///< 任務執行中
    FAILED = -1       ///< 無法執行
};
```

### ErrorCode_RealTimeStatus

```cpp
/**
 * @brief 1002 實時狀態查詢ErrorCode枚舉
 */
enum class ErrorCode_RealTimeStatus {
    SUCCESS = 0,            ///< 操作成功

    INVALID_RESPONSE = 1,   ///< 無效響應
    TIMEOUT = 2,            ///< 超時
    NOT_CONNECTED = 3,      ///< 未連接
    UNKNOWN_ERROR = 4       ///< 未知錯誤
};
```

### MessageType

```cpp
/**
 * @brief 消息類型枚舉
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

## 回調函數

### NavigationResultCallback

```cpp
/**
 * @brief 導航結果回調函數類型
 * @param result 導航結果
 */
using NavigationResultCallback = std::function<void(const NavigationResult& result)>;
```

## 使用示例

參考 `examples/basic/basic_example.cpp` 文件，實現了一個簡單的示例，展示如何使用 SDK 連接到機器狗並發送導航任務。

## 錯誤處理

SDK 使用 `ErrorCode` 枚舉表示操作結果。每個同步方法都返回一個包含結果和錯誤碼的結構體，每個異步方法都在回調函數中提供結果和錯誤碼。

## 線程安全性

SDK 的所有公共 API 都是線程安全的，可以從多個線程同時調用。回調函數在 IO 線程中執行，不應執行長時間操作。

## 版本歷史

| 版本 | 發布日期 | 主要變更 |
|------|----------|---------|
| 0.1.0 | 2025-03-07 | 初始版本 |

## 下一步

- 查看 [快速入門](quick_start.zh-TW.md) 了解 SDK 的整體架構和設計理念
- 查看 [SDK 架構概述](architecture.zh-TW.md) 了解 SDK 的整體架構和設計理念
