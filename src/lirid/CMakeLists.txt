PROJECT(LIRID)

#generating the dbus interfaces
# qdbusxml2cpp -l "DeviceList" -m -a dbus_interfaces org.liri.receivers.xml

set (lirid_SRCS lirid.cpp HalManagedDeviceList_dbusproxy.cpp HalManagedDeviceList.cpp Control_dbusif.cpp Control.cpp device/DeviceInstance_dbusif.cpp device/DeviceSettings.cpp device/DeviceListenThread.cpp device/DeviceInitThread.cpp device/DeviceInstance.cpp ../drivers/driver-interface.cpp ${FILEFORMAT_SRC} ${REMOTES_SRC})

set (lirid_SRCS_MOC Control.h Control_dbusif.h HalManagedDeviceList.h HalManagedDeviceList_dbusproxy.h device/DeviceInstance_dbusif.h device/DeviceInstance.h)

#additional modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#generate moc files
QT4_WRAP_CPP(lirid_SRCS_MOC ${lirid_SRCS_MOC})

#to compile instanceSettings with qt convenience functions
add_definitions ( -DSETTINGS_WITH_QT )

add_executable(lirid ${lirid_SRCS} ${lirid_SRCS_MOC})
TARGET_LINK_LIBRARIES(lirid ${CMAKE_DL_LIBS} ${QT_LIBRARIES} )

install(TARGETS lirid DESTINATION bin CONFIGURATIONS Debug Release)
