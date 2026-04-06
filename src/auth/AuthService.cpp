// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "AuthService.h"

#include "LocalHttpServer.h"
#include "Pkce.h"
#include "core/ConfigService.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

namespace {
quint16 redirectPort(const QString &redirectUri)
{
    const QUrl url(redirectUri);
    return url.port(80);
}

QString joinedScopes(const ConfigService *cfg)
{
    return cfg ? cfg->scopes().join(' ') : QString();
}
}

AuthService::AuthService(const ConfigService *config, QObject *parent)
    : QObject(parent)
    , m_config(config)
    , m_localServer(new LocalHttpServer(this))
{
    m_loginTimeout.setSingleShot(true);
    m_loginTimeout.setInterval(120000);
    connect(&m_loginTimeout, &QTimer::timeout, this, [this]() {
        m_localServer->stop();
        setAuthStatusText(QStringLiteral("Login timed out"));
        emit loginFailed(QStringLiteral("OAuth callback timeout after 120 seconds"));
    });

    connect(m_localServer, &LocalHttpServer::callbackReceived, this,
            [this](const QString &code, const QString &state, const QString &error, const QString &errorDescription) {
                m_loginTimeout.stop();
                m_localServer->stop();

                if (!error.isEmpty()) {
                    const QString reason = errorDescription.isEmpty() ? error : errorDescription;
                    setAuthStatusText(QStringLiteral("Login failed: %1").arg(reason));
                    emit loginFailed(reason);
                    return;
                }

                if (state != m_pendingState) {
                    const QString reason = QStringLiteral("State mismatch in OAuth callback");
                    setAuthStatusText(reason);
                    emit loginFailed(reason);
                    return;
                }

                QString exchangeError;
                setAuthStatusText(QStringLiteral("Exchanging token..."));
                if (!exchangeCodeForTokens(code, &exchangeError)) {
                    setAuthStatusText(QStringLiteral("Login failed: %1").arg(exchangeError));
                    emit loginFailed(exchangeError);
                    return;
                }

                setLoggedIn(true);
                setAuthStatusText(QStringLiteral("Login succeeded"));
                emit loginSucceeded();
            });

    loadTokenCacheIfPresent();
}

bool AuthService::loggedIn() const
{
    return m_loggedIn;
}

QString AuthService::authStatusText() const
{
    return m_authStatusText;
}

void AuthService::setLoggedIn(bool v)
{
    if (m_loggedIn == v) {
        return;
    }
    m_loggedIn = v;
    emit loggedInChanged(m_loggedIn);
}

void AuthService::setAuthStatusText(const QString &text)
{
    if (m_authStatusText == text) {
        return;
    }
    m_authStatusText = text;
    emit authStatusTextChanged(m_authStatusText);
}

QString AuthService::authorizeEndpoint() const
{
    const QString tenant = (m_config && !m_config->tenant().isEmpty())
                               ? m_config->tenant()
                               : QStringLiteral("common");
    return QStringLiteral("https://login.microsoftonline.com/%1/oauth2/v2.0/authorize").arg(tenant);
}

QString AuthService::tokenEndpoint() const
{
    const QString tenant = (m_config && !m_config->tenant().isEmpty())
                               ? m_config->tenant()
                               : QStringLiteral("common");
    return QStringLiteral("https://login.microsoftonline.com/%1/oauth2/v2.0/token").arg(tenant);
}

void AuthService::loadTokenCacheIfPresent()
{
    if (!m_config) {
        setAuthStatusText(QStringLiteral("Auth config unavailable"));
        return;
    }

    QFile file(m_config->tokenCachePath());
    if (!file.exists()) {
        setLoggedIn(false);
        setAuthStatusText(QStringLiteral("Not logged in"));
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setLoggedIn(false);
        setAuthStatusText(QStringLiteral("Failed to read token cache"));
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        setLoggedIn(false);
        setAuthStatusText(QStringLiteral("Invalid token cache"));
        return;
    }

    const QJsonObject obj = doc.object();
    m_accessToken = obj.value(QStringLiteral("access_token")).toString();
    m_refreshToken = obj.value(QStringLiteral("refresh_token")).toString();
    m_expiresAtUnix = static_cast<qint64>(obj.value(QStringLiteral("expires_at_unix")).toDouble(0));
    m_scope = obj.value(QStringLiteral("scope")).toString();
    m_tokenType = obj.value(QStringLiteral("token_type")).toString(QStringLiteral("Bearer"));

    const qint64 now = QDateTime::currentSecsSinceEpoch();
    if (!m_accessToken.isEmpty() && m_expiresAtUnix > now + 120) {
        setLoggedIn(true);
        setAuthStatusText(QStringLiteral("Token cache loaded"));
    } else {
        setLoggedIn(false);
        setAuthStatusText(QStringLiteral("Cached token expired or near expiry"));
    }
}

QString AuthService::ensureValidAccessToken()
{
    if (!m_config) {
        return QString();
    }

    const qint64 now = QDateTime::currentSecsSinceEpoch();
    if (!m_accessToken.isEmpty() && m_expiresAtUnix > now + 120) {
        return m_accessToken;
    }

    QString error;
    if (refreshAccessToken(&error)) {
        setLoggedIn(true);
        return m_accessToken;
    }

    setAuthStatusText(QStringLiteral("Token refresh failed: %1").arg(error));
    setLoggedIn(false);
    return QString();
}

void AuthService::login()
{
    if (!m_config) {
        const QString reason = QStringLiteral("Config unavailable");
        setAuthStatusText(reason);
        emit loginFailed(reason);
        return;
    }
    if (m_config->clientId().isEmpty()) {
        const QString reason = QStringLiteral("auth.client_id is empty");
        setAuthStatusText(reason);
        emit loginFailed(reason);
        return;
    }

    m_codeVerifier = Pkce::generateVerifier();
    const QString challenge = Pkce::challengeFromVerifier(m_codeVerifier);
    m_pendingState = Pkce::generateState();

    const quint16 port = redirectPort(m_config->redirectUri());
    if (!m_localServer->start(port)) {
        const QString reason = QStringLiteral("Cannot listen on localhost:%1").arg(port);
        setAuthStatusText(reason);
        emit loginFailed(reason);
        return;
    }

    QUrl authUrl(authorizeEndpoint());
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("client_id"), m_config->clientId());
    query.addQueryItem(QStringLiteral("response_type"), QStringLiteral("code"));
    query.addQueryItem(QStringLiteral("redirect_uri"), m_config->redirectUri());
    query.addQueryItem(QStringLiteral("response_mode"), QStringLiteral("query"));
    query.addQueryItem(QStringLiteral("scope"), joinedScopes(m_config));
    query.addQueryItem(QStringLiteral("code_challenge"), challenge);
    query.addQueryItem(QStringLiteral("code_challenge_method"), QStringLiteral("S256"));
    query.addQueryItem(QStringLiteral("state"), m_pendingState);
    authUrl.setQuery(query);

    setAuthStatusText(QStringLiteral("Opening browser..."));
    if (!QDesktopServices::openUrl(authUrl)) {
        m_localServer->stop();
        const QString reason = QStringLiteral("Failed to open system browser");
        setAuthStatusText(reason);
        emit loginFailed(reason);
        return;
    }

    setAuthStatusText(QStringLiteral("Waiting for callback..."));
    m_loginTimeout.start();
}

void AuthService::logout()
{
    clearTokenState();
    if (m_config) {
        QFile::remove(m_config->tokenCachePath());
    }
    m_localServer->stop();
    m_loginTimeout.stop();
    setLoggedIn(false);
    setAuthStatusText(QStringLiteral("Logged out"));
}

bool AuthService::parseAndStoreTokenResponse(const QByteArray &payload, QString *errorOut)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        if (errorOut) {
            *errorOut = QStringLiteral("Invalid token response payload");
        }
        return false;
    }

    const QJsonObject obj = doc.object();
    if (obj.contains(QStringLiteral("error"))) {
        const QString err = obj.value(QStringLiteral("error")).toString();
        const QString desc = obj.value(QStringLiteral("error_description")).toString();
        if (errorOut) {
            *errorOut = desc.isEmpty() ? err : (err + QStringLiteral(": ") + desc);
        }
        return false;
    }

    const QString accessToken = obj.value(QStringLiteral("access_token")).toString();
    if (accessToken.isEmpty()) {
        if (errorOut) {
            *errorOut = QStringLiteral("Missing access_token in token response");
        }
        return false;
    }

    m_accessToken = accessToken;
    const QString newRefresh = obj.value(QStringLiteral("refresh_token")).toString();
    if (!newRefresh.isEmpty()) {
        m_refreshToken = newRefresh;
    }

    m_scope = obj.value(QStringLiteral("scope")).toString(joinedScopes(m_config));
    m_tokenType = obj.value(QStringLiteral("token_type")).toString(QStringLiteral("Bearer"));
    const int expiresIn = obj.value(QStringLiteral("expires_in")).toInt(3600);
    m_expiresAtUnix = QDateTime::currentSecsSinceEpoch() + expiresIn;

    if (!writeTokenCache()) {
        if (errorOut) {
            *errorOut = QStringLiteral("Failed to write token cache");
        }
        return false;
    }
    return true;
}

bool AuthService::exchangeCodeForTokens(const QString &code, QString *errorOut)
{
    if (!m_config) {
        if (errorOut) {
            *errorOut = QStringLiteral("Config unavailable");
        }
        return false;
    }

    QUrlQuery form;
    form.addQueryItem(QStringLiteral("client_id"), m_config->clientId());
    form.addQueryItem(QStringLiteral("grant_type"), QStringLiteral("authorization_code"));
    form.addQueryItem(QStringLiteral("code"), code);
    form.addQueryItem(QStringLiteral("redirect_uri"), m_config->redirectUri());
    form.addQueryItem(QStringLiteral("code_verifier"), m_codeVerifier);
    form.addQueryItem(QStringLiteral("scope"), joinedScopes(m_config));

    QNetworkRequest req{QUrl(tokenEndpoint())};
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    QNetworkReply *reply = m_network.post(req, form.toString(QUrl::FullyEncoded).toUtf8());

    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(20000);
    bool timedOut = false;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timeout, &QTimer::timeout, &loop, [&]() {
        timedOut = true;
        if (reply->isRunning()) {
            reply->abort();
        }
        loop.quit();
    });
    timeout.start();
    loop.exec();
    timeout.stop();

    if (timedOut) {
        if (errorOut) {
            *errorOut = QStringLiteral("Token exchange request timed out");
        }
        reply->deleteLater();
        return false;
    }

    const QByteArray body = reply->readAll();
    const bool ok = (reply->error() == QNetworkReply::NoError) && parseAndStoreTokenResponse(body, errorOut);
    if (!ok && errorOut && errorOut->isEmpty()) {
        *errorOut = reply->errorString();
    }
    reply->deleteLater();
    return ok;
}

bool AuthService::refreshAccessToken(QString *errorOut)
{
    if (!m_config) {
        if (errorOut) {
            *errorOut = QStringLiteral("Config unavailable");
        }
        return false;
    }
    if (m_refreshToken.isEmpty()) {
        if (errorOut) {
            *errorOut = QStringLiteral("No refresh token available");
        }
        return false;
    }

    QUrlQuery form;
    form.addQueryItem(QStringLiteral("grant_type"), QStringLiteral("refresh_token"));
    form.addQueryItem(QStringLiteral("refresh_token"), m_refreshToken);
    form.addQueryItem(QStringLiteral("client_id"), m_config->clientId());
    form.addQueryItem(QStringLiteral("scope"), joinedScopes(m_config));

    QNetworkRequest req{QUrl(tokenEndpoint())};
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    QNetworkReply *reply = m_network.post(req, form.toString(QUrl::FullyEncoded).toUtf8());

    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(20000);
    bool timedOut = false;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timeout, &QTimer::timeout, &loop, [&]() {
        timedOut = true;
        if (reply->isRunning()) {
            reply->abort();
        }
        loop.quit();
    });
    timeout.start();
    loop.exec();
    timeout.stop();

    if (timedOut) {
        if (errorOut) {
            *errorOut = QStringLiteral("Token refresh request timed out");
        }
        reply->deleteLater();
        return false;
    }

    const QByteArray body = reply->readAll();
    const bool ok = (reply->error() == QNetworkReply::NoError) && parseAndStoreTokenResponse(body, errorOut);
    if (!ok && errorOut && errorOut->isEmpty()) {
        *errorOut = reply->errorString();
    }
    reply->deleteLater();
    return ok;
}

bool AuthService::writeTokenCache() const
{
    if (!m_config) {
        return false;
    }

    const QString path = m_config->tokenCachePath();
    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return false;
    }

    QJsonObject obj;
    obj[QStringLiteral("access_token")] = m_accessToken;
    obj[QStringLiteral("refresh_token")] = m_refreshToken;
    obj[QStringLiteral("expires_at_unix")] = static_cast<double>(m_expiresAtUnix);
    obj[QStringLiteral("scope")] = m_scope;
    obj[QStringLiteral("token_type")] = m_tokenType;

    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

void AuthService::clearTokenState()
{
    m_codeVerifier.clear();
    m_pendingState.clear();
    m_accessToken.clear();
    m_refreshToken.clear();
    m_scope.clear();
    m_tokenType.clear();
    m_expiresAtUnix = 0;
}
