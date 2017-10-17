cmake_minimum_required(VERSION 3.1)

project(core C CXX)

find_package( OpenCV REQUIRED )
find_package( CURL )

set (LIB_PATH "~/")
set (CURL_PATH ${LIB_PATH}/curl-7.55.1)
set (CURL_LIBS ${CURL_PATH}/build/lib/libcurl.so)

include_directories(${CURL_PATH}/include)



set(CORE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/core/inc" PARENT_SCOPE)

include_directories( "${CMAKE_SOURCE_DIR}/core/inc"
    ${OpenCV_INCLUDE_DIRS}
    )

file(GLOB CORE_SOURCES "src/*.cpp" "inc/*.hpp" "inc/*.h")

add_library("${PROJECT_NAME}" SHARED ${CORE_SOURCES})

target_link_libraries( "${PROJECT_NAME}"
    ${OpenCV_LIBS}
    ${CURL_LIBS}
    )

