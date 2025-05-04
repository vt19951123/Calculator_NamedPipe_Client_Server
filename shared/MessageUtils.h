// shared/MessageUtils.h
#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H

#include "Message.h"
#include <sstream>
#include <vector>

namespace IPC {

class MessageUtils {
public:
    // Serialize message thành một chuỗi byte để truyền qua pipe
    static std::vector<char> serializeMessage(const Message& message) {
        // Format: [MessageType]|[ErrorCode]|[DataLength]|[Data]
        std::stringstream ss;
        ss << static_cast<int>(message.type) << "|";
        ss << static_cast<int>(message.errorCode) << "|";
        ss << message.data.length() << "|";
        ss << message.data;
        
        std::string serialized = ss.str();
        return std::vector<char>(serialized.begin(), serialized.end());
    }
    
    // Deserialize chuỗi byte thành message
    static Message deserializeMessage(const std::vector<char>& data) {
        std::string strData(data.begin(), data.end());
        std::stringstream ss(strData);
        
        std::string token;
        
        // Đọc message type
        std::getline(ss, token, '|');
        MessageType type = static_cast<MessageType>(std::stoi(token));
        
        // Đọc error code
        std::getline(ss, token, '|');
        ErrorCode errorCode = static_cast<ErrorCode>(std::stoi(token));
        
        // Đọc data length
        std::getline(ss, token, '|');
        int dataLength = std::stoi(token);
        
        // Đọc data
        std::string messageData;
        std::getline(ss, messageData);
        
        return Message(type, messageData, errorCode);
    }
};

} // namespace IPC

#endif // MESSAGE_UTILS_H