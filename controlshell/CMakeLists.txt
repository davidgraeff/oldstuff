project(propertymonitor)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 COMPONENTS QtCore QtNetwork REQUIRED)

IF (NOT QT_QTCORE_FOUND)
	MESSAGE(STATUS "Client: Not building ${PROJECT_NAME} (Qt4 Gui missing)")
	RETURN()
ELSE()
	MESSAGE(STATUS "Client: ${PROJECT_NAME}")
	OPTION(BUILD_CLIENT_${PROJECT_NAME} "Build ${PROJECT_NAME}" ON)
ENDIF()

IF (NOT BUILD_CLIENT_${PROJECT_NAME})
	RETURN()
ENDIF()

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(propertymonitor_SRCS propertymonitor.cpp main.cpp)
qt4_automoc(${propertymonitor_SRCS})
add_executable(propertymonitor ${propertymonitor_SRCS})
target_link_libraries(propertymonitor ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})
