// server/NamedPipeServer.h
#ifndef NAMED_PIPE_SERVER_H
#define NAMED_PIPE_SERVER_H

#include "../shared/Message.h"
#include "../shared/MessageUtils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>

namespace IPC {

class NamedPipeServer {
public:
    // Constructor
    NamedPipeServer(const std::string& pipeName = PIPE_NAME);

    // Destructor
    ~NamedPipeServer();

    // Ngăn sao chép
    NamedPipeServer(const NamedPipeServer&) = delete;
    NamedPipeServer& operator=(const NamedPipeServer&) = delete;

    // Bắt đầu lắng nghe từ client
    bool start();

    // Dừng server
    void stop();

    // Kiểm tra xem server có đang chạy không
    bool isRunning() const;

private:
    // Xử lý thông điệp từ client
    Message processMessage(const Message& message);

    // Tính toán biểu thức
    std::pair<double, ErrorCode> evaluateExpression(const std::string& expr);

    // Vòng lặp chính của server
    void serverLoop();

    // Tên pipe
    std::string m_pipeName;

    // Handle của pipe (Windows)
#ifdef _WIN32
    HANDLE m_pipeHandle;
#else
    int m_pipeHandle;
#endif

    // Thread cho vòng lặp server
    std::unique_ptr<std::thread> m_serverThread;

    // Cờ báo server đang chạy
    std::atomic<bool> m_isRunning;
};

} // namespace IPC

#endif // NAMED_PIPE_SERVER_H
