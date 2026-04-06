// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "LocalHttpServer.h"

#include <QTcpSocket>
#include <QUrl>
#include <QUrlQuery>

LocalHttpServer::LocalHttpServer(QObject *parent)
    : QObject(parent)
{
    connect(&m_server, &QTcpServer::newConnection, this, &LocalHttpServer::onNewConnection);
}

bool LocalHttpServer::start(quint16 port)
{
    stop();
    return m_server.listen(QHostAddress::LocalHost, port);
}

void LocalHttpServer::stop()
{
    if (m_server.isListening()) {
        m_server.close();
    }
}

void LocalHttpServer::onNewConnection()
{
    while (m_server.hasPendingConnections()) {
        QTcpSocket *socket = m_server.nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, &LocalHttpServer::onSocketReadyRead);
        connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
    }
}

void LocalHttpServer::onSocketReadyRead()
{
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }

    const QByteArray req = socket->readAll();
    const QList<QByteArray> lines = req.split('\n');
    if (lines.isEmpty()) {
        writeHttpResponse(socket, 400, QStringLiteral("<html><body>Bad request.</body></html>"));
        return;
    }

    const QByteArray firstLine = lines.first().trimmed();
    const QList<QByteArray> parts = firstLine.split(' ');
    if (parts.size() < 2 || parts[0] != "GET") {
        writeHttpResponse(socket, 405, QStringLiteral("<html><body>Method not allowed.</body></html>"));
        return;
    }

    const QByteArray target = parts[1];
    const QUrl url(QStringLiteral("http://localhost") + QString::fromUtf8(target));
    if (url.path() != QStringLiteral("/callback")) {
        writeHttpResponse(socket, 404, QStringLiteral("<html><body>Not found.</body></html>"));
        return;
    }

    const QUrlQuery query(url);
    const QString error = query.queryItemValue(QStringLiteral("error"));
    const QString errorDescription = query.queryItemValue(QStringLiteral("error_description"));
    const QString code = query.queryItemValue(QStringLiteral("code"));
    const QString state = query.queryItemValue(QStringLiteral("state"));

    if (!error.isEmpty()) {
        const QString html = QStringLiteral("<html><body>You may close this window. Login failed: %1</body></html>")
                                 .arg(errorDescription.isEmpty() ? error : errorDescription.toHtmlEscaped());
        writeHttpResponse(socket, 200, html);
        emit callbackReceived(QString(), state, error, errorDescription);
        return;
    }

    if (!code.isEmpty()) {
        writeHttpResponse(socket, 200, QStringLiteral("<html><body>You may close this window. Login succeeded.</body></html>"));
        emit callbackReceived(code, state, QString(), QString());
        return;
    }

    writeHttpResponse(socket, 400, QStringLiteral("<html><body>You may close this window. Login failed: missing code.</body></html>"));
    emit callbackReceived(QString(), state, QStringLiteral("invalid_request"), QStringLiteral("missing code"));
}

void LocalHttpServer::writeHttpResponse(QObject *socketObj, int code, const QString &body) const
{
    auto *socket = qobject_cast<QTcpSocket *>(socketObj);
    if (!socket) {
        return;
    }

    const QByteArray bodyUtf8 = body.toUtf8();
    const QByteArray header =
        QByteArrayLiteral("HTTP/1.1 ") + QByteArray::number(code) + QByteArrayLiteral(" OK\r\n")
        + QByteArrayLiteral("Content-Type: text/html; charset=utf-8\r\n")
        + QByteArrayLiteral("Content-Length: ") + QByteArray::number(bodyUtf8.size()) + QByteArrayLiteral("\r\n")
        + QByteArrayLiteral("Connection: close\r\n\r\n");

    socket->write(header);
    socket->write(bodyUtf8);
    socket->flush();
    socket->disconnectFromHost();
}
