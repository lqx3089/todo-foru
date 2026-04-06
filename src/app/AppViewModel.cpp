// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "AppViewModel.h"

#include <QDebug>

AppViewModel::AppViewModel(QObject *parent)
    : QObject(parent)
{}

void AppViewModel::setServices(AuthService *auth, SyncEngine *sync)
{
    m_auth = auth;
    m_sync = sync;

    // Wire auth signals
    if (m_auth) {
        connect(m_auth, &AuthService::loggedInChanged, this, [this](bool v) {
            m_loggedIn = v;
            emit loggedInChanged(v);
        });
        connect(m_auth, &AuthService::authStatusTextChanged, this, [this](const QString &text) {
            m_authStatusText = text;
            emit authStatusTextChanged(text);
        });
    }

    // Wire sync signals
    if (m_sync) {
        connect(m_sync, &SyncEngine::syncStatusChanged, this, [this](const QString &s) {
            m_syncStatus = s;
            emit syncStatusChanged(s);
        });
        connect(m_sync, &SyncEngine::lastSyncTextChanged, this, [this](const QString &t) {
            m_lastSyncText = t;
            emit lastSyncTextChanged(t);
        });
    }
}

bool AppViewModel::loggedIn() const
{
    return m_auth ? m_auth->loggedIn() : m_loggedIn;
}

QString AppViewModel::authStatusText() const
{
    return m_auth ? m_auth->authStatusText() : m_authStatusText;
}

QString AppViewModel::syncStatus() const
{
    return m_sync ? m_sync->syncStatus() : m_syncStatus;
}

QString AppViewModel::lastSyncText() const
{
    return m_sync ? m_sync->lastSyncText() : m_lastSyncText;
}

TaskListsModel *AppViewModel::taskListsModel()
{
    return &m_taskListsModel;
}

TasksModel *AppViewModel::tasksModel()
{
    return &m_tasksModel;
}

void AppViewModel::login()
{
    qInfo() << "[AppViewModel] login() invoked from QML";
    if (m_auth) {
        m_auth->login();
    } else {
        // No auth service yet – simulate directly
        m_loggedIn = true;
        emit loggedInChanged(m_loggedIn);
    }
}

void AppViewModel::logout()
{
    qInfo() << "[AppViewModel] logout() invoked from QML";
    if (m_auth) {
        m_auth->logout();
    } else {
        m_loggedIn = false;
        emit loggedInChanged(m_loggedIn);
    }
}

void AppViewModel::syncNow()
{
    qInfo() << "[AppViewModel] syncNow() invoked from QML";
    if (m_sync) {
        m_sync->syncNow();
    }
}
