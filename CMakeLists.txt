cmake_minimum_required(VERSION 3.16)

project(MomentWise LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia Sql)

add_executable(MomentWise
    src/main.cpp
    src/AppConfig.h
    src/AppConfig.cpp
    src/Database.h
    src/Database.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/TrayManager.h
    src/TrayManager.cpp
    src/PomodoroController.h
    src/PomodoroController.cpp
    src/MiniBallWidget.h
    src/MiniBallWidget.cpp
    src/TodoModel.h
    src/TodoModel.cpp
    src/TodoCardDelegate.h
    src/TodoCardDelegate.cpp
    src/StatsStore.h
    src/StatsStore.cpp
    src/QuoteProvider.h
    src/QuoteProvider.cpp
    src/HotkeyManager_win.h
    src/HotkeyManager_win.cpp
    src/ui/HeaderWidget.h
    src/ui/HeaderWidget.cpp
    src/ui/BottomNav.h
    src/ui/BottomNav.cpp
    src/ui/BreathingRing.h
    src/ui/BreathingRing.cpp
    src/ui/Theme.h
    src/ui/Theme.cpp
    src/ui/AcrylicBlur_win.h
    src/ui/AcrylicBlur_win.cpp
    resources/resources.qrc
)

target_link_libraries(MomentWise PRIVATE Qt5::Widgets Qt5::Multimedia Qt5::Sql)

if (WIN32)
    set_target_properties(MomentWise PROPERTIES WIN32_EXECUTABLE ON)
    target_compile_definitions(MomentWise PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

