// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QObject>
#include <QString>

#include "auth/AuthService.h"
#include "sync/SyncEngine.h"
#include "data/TaskListsModel.h"
#include "data/TasksModel.h"

/**
 * AppViewModel
 *
 * The central QML-accessible singleton that wires together the backend
 * services (AuthService, SyncEngine) with the QML UI.
 *
 * Exposed to QML via QQmlContext::setContextProperty("AppViewModel", &vm).
 *
 * Properties exposed to QML:
 *   loggedIn       – whether the user is authenticated
 *   syncStatus     – human-readable sync state ("Idle" | "Syncing" | "Error")
 *   lastSyncText   – e.g. "Last sync: 14:32:01"
 *   taskListsModel – TaskListsModel instance for the sidebar
 *   tasksModel     – TasksModel instance for the task list
 *
 * Invokables:
 *   login()        – initiate login flow
 *   logout()       – clear credentials
 *   syncNow()      – trigger a manual sync
 */
class AppViewModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString syncStatus READ syncStatus NOTIFY syncStatusChanged)
    Q_PROPERTY(QString lastSyncText READ lastSyncText NOTIFY lastSyncTextChanged)
    Q_PROPERTY(TaskListsModel* taskListsModel READ taskListsModel CONSTANT)
    Q_PROPERTY(TasksModel*     tasksModel     READ tasksModel     CONSTANT)

public:
    explicit AppViewModel(QObject *parent = nullptr);

    bool    loggedIn()     const;
    QString syncStatus()   const;
    QString lastSyncText() const;

    TaskListsModel *taskListsModel();
    TasksModel     *tasksModel();

    /** Called from main() to inject the real service objects. */
    void setServices(AuthService *auth, SyncEngine *sync);

public slots:
    Q_INVOKABLE void login();
    Q_INVOKABLE void logout();
    Q_INVOKABLE void syncNow();

signals:
    void loggedInChanged(bool loggedIn);
    void syncStatusChanged(const QString &status);
    void lastSyncTextChanged(const QString &text);

private:
    AuthService     *m_auth  = nullptr;
    SyncEngine      *m_sync  = nullptr;
    TaskListsModel   m_taskListsModel;
    TasksModel       m_tasksModel;

    // fallback values used before services are injected
    bool    m_loggedIn    = false;
    QString m_syncStatus  = QStringLiteral("Idle");
    QString m_lastSyncText= QStringLiteral("Never");
};
