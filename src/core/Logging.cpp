// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "Logging.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <cstdio>
#include <mutex>

// ─── static members ──────────────────────────────────────────────────────────
QFile Logging::s_logFile;
int   Logging::s_minLevel = 1; // default: info

static std::mutex s_logMutex;

// ─── public ──────────────────────────────────────────────────────────────────

void Logging::install(const QString &logFilePath, const QString &level)
{
    // Set minimum level
    const QString lvl = level.toLower();
    if (lvl == QLatin1String("debug"))        s_minLevel = 0;
    else if (lvl == QLatin1String("info"))    s_minLevel = 1;
    else if (lvl == QLatin1String("warning")) s_minLevel = 2;
    else                                       s_minLevel = 3; // error/critical

    // Open log file
    if (!logFilePath.isEmpty()) {
        QDir().mkpath(QFileInfo(logFilePath).absolutePath());
        s_logFile.setFileName(logFilePath);
        if (!s_logFile.open(QIODevice::Append | QIODevice::Text)) {
            // Can't open log file – fall through, console logging still works
            fprintf(stderr, "[Logging] Cannot open log file: %s\n",
                    qPrintable(logFilePath));
        }
    }

    qInstallMessageHandler(Logging::messageHandler);
    qInfo() << "[Logging] Initialized | level:" << level
            << "| file:" << (logFilePath.isEmpty() ? "(console only)" : logFilePath);
}

QString Logging::currentLogFile()
{
    return s_logFile.isOpen() ? s_logFile.fileName() : QString();
}

// ─── private ─────────────────────────────────────────────────────────────────

void Logging::messageHandler(QtMsgType type,
                              const QMessageLogContext & /*context*/,
                              const QString &msg)
{
    // Filter by level
    int msgLevel = 1;
    const char *prefix = "INFO ";
    switch (type) {
    case QtDebugMsg:    msgLevel = 0; prefix = "DEBUG"; break;
    case QtInfoMsg:     msgLevel = 1; prefix = "INFO "; break;
    case QtWarningMsg:  msgLevel = 2; prefix = "WARN "; break;
    case QtCriticalMsg: msgLevel = 3; prefix = "ERROR"; break;
    case QtFatalMsg:    msgLevel = 4; prefix = "FATAL"; break;
    }

    if (msgLevel < s_minLevel) return;

    const QString timestamp = QDateTime::currentDateTime()
                                  .toString(Qt::ISODateWithMs);
    const QString line = QStringLiteral("[%1] [%2] %3\n")
                             .arg(timestamp, QString::fromLatin1(prefix), msg);

    std::lock_guard<std::mutex> lock(s_logMutex);

    // Console output
    if (msgLevel >= 2) {
        fprintf(stderr, "%s", qPrintable(line));
    } else {
        fprintf(stdout, "%s", qPrintable(line));
    }
    fflush(stdout);

    // File output
    if (s_logFile.isOpen()) {
        QTextStream stream(&s_logFile);
        stream << line;
        stream.flush();
    }

    if (type == QtFatalMsg) {
        abort();
    }
}
