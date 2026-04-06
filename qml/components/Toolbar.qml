// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
import QtQuick 2.15
import TodoForu 1.0

/**
 * Toolbar – top bar with app title, Sync button and theme toggle.
 *
 * Wired to AppViewModel.syncNow() via the Sync button.
 */
Item {
    id: root

    property string title: "todo-foru"

    implicitHeight: Theme.toolbarHeight
    implicitWidth:  parent ? parent.width : 600

    Rectangle {
        anchors.fill: parent
        color:        Theme.surfaceToolbar
        Rectangle {
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
            height: 1
            color:  Theme.border
        }
    }

    // App title / logo text
    Text {
        id: appTitle
        anchors {
            left:           parent.left
            leftMargin:     Theme.spacingL
            verticalCenter: parent.verticalCenter
        }
        text:            root.title
        color:           Theme.accent
        font.pixelSize:  Theme.fontSizeXl
        font.weight:     Theme.fontWeightBold
        font.family:     Theme.fontFamily
    }

    // Right-side controls
    Row {
        anchors {
            right:          parent.right
            rightMargin:    Theme.spacingL
            verticalCenter: parent.verticalCenter
        }
        spacing: Theme.spacingS

        // ── Sync button ───────────────────────────────────────────────────────
        CuteButton {
            text:    AppViewModel.syncStatus === "Syncing" ? "Syncing…" : "⟳  Sync"
            type:    "secondary"
            loading: AppViewModel.syncStatus === "Syncing"
            enabled: AppViewModel.loggedIn && AppViewModel.syncStatus !== "Syncing"
            width:   90
            onClicked: AppViewModel.syncNow()
        }

        // ── Theme toggle ──────────────────────────────────────────────────────
        CuteButton {
            text:  Theme.isDark ? "☀ Light" : "🌙 Dark"
            type:  "text"
            width: 72
            onClicked: {
                Theme.name = Theme.isDark ? "light-cute" : "dark-cute"
            }
        }

        // ── Logout button (visible only when logged in) ────────────────────────
        CuteButton {
            text:    "Logout"
            type:    "text"
            visible: AppViewModel.loggedIn
            width:   60
            onClicked: AppViewModel.logout()
        }
    }
}
