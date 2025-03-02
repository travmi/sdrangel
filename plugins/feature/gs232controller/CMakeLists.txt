project(gs232controller)

set(gs232controller_SOURCES
    gs232controller.cpp
    gs232controllersettings.cpp
    gs232controllerplugin.cpp
    gs232controllerworker.cpp
    gs232controllerwebapiadapter.cpp
)

set(gs232controller_HEADERS
    gs232controller.h
    gs232controllersettings.h
    gs232controllerplugin.h
    gs232controllerreport.h
    gs232controllerworker.h
    gs232controllerwebapiadapter.h
)

include_directories(
    ${CMAKE_SOURCE_DIR}/swagger/sdrangel/code/qt5/client
)

if(NOT SERVER_MODE)
    set(gs232controller_SOURCES
        ${gs232controller_SOURCES}
        gs232controllergui.cpp
        gs232controllergui.ui
    )
    set(gs232controller_HEADERS
        ${gs232controller_HEADERS}
        gs232controllergui.h
    )

    set(TARGET_NAME featuregs232controller)
    set(TARGET_LIB Qt5::Widgets)
    set(TARGET_LIB_GUI "sdrgui")
    set(INSTALL_FOLDER ${INSTALL_PLUGINS_DIR})
else()
    set(TARGET_NAME featuregs232controllersrv)
    set(TARGET_LIB "")
    set(TARGET_LIB_GUI "")
    set(INSTALL_FOLDER ${INSTALL_PLUGINSSRV_DIR})
endif()

add_library(${TARGET_NAME} SHARED
    ${gs232controller_SOURCES}
)

target_link_libraries(${TARGET_NAME}
    Qt5::Core
    Qt5::SerialPort
    ${TARGET_LIB}
    sdrbase
    ${TARGET_LIB_GUI}
)

install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_FOLDER})

if(WIN32)
  # Run deployqt for serial libraries
  include(DeployQt)
  windeployqt(${TARGET_NAME} ${SDRANGEL_BINARY_BIN_DIR} ${PROJECT_SOURCE_DIR}/../../../sdrgui/resources)
endif()

# Install debug symbols
if (WIN32)
    install(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> CONFIGURATIONS Debug RelWithDebInfo DESTINATION ${INSTALL_FOLDER} )
endif()
