// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

#include "core/ConfigService.h"
#include "core/Logging.h"
#include "auth/AuthService.h"
#include "sync/SyncEngine.h"
#include "data/Database.h"
#include "app/AppViewModel.h"

int main(int argc, char *argv[])
{
    // ── 1. Qt Application ────────────────────────────────────────────────────
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("todo-foru"));
    app.setOrganizationName(QStringLiteral("todo-foru"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));

    // ── 2. Configuration ─────────────────────────────────────────────────────
    ConfigService config;
    config.load();

    // ── 3. Logging ───────────────────────────────────────────────────────────
    // Install after config is loaded so we know the file path and level.
    Logging::install(config.logFile(), config.logLevel());
    qInfo() << "todo-foru" << app.applicationVersion() << "starting up";
    qInfo() << "[Config] File:" << config.configFilePath();

    // ── 4. Database ──────────────────────────────────────────────────────────
    Database db;
    if (!db.open(config.dbPath())) {
        qCritical() << "Failed to open database – exiting";
        return 1;
    }

    // ── 5. Services ──────────────────────────────────────────────────────────
    AuthService  authService(&config);
    SyncEngine   syncEngine;
    syncEngine.configure(config.autoSync(), config.autoSyncIntervalSeconds());

    // ── 6. ViewModel ─────────────────────────────────────────────────────────
    AppViewModel viewModel;
    viewModel.setServices(&authService, &syncEngine);

    // ── 7. QML Engine ────────────────────────────────────────────────────────
    QQmlApplicationEngine engine;

    // Expose singletons to QML via context (alternative to QML_SINGLETON macro
    // approach so we can pass the live instances created above).
    engine.rootContext()->setContextProperty(QStringLiteral("AppViewModel"), &viewModel);
    engine.rootContext()->setContextProperty(QStringLiteral("Config"),       &config);

    const QUrl mainQml = QUrl(QStringLiteral("qrc:/TodoForu/qml/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [mainQml](QObject *obj, const QUrl &objUrl) {
            if (!obj && objUrl == mainQml) {
                qCritical() << "QML root object creation failed – exiting";
                QCoreApplication::exit(1);
            }
        },
        Qt::QueuedConnection
    );

    engine.load(mainQml);
    if (engine.rootObjects().isEmpty()) {
        qCritical() << "No root QML objects – exiting";
        return 1;
    }

    // ── 8. Optional startup sync ─────────────────────────────────────────────
    if (config.syncOnStartup() && authService.loggedIn()) {
        syncEngine.syncNow();
    }

    return app.exec();
}
