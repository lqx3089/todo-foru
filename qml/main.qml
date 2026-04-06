// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
import QtQuick 2.15
import QtQuick.Window 2.15
import TodoForu 1.0

/**
 * main.qml – root application window.
 *
 * Hosts:
 *   - Toolbar (top)
 *   - LoginPage  (shown when not logged in)
 *   - MainPage   (shown when logged in)
 *   - SyncStatusBar (bottom)
 *
 * Theme singleton is accessed via the Theme singleton defined in Theme.qml.
 */
Window {
    id: root

    width:   1100
    height:  700
    minimumWidth:  800
    minimumHeight: 550
    visible: true
    title:   "todo-foru"

    color: Theme.background

    // ── Top toolbar ───────────────────────────────────────────────────────────
    Toolbar {
        id: toolbar
        anchors {
            top:   parent.top
            left:  parent.left
            right: parent.right
        }
    }

    // ── Page area (fills space between toolbar and status bar) ────────────────
    Item {
        id: pageArea
        anchors {
            top:    toolbar.bottom
            left:   parent.left
            right:  parent.right
            bottom: statusBar.top
        }

        // Login page (shown when not authenticated)
        LoginPage {
            id: loginPage
            anchors.fill: parent
            visible:      !AppViewModel.loggedIn
        }

        // Main page (shown after login)
        MainPage {
            id: mainPage
            anchors.fill: parent
            visible:      AppViewModel.loggedIn
        }
    }

    // ── Bottom sync status bar ────────────────────────────────────────────────
    SyncStatusBar {
        id: statusBar
        anchors {
            left:   parent.left
            right:  parent.right
            bottom: parent.bottom
        }
    }
}
