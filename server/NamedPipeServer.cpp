// server/NamedPipeServer.cpp
#include "NamedPipeServer.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <cmath>
#include <algorithm>

namespace IPC {

NamedPipeServer::NamedPipeServer(const std::string& pipeName)
    : m_pipeName(pipeName), m_isRunning(false)
#ifdef _WIN32
    , m_pipeHandle(INVALID_HANDLE_VALUE)
#else
    , m_pipeHandle(-1)
#endif
{
}

NamedPipeServer::~NamedPipeServer() {
    stop();
}

bool NamedPipeServer::start() {
    if (m_isRunning) {
        std::cout << "Server is already running" << std::endl;
        return true;
    }

    m_isRunning = true;
    m_serverThread = std::make_unique<std::thread>(&NamedPipeServer::serverLoop, this);

    std::cout << "Server started on pipe: " << m_pipeName << std::endl;
    return true;
}

void NamedPipeServer::stop() {
    if (!m_isRunning) {
        return;
    }

    m_isRunning = false;

    if (m_serverThread && m_serverThread->joinable()) {
        m_serverThread->join();
    }

#ifdef _WIN32
    if (m_pipeHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_pipeHandle);
        m_pipeHandle = INVALID_HANDLE_VALUE;
    }
#else
    if (m_pipeHandle != -1) {
        close(m_pipeHandle);
        m_pipeHandle = -1;
    }
#endif

    std::cout << "Server stopped" << std::endl;
}

bool NamedPipeServer::isRunning() const {
    return m_isRunning;
}

Message NamedPipeServer::processMessage(const Message& message) {
    if (message.type != MessageType::EXPRESSION) {
        return Message(MessageType::ERROR_MSG, "Invalid message type", ErrorCode::INVALID_EXPRESSION);
    }

    // Xử lý biểu thức
    auto [result, errorCode] = evaluateExpression(message.data);

    if (errorCode != ErrorCode::NONE) {
        return Message(MessageType::ERROR_MSG, "Error evaluating expression", errorCode);
    }

    // Chuyển kết quả thành chuỗi
    std::stringstream ss;
    ss << result;

    return Message(MessageType::RESULT, ss.str());
}

// Hàm tính toán biểu thức đơn giản (hỗ trợ +, -, *, /)
std::pair<double, ErrorCode> NamedPipeServer::evaluateExpression(const std::string& expr) {
    // Ví dụ đơn giản: chỉ hỗ trợ phép tính cơ bản và không có dấu ngoặc
    try {
        std::istringstream tokens(expr);
        std::stack<double> values;
        std::stack<char> ops;

        // Xử lý biểu thức
        char prev = ' ';
        for (std::string token; tokens >> token;) {
            // Nếu là số
            if (isdigit(token[0]) || (token[0] == '-' && (prev == ' ' || prev == '('))) {
                double value = std::stod(token);
                values.push(value);
            }
            // Nếu là toán tử
            else if (token.length() == 1 && (token[0] == '+' || token[0] == '-' ||
                     token[0] == '*' || token[0] == '/')) {
                ops.push(token[0]);
            }
            else {
                return std::make_pair(0.0, ErrorCode::INVALID_EXPRESSION);
            }
            prev = token[token.length() - 1];
        }

        // Thực hiện các phép tính
        while (!ops.empty()) {
            double val2 = values.top(); values.pop();
            double val1 = values.top(); values.pop();
            char op = ops.top(); ops.pop();

            double result = 0.0;
            switch(op) {
                case '+': result = val1 + val2; break;
                case '-': result = val1 - val2; break;
                case '*': result = val1 * val2; break;
                case '/':
                    if (val2 == 0) {
                        return std::make_pair(0.0, ErrorCode::DIVISION_BY_ZERO);
                    }
                    result = val1 / val2;
                    break;
                default:
                    return std::make_pair(0.0, ErrorCode::INVALID_EXPRESSION);
            }

            values.push(result);
        }

        if (values.size() != 1) {
            return std::make_pair(0.0, ErrorCode::INVALID_EXPRESSION);
        }

        return std::make_pair(values.top(), ErrorCode::NONE);
    }
    catch (const std::exception& e) {
        return std::make_pair(0.0, ErrorCode::INVALID_EXPRESSION);
    }
}

void NamedPipeServer::serverLoop() {
    while (m_isRunning) {
#ifdef _WIN32
        // Windows implementation
        m_pipeHandle = CreateNamedPipeA(
            m_pipeName.c_str(),               // tên pipe
            PIPE_ACCESS_DUPLEX,                // đọc/ghi qua pipe
            PIPE_TYPE_MESSAGE |                // message-type pipe
            PIPE_READMODE_MESSAGE |            // message-read mode
            PIPE_WAIT,                         // blocking mode
            PIPE_UNLIMITED_INSTANCES,          // max. instances
            BUFFER_SIZE,                       // output buffer size
            BUFFER_SIZE,                       // input buffer size
            PIPE_TIMEOUT,                      // client time-out
            NULL);                             // default security attribute

        if (m_pipeHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "CreateNamedPipe failed with error " << GetLastError() << std::endl;
            continue;
        }

        std::cout << "Waiting for client connection..." << std::endl;

        // Chờ client kết nối
        BOOL result = ConnectNamedPipe(m_pipeHandle, NULL);
        if (!result && GetLastError() != ERROR_PIPE_CONNECTED) {
            std::cerr << "ConnectNamedPipe failed with error " << GetLastError() << std::endl;
            CloseHandle(m_pipeHandle);
            continue;
        }

        std::cout << "Client connected" << std::endl;

        // Nhận thông điệp từ client
        char buffer[BUFFER_SIZE];
        DWORD bytesRead;
        result = ReadFile(
            m_pipeHandle,     // handle to pipe
            buffer,           // buffer to receive data
            BUFFER_SIZE,      // size of buffer
            &bytesRead,       // number of bytes read
            NULL);            // not overlapped I/O

        if (!result || bytesRead == 0) {
            std::cerr << "ReadFile failed with error " << GetLastError() << std::endl;
            CloseHandle(m_pipeHandle);
            continue;
        }

        // Chuyển buffer thành Message
        std::vector<char> messageData(buffer, buffer + bytesRead);
        Message receivedMessage = MessageUtils::deserializeMessage(messageData);

        std::cout << "Received expression: " << receivedMessage.data << std::endl;

        // Xử lý thông điệp
        Message responseMessage = processMessage(receivedMessage);

        std::cout << "Sending result: " << responseMessage.data << std::endl;

        // Gửi kết quả về client
        std::vector<char> serializedResponse = MessageUtils::serializeMessage(responseMessage);
        DWORD bytesWritten;
        result = WriteFile(
            m_pipeHandle,                     // handle to pipe
            serializedResponse.data(),        // buffer to write from
            serializedResponse.size(),        // number of bytes to write
            &bytesWritten,                    // number of bytes written
            NULL);                            // not overlapped I/O

        if (!result || bytesWritten != serializedResponse.size()) {
            std::cerr << "WriteFile failed with error " << GetLastError() << std::endl;
        }

        // Đóng kết nối với client
        FlushFileBuffers(m_pipeHandle);
        DisconnectNamedPipe(m_pipeHandle);
        CloseHandle(m_pipeHandle);
#else
        // Linux/Unix implementation
        // Xóa fifo cũ nếu có
        unlink(m_pipeName.c_str());

        // Tạo named pipe (FIFO)
        if (mkfifo(m_pipeName.c_str(), 0666) < 0) {
            std::cerr << "Error creating named pipe" << std::endl;
            continue;
        }

        std::cout << "Waiting for client connection..." << std::endl;

        // Mở pipe để đọc
        m_pipeHandle = open(m_pipeName.c_str(), O_RDONLY);
        if (m_pipeHandle < 0) {
            std::cerr << "Error opening pipe for reading" << std::endl;
            continue;
        }

        std::cout << "Client connected" << std::endl;

        // Đọc từ pipe
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead = read(m_pipeHandle, buffer, BUFFER_SIZE);

        if (bytesRead <= 0) {
            std::cerr << "Error reading from pipe" << std::endl;
            close(m_pipeHandle);
            continue;
        }

        // Chuyển buffer thành Message
        std::vector<char> messageData(buffer, buffer + bytesRead);
        Message receivedMessage = MessageUtils::deserializeMessage(messageData);

        std::cout << "Received expression: " << receivedMessage.data << std::endl;

        // Xử lý thông điệp
        Message responseMessage = processMessage(receivedMessage);

        std::cout << "Sending result: " << responseMessage.data << std::endl;

        // Đóng pipe đọc
        close(m_pipeHandle);

        // Mở pipe để ghi
        m_pipeHandle = open(m_pipeName.c_str(), O_WRONLY);
        if (m_pipeHandle < 0) {
            std::cerr << "Error opening pipe for writing" << std::endl;
            continue;
        }

        // Gửi kết quả về client
        std::vector<char> serializedResponse = MessageUtils::serializeMessage(responseMessage);
        write(m_pipeHandle, serializedResponse.data(), serializedResponse.size());

        // Đóng pipe ghi
        close(m_pipeHandle);
        m_pipeHandle = -1;
#endif
    }
}

} // namespace IPC
