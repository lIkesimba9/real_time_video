project(simple_rtp_server LANGUAGES CXX)
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_LINK_WHAT_YOU_USE ON)

file(
    COPY
    config.json
    DESTINATION
    ${CMAKE_BINARY_DIR}/simple_rtp_server/
    )

set(HELP_SOURCE ../common/abstractgstapp.h  ../common/abstractgstapp.cpp
                ../common/json.h  ../common/json.cpp
                ../common/utils.h)
set(SOURCE main.cpp server_class.cpp)
set(HEADER server_class.h )
find_package(PkgConfig REQUIRED)

pkg_check_modules(GST REQUIRED gstreamer-1.0)
include_directories(${GST_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS})
add_definitions(${GST_CFLAGS_OTHER})

pkg_check_modules(GST_RTP REQUIRED gstreamer-rtp-1.0)
include_directories(${GST_RTP_INCLUDE_DIRS})
link_directories(${GST_RTP_LIBRARY_DIRS})
add_definitions(${GST_RTP_CFLAGS_OTHER})

pkg_check_modules(GST_RTP_BASE REQUIRED gstreamer-plugins-base-1.0)
include_directories(${GST_RTP_BASE_INCLUDE_DIRS})
link_directories(${GST_RTP_BASE_LIBRARY_DIRS})
add_definitions(${GST_RTP_BASE_CFLAGS_OTHER})

pkg_check_modules(GST_RTP_GOOD REQUIRED gstreamer-plugins-good-1.0)
include_directories(${GST_RTP_GOOD_INCLUDE_DIRS})
link_directories(${GST_RTP_GOOD_LIBRARY_DIRS})
add_definitions(${GST_RTP_GOOD_CFLAGS_OTHER})




set(LIBS ${LIBS} ${GST_RTP_LIBRARIES} ${GST_LIBRARIES} ${GST_RTP_BASE_LIBRARIES} ${GST_RTP_GOOD_LIBRARIES})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)



add_executable(simple_rtp_server ${SOURCE} ${HEADER} ${HELP_SOURCE})
target_link_libraries(simple_rtp_server ${LIBS})
