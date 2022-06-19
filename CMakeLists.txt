cmake_minimum_required(VERSION 3.6)

project(WebRTC101)


# set_property(TARGET tgt PROPERTY CXX_STANDARD 17)

set (CMAKE_CXX_STANDARD 17)
include_directories(../libdatachannel/include)

add_library(libdatachannel SHARED IMPORTED)

set_property(TARGET libdatachannel PROPERTY IMPORTED_LOCATION /home/sameerwsl/garage/remote/garage/webrtc/echo_server/libdatachannel.so)
add_executable(server src/server.cpp)
add_executable(client src/client.cpp)
target_link_libraries(client libdatachannel)
target_link_libraries(server libdatachannel)