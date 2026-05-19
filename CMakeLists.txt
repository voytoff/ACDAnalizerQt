cmake_minimum_required(VERSION 3.20)
project(ACDAnalizer VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Charts
    Quick
    Core
    Widgets
    Svg
    Gui
    Concurrent
)

include_directories(../ACD2FileQt/lib)
add_subdirectory(../ACD2FileQt ACD2File)

qt_standard_project_setup()

set(RESOURCE_FILES res.qrc)
qt6_add_resources(PROJECT_RESOURCES ${RESOURCE_FILES})

set(APP_ICON_RESOURCE_WINDOWS "appicon.rc")

qt_add_executable(ACDAnalizer
    ${PROJECT_RESOURCES}
    ${APP_ICON_RESOURCE_WINDOWS}
    main.cpp
    mainwindow.h mainwindow.cpp
    treepaintdelegate.h treepaintdelegate.cpp
    treemodel.h treemodel.cpp
    treeitem.h treeitem.cpp
    parameterrow.h parameterrow.cpp
    parametertable.h parametertable.cpp
    tableview.h tableview.cpp
    tablemodel.h tablemodel.cpp
    settingsdlg.h settingsdlg.cpp settingsdlg.ui
    settings.h settings.cpp
    datestyledelegate.h datestyledelegate.cpp
    modeldatawidget.h modeldatawidget.cpp
    contentwidget.h contentwidget.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(ACDAnalizer PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appACDAnalizer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(ACDAnalizer PRIVATE
    Qt6::Charts
    Qt6::Quick
    Qt6::Widgets
    Qt6::Gui
    Qt6::Concurrent
    Qt6::Svg
    ACD2File
)

include(GNUInstallDirs)
install(TARGETS ACDAnalizer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
