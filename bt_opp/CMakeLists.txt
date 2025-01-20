cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "bt_opp")

add_library(${PROJECT_NAME} STATIC 
    opp.c opp.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ".")

target_link_libraries(${PROJECT_NAME} mreapi console)