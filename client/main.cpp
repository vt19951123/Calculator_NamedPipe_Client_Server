// client/main.cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include "NamedPipeClient.h"

int main(int argc, char *argv[])
{
    qDebug() << "Client starting up...";
    QGuiApplication app(argc, argv);

    qDebug() << "Registering NamedPipeClient type...";
    qmlRegisterType<IPC::NamedPipeClient>("IPC", 1, 0, "NamedPipeClient");

    qDebug() << "Creating NamedPipeClient instance...";
    IPC::NamedPipeClient client;

    qDebug() << "Setting up QML engine...";
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("calculatorClient", &client);

    qDebug() << "Loading QML file...";
    engine.load(QUrl("qrc:/qml/main.qml"));

    if (engine.rootObjects().isEmpty()) {
        qDebug() << "Failed to load QML file!";
        return -1;
    }

    qDebug() << "Client started successfully!";
    return app.exec();
}
