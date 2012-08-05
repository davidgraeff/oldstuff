cmake_minimum_required(VERSION 2.8)
#Only allow this file to be included by the root cmake file
IF (NOT DEFINED PRODUCTID)
	RETURN()
ENDIF()

project(roomcontrolcurtain)
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

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CLIENTSDIR} "../../")

set(curtain_SRCS curtain.cpp main.cpp ${SRCS_SHARED_CLIENTS} ${SRCS_DATABASE})
qt4_automoc(${curtain_SRCS})
add_executable(${PROJECT_NAME} ${curtain_SRCS})
target_link_libraries(${PROJECT_NAME} ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin COMPONENT Client)