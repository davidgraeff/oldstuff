PROJECT(LIRISOURCE)

# Include the root build directory for the configuration file
include_directories( ${CMAKE_BINARY_DIR})

# Include dbus directory
# Necessary for the dbus types
include_directories( /usr/include/dbus-1.0 )

# Include this directory for shared source files like the error/state defines (liri-errors.h)
# the log class, the driver interface, profiles, remotes etc
include_directories( ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/drivers)

# prepare configuration file
configure_file(config.h.in config.h @ONLY)

# Depend on Qt4Core (>=4.4) (qDBUS, Mainloop, QString)
set(QT_DONT_USE_QTGUI TRUE)
set(QT_USE_QTDBUS TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDES})

# Classes for accessing remote and profile files
set (FILEFORMAT_SRC
${LIRISOURCE_SOURCE_DIR}/fileformats/DesktopFile.cpp
)

set (PROFILES_SRC
${LIRISOURCE_SOURCE_DIR}/fileformats/DesktopProfileFile.cpp
${LIRISOURCE_SOURCE_DIR}/fileformats/ApplicationProfileFile.cpp
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/Action.cpp
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionBus.cpp
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionProgram.cpp
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionChangeMode.cpp
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionChangeBusInstance.cpp
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionChangeVariable.cpp
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionGroup.cpp
${LIRISOURCE_SOURCE_DIR}/liriexec/device/TargetList.cpp
)

set (PROFILES_SRC_MOC
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/Action.h
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionBus.h
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionProgram.h
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionChangeMode.h
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionChangeBusInstance.h
${LIRISOURCE_SOURCE_DIR}/liriexec/actions/ActionChangeVariable.h
${LIRISOURCE_SOURCE_DIR}/liriexec/device/TargetList.h
)

set (REMOTES_SRC
${LIRISOURCE_SOURCE_DIR}/fileformats/RemoteFile.cpp
${LIRISOURCE_SOURCE_DIR}/fileformats/RemoteLoadProfilesFile.cpp
)

# Bus wrapping classes.
set (BUSINTERCONNECTION_SRC
${LIRISOURCE_SOURCE_DIR}/businterconnect/BusServiceList.cpp
${LIRISOURCE_SOURCE_DIR}/businterconnect/Control_DevManager_dbusproxy.cpp
${LIRISOURCE_SOURCE_DIR}/businterconnect/Control_Execution_dbusproxy.cpp
${LIRISOURCE_SOURCE_DIR}/businterconnect/Receiver_DevManager_dbusproxy.cpp
${LIRISOURCE_SOURCE_DIR}/businterconnect/Receiver_Execution_dbusproxy.cpp
${LIRISOURCE_SOURCE_DIR}/businterconnect/BusConnection.cpp
)

set (BUSINTERCONNECTION_SRC_MOC
${LIRISOURCE_SOURCE_DIR}/businterconnect/BusServiceList.h
${LIRISOURCE_SOURCE_DIR}/businterconnect/Control_DevManager_dbusproxy.h
${LIRISOURCE_SOURCE_DIR}/businterconnect/Control_Execution_dbusproxy.h
${LIRISOURCE_SOURCE_DIR}/businterconnect/Receiver_DevManager_dbusproxy.h
${LIRISOURCE_SOURCE_DIR}/businterconnect/Receiver_Execution_dbusproxy.h
${LIRISOURCE_SOURCE_DIR}/businterconnect/BusConnection.h
)
add_subdirectory(drivers/usb)
add_subdirectory(drivers/alsa)
add_subdirectory(drivers/ieee1394)
add_subdirectory(liridhalhelper)
add_subdirectory(lirid)
add_subdirectory(liriexec)
add_subdirectory(liricontrol)
#add_subdirectory(gui)
