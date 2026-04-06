# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.22)

project(todo-foru
    VERSION 0.1.0
    DESCRIPTION "A lightweight Microsoft To Do client for Linux"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt automatic tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 modules
find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    Qml
    Sql
)

qt_standard_project_setup()
include(GNUInstallDirs)

# Collect source files
set(APP_SOURCES
    src/main.cpp
    src/core/ConfigService.h
    src/core/ConfigService.cpp
    src/core/Logging.h
    src/core/Logging.cpp
    src/auth/AuthService.h
    src/auth/AuthService.cpp
    src/sync/SyncEngine.h
    src/sync/SyncEngine.cpp
    src/data/Database.h
    src/data/Database.cpp
    src/data/TaskListsModel.h
    src/data/TaskListsModel.cpp
    src/data/TasksModel.h
    src/data/TasksModel.cpp
    src/app/AppViewModel.h
    src/app/AppViewModel.cpp
)

qt_add_executable(todo-foru
    ${APP_SOURCES}
)

# QML module – attaches to the existing target, no need to repeat SOURCES
qt_add_qml_module(todo-foru
    URI "TodoForu"
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/pages/LoginPage.qml
        qml/pages/MainPage.qml
        qml/components/Toolbar.qml
        qml/components/SyncStatusBar.qml
        qml/components/CuteButton.qml
        qml/components/CuteCard.qml
        qml/components/TaskListDelegate.qml
        qml/components/TaskDelegate.qml
        qml/theme/Theme.qml
)

# Include paths
target_include_directories(todo-foru PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link Qt modules
target_link_libraries(todo-foru PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::Sql
)

# Installation
install(TARGETS todo-foru
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
