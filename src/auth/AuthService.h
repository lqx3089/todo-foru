// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QTimer>

class ConfigService;
class LocalHttpServer;

/**
 * AuthService
 *
 * Manages OAuth2 + PKCE auth flow, token cache loading/persistence,
 * token refresh, and login/logout state.
 */
class AuthService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString authStatusText READ authStatusText NOTIFY authStatusTextChanged)

public:
    explicit AuthService(const ConfigService *config, QObject *parent = nullptr);

    bool loggedIn() const;
    QString authStatusText() const;

    QString ensureValidAccessToken();
    void loadTokenCacheIfPresent();

public slots:
    /** Begin OAuth2 + PKCE login flow via system browser + localhost callback. */
    void login();

    /** Clear credentials and set loggedIn = false. */
    void logout();

signals:
    void loggedInChanged(bool loggedIn);
    void authStatusTextChanged(const QString &text);
    void loginFailed(const QString &reason);
    void loginSucceeded();

private:
    void setLoggedIn(bool v);
    void setAuthStatusText(const QString &text);
    QString authorizeEndpoint() const;
    QString tokenEndpoint() const;
    bool parseAndStoreTokenResponse(const QByteArray &payload, QString *errorOut);
    bool exchangeCodeForTokens(const QString &code, QString *errorOut);
    bool refreshAccessToken(QString *errorOut);
    bool writeTokenCache() const;
    void clearTokenState();

    const ConfigService *m_config = nullptr;
    QNetworkAccessManager m_network;
    LocalHttpServer *m_localServer = nullptr;
    QTimer m_loginTimeout;

    QString m_codeVerifier;
    QString m_pendingState;
    QString m_accessToken;
    QString m_refreshToken;
    QString m_scope;
    QString m_tokenType;
    qint64 m_expiresAtUnix = 0;

    bool m_loggedIn = false;
    QString m_authStatusText = QStringLiteral("Not logged in");
};
