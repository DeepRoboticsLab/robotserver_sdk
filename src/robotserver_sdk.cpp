#include <robotserver_sdk.h>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "network/asio_network_model.hpp"
#include "protocol/messages.hpp"

namespace robotserver_sdk {

// SDK版本
static const std::string SDK_VERSION = "0.1.0";

/**
 * @brief 作用域保护类
 * @tparam F 函数类型
 */
template <typename F>
class ScopeGuard {
    F f_;
public:
    ScopeGuard(F f) : f_(std::move(f)) {}
    ~ScopeGuard() { f_(); }

    // 禁止复制和移动
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;
};

/**
 * @brief 创建作用域保护类
 * @tparam F 函数类型
 * @param f 函数对象
 * @return 作用域保护类
 */
template <typename F>
ScopeGuard<F> makeScopeGuard(F f) {
    return ScopeGuard<F>(std::move(f));
}

/**
 * @brief 安全回调包装函数，用于捕获和处理用户回调函数中可能抛出的异常
 * @tparam Callback 回调函数类型
 * @tparam Args 回调函数参数类型
 * @param callback 用户回调函数
 * @param callbackType 回调函数类型描述，用于日志记录
 * @param args 回调函数参数
 */
template<typename Callback, typename... Args>
void safeCallback(const Callback& callback, const std::string& callbackType, Args&&... args) {
    if (!callback) {
        return;
    }

    try {
        callback(std::forward<Args>(args)...);
    } catch (const std::exception& e) {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");

        std::cerr << "[" << ss.str() << "] " << callbackType << " 回调函数异常: " << e.what() << std::endl;
    } catch (...) {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");

        std::cerr << "[" << ss.str() << "] " << callbackType << " 回调函数发生未知异常" << std::endl;
    }
}

RealTimeStatus convertToRealTimeStatus(const protocol::GetRealTimeStatusResponse& realTimeResp) {
    RealTimeStatus status;
    status.motionState = realTimeResp.motionState;
    status.posX = realTimeResp.posX;
    status.posY = realTimeResp.posY;
    status.posZ = realTimeResp.posZ;
    status.angleYaw = realTimeResp.angleYaw;
    status.roll = realTimeResp.roll;
    status.pitch = realTimeResp.pitch;
    status.yaw = realTimeResp.yaw;
    status.speed = realTimeResp.speed;
    status.curOdom = realTimeResp.curOdom;
    status.sumOdom = realTimeResp.sumOdom;
    status.curRuntime = realTimeResp.curRuntime;
    status.sumRuntime = realTimeResp.sumRuntime;
    status.res = realTimeResp.res;
    status.x0 = realTimeResp.x0;
    status.y0 = realTimeResp.y0;
    status.h = realTimeResp.h;
    status.electricity = realTimeResp.electricity;
    status.location = realTimeResp.location;
    status.RTKState = realTimeResp.RTKState;
    status.onDockState = realTimeResp.onDockState;
    status.gaitState = realTimeResp.gaitState;
    status.motorState = realTimeResp.motorState;
    status.chargeState = realTimeResp.chargeState;
    status.controlMode = realTimeResp.controlMode;
    status.mapUpdateState = realTimeResp.mapUpdateState;

    return status;
}

// SDK实现类
class RobotServerSdkImpl : public network::INetworkCallback {
public:
    RobotServerSdkImpl(const SdkOptions& options)
        : options_(options),
          network_model_(std::make_unique<network::AsioNetworkModel>(*this)) {
        // 设置网络模型的连接超时时间
        network_model_->setConnectionTimeout(options_.connectionTimeout);
    }

    ~RobotServerSdkImpl() {
        disconnect();
    }

    bool connect(const std::string& host, uint16_t port) {
        try {
            if (isConnected()) {
                return true;
            }

            return network_model_->connect(host, port);
        } catch (const std::exception& e) {
            std::cerr << "connect 异常: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "connect 未知异常" << std::endl;
            return false;
        }
    }
    void disconnect() {
        try {
            if (!isConnected()) {
                return;
            }

            network_model_->disconnect();
        } catch (const std::exception& e) {
            std::cerr << "disconnect 异常: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "disconnect 未知异常" << std::endl;
        }
    }

    bool isConnected() const {
        try {
            return network_model_->isConnected();
        } catch (const std::exception& e) {
            std::cerr << "isConnected 异常: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "isConnected 未知异常" << std::endl;
            return false;
        }
    }

    RealTimeStatus request1002_RunTimeState() {
        try {
            if (!isConnected()) {
                RealTimeStatus status;
                status.errorCode = ErrorCode_RealTimeStatus::NOT_CONNECTED;
                return status;
            }

            // 创建请求消息
            protocol::GetRealTimeStatusRequest request;
            request.timestamp = getCurrentTimestamp();

            // 生成并设置序列号
            uint16_t seqNum = generateSequenceNumber();
            request.setSequenceNumber(seqNum);

            // 添加到待处理请求，标记为同步请求
            addPendingRequest(seqNum, protocol::MessageType::GET_REAL_TIME_STATUS_RESP);

            // 创建ScopeGuard，在函数结束时自动移除请求
            auto guard = makeScopeGuard([this, seqNum]() {
                removePendingRequest(seqNum);
            });

            // 发送请求
            network_model_->sendMessage(request);

            // 等待响应
            std::unique_ptr<protocol::GetRealTimeStatusResponse> realTimeResp;
            {
                std::unique_lock<std::mutex> lock(pending_requests_mutex_);
                auto& pendingReq = pendingRequests_[seqNum];

                if (!pendingReq.responseReceived) {
                    pendingReq.cv->wait_for(lock, options_.requestTimeout,
                        [&pendingReq]() { return pendingReq.responseReceived; });
                }

                if (!pendingReq.responseReceived) {
                    RealTimeStatus status;
                    status.errorCode = ErrorCode_RealTimeStatus::TIMEOUT;
                    return status;
                }

                auto* castedPtr = dynamic_cast<protocol::GetRealTimeStatusResponse*>(pendingReq.response.get());
                if (castedPtr) {
                    realTimeResp = std::unique_ptr<protocol::GetRealTimeStatusResponse>(castedPtr);
                    pendingReq.response.release();  // 释放所有权但不删除对象
                }
            }

            if (!realTimeResp) {
                RealTimeStatus status;
                status.errorCode = ErrorCode_RealTimeStatus::INVALID_RESPONSE;
                return status;
            }

            // 转换为SDK的RealTimeStatus
            return convertToRealTimeStatus(*realTimeResp);

        } catch (const std::exception& e) {
            std::cerr << "request1002_RunTimeState 异常: " << e.what() << std::endl;
            RealTimeStatus status;
            status.errorCode = ErrorCode_RealTimeStatus::UNKNOWN_ERROR;
            return status;
        } catch (...) {
            std::cerr << "request1002_RunTimeState 未知异常" << std::endl;
            RealTimeStatus status;
            status.errorCode = ErrorCode_RealTimeStatus::UNKNOWN_ERROR;
            return status;
        }
    }

    // 添加基于回调的异步方法实现
    void request1003_StartNavTask(const std::vector<NavigationPoint>& points, NavigationResultCallback callback) {
        try {
            if (!callback || points.empty()) {
                NavigationResult failResult;
                failResult.errorCode = ErrorCode_Navigation::INVALID_PARAM;
                safeCallback(callback, "导航结果", failResult);
                return;
            }

            if (!isConnected()) {
                NavigationResult failResult;
                failResult.errorCode = ErrorCode_Navigation::NOT_CONNECTED;
                safeCallback(callback, "导航结果", failResult);
                return;
            }

            // 创建请求消息
            protocol::NavigationTaskRequest request;
            request.timestamp = getCurrentTimestamp();

            // 生成并设置序列号
            uint16_t seqNum = generateSequenceNumber();
            request.setSequenceNumber(seqNum);

            // 转换导航点
            for (const auto& point : points) {
                protocol::NavigationPoint proto_point;
                proto_point.mapId = point.mapId;
                proto_point.value = point.value;
                proto_point.posX = point.posX;
                proto_point.posY = point.posY;
                proto_point.posZ = point.posZ;
                proto_point.angleYaw = point.angleYaw;
                proto_point.pointInfo = point.pointInfo;
                proto_point.gait = point.gait;
                proto_point.speed = point.speed;
                proto_point.manner = point.manner;
                proto_point.obsMode = point.obsMode;
                proto_point.navMode = point.navMode;
                proto_point.terrain = point.terrain;
                proto_point.posture = point.posture;
                request.points.push_back(proto_point);
            }

            // 保存回调函数
            {
                std::lock_guard<std::mutex> lock(navigation_result_callbacks_mutex_);
                navigation_result_callbacks_[seqNum] = std::move(callback);
            }

            // 发送请求
            network_model_->sendMessage(request);
        } catch (const std::exception& e) {
            std::cerr << "request1003_StartNavTask 异常: " << e.what() << std::endl;
            NavigationResult failResult;
            failResult.errorCode = ErrorCode_Navigation::UNKNOWN_ERROR;
            safeCallback(callback, "导航结果", failResult);
        } catch (...) {
            std::cerr << "request1003_StartNavTask 未知异常" << std::endl;
            NavigationResult failResult;
            failResult.errorCode = ErrorCode_Navigation::UNKNOWN_ERROR;
            safeCallback(callback, "导航结果", failResult);
        }
    }

    bool request1004_CancelNavTask() {
        try {
            if (!isConnected()) {
                return false;
            }

            // 创建请求消息
            protocol::CancelTaskRequest request;
            request.timestamp = getCurrentTimestamp();

            // 生成并设置序列号
            uint16_t seqNum = generateSequenceNumber();
            request.setSequenceNumber(seqNum);

            // 添加到待处理请求，标记为同步请求
            addPendingRequest(seqNum, protocol::MessageType::CANCEL_TASK_RESP);

            // 创建ScopeGuard，在函数结束时自动移除请求
            auto guard = makeScopeGuard([this, seqNum]() {
                removePendingRequest(seqNum);
            });

            // 发送请求
            network_model_->sendMessage(request);

            // 等待响应
            std::unique_ptr<protocol::CancelTaskResponse> cancelResp;
            {
                std::unique_lock<std::mutex> lock(pending_requests_mutex_);
                auto& pendingReq = pendingRequests_[seqNum];

                if (!pendingReq.responseReceived) {
                    pendingReq.cv->wait_for(lock, options_.requestTimeout,
                        [&pendingReq]() { return pendingReq.responseReceived; });
                }

                if (!pendingReq.responseReceived) {
                    return false;
                }

                auto* castedPtr = dynamic_cast<protocol::CancelTaskResponse*>(pendingReq.response.get());
                if (castedPtr) {
                    cancelResp = std::unique_ptr<protocol::CancelTaskResponse>(castedPtr);
                    pendingReq.response.release();  // 释放所有权但不删除对象
                }
            }

            return cancelResp && cancelResp->errorCode == protocol::ErrorCode_CancelTask::SUCCESS;

        } catch (const std::exception& e) {
            std::cerr << "request1004_CancelNavTask 异常: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "request1004_CancelNavTask 未知异常" << std::endl;
            return false;
        }
    }

    TaskStatusResult request1007_NavTaskState() {
        try {
            if (!isConnected()) {
                TaskStatusResult result;
                result.errorCode = ErrorCode_QueryStatus::NOT_CONNECTED;
                return result;
            }

            // 创建请求消息
            protocol::QueryStatusRequest request;
            request.timestamp = getCurrentTimestamp();

            // 生成并设置序列号
            uint16_t seqNum = generateSequenceNumber();
            request.setSequenceNumber(seqNum);

            // 添加到待处理请求，标记为同步请求
            addPendingRequest(seqNum, protocol::MessageType::QUERY_STATUS_RESP);

            // 创建ScopeGuard，在函数结束时自动移除请求
            auto guard = makeScopeGuard([this, seqNum]() {
                removePendingRequest(seqNum);
            });

            // 发送请求
            network_model_->sendMessage(request);

            // 等待响应
            std::unique_ptr<protocol::QueryStatusResponse> statusResp;
            {
                std::unique_lock<std::mutex> lock(pending_requests_mutex_);
                auto& pendingReq = pendingRequests_[seqNum];

                if (!pendingReq.responseReceived) {
                    pendingReq.cv->wait_for(lock, options_.requestTimeout,
                        [&pendingReq]() { return pendingReq.responseReceived; });
                }

                if (!pendingReq.responseReceived) {
                    TaskStatusResult result;
                    result.errorCode = ErrorCode_QueryStatus::TIMEOUT;
                    return result;
                }

                auto* castedPtr = dynamic_cast<protocol::QueryStatusResponse*>(pendingReq.response.get());
                if (castedPtr) {
                    statusResp = std::unique_ptr<protocol::QueryStatusResponse>(castedPtr);
                    pendingReq.response.release();  // 释放所有权但不删除对象
                }
            }

            if (!statusResp) {
                TaskStatusResult result;
                result.errorCode = ErrorCode_QueryStatus::INVALID_RESPONSE;
                return result;
            }

            // 转换为SDK的TaskStatusResult
            TaskStatusResult result;
            result.status = static_cast<Status_QueryStatus>(statusResp->status);
            result.errorCode = static_cast<ErrorCode_QueryStatus>(statusResp->errorCode);
            result.value = statusResp->value;

            return result;

        } catch (const std::exception& e) {
            std::cerr << "request1007_NavTaskState 异常: " << e.what() << std::endl;
            TaskStatusResult result;
            result.errorCode = ErrorCode_QueryStatus::UNKNOWN_ERROR;
            return result;
        } catch (...) {
            std::cerr << "request1007_NavTaskState 未知异常" << std::endl;
            TaskStatusResult result;
            result.errorCode = ErrorCode_QueryStatus::UNKNOWN_ERROR;
            return result;
        }
    }

    // 实现网络回调接口
    void onMessageReceived(std::unique_ptr<protocol::IMessage> message) override {
        try {
            if (!message) {
                return;
            }

            uint16_t seqNum = message->getSequenceNumber();
            protocol::MessageType msgType = message->getType();

            if (msgType == protocol::MessageType::NAVIGATION_TASK_RESP) {

                NavigationResultCallback callback;
                // 检查是否有等待此响应的请求
                {
                    std::lock_guard<std::mutex> lock(navigation_result_callbacks_mutex_);
                    auto it = navigation_result_callbacks_.find(seqNum);
                    if (it != navigation_result_callbacks_.end()) {
                        callback = it->second;
                        navigation_result_callbacks_.erase(it);
                    }
                }

                // 如果有回调，则使用安全回调包装函数调用
                if (callback) {
                    auto* resp = dynamic_cast<protocol::NavigationTaskResponse*>(message.get());
                    if (resp) {
                        NavigationResult result;
                        result.value = resp->value;
                        result.errorCode = static_cast<ErrorCode_Navigation>(resp->errorCode);
                        result.errorStatus = static_cast<ErrorStatus_Navigation>(resp->errorStatus);
                        safeCallback(callback, "导航结果", result);
                    }
                }

                return;
            }

            // 处理其他类型的响应消息
            std::lock_guard<std::mutex> lock(pending_requests_mutex_);
            auto it = pendingRequests_.find(seqNum);
            if (it != pendingRequests_.end() && it->second.expectedResponseType == msgType) {
                it->second.response = std::move(message);
                it->second.responseReceived = true;
                it->second.cv->notify_one();
            }
        } catch (const std::exception& e) {
            std::cerr << "onMessageReceived 异常: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "onMessageReceived 未知异常" << std::endl;
        }
    }

private:

    void addPendingRequest(uint16_t sequenceNumber, protocol::MessageType expectedType) {
        std::lock_guard<std::mutex> lock(pending_requests_mutex_);
        PendingRequest req;
        req.expectedResponseType = expectedType;
        req.responseReceived = false;
        req.cv = std::make_shared<std::condition_variable>();
        pendingRequests_.emplace(sequenceNumber, std::move(req));
    }

    void removePendingRequest(uint16_t sequenceNumber) {
        std::lock_guard<std::mutex> lock(pending_requests_mutex_);
        pendingRequests_.erase(sequenceNumber);
    }

    // 获取当前时间戳
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    SdkOptions options_;
    std::unique_ptr<network::AsioNetworkModel> network_model_;

    // 生成序列号， 从0到65535后溢出回到0
    uint16_t generateSequenceNumber() {
        static std::atomic<uint16_t> sequenceNumber = 0;
        return ++sequenceNumber;
    }

    struct PendingRequest {
        protocol::MessageType expectedResponseType{};
        std::unique_ptr<protocol::IMessage> response{};
        bool responseReceived{false};
        std::shared_ptr<std::condition_variable> cv;
    };

    std::mutex pending_requests_mutex_;  // 保护 pendingRequests_ 的互斥锁
    std::map<uint16_t, PendingRequest> pendingRequests_;

    // TODO: 没有超时清理
    std::mutex navigation_result_callbacks_mutex_;
    std::map<uint16_t, NavigationResultCallback> navigation_result_callbacks_;
};

// RobotServerSdk类的实现
RobotServerSdk::RobotServerSdk(const SdkOptions& options)
    : impl_(std::make_unique<RobotServerSdkImpl>(options)) {
}

RobotServerSdk::~RobotServerSdk() = default;

bool RobotServerSdk::connect(const std::string& host, uint16_t port) {
    return impl_->connect(host, port);
}

void RobotServerSdk::disconnect() {
    impl_->disconnect();
}

bool RobotServerSdk::isConnected() const {
    return impl_->isConnected();
}

RealTimeStatus RobotServerSdk::request1002_RunTimeState() {
    return impl_->request1002_RunTimeState();
}

// 添加基于回调的异步方法实现
void RobotServerSdk::request1003_StartNavTask(const std::vector<NavigationPoint>& points, NavigationResultCallback callback) {
    impl_->request1003_StartNavTask(points, std::move(callback));
}

bool RobotServerSdk::request1004_CancelNavTask() {
    return impl_->request1004_CancelNavTask();
}

TaskStatusResult RobotServerSdk::request1007_NavTaskState() {
    return impl_->request1007_NavTaskState();
}

std::string RobotServerSdk::getVersion() {
    return SDK_VERSION;
}

} // namespace robotserver_sdk
