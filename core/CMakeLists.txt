cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "core")

set(COMMON_SOURCES
    cpp_fix.cpp
    c_fix.c
)

if(${CMAKE_SYSTEM_NAME} STREQUAL Generic) # For Phone
    add_executable (${PROJECT_NAME} ${COMMON_SOURCES} gccmain.c)

    target_link_libraries(${PROJECT_NAME} "-T ${PROJECT_SOURCE_DIR}/core/scat.ld")
    
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".axf")
elseif(WIN32) # For MoDis
    add_library(${PROJECT_NAME} SHARED ${COMMON_SOURCES} null.cpp dll.def)
else()
    message(FATAL_ERROR "Wrong target system")
endif()

target_link_libraries(${PROJECT_NAME} main mreapi)
