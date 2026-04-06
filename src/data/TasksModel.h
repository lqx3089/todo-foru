// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QString>

/**
 * TasksModel (stub)
 *
 * A QAbstractListModel exposing task data for a given task list to QML.
 * Contains sample data so the UI can be demonstrated immediately.
 * Real data loading from the SQLite database is planned for M3/M4.
 */
class TasksModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(int currentListId READ currentListId WRITE setCurrentListId NOTIFY currentListIdChanged)

public:
    enum Roles {
        LocalIdRole = Qt::UserRole + 1,
        ServerIdRole,
        TitleRole,
        BodyRole,
        StatusRole,
        ImportanceRole,
        DueDateRole,
        ReminderDateRole,
        CategoriesRole,
        CompletedRole,
    };
    Q_ENUM(Roles)

    explicit TasksModel(QObject *parent = nullptr);

    int currentListId() const;
    void setCurrentListId(int listId);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    /** Load sample tasks for the given list id. */
    Q_INVOKABLE void loadForList(int listId);

signals:
    void countChanged();
    void currentListIdChanged(int listId);

private:
    struct TaskItem {
        int     localId;
        int     listLocalId;
        QString serverId;
        QString title;
        QString body;
        QString status;     // "notStarted" | "inProgress" | "completed"
        QString importance; // "normal" | "high" | "low"
        QString dueDate;
        QString reminderDate;
        QStringList categories;
        bool    completed;
    };

    QList<TaskItem> m_items;
    int             m_currentListId = -1;
};
