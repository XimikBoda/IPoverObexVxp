cmake_minimum_required (VERSION 3.8)

project ("IPoverObex")

set(APP_NAME "IPoverObexTest")
set(DEVELOPER_NAME "Ximik_Boda")
set(RAM "1000" CACHE STRING "Ram requirement (in kb)")
set(IMSI "1234567890" CACHE STRING "Imsi of first SIM-card")
set(API "File SIM card ProMng BT TCP")
option(BACKGROUND "App can work background" OFF)

set(MRE_SDK "$ENV{MRE_SDK}" CACHE STRING "MRE SDK dir")
set(TinyMRESDK "$ENV{TinyMRESDK}" CACHE STRING "TinyMRESDK dir")

if(BACKGROUND)
    add_definitions(-DSUPPORT_BG)
endif()

add_subdirectory(resourses)
add_subdirectory(core)
add_subdirectory(mreapi)
add_subdirectory(vxp)
add_subdirectory(run)
add_subdirectory(bt_opp)
add_subdirectory(tcp)
add_subdirectory(console)
add_subdirectory(IPtoStream)

add_subdirectory(main)