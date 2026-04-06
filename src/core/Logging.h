// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QObject>
#include <QString>
#include <QFile>

/**
 * Logging
 *
 * Installs a Qt message handler that writes to stdout/stderr and,
 * optionally, to a log file specified in config.
 *
 * Call Logging::install() early in main() before any Qt logging.
 */
class Logging
{
public:
    /**
     * Install the global Qt message handler.
     * @param logFilePath  If non-empty, log messages are also appended to this file.
     * @param level        Minimum log level: "debug", "info", "warning", "error".
     */
    static void install(const QString &logFilePath = QString(),
                        const QString &level       = QStringLiteral("info"));

    /** Returns the path of the currently open log file, or empty string if none. */
    static QString currentLogFile();

private:
    static void messageHandler(QtMsgType type,
                               const QMessageLogContext &context,
                               const QString &msg);

    static QFile   s_logFile;
    static int     s_minLevel; // 0=debug 1=info 2=warning 3=critical/error
};
