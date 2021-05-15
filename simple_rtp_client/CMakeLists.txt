project(simple_rtp_client LANGUAGES CXX)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_LINK_WHAT_YOU_USE ON)

file(
    COPY
    config.json
    DESTINATION
    ${CMAKE_BINARY_DIR}/simple_rtp_server/
    )


find_package(PkgConfig REQUIRED)


pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GST_RTP REQUIRED gstreamer-rtp-1.0)
pkg_check_modules(GST_RTP_BASE REQUIRED gstreamer-plugins-base-1.0)
pkg_check_modules(GST_RTP_GOOD REQUIRED gstreamer-plugins-good-1.0)

add_executable(simple_rtp_client ${SOURCE} ${HELP_SOURCE})
target_sources(simple_rtp_client PRIVATE main.cpp client_class.cpp client_class.h)
target_sources(simple_rtp_client PRIVATE ../common/abstractgstapp.h  ../common/abstractgstapp.cpp
                ../common/json.h  ../common/json.cpp
                ../common/utils.h)


target_include_directories(simple_rtp_client PRIVATE ${GST_INCLUDE_DIRS})
target_link_directories(simple_rtp_client PRIVATE ${GST_LIBRARY_DIRS})
target_compile_definitions(simple_rtp_client PRIVATE ${GST_CFLAGS_OTHER})

target_include_directories(simple_rtp_client PRIVATE ${GST_RTP_INCLUDE_DIRS})
target_link_directories(simple_rtp_client PRIVATE ${GST_RTP_LIBRARY_DIRS})
target_compile_definitions(simple_rtp_client PRIVATE ${GST_RTP_CFLAGS_OTHER})

target_include_directories(simple_rtp_client PRIVATE ${GST_RTP_BASE_INCLUDE_DIRS})
target_link_directories(simple_rtp_client PRIVATE ${GST_RTP_BASE_LIBRARY_DIRS})
target_compile_definitions(simple_rtp_client PRIVATE ${GST_RTP_BASE_CFLAGS_OTHER})

target_include_directories(simple_rtp_client PRIVATE ${GST_RTP_GOOD_INCLUDE_DIRS})
target_link_directories(simple_rtp_client PRIVATE ${GST_RTP_GOOD_LIBRARY_DIRS})
target_compile_definitions(simple_rtp_client PRIVATE ${GST_RTP_GOOD_CFLAGS_OTHER})


