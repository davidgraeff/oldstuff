PROJECT(LIRIEXECUTION)

set (liriexecution_SRCS ${FILEFORMAT_SRC} ${PROFILES_SRC} ${REMOTES_SRC} ${BUSINTERCONNECTION_SRC}
device/DeviceInstance_dbusif.cpp
device/DeviceInstance.cpp
ExecutionJob.cpp
Control_dbusif.cpp
Control.cpp
DeviceList.cpp
liriexec.cpp
)

set (liriexecution_SRCS_MOC
${BUSINTERCONNECTION_SRC_MOC}
${PROFILES_SRC_MOC}
device/DeviceInstance_dbusif.h
device/DeviceInstance.h
ExecutionJob.h
Control_dbusif.h
Control.h
DeviceList.h
)

include_directories( . )

#generate moc files
QT4_WRAP_CPP(liriexecution_SRCS_MOC ${liriexecution_SRCS_MOC})

add_executable(liriexec ${liriexecution_SRCS} ${liriexecution_SRCS_MOC})
TARGET_LINK_LIBRARIES(liriexec ${QT_LIBRARIES} )

install(TARGETS liriexec DESTINATION bin CONFIGURATIONS Debug Release)
