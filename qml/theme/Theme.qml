// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
pragma Singleton
import QtQuick 2.15

/**
 * Theme singleton – centralized design tokens for the cute themes.
 *
 * Switch between "light-cute" and "dark-cute" by changing the `name` property.
 * All other values update automatically via the bindings below.
 */
QtObject {
    id: root

    /** Active theme name: "light-cute" | "dark-cute" */
    property string name: "light-cute"

    readonly property bool isDark: name === "dark-cute"

    // ── Colors ───────────────────────────────────────────────────────────────

    /** Primary accent – soft rose/coral */
    readonly property color accent:          isDark ? "#e8759a" : "#e05c85"
    readonly property color accentLight:     isDark ? "#f4a5c0" : "#f0a0bc"
    readonly property color accentDark:      isDark ? "#c44070" : "#b83070"

    /** Backgrounds */
    readonly property color background:      isDark ? "#1e1e2e" : "#fdf6fb"
    readonly property color surfaceCard:     isDark ? "#2a2a3e" : "#ffffff"
    readonly property color surfaceSidebar:  isDark ? "#252535" : "#f5edf8"
    readonly property color surfaceToolbar:  isDark ? "#1a1a2a" : "#f0e6f5"

    /** Text */
    readonly property color textPrimary:     isDark ? "#cdd6f4" : "#2d1b3d"
    readonly property color textSecondary:   isDark ? "#a6adc8" : "#6b5577"
    readonly property color textDisabled:    isDark ? "#585b70" : "#b8a8c0"
    readonly property color textOnAccent:    "#ffffff"

    /** Borders */
    readonly property color border:          isDark ? "#3a3a5a" : "#e8d5f0"
    readonly property color borderFocus:     accent

    /** Status */
    readonly property color success:         isDark ? "#a6e3a1" : "#3db86e"
    readonly property color warning:         isDark ? "#f9e2af" : "#d97706"
    readonly property color error:           isDark ? "#f38ba8" : "#dc2626"
    readonly property color info:            isDark ? "#89b4fa" : "#3b82f6"

    /** Overlay / shadow */
    readonly property color shadow:          isDark ? "#00000080" : "#00000020"
    readonly property color overlay:         isDark ? "#00000060" : "#00000030"

    // ── Geometry ─────────────────────────────────────────────────────────────

    readonly property real radiusSmall:  6
    readonly property real radiusMedium: 10
    readonly property real radiusLarge:  16
    readonly property real radiusRound:  9999

    readonly property real spacingXs:  4
    readonly property real spacingS:   8
    readonly property real spacingM:  12
    readonly property real spacingL:  16
    readonly property real spacingXl: 24
    readonly property real spacingXxl:32

    // ── Typography ───────────────────────────────────────────────────────────

    readonly property string fontFamily:       "Segoe UI, Ubuntu, Noto Sans, sans-serif"
    readonly property real   fontSizeXs:       10
    readonly property real   fontSizeS:        12
    readonly property real   fontSizeM:        14
    readonly property real   fontSizeL:        16
    readonly property real   fontSizeXl:       20
    readonly property real   fontSizeTitle:    24
    readonly property int    fontWeightNormal: Font.Normal
    readonly property int    fontWeightMedium: Font.Medium
    readonly property int    fontWeightBold:   Font.Bold

    // ── Sidebar ──────────────────────────────────────────────────────────────

    readonly property real sidebarWidth:      220
    readonly property real taskDetailWidth:   300
    readonly property real toolbarHeight:      48
    readonly property real statusBarHeight:    28

    // ── Animation durations (ms) ─────────────────────────────────────────────

    readonly property int durationFast:   120
    readonly property int durationNormal: 200
    readonly property int durationSlow:   350
}
