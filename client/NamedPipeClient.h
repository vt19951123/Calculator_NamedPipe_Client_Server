// client/NamedPipeClient.h
#ifndef NAMED_PIPE_CLIENT_H
#define NAMED_PIPE_CLIENT_H

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
#include <vector>
#include <QObject>
#include <QString>

namespace IPC {

class NamedPipeClient : public QObject {
    Q_OBJECT

public:
    // Constructor
    explicit NamedPipeClient(const std::string& pipeName = PIPE_NAME, QObject* parent = nullptr);
    
    // Destructor
    ~NamedPipeClient();

    // Ngăn sao chép
    NamedPipeClient(const NamedPipeClient&) = delete;
    NamedPipeClient& operator=(const NamedPipeClient&) = delete;

public slots:
    // Gửi biểu thức để tính toán
    QString calculateExpression(const QString& expression);

signals:
    // Signal báo kết quả tính toán
    void resultReady(const QString& result);
    
    // Signal báo lỗi
    void errorOccurred(const QString& errorMessage);

private:
    // Kết nối đến pipe
    bool connectToPipe();
    
    // Đóng kết nối pipe
    void closeConnection();

    // Tên pipe
    std::string m_pipeName;
    
    // Handle của pipe (Windows)
#ifdef _WIN32
    HANDLE m_pipeHandle;
#else
    int m_pipeHandle;
#endif
};

} // namespace IPC

#endif // NAMED_PIPE_CLIENT_H