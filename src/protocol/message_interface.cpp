#include "message_interface.hpp"
#include "messages.hpp"

namespace protocol {

std::unique_ptr<IMessage> createMessage(MessageType type) {
    switch (type) {
        case MessageType::GET_REAL_TIME_STATUS_REQ:
            return std::make_unique<GetRealTimeStatusRequest>();
        case MessageType::GET_REAL_TIME_STATUS_RESP:
            return std::make_unique<GetRealTimeStatusResponse>();
        case MessageType::NAVIGATION_TASK_REQ:
            return std::make_unique<NavigationTaskRequest>();
        case MessageType::NAVIGATION_TASK_RESP:
            return std::make_unique<NavigationTaskResponse>();
        case MessageType::CANCEL_TASK_REQ:
            return std::make_unique<CancelTaskRequest>();
        case MessageType::CANCEL_TASK_RESP:
            return std::make_unique<CancelTaskResponse>();
        case MessageType::QUERY_STATUS_REQ:
            return std::make_unique<QueryStatusRequest>();
        case MessageType::QUERY_STATUS_RESP:
            return std::make_unique<QueryStatusResponse>();
        default:
            return nullptr;
    }
}

} // namespace protocol
