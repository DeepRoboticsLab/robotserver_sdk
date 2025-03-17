#include "asio_network_model.hpp"
#include "protocol/serializer.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace network {

AsioNetworkModel::AsioNetworkModel(INetworkCallback& callback)
    : socket_(io_context_),
      strand_(io_context_),
      connected_(false),
      callback_(callback) {
}

AsioNetworkModel::~AsioNetworkModel() {
    disconnect();
}

void AsioNetworkModel::setConnectionTimeout(std::chrono::milliseconds timeout) {
    connection_timeout_ = timeout;
}

bool AsioNetworkModel::connect(const std::string& host, uint16_t port) {
    // 如果已经连接，直接返回成功
    if (connected_) {
        return true;
    }

    try {
        // 重置io_context，确保它处于干净状态
        io_context_.restart();

        // 解析主机地址
        boost::asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        // 设置连接超时
        socket_.close(); // 确保套接字是关闭的
        socket_ = boost::asio::ip::tcp::socket(io_context_); // 重新创建套接字

        // 使用async_connect启动异步连接
        boost::system::error_code connect_ec;
        bool connect_completed = false;

        // 开始异步连接
        boost::asio::async_connect(socket_, endpoints,
            [&connect_ec, &connect_completed](const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint&) {
                connect_ec = ec;
                connect_completed = true;
            });

        // 创建一个定时器用于超时控制
        boost::asio::steady_timer timeout_timer(io_context_);
        timeout_timer.expires_after(connection_timeout_);
        timeout_timer.async_wait([this, &connect_completed](const boost::system::error_code& ec) {
            if (!ec && !connect_completed) {
                // 超时，取消连接操作
                socket_.cancel();
            }
        });

        // 运行io_context直到连接完成或超时
        while (!connect_completed && io_context_.run_one()) {
            // 继续运行直到有事件发生
        }

        // 取消定时器
        timeout_timer.cancel();

        // 检查连接是否成功
        if (connect_ec || !connect_completed) {
            std::cerr << "连接失败: " << connect_ec.message() << std::endl;
            socket_.close();
            return false;
        }

        // 连接成功
        connected_ = true;

        // 确保之前的IO线程已经结束
        if (io_thread_.joinable()) {
            io_thread_.join();
        }

        // 启动IO线程
        io_thread_ = std::thread(&AsioNetworkModel::ioThreadFunc, this);

        // 启动接收
        startReceive();

        return true;
    } catch (const std::exception& e) {
        std::cerr << "连接异常: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "连接异常: 未知异常" << std::endl;
        return false;
    }
}

void AsioNetworkModel::disconnect() {
    if (!connected_ && !socket_.is_open()) {
        return;
    }

    try {
        // 取消所有异步操作
        boost::system::error_code ec;
        socket_.cancel(ec);

        // 使用 strand 确保安全关闭
        boost::asio::post(strand_, [this]() {
            // 关闭socket
            boost::system::error_code error;
            socket_.close(error);

            if (error) {
                std::cerr << "关闭socket错误: " << error.message() << std::endl;
            }
        });

        // 停止IO上下文
        io_context_.stop();

        // 等待IO线程结束
        if (io_thread_.joinable()) {
            io_thread_.join();
        }

        connected_ = false;
    } catch (const std::exception& e) {
        std::cerr << "断开连接异常: " << e.what() << std::endl;
    }
}

bool AsioNetworkModel::isConnected() const {
    return connected_ && socket_.is_open();
}

bool AsioNetworkModel::sendMessage(const protocol::IMessage& message) {
    if (!isConnected()) {
        return false;
    }

    try {
        // 序列化消息
        protocol::Serializer serializer;
        std::string data = serializer.serializeMessage(message);

        // 使用 strand 包装异步写入操作，确保线程安全
        boost::asio::post(strand_, [this, data = std::move(data)]() {
            if (!isConnected()) {
                return;
            }

            boost::asio::async_write(
                socket_,
                boost::asio::buffer(data),
                boost::asio::bind_executor(strand_,
                    [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
                        onSend(error, bytes_transferred);
                    }
                )
            );
        });

        return true;
    } catch (const std::exception& e) {
        std::cerr << "发送消息异常: " << e.what() << std::endl;
        return false;
    }
}

void AsioNetworkModel::startReceive() {
    if (!isConnected()) {
        return;
    }

    // 使用 strand 包装异步读取操作，确保线程安全
    socket_.async_read_some(
        boost::asio::buffer(receive_buffer_),
        boost::asio::bind_executor(strand_,
            [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
                onReceive(error, bytes_transferred);
            }
        )
    );
}

/**
 * @brief 安全回调包装函数，用于捕获和处理回调函数中可能抛出的异常
 * @tparam Callback 回调函数类型
 * @tparam Args 回调函数参数类型
 * @param callback 回调函数
 * @param callbackType 回调函数类型描述，用于日志记录
 * @param args 回调函数参数
 */
template<typename Callback, typename... Args>
void safeCallback(const Callback& callback, const std::string& callbackType, Args&&... args) {
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

void AsioNetworkModel::onReceive(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (error) {
        if (error != boost::asio::error::operation_aborted) {
            std::cerr << "接收数据错误: " << error.message() << std::endl;
            disconnect();
        }
        return;
    }

    // 将接收到的数据追加到缓冲区
    receive_data_.append(receive_buffer_.data(), bytes_transferred);

    // 尝试解析消息
    protocol::Serializer serializer;
    auto message = serializer.deserializeMessage(receive_data_);
    if (message) {
        // 清空接收缓冲区
        receive_data_.clear();

        // 使用 strand 确保回调在同一线程上下文中执行
        boost::asio::post(strand_, [this, msg = std::move(message)]() mutable {
            safeCallback(
                [this](std::unique_ptr<protocol::IMessage>& msg) {
                    callback_.onMessageReceived(std::move(msg));
                },
                "网络消息接收",
                msg
            );
        });
    }

    // 继续接收
    startReceive();
}

void AsioNetworkModel::onSend(const boost::system::error_code& error, std::size_t) {
    if (error) {
        std::cerr << "发送数据错误: " << error.message() << std::endl;
        if (error != boost::asio::error::operation_aborted) {
            disconnect();
        }
    }
}

void AsioNetworkModel::ioThreadFunc() {
    try {
        // 使用 work guard 防止 io_context 在没有任务时退出
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard(io_context_.get_executor());

        // 运行 io_context，直到显式调用 stop()
        io_context_.run();
    } catch (const std::exception& e) {
        std::cerr << "IO线程异常: " << e.what() << std::endl;

        // 如果发生异常，尝试断开连接
        if (connected_) {
            try {
                disconnect();
            } catch (...) {
                // 忽略断开连接时的异常
            }
        }
    }
}

} // namespace network
