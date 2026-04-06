// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
import QtQuick 2.15
import TodoForu 1.0

/**
 * TaskListDelegate – one item in the left sidebar task-list list.
 *
 * Expected model roles: displayName, isDefault, localId
 */
Item {
    id: root

    property bool isSelected: false
    property string displayName: model.displayName ?? ""
    property bool   isDefault:   model.isDefault   ?? false

    signal listClicked(int localId)

    implicitHeight: 40
    implicitWidth:  parent ? parent.width : 200

    Rectangle {
        anchors.fill:  parent
        radius:        Theme.radiusMedium
        color:         root.isSelected ? Theme.accent : (hoverArea.containsMouse ? Theme.surfaceCard : "transparent")
        Behavior on color { ColorAnimation { duration: Theme.durationFast } }

        // Leading colored strip for selected item
        Rectangle {
            anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
            width:   3
            radius:  Theme.radiusSmall
            color:   Theme.accent
            visible: root.isSelected
        }

        Text {
            anchors {
                left:           parent.left
                leftMargin:     Theme.spacingL
                verticalCenter: parent.verticalCenter
                right:          badge.visible ? badge.left : parent.right
                rightMargin:    Theme.spacingS
            }
            text:           (root.isDefault ? "★ " : "☰ ") + root.displayName
            color:          root.isSelected ? Theme.textOnAccent : Theme.textPrimary
            font.pixelSize: Theme.fontSizeM
            font.weight:    root.isSelected ? Theme.fontWeightMedium : Theme.fontWeightNormal
            font.family:    Theme.fontFamily
            elide:          Text.ElideRight
        }

        // Placeholder badge (task count – to be wired in M3)
        Rectangle {
            id: badge
            visible: false  // hidden until count data is available
            anchors {
                right:          parent.right
                rightMargin:    Theme.spacingS
                verticalCenter: parent.verticalCenter
            }
            width:  24; height: 18
            radius: Theme.radiusRound
            color:  root.isSelected ? Qt.rgba(1,1,1,0.25) : Theme.accentLight
        }
    }

    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape:  Qt.PointingHandCursor
        onClicked:    root.listClicked(model.localId ?? 0)
    }
}
