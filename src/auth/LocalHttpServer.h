// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QObject>
#include <QTcpServer>

class LocalHttpServer : public QObject
{
    Q_OBJECT
public:
    explicit LocalHttpServer(QObject *parent = nullptr);
    bool start(quint16 port);
    void stop();

signals:
    void callbackReceived(const QString &code, const QString &state, const QString &error, const QString &errorDescription);

private slots:
    void onNewConnection();
    void onSocketReadyRead();

private:
    void writeHttpResponse(QObject *socketObj, int code, const QString &body) const;

    QTcpServer m_server;
};
