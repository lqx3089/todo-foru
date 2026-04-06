// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QString>

/**
 * TaskListsModel (stub)
 *
 * A QAbstractListModel exposing task list data to QML.
 * Contains sample data so the UI can be demonstrated immediately.
 * Real data loading from the SQLite database is planned for M3.
 */
class TaskListsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        LocalIdRole = Qt::UserRole + 1,
        ServerIdRole,
        DisplayNameRole,
        IsDefaultRole,
    };
    Q_ENUM(Roles)

    explicit TaskListsModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    /** Load sample data for demonstration. */
    void loadSampleData();

signals:
    void countChanged();

private:
    struct TaskListItem {
        int     localId;
        QString serverId;
        QString displayName;
        bool    isDefault;
    };

    QList<TaskListItem> m_items;
};
