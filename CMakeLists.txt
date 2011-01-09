PROJECT(LIRI)

cmake_minimum_required(VERSION 2.8.0)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX
    "/usr" CACHE PATH "Liri install prefix" FORCE
    )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

add_definitions ( -ansi -Wall -DQT_NO_CAST_FROM_ASCII )

include(FindPkgConfig)
pkg_check_modules(DBUS REQUIRED dbus-1)

# set default build type
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build Type")
  SET(CMAKE_CONFIGURATION_TYPES RelWithDebInfo Release Debug CACHE STRING "")
ENDIF (NOT CMAKE_BUILD_TYPE)

SET(LIRI_VERSION "0.1.0")
SET(LIRI_USER_DATA_DIR ".config/liri" CACHE STRING "Set the directory where user application profiles and configurations are stored")
SET(LIRI_USER_CONFIG_FILE "configuration" CACHE STRING "Set the configuration file within the user_config_dir. Necessary if you install more than one version of liri.")
SET(LIRI_SYSTEM_DATA_DIR "share/liri" CACHE STRING "Set the directory where remotes and system application profiles are stored")
SET(LIRI_SYSTEM_DRIVERS_DIR "lib/liri/drivers" CACHE STRING "Set the directory where receiver drivers are located")

SET(BUILDGUI "" CACHE INTERNAL "" FORCE)
SET(BUILDDRIVERS "" CACHE INTERNAL "" FORCE)

SET(LIRI_SYSTEM_CONFIGNAME "lirid" CACHE STRING "Name of the system configuration file")
SET(LIRI_SYSTEM_CONFIGDIR "/etc" CACHE STRING "Location of the system configuration file")

add_subdirectory(data)
add_subdirectory(src)

MESSAGE(STATUS "#########################")
MESSAGE(STATUS "Building Gui: ${BUILDGUI}")
MESSAGE(STATUS "Building Drivers: ${BUILDDRIVERS}")
MESSAGE(STATUS "#########################")

# check if lirc is installed under unix systems and warn the user
IF (UNIX)
	execute_process(COMMAND uname -r OUTPUT_VARIABLE linuxVersion)
	string(REPLACE "\n" "" linuxVersion ${linuxVersion})

	IF(EXISTS "/usr/bin/lircd")
		SET(LIRC_INSTALLED TRUE)
	ELSEIF(EXISTS "/usr/local/bin/lircd")
		SET(LIRC_INSTALLED TRUE)
	ELSEIF(EXISTS "/lib/modules/${linuxVersion}/ubuntu/media/lirc/lirc_dev/lirc_dev.ko")
		SET(LIRC_INSTALLED TRUE)
	ELSEIF(EXISTS "/lib/modules/${linuxVersion}/kernel/ubuntu/lirc/lirc_dev/lirc_dev.ko")
		SET(LIRC_INSTALLED TRUE)
	ENDIF(EXISTS "/usr/bin/lircd")

	IF (LIRC_INSTALLED)
		MESSAGE(STATUS "WARNING: LIRC detected")
		MESSAGE(STATUS "Lirc Kernel drivers will be blacklisted if you install liri.")
		MESSAGE(STATUS "Please be sure to reboot or unload the lirc drivers after installation.")
		MESSAGE(STATUS "#########################")
	ELSE  (LIRC_INSTALLED)
		MESSAGE(STATUS "Lirc executable and kernel drivers: No (${linuxVersion})")
	ENDIF (LIRC_INSTALLED)
ENDIF (UNIX)

# --- Package generation
set ( PACKAGE_NAME ${PROJECT_NAME} )
set ( PACKAGE_VERSION ${LIRI_VERSION} )
set ( PACKAGE_RELEASE 0 )
set ( PACKAGE_MAINTAINER_NAME "Liri Team" )
set ( PACKAGE_MAINTAINER_EMAIL "david.graeff@cs.udo.edu" )
set ( PACKAGE_DESCRIPTION_SUMMARY "Linux Remote Infrastructure" )
set ( PACKAGE_DESCRIPTION "Liri provides all from usb/firewire remote receiver drivers to KDE4 Gnome Applications to install and manage remotes. Its main purpose is to control other applications via the de facto default linux interprocess communication bus DBUS. Both of the major desktop environments KDE4 and Gnome and almost all of their applications support that bus system. You may for instance control your multimedia player while lying in your bed or giving a speech without beeing tied to your computer. The Liri backend uses hal to detect if known receivers are plugged in and the gui frontends load automaticly the default remote description file and application control files." )
set( PACKAGE_GROUP "utils" )
set( PACKAGE_SECTION "utils" )
set( PACKAGE_LICENSE "GPL" )
set( PACKAGE_PRIORITY "optional" )
set( PACKAGE_DEPENDS "libqtcore4, libqtgui4, kdelibs-bin >= 4:4.1.0-0" )

# Debian packages
include ( cmake/DpkgDeb.cmake )
if ( DPKG_FOUND )
    add_debian_package ( ${PROJECT_NAME} )
endif ( DPKG_FOUND )

# RPM packages
include ( cmake/Rpmbuild.cmake )
set ( PACKAGE_GROUP "Groupe" )
set ( PACKAGE_LICENSE "GPL" )

if ( RPMBUILD_FOUND )
    add_rpm ( ${PROJECT_NAME} )
endif ( RPMBUILD_FOUND )
