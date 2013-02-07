PROJECT(LIRIAPPLICATIONS)

OPTION(WITH_KDE4 "Build the KDE4 GUI applications" ON)
OPTION(WITH_GNOME2 "Build the Gnome2 GUI applications" OFF)

#KDE4. Depend on Kde4libs-gui
IF(WITH_KDE4)
  add_subdirectory(kde4)
ENDIF(WITH_KDE4)

#Gnome2. Depend on gtk+ and gconf
IF(WITH_GNOME2)
  add_subdirectory(gnome2)
ENDIF(WITH_GNOME2)
