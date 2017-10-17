
cmake_minimum_required(VERSION 3.1)

project(main)

include_directories(  
    ${CORE_INCLUDE_PATH} 
    )

set(SOURCES "main.cpp")

add_executable(videoAnalyzer ${SOURCES})
target_link_libraries( videoAnalyzer
    core
    )

