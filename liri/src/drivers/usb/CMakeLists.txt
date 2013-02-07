PROJECT(LIRIUSBDRIVERS)

#additional modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#find_package(LibUSB REQUIRED)

######## USB via liusb0.1 #########

set(LibUSB_FOUND 1)
set(LibUSB_INCLUDE_DIRS "/usr/include/libusb-1.0")
set(LibUSB_LIBRARIES "usb-1.0")

if (LibUSB_FOUND)

include_directories(${LibUSB_INCLUDE_DIRS})

# list all directories with the file "driver.cpp"
file(GLOB_RECURSE DRIVERDIRS "driver.cpp")

### loop through all driver subdirectories:
### although it should not be neccessary you may use your own CMakeLists.txt
### for example to add other source files to ${DRIVERSOURCE}

foreach(DRIVERNAME ${DRIVERDIRS})
	# cut off the full path
	string(REPLACE "${LIRIUSBDRIVERS_SOURCE_DIR}/" "" DRIVERNAME ${DRIVERNAME})
	# cut off the filename
	string(REPLACE "/driver.cpp" "" DRIVERNAME ${DRIVERNAME})

	SET (BUILDDRIVERS "${BUILDDRIVERS}${DRIVERNAME} " CACHE INTERNAL "" FORCE)

	add_subdirectory( ${DRIVERNAME} )
	add_library( ${DRIVERNAME} SHARED "${DRIVERNAME}/driver.cpp")
	set_target_properties(${DRIVERNAME} PROPERTIES PREFIX "")

	target_link_libraries(${DRIVERNAME} ${LibUSB_LIBRARIES})

	INSTALL(TARGETS ${DRIVERNAME} DESTINATION ${LIRI_SYSTEM_DRIVERS_DIR} CONFIGURATIONS Debug Release)
	INSTALL(FILES "${DRIVERNAME}/driver.desktop" DESTINATION "${LIRI_SYSTEM_DATA_DIR}/drivers" RENAME "${DRIVERNAME}.desktop")
	INSTALL(FILES "${DRIVERNAME}/driver.rules" DESTINATION /etc/udev/rules.d RENAME "86-liri-${DRIVERNAME}.rules")

endforeach(DRIVERNAME)

endif (LibUSB_FOUND)
