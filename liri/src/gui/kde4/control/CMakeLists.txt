
SET (SRCS ${BUSINTERCONNECTION_SRC} ${FILEFORMAT_SRC} ${PROFILES_SRC} ${REMOTES_SRC}
main.cpp mainwindow.cpp messageswindow.cpp driverlistwindow.cpp selectremotewindow.cpp
messages.cpp LogfileModel.cpp DriverListModel.cpp MessagesModel.cpp
ExecutionListModel.cpp TargetListModel.cpp DeviceListModel.cpp RemotesModel.cpp
)

INCLUDE_DIRECTORIES( ${KDE4_INCLUDES})

SET(EXECUTENAME "liri-control-kde4")

KDE4_ADD_UI_FILES(SRCS "ui/mainwindow.ui" "ui/messageswindow.ui" "ui/driverlistwindow.ui" "ui/selectremotewindow.ui")

KDE4_ADD_EXECUTABLE(${EXECUTENAME} ${SRCS})
TARGET_LINK_LIBRARIES(${EXECUTENAME} ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})

INSTALL(TARGETS ${EXECUTENAME} DESTINATION bin CONFIGURATIONS Debug Release)
INSTALL( FILES ${EXECUTENAME}.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
#install( FILES ${EXECUTENAME}.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
INSTALL( FILES ${EXECUTENAME}ui.rc  DESTINATION  ${DATA_INSTALL_DIR}/${EXECUTENAME} )
