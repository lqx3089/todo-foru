// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "AuthService.h"

#include <QDebug>

AuthService::AuthService(QObject *parent)
    : QObject(parent)
{}

bool AuthService::loggedIn() const
{
    return m_loggedIn;
}

void AuthService::login()
{
    qInfo() << "[AuthService] login() called (stub – real OAuth2+PKCE planned for M2)";

    // TODO M2: Open system browser with PKCE auth URL,
    //          start a localhost HTTP server on redirect_uri port,
    //          exchange code for tokens, store token cache.

    if (!m_loggedIn) {
        m_loggedIn = true;
        emit loggedInChanged(m_loggedIn);
        emit loginSucceeded();
        qInfo() << "[AuthService] loggedIn set to true (stub)";
    }
}

void AuthService::logout()
{
    qInfo() << "[AuthService] logout() called";
    if (m_loggedIn) {
        m_loggedIn = false;
        emit loggedInChanged(m_loggedIn);
    }
}
