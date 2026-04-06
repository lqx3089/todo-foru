// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

/**
 * ConfigService
 *
 * Loads configuration from ~/.config/todo-foru/config.json.
 * Expands ~ in path values and provides defaults for required keys.
 * Exposes config values to QML as read-only properties.
 */
class ConfigService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString clientId READ clientId CONSTANT)
    Q_PROPERTY(QString redirectUri READ redirectUri CONSTANT)
    Q_PROPERTY(QString dbPath READ dbPath CONSTANT)
    Q_PROPERTY(QString tokenCachePath READ tokenCachePath CONSTANT)
    Q_PROPERTY(QString theme READ theme CONSTANT)
    Q_PROPERTY(QString logFile READ logFile CONSTANT)
    Q_PROPERTY(QString logLevel READ logLevel CONSTANT)
    Q_PROPERTY(bool syncOnStartup READ syncOnStartup CONSTANT)
    Q_PROPERTY(bool autoSync READ autoSync CONSTANT)
    Q_PROPERTY(int autoSyncIntervalSeconds READ autoSyncIntervalSeconds CONSTANT)

public:
    explicit ConfigService(QObject *parent = nullptr);

    /** Returns the path of the config file that was loaded (or would be loaded). */
    QString configFilePath() const;

    /**
     * Loads (or reloads) config.json. Returns true on success.
     * On first run with no config file, writes a template file and uses defaults.
     */
    bool load();

    // auth
    QString clientId() const;
    QString redirectUri() const;
    QStringList scopes() const;
    QString tokenCachePath() const;
    QString tenant() const;

    // storage
    QString dbPath() const;
    bool enableOfflineMode() const;
    bool outboxEnabled() const;

    // sync
    bool syncOnStartup() const;
    bool autoSync() const;
    int autoSyncIntervalSeconds() const;
    int networkTimeoutSeconds() const;

    // ui
    QString theme() const;
    bool showCompletedTasks() const;

    // logging
    QString logLevel() const;
    QString logFile() const;

    // print
    QString defaultExportDir() const;

private:
    QString expandPath(const QString &path) const;
    void ensureDirectoriesExist() const;
    void writeDefaultConfig(const QString &path) const;

    QVariantMap m_root;
    QString m_configFilePath;
};
