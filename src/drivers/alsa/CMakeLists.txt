PROJECT(LIRIALSADRIVERS)

#additional modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Alsa REQUIRED)

if (ALSA_FOUND)

include_directories(${ALSA_INCLUDES})

# list all directories with the file "driver.cpp"
file(GLOB_RECURSE DRIVERDIRS "driver.cpp")

### loop through all driver subdirectories:
### although it should not be neccessary you may use your own CMakeLists.txt
### for example to add other source files to ${DRIVERSOURCE}

foreach(DRIVERNAME ${DRIVERDIRS})
	# cut off the full path
	string(REPLACE "${LIRIALSADRIVERS_SOURCE_DIR}/" "" DRIVERNAME ${DRIVERNAME})
	# cut off the filename
	string(REPLACE "/driver.cpp" "" DRIVERNAME ${DRIVERNAME})

	SET (BUILDDRIVERS "${BUILDDRIVERS}${DRIVERNAME} " CACHE INTERNAL "" FORCE)

	set(DRIVERSOURCE
		${LIRISOURCE_SOURCE_DIR}/drivers/driver-interface.cpp
		${LIRISOURCE_SOURCE_DIR}/drivers/driver-interface-alsa.cpp
	)

	add_subdirectory( ${DRIVERNAME} )
	add_library( ${DRIVERNAME} SHARED ${DRIVERSOURCE} "${DRIVERNAME}/driver.cpp")
	set_target_properties(${DRIVERNAME} PROPERTIES PREFIX "")

	target_link_libraries(${DRIVERNAME} ${ASOUND_LIBRARY})

	INSTALL(TARGETS ${DRIVERNAME} DESTINATION ${LIRI_SYSTEM_DRIVERS_DIR} CONFIGURATIONS Debug Release)
	INSTALL(FILES "${DRIVERNAME}/driver.desktop" DESTINATION "${LIRI_SYSTEM_DATA_DIR}/drivers" RENAME "${DRIVERNAME}.desktop")
	INSTALL(FILES "${DRIVERNAME}/driver.fdi" DESTINATION share/hal/fdi/information/10freedesktop RENAME "10-liri-${DRIVERNAME}.fdi")

endforeach(DRIVERNAME)

endif (ALSA_FOUND)
