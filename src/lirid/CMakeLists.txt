PROJECT(LIRID)

#generating the dbus interfaces
# qdbusxml2cpp -l "DeviceList" -m -a dbus_interfaces org.liri.receivers.xml

######## UDEV ########
#include_directories("/usr/include/udev")

set (lirid_SRCS
lirid.cpp
ManagedDeviceList.cpp
Control_dbusif.cpp
Control.cpp
device/DeviceInstance_dbusif.cpp
device/DeviceInstance.cpp
${FILEFORMAT_SRC} ${REMOTES_SRC})

set (lirid_SRCS_MOC
Control.h
Control_dbusif.h
ManagedDeviceList.h
device/DeviceInstance_dbusif.h
device/DeviceInstance.h)

#additional modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#generate moc files
QT4_WRAP_CPP(lirid_SRCS_MOC ${lirid_SRCS_MOC})

add_executable(lirid ${lirid_SRCS} ${lirid_SRCS_MOC})
#SET_TARGET_PROPERTIES(lirid PROPERTIES LINK_FLAGS -L/usr/lib64/libudev.so)
TARGET_LINK_LIBRARIES(lirid ${CMAKE_DL_LIBS} ${QT_LIBRARIES} udev)

install(TARGETS lirid DESTINATION bin CONFIGURATIONS Debug Release)
