cmake_minimum_required(VERSION 3.16)

project(MomentWise LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia Sql LinguistTools)

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

set(TS_FILES
    i18n/MomentWise_en_US.ts
    i18n/MomentWise_zh_CN.ts
)
qt5_add_translation(QM_FILES ${TS_FILES})
add_custom_target(translations ALL DEPENDS ${QM_FILES})
add_dependencies(MomentWise translations)

add_custom_command(TARGET MomentWise POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:MomentWise>/i18n"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QM_FILES} "$<TARGET_FILE_DIR:MomentWise>/i18n"
)

if (WIN32)
    set_target_properties(MomentWise PROPERTIES WIN32_EXECUTABLE ON)
    target_compile_definitions(MomentWise PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

