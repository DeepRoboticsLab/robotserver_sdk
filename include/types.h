#pragma once

#include <functional>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
namespace robotserver_sdk {

/**
 * @brief 1003 导航任务响应ErrorCode枚举
 */
enum class ErrorCode_Navigation {
    SUCCESS = 0,      ///< 操作成功
    FAILURE = 1,      ///< 操作失败
    CANCELLED = 2,    ///< 操作被取消

    INVALID_PARAM = 3,///< 无效参数
    NOT_CONNECTED = 4,///< 未连接
    UNKNOWN_ERROR = 5 ///< 未知错误
};

/**
 * @brief 1003 导航任务响应ErrorStatus枚举
 */
enum class ErrorStatus_Navigation {
    DEFAULT = 0,                                                                 ///< 默认值
    SINGLE_POINT_INSPECTION_TASK_CANCELLED = 8962,                               ///< 单点巡检任务被取消
    SINGLE_POINT_INSPECTION_TASK_COMPLETED = 8960,                               ///< 单点巡检任务执行完成
    MOTION_STATE_EXCEPTION_FAILED = 41729,                                       ///< 运动状态异常，任务失败 (软急停、摔倒)
    LOW_POWER_FAILED = 41730,                                                    ///< 电量过低，任务失败
    MOTOR_OVER_TEMPERATURE_EXCEPTION_FAILED = 41731,                             ///< 电机过温异常，任务失败
    USING_CHARGER_CHARGING_FAILED = 41732,                                       ///< 正在使用充电器充电，任务失败
    NAVIGATION_PROCESS_NOT_STARTED_FAILED = 41745,                               ///< 导航进程未启动，无法下发任务
    NAVIGATION_MODULE_COMMUNICATION_EXCEPTION_FAILED = 41746,                    ///< 导航模块通讯异常，无法下发任务
    POSITION_STATE_CONTINUOUSLY_EXCEPTION_FAILED = 41747,                        ///< 定位状态持续异常 (超过 30s)
    TERRAIN_MODULE_STATE_EXCEPTION_FAILED = 41748,                               ///< 地形模块状态异常
    STAND_UP_FAILED = 41761,                                                     ///< 发送起立失败
    EXECUTE_STAND_UP_FAILED = 41762,                                             ///< 执行起立失败
    SWITCH_FORCE_CONTROL_FAILED = 41763,                                         ///< 切换力控失败
    SWITCH_WALKING_MODE_FAILED = 41764,                                          ///< 切换行走模式失败
    PUP_FAILED = 41765,                                                          ///< 趴下失败
    SOFT_EMERGENCY_STOP_FAILED = 41766,                                          ///< 被软急停
    SWITCH_GAIT_FAILED = 41767,                                                  ///< 切换步态失败
    SWITCH_NAVIGATION_MODE_FAILED = 41768,                                       ///< 切换导航模式失败
    SWITCH_MANUAL_MODE_FAILED = 41769,                                           ///< 切换手动模式失败
    SWITCH_NORMAL_OR_CRAWL_HEIGHT_STATE_FAILED = 41770,                          ///< 切换正常 / 匍匐身高状态失败
    SWITCH_STOP_AVOIDANCE_MODULE_SPEED_INPUT_SOURCE_FAILED = 41777,              ///< 切换停避障模块的速度输入源失败
    SET_TERRAIN_MAP_PARAMETER_FAILED = 41778,                                    ///< 设置地形图参数失败
    CURRENTLY_EXECUTING_TASK_FAILED = 41793,                                     ///< 当前正在执行任务，下发新任务失败
    SCHEDULE_EXIT_SELF_CHARGING_FAILED = 41794,                                  ///< 调度退出自主充电失败
    EXIT_SELF_CHARGING_EXECUTION_FAILED = 41795,                                 ///< 退出自主充电执行失败
    SCHEDULE_ENTER_SELF_CHARGING_FAILED = 41796,                                 ///< 调度进入自主充电失败
    ENTER_SELF_CHARGING_EXECUTION_FAILED = 41797,                                ///< 进入自主充电执行失败
    EXIT_PILE_RELOCATION_FAILED = 41798,                                         ///< 退桩后重定位失败
    OPEN_ACCUMULATION_FRAME_FAILED = 41799,                                      ///< 开启累积帧失败
    CLOSE_ACCUMULATION_FRAME_FAILED = 41800,                                     ///< 关闭累积帧失败
    SWITCH_MAP_FAILED = 41801,                                                   ///< 切换地图失败
    EXIST_UPPER_MACHINE_CONNECTION_DISCONNECTED_AUTO_STOP_TASK_FAILED = 41802,   ///< 存在上位机连接断开，自动停止任务
    STOP_AVOIDANCE_MODULE_STATE_EXCEPTION_FAILED = 41803,                        ///< 持续停障异常，导航失败
    NAVIGATION_GLOBAL_PLANNING_FAILED = 41804,                                   ///< 导航全局规划失败
    NAVIGATION_CONTINUOUS_NAVIGATION_SPEED_NOT_REFRESHED_FAILED = 41805,         ///< 持续导航速度未刷新，导航失败
    SELF_CHARGING_PROCESS_FAILED = 41806,                                        ///< 自主充电流程中，下发任务失败
    RELOCATION_FAILED = 41881,                                                   ///< 重定位失败
    PROCESS_MANUAL_RESTART_STOP_TASK_FAILED = 41983                              ///< 进程手动重启中，停止任务
};

/**
 * @brief 1007 任务状态查询错误码枚举
 */
enum class ErrorCode_QueryStatus {
    COMPLETED = 0,          ///< 任务已完成
    EXECUTING = 1,          ///< 任务执行中
    FAILED = -1,            ///< 无法执行

    INVALID_RESPONSE = 2,   ///< 无效响应
    TIMEOUT = 3,            ///< 超时
    NOT_CONNECTED = 4,      ///< 未连接
    UNKNOWN_ERROR = 5       ///< 未知错误
};

/**
 * @brief 1007 任务状态查询status 枚举
 */
enum class Status_QueryStatus {
    COMPLETED = 0,    ///< 任务已完成
    EXECUTING = 1,    ///< 任务执行中
    FAILED = -1       ///< 无法执行
};

/**
 * @brief 1002 实时状态查询错误码枚举
 */
enum class ErrorCode_RealTimeStatus {
    SUCCESS = 0,            ///< 操作成功

    INVALID_RESPONSE = 1,   ///< 无效响应
    TIMEOUT = 2,            ///< 超时
    NOT_CONNECTED = 3,      ///< 未连接
    UNKNOWN_ERROR = 4       ///< 未知错误
};

/**
 * @brief 导航点信息
 */
struct NavigationPoint {
    int mapId = 0;       ///< 地图ID
    int value = 0;       ///< 点值
    double posX = 0.0;   ///< X坐标
    double posY = 0.0;   ///< Y坐标
    double posZ = 0.0;   ///< Z坐标
    double angleYaw = 0.0;///< Yaw角度
    int pointInfo = 0;   ///< 点信息
    int gait = 0;        ///< 步态
    int speed = 0;       ///< 速度
    int manner = 0;      ///< 方式
    int obsMode = 0;     ///< 障碍物模式
    int navMode = 0;     ///< 导航模式
    int terrain = 0;     ///< 地形
    int posture = 0;     ///< 姿态

    static NavigationPoint fromJson(const nlohmann::json& json) {
        NavigationPoint point;
        point.mapId = json.value("MapID", 0);
        point.value = json.value("Value", 0);
        point.posX = json.value("PosX", 0.0);
        point.posY = json.value("PosY", 0.0);
        point.posZ = json.value("PosZ", 0.0);
        point.angleYaw = json.value("AngleYaw", 0.0);
        point.pointInfo = json.value("PointInfo", 0);
        point.gait = json.value("Gait", 0);
        point.speed = json.value("Speed", 0);
        point.manner = json.value("Manner", 0);
        point.obsMode = json.value("ObsMode", 0);
        point.navMode = json.value("NavMode", 0);
        point.terrain = json.value("Terrain", 0);
        point.posture = json.value("Posture", 0);
        return point;
    }
};

/**
 * @brief 1002 实时状态信息
 */
struct RealTimeStatus {
    int motionState = 0;                ///< 运动状态
    double posX = 0.0;                  ///< 位置X
    double posY = 0.0;                  ///< 位置Y
    double posZ = 0.0;                  ///< 位置Z
    double angleYaw = 0.0;              ///< 角度Yaw
    double roll = 0.0;                  ///< 角度Roll
    double pitch = 0.0;                 ///< 角度Pitch
    double yaw = 0.0;                   ///< 角度Yaw
    double speed = 0.0;                 ///< 速度
    double curOdom = 0.0;               ///< 当前里程
    double sumOdom = 0.0;               ///< 累计里程
    uint64_t curRuntime = 0;            ///< 当前运行时间
    uint64_t sumRuntime = 0;            ///< 累计运行时间
    double res = 0.0;                   ///< 响应时间
    double x0 = 0.0;                    ///< 坐标X0
    double y0 = 0.0;                    ///< 坐标Y0
    int h = 0;                          ///< 高度
    int electricity = 0;                ///< 电量
    int location = 0;                   ///< 位置  定位正常=0, 定位丢失=1
    int RTKState = 0;                   ///< RTK状态
    int onDockState = 0;                ///< 上岸状态
    int gaitState = 0;                  ///< 步态状态
    int motorState = 0;                 ///< 电机状态
    int chargeState = 0;                ///< 充电状态
    int controlMode = 0;                ///< 控制模式
    int mapUpdateState = 0;             ///< 地图更新状态

    ErrorCode_RealTimeStatus errorCode = ErrorCode_RealTimeStatus::SUCCESS; ///< 错误码
};
/**
 * @brief 1003 导航任务结果
 */
struct NavigationResult {
    int value = 0;                                                  ///< 导航任务目标点编号，与下发导航任务请求对应
    ErrorCode_Navigation errorCode = ErrorCode_Navigation::SUCCESS; ///< 错误码 0:成功; 1:失败; 2:取消
    ErrorStatus_Navigation errorStatus = ErrorStatus_Navigation::DEFAULT;                 ///< 错误状态码; 导航任务失败的具体原因
};

/**
 * @brief 1007 任务状态查询结果
 */
struct TaskStatusResult {
    int value = 0;                                                      ///< 导航任务目标点编号，与下发导航任务请求对应
    Status_QueryStatus status = Status_QueryStatus::COMPLETED;              ///< 导航状态:  0:已完成; 1:执行中; 2:失败
    ErrorCode_QueryStatus errorCode = ErrorCode_QueryStatus::COMPLETED; ///< 错误码:   0:成功; 1:执行中; 2:失败
};

/**
 * @brief SDK配置选项
 */
struct SdkOptions {
    std::chrono::milliseconds connectionTimeout{5000}; ///< 连接超时时间
    std::chrono::milliseconds requestTimeout{3000};    ///< 请求超时时间
};

/**
 * @brief 导航任务结果回调函数类型
 */
using NavigationResultCallback = std::function<void(const NavigationResult&)>;

} // namespace robotserver_sdk
