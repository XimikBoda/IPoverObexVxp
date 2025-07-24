set(PROJECT_NAME "bt_opp")

add_library(${PROJECT_NAME} STATIC 
    opp.c opp.h opp_types.h
    opp_inject.c
)

target_include_directories(${PROJECT_NAME} PUBLIC ".")

target_link_libraries(${PROJECT_NAME} mreapi console)