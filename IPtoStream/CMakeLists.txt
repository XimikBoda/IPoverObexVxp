set(PROJECT_NAME "IPtoStream")

add_library(${PROJECT_NAME} STATIC 
    ItemMng.h
    IPtoStream.h IPtoStream.cpp
    Log.h Log.cpp
    TCP.h TCP.cpp
    TCPSock.h TCPSock.cpp
    TCPListener.h TCPListener.cpp
    PacketMaker.h PacketMaker.cpp
    PacketReader.h PacketReader.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ".")

target_link_libraries(${PROJECT_NAME} mreapi bt_opp tcp)