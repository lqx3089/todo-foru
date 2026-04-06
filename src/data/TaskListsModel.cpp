// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "TaskListsModel.h"

TaskListsModel::TaskListsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    loadSampleData();
}

void TaskListsModel::loadSampleData()
{
    beginResetModel();
    m_items = {
        { 1, QString(), QStringLiteral("Tasks"),          true  },
        { 2, QString(), QStringLiteral("Work"),           false },
        { 3, QString(), QStringLiteral("Shopping"),       false },
        { 4, QString(), QStringLiteral("Personal"),       false },
        { 5, QString(), QStringLiteral("Reading List"),   false },
    };
    endResetModel();
    emit countChanged();
}

int TaskListsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return static_cast<int>(m_items.size());
}

QVariant TaskListsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return {};

    const TaskListItem &item = m_items.at(index.row());
    switch (role) {
    case LocalIdRole:     return item.localId;
    case ServerIdRole:    return item.serverId;
    case DisplayNameRole: return item.displayName;
    case IsDefaultRole:   return item.isDefault;
    case Qt::DisplayRole: return item.displayName;
    default: break;
    }
    return {};
}

QHash<int, QByteArray> TaskListsModel::roleNames() const
{
    return {
        { LocalIdRole,     "localId"     },
        { ServerIdRole,    "serverId"    },
        { DisplayNameRole, "displayName" },
        { IsDefaultRole,   "isDefault"   },
    };
}
