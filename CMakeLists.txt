cmake_minimum_required(VERSION 3.20)
project(ACDAnalizer VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Quick
    Core
    Widgets
    LinguistTools
    Svg
    Gui
    Concurrent
)
qt_standard_project_setup()

set(RESOURCE_FILES res.qrc)
qt6_add_resources(PROJECT_RESOURCES ${RESOURCE_FILES})

qt_add_executable(ACDAnalizer
    main.cpp
    mainwindow.h mainwindow.cpp
    ${PROJECT_RESOURCES}
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
    Qt6::Quick
    Qt6::Widgets
    Qt6::Gui
)

include(GNUInstallDirs)
install(TARGETS ACDAnalizer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
