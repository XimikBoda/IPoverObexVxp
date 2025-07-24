cmake_minimum_required (VERSION 3.10)

project ("IPoverObex")

set(APP_NAME "IPoverObexTest")
set(DEVELOPER_NAME "Ximik_Boda")
set(RAM "1000" CACHE STRING "Ram requirement (in kb)")
set(IMSI "1234567890" CACHE STRING "Imsi of first SIM-card")
set(API "File SIM card ProMng BT TCP")
set(APPID "-1") 
set(CERTID "1" CACHE STRING "") 
set(CERT "none" CACHE STRING "")
option(BACKGROUND "App can work background" OFF)

set(MRE_SDK "$ENV{MRE_SDK}" CACHE STRING "MRE SDK dir")
set(TinyMRESDK "$ENV{TinyMRESDK}" CACHE STRING "TinyMRESDK dir")

if(BACKGROUND)
    add_definitions(-DSUPPORT_BG)
endif()

include(cmake/MreUtils.cmake)

add_subdirectory(resources)
add_subdirectory(mreapi)
add_subdirectory(run)
add_subdirectory(bt_opp)
add_subdirectory(tcp)
add_subdirectory(console)
add_subdirectory(IPtoStream)

add_subdirectory(main)