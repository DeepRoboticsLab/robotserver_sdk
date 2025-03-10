#include <cstdint>
#include <robotserver_sdk.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <filesystem>
#include <fstream>

std::vector<robotserver_sdk::NavigationPoint> loadDefaultNavigationPoints(const std::string& configPath) {
    std::vector<robotserver_sdk::NavigationPoint> points;
    try {
        // 检查文件是否存在
        if (!std::filesystem::exists(configPath)) {
            std::cerr << "配置文件不存在: " << configPath << std::endl;
            return points;
        }

        // 读取JSON文件
        std::ifstream file(configPath);
        nlohmann::json jsonArray;
        file >> jsonArray;

        // 解析每个导航点
        for (const auto& jsonPoint : jsonArray) {
            points.push_back(robotserver_sdk::NavigationPoint::fromJson(jsonPoint));
        }

        std::cout << "成功从配置文件加载了 " << points.size() << " 个导航点" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "加载配置文件失败: " << e.what() << std::endl;
    }
    return points;
}

// 辅助函数：加载默认导航点
std::vector<robotserver_sdk::NavigationPoint> loadNavigationPoints() {
    // 尝试多个可能的路径
    std::vector<std::filesystem::path> possiblePaths;

    try {
        // 1. 尝试相对于可执行文件的路径
        std::filesystem::path exePath = std::filesystem::canonical("/proc/self/exe");
        std::filesystem::path exeDir = exePath.parent_path();

        possiblePaths.push_back(exeDir / "default_navigation_points.json");                  // 与可执行文件同目录
        possiblePaths.push_back(exeDir / "basic" / "default_navigation_points.json");        // 可执行文件的 basic 子目录
        possiblePaths.push_back(exeDir.parent_path() / "examples" / "basic" / "default_navigation_points.json"); // bin/../examples/basic/

        // 2. 尝试相对于当前工作目录的路径
        std::filesystem::path currentDir = std::filesystem::current_path();
        possiblePaths.push_back(currentDir / "default_navigation_points.json");
        possiblePaths.push_back(currentDir / "examples" / "basic" / "default_navigation_points.json");

        // 3. 尝试源代码目录的路径（假设在构建目录中运行）
        possiblePaths.push_back(currentDir.parent_path() / "examples" / "basic" / "default_navigation_points.json");
    }
    catch (const std::exception& e) {
        std::cerr << "获取可执行文件路径时出错: " << e.what() << std::endl;
    }

    // 尝试每个可能的路径
    for (const auto& path : possiblePaths) {
        std::cout << "尝试加载配置文件: " << path.string() << std::endl;
        if (std::filesystem::exists(path)) {
            std::vector<robotserver_sdk::NavigationPoint> points = loadDefaultNavigationPoints(path.string());
            if (!points.empty()) {
                return points;
            }
        }
    }

    // 如果所有路径都失败，尝试使用硬编码的路径
    std::cerr << "无法找到配置文件，尝试使用硬编码路径" << std::endl;
    return loadDefaultNavigationPoints("./default_navigation_points.json");
}

std::vector<robotserver_sdk::NavigationPoint> g_points = loadNavigationPoints();

/**
 * @brief 打印实时状态信息
 * @param status 实时状态信息
 */
void printStatus(const robotserver_sdk::RealTimeStatus& status) {
    std::cout << "===== 实时状态信息 =====" << std::endl;
    std::cout << "位置: (" << status.posX << ", " << status.posY << ", " << status.posZ << ")" << std::endl;
    std::cout << "角度: " << status.angleYaw << "°" << std::endl;
    std::cout << "速度: " << status.speed << std::endl;
    std::cout << "电量: " << status.electricity << "%" << std::endl;
    std::cout << "运动状态: " << status.motionState << std::endl;
    std::cout << "========================" << std::endl;
}

/**
 * @brief 打印任务状态信息
 * @param status 任务状态查询结果
 */
void printTaskStatus(const robotserver_sdk::TaskStatusResult& status) {
    std::cout << "===== 任务状态信息 =====" << std::endl;
    std::cout << "目标点编号: " << status.value << std::endl;
    std::cout << "状态: ";

    switch (status.status) {
        case robotserver_sdk::Status_QueryStatus::COMPLETED:
            std::cout << "已完成" << std::endl;
            break;
        case robotserver_sdk::Status_QueryStatus::EXECUTING:
            std::cout << "执行中" << std::endl;
            break;
        case robotserver_sdk::Status_QueryStatus::FAILED:
            std::cout << "失败" << std::endl;
            break;
    }

    std::cout << "错误码: " << static_cast<int>(status.errorCode) << std::endl;
    // std::cout << "时间戳: " << status.timestamp << std::endl;
    std::cout << "========================" << std::endl;
}

/**
 * @brief 打印导航结果
 * @param result 导航任务结果
 */
void printNavigationResult(const robotserver_sdk::NavigationResult& result) {
    std::cout << "===== 导航任务结果 =====" << std::endl;
    std::cout << "目标点编号: " << result.value << std::endl;
    std::cout << "错误码: " << static_cast<int>(result.errorCode) << std::endl;
    std::cout << "错误状态: " << static_cast<int>(result.errorStatus) << std::endl;
    // std::cout << "时间戳: " << result.timestamp << std::endl;
    std::cout << "========================" << std::endl;
}

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc < 3) {
        std::cerr << "用法: " << argv[0] << " <主机地址> <端口>" << std::endl;
        return 1;
    }

    std::string host = argv[1];
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));

    std::cout << "机器狗 RobotServer SDK 示例程序" << std::endl;
    std::cout << "SDK 版本: " << robotserver_sdk::RobotServerSdk::getVersion() << std::endl;
    std::cout << "连接到: " << host << ":" << port << std::endl;

    try {
        // 创建 SDK 实例
        robotserver_sdk::SdkOptions options;
        options.connectionTimeout = std::chrono::milliseconds(5000);
        options.requestTimeout = std::chrono::milliseconds(3000);

        robotserver_sdk::RobotServerSdk sdk(options);

        // 连接到机器狗控制系统
        if (!sdk.connect(host, port)) {
            std::cerr << "连接失败!" << std::endl;
            return 1;
        }

        std::cout << "连接成功!" << std::endl;

        // 获取初始实时状态
        robotserver_sdk::RealTimeStatus status = sdk.request1002_RunTimeState();
        std::cout << "request1002_RunTimeState complete" << std::endl;
        printStatus(status);

        // 创建导航点
        std::vector<robotserver_sdk::NavigationPoint> points = g_points;

        // 标记是否收到导航响应
        std::atomic<bool> navigationResponseReceived{false};

        // 异步发送导航任务（使用回调形式）
        std::cout << "开始导航任务..." << std::endl;
        sdk.request1003_StartNavTask(points, [&](const robotserver_sdk::NavigationResult& result) {
            // 打印导航任务结果
            printNavigationResult(result);

            // 标记已收到导航响应
            navigationResponseReceived = true;
        });

        // 轮询查询任务状态和实时状态，直到收到导航响应
        int pollCount = 0;
        const int MAX_POLL_COUNT = 120; // 最多轮询120次
        const auto POLL_INTERVAL = std::chrono::milliseconds(1000); // 轮询间隔1秒

        while (!navigationResponseReceived && pollCount < MAX_POLL_COUNT) {
            std::this_thread::sleep_for(POLL_INTERVAL);
            pollCount++;

            std::cout << "\n轮询 #" << pollCount << ":" << std::endl;

            // 查询任务状态
            auto taskStatus = sdk.request1007_NavTaskState();
            printTaskStatus(taskStatus);

            // 获取实时状态
            auto status = sdk.request1002_RunTimeState();
            printStatus(status);
        }

        if (!navigationResponseReceived) {
            std::cout << "达到最大轮询次数，尝试取消任务..." << std::endl;
            if (sdk.request1004_CancelNavTask()) {
                std::cout << "导航任务已取消" << std::endl;
            } else {
                std::cout << "导航任务取消失败" << std::endl;
            }
        }

        // 等待2秒
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // 断开连接
        sdk.disconnect();
        std::cout << "已断开连接" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "发生异常: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
