cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "bt")

add_library(${PROJECT_NAME} STATIC 
    bt.c bt.h
    bt_inject.c
)

target_include_directories(${PROJECT_NAME} PUBLIC ".")

target_link_libraries(${PROJECT_NAME} mreapi console)