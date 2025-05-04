// server/main.cpp
#include "NamedPipeServer.h"
#include <iostream>
#include <string>
#include <csignal>

// Global server instance
std::unique_ptr<IPC::NamedPipeServer> g_server;

// Signal handler để dừng server khi nhận SIGINT (Ctrl+C)
void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nReceived SIGINT, stopping server..." << std::endl;
        if (g_server) {
            g_server->stop();
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== Calculator Server ===" << std::endl;

    // Đăng ký signal handler
    std::signal(SIGINT, signalHandler);

    // Tạo và khởi động server
    g_server = std::make_unique<IPC::NamedPipeServer>();

    if (!g_server->start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;

    // Vòng lặp chính, giữ cho chương trình chạy
    while (g_server->isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
