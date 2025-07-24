set(PROJECT_NAME "tcp")

add_library(${PROJECT_NAME} STATIC 
    tcp.c stcp.h 
)

target_include_directories(${PROJECT_NAME} PUBLIC ".")

target_link_libraries(${PROJECT_NAME} mreapi console)