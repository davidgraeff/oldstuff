PROJECT(LIRIDATAFILES)

IF (LIRI_SYSTEM_CONFIGNAME)
install(FILES systemconfig/${LIRI_SYSTEM_CONFIGNAME} DESTINATION ${LIRI_SYSTEM_CONFIGDIR})
ENDIF (LIRI_SYSTEM_CONFIGNAME)

FILE(GLOB files "dbus_policy/*.conf")
install(FILES ${files} DESTINATION /etc/dbus-1/system.d)

FILE(GLOB files "dbus_service/*.service")
INSTALL(FILES ${files} DESTINATION share/dbus-1/system-services)

FILE(GLOB files "remotes/*.desktop")
INSTALL(FILES ${files} DESTINATION share/liri/remotes)

FILE(GLOB files "images/*.png")
INSTALL(FILES ${files} DESTINATION share/pixmaps)

FILE(GLOB files "modprobe.d/*.blacklist")
INSTALL(FILES ${files} DESTINATION /etc/modprobe.d)

FILE(GLOB files "remote_load_profiles/*.loader")
INSTALL(FILES ${files} DESTINATION share/liri/remote_load_profiles)

FILE(GLOB files "receiver_load_remote/*")
INSTALL(FILES ${files} DESTINATION share/liri/associatedRemote)

FILE(GLOB files "profiles/desktop/*.desktop")
INSTALL(FILES ${files} DESTINATION share/liri/profiles)

FILE(GLOB files "profiles/applications/*.desktop")
INSTALL(FILES ${files} DESTINATION share/liri/profiles/applications)
