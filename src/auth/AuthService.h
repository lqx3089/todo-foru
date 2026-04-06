// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QObject>
#include <QString>

/**
 * AuthService (stub)
 *
 * Manages the authentication lifecycle.
 * Currently uses a stub: login() sets a loggedIn flag immediately.
 * Real OAuth2 + PKCE implementation (browser open + localhost callback) is planned for M2.
 */
class AuthService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)

public:
    explicit AuthService(QObject *parent = nullptr);

    bool loggedIn() const;

public slots:
    /** Begin the login flow.  (Stub: sets loggedIn = true immediately.) */
    void login();

    /** Clear credentials and set loggedIn = false. */
    void logout();

signals:
    void loggedInChanged(bool loggedIn);
    void loginFailed(const QString &reason);
    void loginSucceeded();

private:
    bool m_loggedIn = false;
};
