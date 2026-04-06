// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "ConfigService.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDebug>

// ─── helpers ─────────────────────────────────────────────────────────────────

static QVariantMap jsonObjectToVariantMap(const QJsonObject &obj)
{
    return obj.toVariantMap();
}

static QJsonObject defaultConfig()
{
    QJsonObject auth;
    auth["client_id"]         = "";
    auth["tenant"]            = "common";
    auth["redirect_uri"]      = "http://localhost:53682/callback";
    auth["scopes"]            = QJsonArray{
        "offline_access", "openid", "profile", "User.Read", "Tasks.ReadWrite"
    };
    auth["token_cache_path"]  = "~/.config/todo-foru/token_cache.json";

    QJsonObject storage;
    storage["db_path"]            = "~/.local/share/todo-foru/todo.db";
    storage["enable_offline_mode"]= true;
    storage["outbox_enabled"]     = true;

    QJsonObject sync;
    sync["sync_on_startup"]           = true;
    sync["auto_sync"]                 = true;
    sync["auto_sync_interval_seconds"]= 300;
    sync["network_timeout_seconds"]   = 20;
    sync["conflict_policy"]           = "last_write_wins";

    QJsonObject ui;
    ui["theme"]               = "light-cute";
    ui["show_completed_tasks"]= false;
    ui["date_format"]         = "yyyy-MM-dd";
    ui["time_format"]         = "HH:mm";

    QJsonObject print;
    print["include_notes"]       = true;
    print["default_export_dir"]  = "~/Documents";

    QJsonObject logging;
    logging["level"] = "info";
    logging["file"]  = "~/.local/share/todo-foru/app.log";

    QJsonObject root;
    root["auth"]    = auth;
    root["storage"] = storage;
    root["sync"]    = sync;
    root["ui"]      = ui;
    root["print"]   = print;
    root["logging"] = logging;
    return root;
}

// ─── ConfigService ────────────────────────────────────────────────────────────

ConfigService::ConfigService(QObject *parent)
    : QObject(parent)
{
    // Determine config file path
    const QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    // AppConfigLocation on Linux → ~/.config/<app-name>
    // We also honour the XDG convention: ~/.config/todo-foru/config.json
    m_configFilePath = configDir + QStringLiteral("/config.json");

    // Pre-fill with defaults so the object is always usable even before load()
    m_root = jsonObjectToVariantMap(defaultConfig());
}

QString ConfigService::configFilePath() const
{
    return m_configFilePath;
}

bool ConfigService::load()
{
    qInfo() << "[Config] Loading config from:" << m_configFilePath;

    QFile file(m_configFilePath);
    if (!file.exists()) {
        qInfo() << "[Config] Config file not found – writing default template to" << m_configFilePath;
        writeDefaultConfig(m_configFilePath);
        // Keep the default in-memory values
        ensureDirectoriesExist();
        return true;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Config] Cannot open config file:" << file.errorString();
        return false;
    }

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();

    if (err.error != QJsonParseError::NoError) {
        qWarning() << "[Config] JSON parse error:" << err.errorString();
        return false;
    }

    if (!doc.isObject()) {
        qWarning() << "[Config] Config root is not a JSON object";
        return false;
    }

    // Merge loaded values on top of defaults
    const QJsonObject loaded = doc.object();
    const QJsonObject def    = defaultConfig();

    // For each top-level section, merge individually so missing keys fall back to defaults
    QJsonObject merged = def;
    for (auto it = loaded.constBegin(); it != loaded.constEnd(); ++it) {
        if (it.value().isObject() && def.contains(it.key()) && def[it.key()].isObject()) {
            QJsonObject section = def[it.key()].toObject();
            const QJsonObject loadedSection = it.value().toObject();
            for (auto sit = loadedSection.constBegin(); sit != loadedSection.constEnd(); ++sit) {
                section[sit.key()] = sit.value();
            }
            merged[it.key()] = section;
        } else {
            merged[it.key()] = it.value();
        }
    }

    m_root = jsonObjectToVariantMap(merged);

    // Validate required keys
    if (clientId().isEmpty()) {
        qWarning() << "[Config] auth.client_id is empty – OAuth login will not work until set in" << m_configFilePath;
    }

    ensureDirectoriesExist();
    qInfo() << "[Config] Effective config:" << m_configFilePath
            << "| theme:" << theme()
            << "| db:" << dbPath();
    return true;
}

// ─── path expansion ───────────────────────────────────────────────────────────

QString ConfigService::expandPath(const QString &path) const
{
    if (path.startsWith(QLatin1String("~/"))) {
        return QDir::homePath() + path.mid(1);
    }
    if (path == QLatin1String("~")) {
        return QDir::homePath();
    }
    return path;
}

void ConfigService::ensureDirectoriesExist() const
{
    // Ensure parent directories for db and log exist
    QDir().mkpath(QFileInfo(dbPath()).absolutePath());
    if (!logFile().isEmpty()) {
        QDir().mkpath(QFileInfo(logFile()).absolutePath());
    }
}

void ConfigService::writeDefaultConfig(const QString &path) const
{
    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        const QJsonDocument doc(defaultConfig());
        f.write(doc.toJson(QJsonDocument::Indented));
        f.close();
        qInfo() << "[Config] Default config written to" << path;
    } else {
        qWarning() << "[Config] Could not write default config:" << f.errorString();
    }
}

// ─── accessors ───────────────────────────────────────────────────────────────

static QVariantMap section(const QVariantMap &root, const QString &key)
{
    return root.value(key).toMap();
}

// Qt6 removed the default-value overloads of QVariant::toString/toBool/toInt.
// These helpers restore the "return default when value is missing or invalid" semantics.
static QString variantToString(const QVariant &v, const QString &defaultVal)
{
    return (v.isValid() && !v.isNull()) ? v.toString() : defaultVal;
}

static bool variantToBool(const QVariant &v, bool defaultVal)
{
    return (v.isValid() && !v.isNull()) ? v.toBool() : defaultVal;
}

static int variantToInt(const QVariant &v, int defaultVal)
{
    bool ok = false;
    const int n = v.toInt(&ok);
    return ok ? n : defaultVal;
}

QString ConfigService::clientId() const
{
    return section(m_root, "auth").value("client_id").toString();
}

QString ConfigService::redirectUri() const
{
    return section(m_root, "auth").value("redirect_uri").toString();
}

QStringList ConfigService::scopes() const
{
    const QVariantList list = section(m_root, "auth").value("scopes").toList();
    QStringList out;
    for (const QVariant &v : list) {
        out << v.toString();
    }
    return out;
}

QString ConfigService::tokenCachePath() const
{
    return expandPath(section(m_root, "auth").value("token_cache_path").toString());
}

QString ConfigService::tenant() const
{
    return variantToString(section(m_root, "auth").value("tenant"), QStringLiteral("common"));
}

QString ConfigService::dbPath() const
{
    return expandPath(section(m_root, "storage").value("db_path").toString());
}

bool ConfigService::enableOfflineMode() const
{
    return variantToBool(section(m_root, "storage").value("enable_offline_mode"), true);
}

bool ConfigService::outboxEnabled() const
{
    return variantToBool(section(m_root, "storage").value("outbox_enabled"), true);
}

bool ConfigService::syncOnStartup() const
{
    return variantToBool(section(m_root, "sync").value("sync_on_startup"), true);
}

bool ConfigService::autoSync() const
{
    return variantToBool(section(m_root, "sync").value("auto_sync"), true);
}

int ConfigService::autoSyncIntervalSeconds() const
{
    return variantToInt(section(m_root, "sync").value("auto_sync_interval_seconds"), 300);
}

int ConfigService::networkTimeoutSeconds() const
{
    return variantToInt(section(m_root, "sync").value("network_timeout_seconds"), 20);
}

QString ConfigService::theme() const
{
    return variantToString(section(m_root, "ui").value("theme"), QStringLiteral("light-cute"));
}

bool ConfigService::showCompletedTasks() const
{
    return variantToBool(section(m_root, "ui").value("show_completed_tasks"), false);
}

QString ConfigService::logLevel() const
{
    return variantToString(section(m_root, "logging").value("level"), QStringLiteral("info"));
}

QString ConfigService::logFile() const
{
    return expandPath(section(m_root, "logging").value("file").toString());
}

QString ConfigService::defaultExportDir() const
{
    return expandPath(section(m_root, "print").value("default_export_dir").toString());
}
