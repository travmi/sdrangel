project(map)

set(map_SOURCES
    map.cpp
    mapsettings.cpp
    mapplugin.cpp
    mapwebapiadapter.cpp
    osmtemplateserver.cpp
    ibpbeacon.cpp
    webserver.cpp
)

set(map_HEADERS
    map.h
    mapsettings.h
    mapplugin.h
    mapreport.h
    mapwebapiadapter.h
    osmtemplateserver.h
    beacon.h
    ibpbeacon.h
    webserver.h
)

include_directories(
    ${CMAKE_SOURCE_DIR}/swagger/sdrangel/code/qt5/client
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

if(NOT SERVER_MODE)
    set(map_SOURCES
        ${map_SOURCES}
        mapgui.cpp
        mapgui.ui
        maplocationdialog.cpp
        maplocationdialog.ui
        mapmaidenheaddialog.cpp
        mapmaidenheaddialog.ui
        mapsettingsdialog.cpp
        mapsettingsdialog.ui
        mapbeacondialog.cpp
        mapbeacondialog.ui
        mapibpbeacondialog.cpp
        mapibpbeacondialog.ui
        mapradiotimedialog.cpp
        mapradiotimedialog.ui
        mapcolordialog.cpp
        mapmodel.cpp
        mapwebsocketserver.cpp
        cesiuminterface.cpp
        czml.cpp
        map.qrc
        icons.qrc
        cesium.qrc
    )
    set(map_HEADERS
        ${map_HEADERS}
        mapgui.h
        maplocationdialog.h
        mapmaidenheaddialog.h
        mapsettingsdialog.h
        mapbeacondialog.h
        mapibpbeacon.h
        mapradiotimedialog.h
        mapcolordialog.h
        mapmodel.h
        mapwebsocketserver.h
        cesiuminterface.h
        czml.h
    )

    set(TARGET_NAME map)
    set(TARGET_LIB "Qt5::Widgets" Qt5::Quick Qt5::QuickWidgets Qt5::Positioning Qt5::Location Qt5::WebEngine Qt5::WebEngineCore Qt5::WebEngineWidgets)
    set(TARGET_LIB_GUI "sdrgui")
    set(INSTALL_FOLDER ${INSTALL_PLUGINS_DIR})
else()
    set(TARGET_NAME mapsrv)
    set(TARGET_LIB "")
    set(TARGET_LIB_GUI "")
    set(INSTALL_FOLDER ${INSTALL_PLUGINSSRV_DIR})
endif()

add_library(${TARGET_NAME} SHARED
    ${map_SOURCES}
)

target_link_libraries(${TARGET_NAME}
    Qt5::Core
    ${TARGET_LIB}
    sdrbase
    ${TARGET_LIB_GUI}
)

install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_FOLDER})

if(WIN32)
  # Run deployqt for QtQuick etc
  include(DeployQt)
  windeployqt(${TARGET_NAME} ${SDRANGEL_BINARY_BIN_DIR} ${PROJECT_SOURCE_DIR}/map)
endif()

# Install debug symbols
if (WIN32)
    install(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> CONFIGURATIONS Debug RelWithDebInfo DESTINATION ${INSTALL_FOLDER} )
endif()
