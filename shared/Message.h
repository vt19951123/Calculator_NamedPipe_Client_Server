// shared/Message.h
#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>

namespace IPC {

// Định nghĩa các loại thông điệp
enum class MessageType {
    EXPRESSION,  // Biểu thức tính toán
    RESULT,      // Kết quả tính toán
    ERROR        // Thông báo lỗi
};

// Định nghĩa các mã lỗi
enum class ErrorCode {
    NONE,
    INVALID_EXPRESSION,
    DIVISION_BY_ZERO,
    PIPE_ERROR,
    UNKNOWN_ERROR
};

// Cấu trúc thông điệp
struct Message {
    MessageType type;
    std::string data;
    ErrorCode errorCode = ErrorCode::NONE;
    
    // Constructor
    Message() : type(MessageType::EXPRESSION), data(""), errorCode(ErrorCode::NONE) {}
    
    Message(MessageType t, const std::string& d) 
        : type(t), data(d), errorCode(ErrorCode::NONE) {}
        
    Message(MessageType t, const std::string& d, ErrorCode e) 
        : type(t), data(d), errorCode(e) {}
};

// Các hằng số
const std::string PIPE_NAME = "\\\\.\\pipe\\CalculatorPipe";
const int BUFFER_SIZE = 1024;
const int PIPE_TIMEOUT = 5000; // Timeout đọc/ghi pipe (ms)

} // namespace IPC

#endif // MESSAGE_H