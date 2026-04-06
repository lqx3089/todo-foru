// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
import QtQuick 2.15
import TodoForu 1.0

/**
 * SyncStatusBar – bottom status bar showing sync state and last sync time.
 */
Item {
    id: root
    implicitHeight: Theme.statusBarHeight
    implicitWidth:  parent ? parent.width : 600

    Rectangle {
        anchors.fill: parent
        color: Theme.surfaceToolbar
        Rectangle {
            anchors { left: parent.left; right: parent.right; top: parent.top }
            height: 1
            color:  Theme.border
        }
    }

    // Sync state indicator dot
    Rectangle {
        id: dot
        anchors {
            left:           parent.left
            leftMargin:     Theme.spacingL
            verticalCenter: parent.verticalCenter
        }
        width:  8
        height: 8
        radius: 4
        color: {
            switch (AppViewModel.syncStatus) {
            case "Syncing": return Theme.warning
            case "Error":   return Theme.error
            default:        return Theme.success
            }
        }
        Behavior on color { ColorAnimation { duration: Theme.durationFast } }

        // Pulse animation while syncing
        SequentialAnimation on opacity {
            running:  AppViewModel.syncStatus === "Syncing"
            loops:    Animation.Infinite
            NumberAnimation { to: 0.3; duration: 600 }
            NumberAnimation { to: 1.0; duration: 600 }
        }
    }

    // Sync status text
    Text {
        anchors {
            left:           dot.right
            leftMargin:     Theme.spacingS
            verticalCenter: parent.verticalCenter
        }
        text:           AppViewModel.syncStatus
        color:          Theme.textSecondary
        font.pixelSize: Theme.fontSizeXs
        font.family:    Theme.fontFamily
    }

    // Last sync time
    Text {
        anchors {
            right:          parent.right
            rightMargin:    Theme.spacingL
            verticalCenter: parent.verticalCenter
        }
        text:           AppViewModel.lastSyncText
        color:          Theme.textDisabled
        font.pixelSize: Theme.fontSizeXs
        font.family:    Theme.fontFamily
    }
}
