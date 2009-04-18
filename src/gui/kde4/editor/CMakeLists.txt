
SET (SRCS
${FILEFORMAT_SRC} ${PROFILES_SRC} ${REMOTES_SRC} ${BUSINTERCONNECTION_SRC}
AP_Model.cpp DP_Model.cpp Remotes_Model.cpp
main.cpp
editAppProfile/editAppProfile.cpp editAppProfile/Variables_Model.cpp
editAppProfile/Busservices_Model.cpp
editDesktopProfile/editDesktopProfile.cpp editRemote/editRemote.cpp
editRemote/Settings_Model.cpp editRemote/Keys_Model.cpp editRemote/Receivers_Model.cpp
mainwindow.cpp
)

include_directories( ${KDE4_INCLUDES})

kde4_add_ui_files(SRCS ui/mainwindow.ui ui/edit_application.ui
ui/edit_desktop.ui ui/edit_remote.ui)

set(EXECUTENAME "liri-editor-kde4")

kde4_ADD_EXECUTABLE(${EXECUTENAME} ${SRCS})
TARGET_LINK_LIBRARIES(${EXECUTENAME} ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})

install(TARGETS ${EXECUTENAME} DESTINATION bin CONFIGURATIONS Debug Release)
install( FILES ${EXECUTENAME}.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
#install( FILES ${EXECUTENAME}.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES ${EXECUTENAME}ui.rc  DESTINATION
${DATA_INSTALL_DIR}/${EXECUTENAME} )
