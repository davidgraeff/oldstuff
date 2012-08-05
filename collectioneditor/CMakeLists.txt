cmake_minimum_required(VERSION 2.8)
#Only allow this file to be included by the root cmake file
IF (NOT DEFINED PRODUCTID)
	RETURN()
ENDIF()

project(collectioneditor)
find_package(Qt4 COMPONENTS QtCore QtGui QtDeclarative QUIET)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/mongoclientextern")
find_package(MongoClient QUIET)

IF (NOT MONGOCLIENT_FOUND)
	MESSAGE("Not building ${PROJECT_NAME} (MongoDB Client)")
	RETURN()
ENDIF()

IF (NOT QT_QTGUI_FOUND)
	MESSAGE("Not building ${PROJECT_NAME} (Qt4 Gui missing)")
	RETURN()
ENDIF()

OPTION(BUILD_CLIENT_${PROJECT_NAME} "Build ${PROJECT_NAME}" ON)

IF (NOT BUILD_CLIENT_${PROJECT_NAME})
	RETURN()
ENDIF()

MESSAGE(STATUS "Client: ${PROJECT_NAME}")
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}
${MONGOCLIENT_INCLUDE_DIRECTORIES} ${CLIENTSDIR} ${ROOTDIR})

set(collectioneditor_SRCS models/collectionsmodel.cpp models/collectiontagfilterproxy.cpp
	models/couchdatamodel.cpp models/servicedatamodel.cpp models/collectiontagsmodel.cpp models/notificationmodel.cpp
	main.cpp actioncontroller.cpp
	${SRCS_DATABASE}
	${SRCS_SHARED_CLIENTS}
)

QT4_ADD_RESOURCES(RCC_SRCS "res.qrc")
qt4_automoc(${collectioneditor_SRCS})
QT_WRAP_CPP(${PROJECT_NAME} SRCS_MOCS ${SRCS_DATABASE_H})
link_directories(${MONGOCLIENT_LIBRARY_DIRECTORIES})
add_executable(collectioneditor ${collectioneditor_SRCS} ${RCC_SRCS} ${SRCS_MOCS})
target_link_libraries(collectioneditor ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
${QT_QTNETWORK_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${MONGOCLIENT_LIBRARY} ${Boost_LIBRARIES} )

add_dependencies(${PROJECT_NAME} mongoclient)
	