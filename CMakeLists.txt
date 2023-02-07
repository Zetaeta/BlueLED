cmake_minimum_required(VERSION 3.5)

project(bluetooth_ledstrips VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Bluetooth)
find_package(Qt6 REQUIRED COMPONENTS DBus)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        bluetoothdialog.h
        bluetoothdialog.cpp
        bluetoothdialog.ui
        bluetoothinterface.h
        bluetoothinterface.cpp
        btdbusinterface.h btdbusinterface.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(bluetooth_ledstrips
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
qt_add_executable(ledaemon
    MANUAL_FINALIZATION
    daemon.cpp
    daemon.h
    bluetoothinterface.h
    bluetoothinterface.cpp
)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET bluetooth_ledstrips APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(bluetooth_ledstrips SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(bluetooth_ledstrips
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(bluetooth_ledstrips PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(bluetooth_ledstrips PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth Qt6::DBus)
target_link_libraries(ledaemon PRIVATE Qt6::Core Qt6::DBus Qt6::Bluetooth)

set_target_properties(bluetooth_ledstrips PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS bluetooth_ledstrips
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(bluetooth_ledstrips)
endif()
