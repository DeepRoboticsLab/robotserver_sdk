#pragma once

#include "message_interface.hpp"
#include <memory>
#include <string>
#include <map>

namespace protocol {

/**
 * @brief 协议序列化类
 */
class Serializer {
public:
    /**
     * @brief 构造函数
     */
    Serializer() = default;

    /**
     * @brief 析构函数
     */
    ~Serializer() = default;

    /**
     * @brief 解析接收到的数据
     * @param data 接收到的数据
     * @return 解析出的消息对象
     */
    std::unique_ptr<IMessage> deserializeMessage(const std::string& data);

    /**
     * @brief 序列化消息为发送数据
     * @param message 要发送的消息
     * @return 序列化后的数据
     */
    std::string serializeMessage(const IMessage& message);

private:
    /**
     * @brief 从数据中提取消息类型
     * @param data 接收到的数据
     * @return 消息类型
     */
    MessageType extractMessageType(const std::string& data);

    /**
     * @brief 从XML数据中提取Type字段的值
     * @param data XML数据
     * @return Type字段的值，如果提取失败则返回0
     */
    int extractTypeFromXml(const std::string& data);

    /**
     * @brief 从XML数据中提取Command字段的值
     * @param data XML数据
     * @return Command字段的值，如果提取失败则返回0
     */
    int extractCommandFromXml(const std::string& data);

    /**
     * @brief 根据Type值确定消息类型
     * @param type Type字段的值
     * @return 消息类型
     */
    MessageType determineMessageType(int type);

    // Type值到消息类型的映射
    const std::map<int, MessageType> type_to_message_type_ = {
        {1002, MessageType::GET_REAL_TIME_STATUS_RESP},
        {1003, MessageType::NAVIGATION_TASK_RESP},
        {1004, MessageType::CANCEL_TASK_RESP},
        {1007, MessageType::QUERY_STATUS_RESP}
    };
};

} // namespace protocol
