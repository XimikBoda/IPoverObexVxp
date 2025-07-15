cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "IPtoStream")

add_library(${PROJECT_NAME} STATIC 
    ItemMng.h
    IPtoStream.h IPtoStream.cpp
    TCP.h TCP.cpp
    TCPSock.h TCPSock.cpp
    TCPListener.h TCPListener.cpp
    PacketMaker.h PacketMaker.cpp
    PacketReader.h PacketReader.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ".")

target_link_libraries(${PROJECT_NAME} mreapi bt_opp tcp)