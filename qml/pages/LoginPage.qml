// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
import QtQuick 2.15
import TodoForu 1.0

/**
 * LoginPage – shown when the user is not authenticated.
 *
 * Contains a "Sign in with Microsoft" button that calls AppViewModel.login().
 */
Item {
    id: root

    Rectangle {
        anchors.fill: parent
        color: Theme.background
    }

    Column {
        anchors.centerIn: parent
        spacing: Theme.spacingXl

        // ── App icon / logo ───────────────────────────────────────────────────
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width:  96; height: 96
            radius: Theme.radiusLarge
            color:  Theme.accent

            Text {
                anchors.centerIn: parent
                text:    "✓"
                color:   Theme.textOnAccent
                font.pixelSize: 52
                font.weight:    Theme.fontWeightBold
            }

            // Subtle glow ring
            Rectangle {
                anchors.centerIn: parent
                width:  parent.width  + 12
                height: parent.height + 12
                radius: parent.radius + 6
                color:  "transparent"
                border.width: 2
                border.color: Theme.accentLight
                opacity: 0.5
            }
        }

        // ── App name ──────────────────────────────────────────────────────────
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: Theme.spacingXs

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:            "todo-foru"
                color:           Theme.textPrimary
                font.pixelSize:  Theme.fontSizeTitle
                font.weight:     Theme.fontWeightBold
                font.family:     Theme.fontFamily
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:            "A cute Microsoft To Do client for Linux"
                color:           Theme.textSecondary
                font.pixelSize:  Theme.fontSizeS
                font.family:     Theme.fontFamily
            }
        }

        // ── Login button ──────────────────────────────────────────────────────
        CuteButton {
            anchors.horizontalCenter: parent.horizontalCenter
            text:   "Sign in with Microsoft"
            type:   "primary"
            width:  240
            height: 44
            onClicked: AppViewModel.login()
        }

        // ── Info note ─────────────────────────────────────────────────────────
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:            "Your Microsoft account credentials are handled\nby OAuth2 and never stored by this app."
            color:           Theme.textDisabled
            font.pixelSize:  Theme.fontSizeXs
            font.family:     Theme.fontFamily
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
