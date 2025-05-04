// client/NamedPipeClient.cpp
#include "NamedPipeClient.h"
#include <iostream>

namespace IPC {

NamedPipeClient::NamedPipeClient(const std::string& pipeName, QObject* parent)
    : QObject(parent), m_pipeName(pipeName)
#ifdef _WIN32
    , m_pipeHandle(INVALID_HANDLE_VALUE)
#else
    , m_pipeHandle(-1)
#endif
{
}

NamedPipeClient::~NamedPipeClient() {
    closeConnection();
}

QString NamedPipeClient::calculateExpression(const QString& expression) {
    // Kết nối đến pipe
    if (!connectToPipe()) {
        emit errorOccurred("Failed to connect to server");
        return "Error: Failed to connect to server";
    }
    
    // Tạo message chứa biểu thức
    Message message(MessageType::EXPRESSION, expression.toStdString());
    
    // Serialize message
    std::vector<char> serializedMessage = MessageUtils::serializeMessage(message);
    
#ifdef _WIN32
    // Windows implementation
    DWORD bytesWritten;
    BOOL result = WriteFile(
        m_pipeHandle,                      // handle to pipe
        serializedMessage.data(),          // buffer to write from
        serializedMessage.size(),          // number of bytes to write
        &bytesWritten,                     // number of bytes written
        NULL);                             // not overlapped I/O
    
    if (!result || bytesWritten != serializedMessage.size()) {
        emit errorOccurred("Failed to send expression to server");
        closeConnection();
        return "Error: Failed to send expression to server";
    }
    
    // Đọc kết quả từ server
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;
    result = ReadFile(
        m_pipeHandle,                      // handle to pipe
        buffer,                            // buffer to receive data
        BUFFER_SIZE,                       // size of buffer
        &bytesRead,                        // number of bytes read
        NULL);                             // not overlapped I/O
    
    if (!result || bytesRead == 0) {
        emit errorOccurred("Failed to read response from server");
        closeConnection();
        return "Error: Failed to read response from server";
    }
#else
    // Linux/Unix implementation
    // Gửi biểu thức đến server
    ssize_t bytesWritten = write(m_pipeHandle, serializedMessage.data(), serializedMessage.size());
    if (bytesWritten != static_cast<ssize_t>(serializedMessage.size())) {
        emit errorOccurred("Failed to send expression to server");
        closeConnection();
        return "Error: Failed to send expression to server";
    }
    
    // Đóng pipe ghi
    close(m_pipeHandle);
    
    // Mở pipe để đọc
    m_pipeHandle = open(m_pipeName.c_str(), O_RDONLY);
    if (m_pipeHandle < 0) {
        emit errorOccurred("Failed to open pipe for reading");
        return "Error: Failed to open pipe for reading";
    }
    
    // Đọc kết quả từ server
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = read(m_pipeHandle, buffer, BUFFER_SIZE);
    if (bytesRead <= 0) {
        emit errorOccurred("Failed to read response from server");
        closeConnection();
        return "Error: Failed to read response from server";
    }
#endif
    
    // Deserialize kết quả
    std::vector<char> messageData(buffer, buffer + bytesRead);
    Message response = MessageUtils::deserializeMessage(messageData);
    
    // Đóng kết nối
    closeConnection();
    
    // Kiểm tra loại message
    if (response.type == MessageType::RESULT) {
        QString result = QString::fromStdString(response.data);
        emit resultReady(result);
        return result;
    } else if (response.type == MessageType::ERROR_MSG) {
        QString errorMsg = QString::fromStdString(response.data);
        emit errorOccurred(errorMsg);
        return "Error: " + errorMsg;
    } else {
        emit errorOccurred("Invalid response type from server");
        return "Error: Invalid response type from server";
    }
}

bool NamedPipeClient::connectToPipe() {
#ifdef _WIN32
    // Windows implementation
    // WaitNamedPipe timeout 5 giây
    if (!WaitNamedPipeA(m_pipeName.c_str(), 5000)) {
        std::cerr << "Could not open pipe: " << GetLastError() << std::endl;
        return false;
    }
    
    // Mở pipe
    m_pipeHandle = CreateFileA(
        m_pipeName.c_str(),                // tên pipe
        GENERIC_READ | GENERIC_WRITE,      // đọc và ghi
        0,                                  // không chia sẻ
        NULL,                               // default security attributes
        OPEN_EXISTING,                      // opens existing pipe
        0,                                  // default attributes
        NULL);                              // no template file
    
    if (m_pipeHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not open pipe: " << GetLastError() << std::endl;
        return false;
    }
    
    // Thiết lập chế độ đọc message
    DWORD dwMode = PIPE_READMODE_MESSAGE;
    BOOL success = SetNamedPipeHandleState(
        m_pipeHandle,                     // pipe handle
        &dwMode,                          // new pipe mode
        NULL,                             // don't set maximum bytes
        NULL);                            // don't set maximum time
    
    if (!success) {
        std::cerr << "SetNamedPipeHandleState failed: " << GetLastError() << std::endl;
        closeConnection();
        return false;
    }
#else
    // Linux/Unix implementation
    // Mở pipe để ghi
    m_pipeHandle = open(m_pipeName.c_str(), O_WRONLY);
    if (m_pipeHandle < 0) {
        std::cerr << "Failed to open pipe for writing" << std::endl;
        return false;
    }
#endif
    
    return true;
}

void NamedPipeClient::closeConnection() {
#ifdef _WIN32
    if (m_pipeHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_pipeHandle);
        m_pipeHandle = INVALID_HANDLE_VALUE;
    }
#else
    if (m_pipeHandle >= 0) {
        close(m_pipeHandle);
        m_pipeHandle = -1;
    }
#endif
}

} // namespace IPC