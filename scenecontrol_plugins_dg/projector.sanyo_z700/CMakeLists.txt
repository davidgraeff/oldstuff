cmake_minimum_required(VERSION 2.8.8)
SET(PLUGIN_DEPENDENCIES_FOUND TRUE)
find_package(RoomPlugin REQUIRED)
find_package(SerialDevice REQUIRED)

FILE(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB SRCS_H "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
QT4_WRAP_CPP(SRCS_MOCS ${SRCS_H})
add_executable(${PROJECT_NAME} ${SRCS} ${SRCS_MOCS} $<TARGET_OBJECTS:PluginLib>
${SRCS_SERIAL_MOCS} ${SRCS_SERIAL})
target_link_libraries(${PROJECT_NAME} ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARIES})

install_schemas()
install_lib()
