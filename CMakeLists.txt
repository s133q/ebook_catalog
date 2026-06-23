cmake_minimum_required(VERSION 3.16)
project(EBookCatalog VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql PrintSupport)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp    src/mainwindow.h
    src/bookdialog.cpp    src/bookdialog.h
    src/databasemanager.cpp src/databasemanager.h
    src/pdfexporter.cpp   src/pdfexporter.h
    src/book.h
    src/thememanager.h
)

qt_add_executable(EBookCatalog MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(EBookCatalog PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::PrintSupport
)

set_target_properties(EBookCatalog PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(EBookCatalog)
