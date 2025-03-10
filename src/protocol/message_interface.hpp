#pragma once

#include <string>
#include <memory>

namespace protocol {

/**
 * @brief 消息类型枚举
 */
enum class MessageType {
    UNKNOWN = 0,
    GET_REAL_TIME_STATUS_REQ,
    GET_REAL_TIME_STATUS_RESP,
    NAVIGATION_TASK_REQ,
    NAVIGATION_TASK_RESP,
    CANCEL_TASK_REQ,
    CANCEL_TASK_RESP,
    QUERY_STATUS_REQ,
    QUERY_STATUS_RESP
};

/**
 * @brief 1003 下发导航任务响应错误码枚举
 */
enum class ErrorCode_Navigation {
    SUCCESS = 0,
    FAILURE = 1,
    CANCELLED = 2
};

/**
 * @brief 1004 取消任务响应错误码枚举
 */
enum class ErrorCode_CancelTask {
    SUCCESS = 0,
    FAILURE = 1,
};

/**
 * @brief 1007 查询任务状态响应错误码枚举
 */
enum class ErrorCode_QueryStatus {
    COMPLETED = 0,
    EXECUTING = 1,
    FAILED = -1
};

/**
 * @brief 消息接口基类
 */
class IMessage {
public:
    virtual ~IMessage() = default;

    /**
     * @brief 获取消息类型
     * @return 消息类型
     */
    virtual MessageType getType() const = 0;

    /**
     * @brief 序列化消息为字符串
     * @return 序列化后的字符串
     */
    virtual std::string serialize() const = 0;

    /**
     * @brief 从字符串反序列化消息
     * @param data 序列化的字符串
     * @return 是否成功
     */
    virtual bool deserialize(const std::string& data) = 0;

    /**
     * @brief 获取消息序列号
     * @return 消息序列号
     */
    virtual uint16_t getSequenceNumber() const = 0;

    /**
     * @brief 设置消息序列号
     * @param sequenceNumber 消息序列号
     */
    virtual void setSequenceNumber(uint16_t sequenceNumber) = 0;
};

/**
 * @brief 创建消息对象
 * @param type 消息类型
 * @return 消息对象指针
 */
std::unique_ptr<IMessage> createMessage(MessageType type);

} // namespace protocol
