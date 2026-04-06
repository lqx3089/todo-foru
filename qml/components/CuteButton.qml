// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
import QtQuick 2.15
import TodoForu 1.0

/**
 * CuteButton – themed push button used throughout the app.
 *
 * Variants:
 *   type: "primary"   – filled accent button
 *         "secondary" – outlined button
 *         "text"      – flat text button
 */
Item {
    id: root

    property string text:    ""
    property string type:    "primary"  // "primary" | "secondary" | "text"
    property string iconName: ""
    property bool   enabled: true
    property bool   loading: false

    signal clicked()

    implicitWidth:  label.implicitWidth + 2 * Theme.spacingL
    implicitHeight: 36

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: Theme.radiusMedium
        color: {
            if (!root.enabled)              return Theme.textDisabled
            if (root.type === "primary")    return hoverArea.containsMouse ? Theme.accentDark : Theme.accent
            if (root.type === "secondary")  return hoverArea.containsMouse ? Theme.surfaceSidebar : "transparent"
            return "transparent"
        }
        border.width: root.type === "secondary" ? 1.5 : 0
        border.color: Theme.accent

        Behavior on color { ColorAnimation { duration: Theme.durationFast } }
    }

    Text {
        id: label
        anchors.centerIn: parent
        text: root.loading ? "…" : root.text
        color: {
            if (!root.enabled)              return Theme.textDisabled
            if (root.type === "primary")    return Theme.textOnAccent
            return Theme.accent
        }
        font.pixelSize: Theme.fontSizeM
        font.weight:    Theme.fontWeightMedium
        font.family:    Theme.fontFamily
    }

    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape:  root.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: if (root.enabled && !root.loading) root.clicked()
    }

    scale: hoverArea.pressed && root.enabled ? 0.96 : 1.0
    Behavior on scale { NumberAnimation { duration: Theme.durationFast } }
}
