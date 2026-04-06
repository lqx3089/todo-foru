// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
import QtQuick 2.15
import TodoForu 1.0

/**
 * CuteCard – a rounded rectangle card with optional shadow effect.
 *
 * Usage:
 *   CuteCard {
 *       width: parent.width; height: 80
 *       // put content inside default children
 *       Text { anchors.centerIn: parent; text: "Hello" }
 *   }
 */
Item {
    id: root

    property color  color:       Theme.surfaceCard
    property real   radius:      Theme.radiusMedium
    property real   elevation:   1   // 0 = flat, 1 = subtle, 2 = raised
    property bool   hovered:     false

    default property alias content: inner.data

    // Shadow layer (simulated via a slightly blurred rectangle underneath)
    Rectangle {
        anchors {
            fill: parent
            topMargin:  -elevation
            leftMargin: -elevation
        }
        width:  root.width  + elevation * 2
        height: root.height + elevation * 2
        radius: root.radius + elevation
        color:  Theme.shadow
        visible: elevation > 0
        opacity: root.hovered ? 0.25 : 0.12
        Behavior on opacity { NumberAnimation { duration: Theme.durationFast } }
    }

    Rectangle {
        id: inner
        anchors.fill: parent
        radius:       root.radius
        color:        root.color
        border.width: 1
        border.color: root.hovered ? Theme.accent : Theme.border

        Behavior on border.color { ColorAnimation { duration: Theme.durationFast } }
    }
}
