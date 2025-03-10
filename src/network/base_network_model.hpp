#pragma once

#include <string>
#include "protocol/message_interface.hpp"

namespace network {

/**
 * @brief 基础网络模型接口
 */
class BaseNetworkModel {
public:
    virtual ~BaseNetworkModel() = default;

    /**
     * @brief 连接到服务器
     * @param host 主机地址
     * @param port 端口号
     * @return 是否连接成功
     */
    virtual bool connect(const std::string& host, uint16_t port) = 0;

    /**
     * @brief 断开连接
     */
    virtual void disconnect() = 0;

    /**
     * @brief 检查是否已连接
     * @return 是否已连接
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief 发送消息
     * @param message 要发送的消息
     * @return 是否发送成功
     */
    virtual bool sendMessage(const protocol::IMessage& message) = 0;
};

} // namespace network
