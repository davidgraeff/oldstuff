PROJECT(KDE4GUI)

SET (BUILDGUI "${BUILDGUI}KDE4 " CACHE INTERNAL "" FORCE)

include_directories( . )

set(KDE_MIN_VERSION "4.2.0")
find_package(KDE4 "4.2.0" REQUIRED)
include (KDE4Defaults)

add_subdirectory(tray)
add_subdirectory(editor)
add_subdirectory(control)
#add_subdirectory(plasma-dataengine)
#add_subdirectory(plasma-applet)
