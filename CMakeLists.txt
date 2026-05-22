cmake_minimum_required(VERSION 3.20)
project(ACDAnalizer VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Charts
    Quick
    Core
    Widgets
    Svg
    Gui
    Concurrent
)

set(ACDFILEDIR "../ACD2FileQt")
include_directories(${ACDFILEDIR}/lib)
add_subdirectory(${ACDFILEDIR} ACD2File)

#set(ASPOSEDIR "../Aspose")
#include_directories(${ASPOSEDIR}/include)
#add_subdirectory(${ASPOSEDIR} Aspose)
#set(TRANSDIR "../Translator")
#include_directories(${TRANSDIR}/include)
#add_subdirectory(${TRANSDIR} Translator)

# LibreOffice #
#set(LOROOT "C:/Program Files/LibreOffice")
#include_directories(${LOROOT}/sdk/include)
#link_directories(${LOROOT}/program ${LOROOT}/sdk/lib)

# Ods2 #
include(FetchContent)
FetchContent_Declare(
    Ods2
    GIT_REPOSITORY https://github.com/f35f22fan/Ods2
    GIT_TAG        master
)
FetchContent_MakeAvailable(Ods2)

include_directories("${Ods2_SOURCE_DIR}" "${Ods2_SOURCE_DIR}/libzip/lib")
link_directories("${Ods2_BINARY_DIR}/lib" "${Ods2_BINARY_DIR}/lib/Debug" "${Ods2_BINARY_DIR}/lib/Release")

#set(ODS2_SOURCE_DIR "../ods2") #https://github.com/f35f22fan/Ods2
#set(ODS2_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
#include_directories("${ODS2_SOURCE_DIR}" "${ODS2_SOURCE_DIR}/libzip/lib")
#link_directories("${ODS2_BINARY_DIR}/lib" "${ODS2_BINARY_DIR}/lib/Debug" "${ODS2_BINARY_DIR}/lib/Release")
##add_subdirectory(${ODS2_SOURCE_DIR} libzip)
#add_subdirectory(${ODS2_SOURCE_DIR}/ Ods2)

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
    odsutils.h odsutils.cpp
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
#target_compile_definitions(ACDAnalizer PRIVATE _MSC_VER=1944) #windows

target_link_libraries(ACDAnalizer PRIVATE
    Qt6::Charts
    Qt6::Quick
    Qt6::Widgets
    Qt6::Gui
    Qt6::Concurrent
    Qt6::Svg
    #sal3
    #cppu3
    #cppuhelper3MSC
    #mergedlo
    ACD2File
    ods #${zlib_libs}
)

include(GNUInstallDirs)
install(TARGETS ACDAnalizer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
