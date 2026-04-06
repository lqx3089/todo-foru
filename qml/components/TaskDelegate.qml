// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
import QtQuick 2.15
import TodoForu 1.0

/**
 * TaskDelegate – one task row in the task list.
 *
 * Expected model roles: title, status, importance, dueDate, categories, completed
 */
Item {
    id: root

    property bool isSelected: false
    signal taskClicked(int localId)

    implicitHeight: 56
    implicitWidth:  parent ? parent.width : 300

    CuteCard {
        anchors {
            fill:           parent
            leftMargin:     Theme.spacingS
            rightMargin:    Theme.spacingS
            topMargin:      Theme.spacingXs
            bottomMargin:   Theme.spacingXs
        }
        hovered:   hoverArea.containsMouse || root.isSelected
        elevation: root.isSelected ? 2 : 1

        // Completion checkbox
        Rectangle {
            id: checkbox
            anchors {
                left:           parent.left
                leftMargin:     Theme.spacingM
                verticalCenter: parent.verticalCenter
            }
            width:  20; height: 20
            radius: Theme.radiusSmall
            color:  (model.completed ?? false) ? Theme.accent : "transparent"
            border.width: 2
            border.color: (model.completed ?? false) ? Theme.accent : Theme.border

            Text {
                anchors.centerIn: parent
                text:    "✓"
                color:   Theme.textOnAccent
                visible: model.completed ?? false
                font.pixelSize: Theme.fontSizeS
            }
        }

        // ── Task title ───────────────────────────────────────────────────────
        Text {
            id: titleText
            anchors {
                left:           checkbox.right
                leftMargin:     Theme.spacingM
                verticalCenter: parent.verticalCenter
                right:          metaColumn.left
                rightMargin:    Theme.spacingS
            }
            text:           model.title ?? ""
            color:          (model.completed ?? false) ? Theme.textDisabled : Theme.textPrimary
            font.pixelSize: Theme.fontSizeM
            font.weight:    root.isSelected ? Theme.fontWeightMedium : Theme.fontWeightNormal
            font.family:    Theme.fontFamily
            font.strikeout: model.completed ?? false
            elide:          Text.ElideRight
        }

        // ── Right meta: importance + due date ────────────────────────────────
        Column {
            id: metaColumn
            anchors {
                right:          parent.right
                rightMargin:    Theme.spacingM
                verticalCenter: parent.verticalCenter
            }
            spacing: 2

            Text {
                anchors.right: parent.right
                text:    model.importance === "high" ? "★" : ""
                color:   Theme.warning
                font.pixelSize: Theme.fontSizeM
                visible: model.importance === "high"
            }

            Text {
                anchors.right: parent.right
                text:    model.dueDate ?? ""
                color:   Theme.textDisabled
                font.pixelSize: Theme.fontSizeXs
                font.family:    Theme.fontFamily
                visible: (model.dueDate ?? "") !== ""
            }
        }
    }

    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape:  Qt.PointingHandCursor
        onClicked:    root.taskClicked(model.localId ?? 0)
    }
}
