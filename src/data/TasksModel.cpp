// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "TasksModel.h"

TasksModel::TasksModel(QObject *parent)
    : QAbstractListModel(parent)
{
    loadForList(1); // load default sample
}

int TasksModel::currentListId() const
{
    return m_currentListId;
}

void TasksModel::setCurrentListId(int listId)
{
    if (m_currentListId == listId) return;
    m_currentListId = listId;
    emit currentListIdChanged(m_currentListId);
    loadForList(listId);
}

void TasksModel::loadForList(int listId)
{
    // Sample data to demonstrate the UI
    beginResetModel();
    m_items.clear();

    if (listId == 1) {
        m_items = {
            { 1, 1, {}, QStringLiteral("Buy groceries"),
              QStringLiteral("Milk, eggs, bread, butter"),
              QStringLiteral("notStarted"), QStringLiteral("normal"),
              QStringLiteral("2026-04-10"), {},
              { QStringLiteral("errands") }, false },
            { 2, 1, {}, QStringLiteral("Finish project report"),
              QStringLiteral("Include Q1 numbers and summary"),
              QStringLiteral("inProgress"), QStringLiteral("high"),
              QStringLiteral("2026-04-07"), QStringLiteral("2026-04-07T09:00:00"),
              { QStringLiteral("work") }, false },
            { 3, 1, {}, QStringLiteral("Schedule dentist appointment"),
              {},
              QStringLiteral("notStarted"), QStringLiteral("low"),
              {}, {},
              {}, false },
            { 4, 1, {}, QStringLiteral("Read Qt 6 docs"),
              {},
              QStringLiteral("completed"), QStringLiteral("normal"),
              {}, {},
              { QStringLiteral("learning") }, true },
        };
    } else if (listId == 2) {
        m_items = {
            { 10, 2, {}, QStringLiteral("Prepare presentation slides"), {},
              QStringLiteral("inProgress"), QStringLiteral("high"),
              QStringLiteral("2026-04-08"), {},
              { QStringLiteral("work") }, false },
            { 11, 2, {}, QStringLiteral("Send weekly status email"), {},
              QStringLiteral("notStarted"), QStringLiteral("normal"),
              QStringLiteral("2026-04-11"), {},
              {}, false },
        };
    } else {
        // Empty for other lists
    }

    endResetModel();
    emit countChanged();
}

int TasksModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return static_cast<int>(m_items.size());
}

QVariant TasksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return {};

    const TaskItem &item = m_items.at(index.row());
    switch (role) {
    case LocalIdRole:      return item.localId;
    case ServerIdRole:     return item.serverId;
    case TitleRole:        return item.title;
    case BodyRole:         return item.body;
    case StatusRole:       return item.status;
    case ImportanceRole:   return item.importance;
    case DueDateRole:      return item.dueDate;
    case ReminderDateRole: return item.reminderDate;
    case CategoriesRole:   return item.categories;
    case CompletedRole:    return item.completed;
    case Qt::DisplayRole:  return item.title;
    default: break;
    }
    return {};
}

QHash<int, QByteArray> TasksModel::roleNames() const
{
    return {
        { LocalIdRole,      "localId"      },
        { ServerIdRole,     "serverId"     },
        { TitleRole,        "title"        },
        { BodyRole,         "body"         },
        { StatusRole,       "status"       },
        { ImportanceRole,   "importance"   },
        { DueDateRole,      "dueDate"      },
        { ReminderDateRole, "reminderDate" },
        { CategoriesRole,   "categories"   },
        { CompletedRole,    "completed"    },
    };
}
