// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
import QtQuick 2.15
import TodoForu 1.0

/**
 * MainPage – the primary three-column layout:
 *
 *   ┌─────────────┬─────────────────────────┬──────────────────────────┐
 *   │  ListSidebar│   Tasks list             │  Task detail placeholder │
 *   │  (left)     │   (centre)               │  (right)                 │
 *   └─────────────┴─────────────────────────┴──────────────────────────┘
 *
 * Wired to AppViewModel.taskListsModel and AppViewModel.tasksModel.
 */
Item {
    id: root

    // Currently selected list / task
    property int  selectedListLocalId: 1
    property int  selectedTaskLocalId: -1

    Rectangle {
        anchors.fill: parent
        color: Theme.background
    }

    Row {
        id: contentRow
        anchors.fill: parent

        // ── LEFT SIDEBAR ──────────────────────────────────────────────────────
        Rectangle {
            id: sidebar
            width:  Theme.sidebarWidth
            height: parent.height
            color:  Theme.surfaceSidebar

            Rectangle {
                anchors { right: parent.right; top: parent.top; bottom: parent.bottom }
                width: 1
                color: Theme.border
            }

            Column {
                anchors.fill: parent

                // Sidebar header
                Item {
                    width: parent.width; height: 52
                    Text {
                        anchors {
                            left:           parent.left
                            leftMargin:     Theme.spacingL
                            verticalCenter: parent.verticalCenter
                        }
                        text:            "My Lists"
                        color:           Theme.textSecondary
                        font.pixelSize:  Theme.fontSizeS
                        font.weight:     Theme.fontWeightBold
                        font.family:     Theme.fontFamily
                    }
                }

                // Task list view
                ListView {
                    id: listView
                    width:  parent.width
                    height: parent.height - 52 - addListRow.height
                    clip:   true
                    model:  AppViewModel.taskListsModel

                    delegate: TaskListDelegate {
                        width:      listView.width
                        isSelected: model.localId === root.selectedListLocalId
                        onListClicked: function(id) {
                            root.selectedListLocalId = id
                            AppViewModel.tasksModel.loadForList(id)
                            root.selectedTaskLocalId = -1
                        }
                    }
                }

                // Add new list button
                Item {
                    id: addListRow
                    width: parent.width; height: 44
                    CuteButton {
                        anchors {
                            fill:    parent
                            margins: Theme.spacingS
                        }
                        text: "+ New List"
                        type: "text"
                        onClicked: { /* TODO M4: open new-list dialog */ }
                    }
                }
            }
        }

        // ── CENTRE: Task list ─────────────────────────────────────────────────
        Item {
            id: taskListPanel
            width:  root.width - Theme.sidebarWidth - Theme.taskDetailWidth
            height: parent.height

            Column {
                anchors.fill: parent

                // Panel header
                Item {
                    width: parent.width; height: 52

                    Rectangle {
                        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
                        height: 1; color: Theme.border
                    }

                    Text {
                        anchors {
                            left:           parent.left
                            leftMargin:     Theme.spacingL
                            verticalCenter: parent.verticalCenter
                        }
                        text:            "Tasks"
                        color:           Theme.textPrimary
                        font.pixelSize:  Theme.fontSizeL
                        font.weight:     Theme.fontWeightBold
                        font.family:     Theme.fontFamily
                    }

                    // Add task button
                    CuteButton {
                        anchors {
                            right:          parent.right
                            rightMargin:    Theme.spacingM
                            verticalCenter: parent.verticalCenter
                        }
                        text:  "+ Add Task"
                        type:  "primary"
                        width: 100
                        onClicked: { /* TODO M4: open new-task dialog */ }
                    }
                }

                // Task list
                ListView {
                    id: tasksView
                    width:  parent.width
                    height: parent.height - 52
                    clip:   true
                    model:  AppViewModel.tasksModel
                    spacing: 0

                    delegate: TaskDelegate {
                        width:      tasksView.width
                        isSelected: model.localId === root.selectedTaskLocalId
                        onTaskClicked: function(id) {
                            root.selectedTaskLocalId = id
                        }
                    }

                    // Empty-state placeholder
                    Text {
                        anchors.centerIn: parent
                        visible: tasksView.count === 0
                        text:    "No tasks yet – add one above!"
                        color:   Theme.textDisabled
                        font.pixelSize: Theme.fontSizeM
                        font.family:    Theme.fontFamily
                    }
                }
            }
        }

        // ── RIGHT: Task detail placeholder ────────────────────────────────────
        Rectangle {
            id: detailPanel
            width:  Theme.taskDetailWidth
            height: parent.height
            color:  Theme.surfaceCard

            Rectangle {
                anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
                width: 1; color: Theme.border
            }

            // Placeholder content
            Column {
                anchors.centerIn: parent
                spacing: Theme.spacingM
                visible: root.selectedTaskLocalId === -1

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text:            "📋"
                    font.pixelSize:  40
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text:            "Select a task to view details"
                    color:           Theme.textDisabled
                    font.pixelSize:  Theme.fontSizeS
                    font.family:     Theme.fontFamily
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            // TODO M4: Replace with real TaskDetailView when a task is selected
            Text {
                anchors {
                    top:        parent.top
                    left:       parent.left
                    topMargin:  Theme.spacingL
                    leftMargin: Theme.spacingL
                }
                visible:         root.selectedTaskLocalId !== -1
                text:            "Task #" + root.selectedTaskLocalId + " selected\n\n(Detail editor coming in M4)"
                color:           Theme.textSecondary
                font.pixelSize:  Theme.fontSizeS
                font.family:     Theme.fontFamily
            }
        }
    }
}
