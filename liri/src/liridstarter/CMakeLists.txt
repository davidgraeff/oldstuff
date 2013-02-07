PROJECT(LIRICALLOUT)

######## DBUS ########
include_directories("/usr/include/dbus-1.0" "/usr/lib/dbus-1.0/include" "/usr/lib64/dbus-1.0/include/")

add_executable(liricallout liricallout.cpp)
TARGET_LINK_LIBRARIES(liricallout dl dbus-1)

install(TARGETS liricallout DESTINATION bin CONFIGURATIONS Debug Release)
