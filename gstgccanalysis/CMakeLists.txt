cmake_minimum_required(VERSION 3.20)
project(gstgccanalysis LANGUAGES C)

set(CMAKE_LINK_WHAT_YOU_USE ON)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.0)
pkg_check_modules(GST_RTP REQUIRED IMPORTED_TARGET gstreamer-rtp-1.0>=1.0)

add_library(gstgccanalysis SHARED)
target_sources(gstgccanalysis PRIVATE gstgccanalysis.c)
target_sources(gstgccanalysis PUBLIC gstgccanalysis.h)
target_link_libraries(gstgccanalysis PRIVATE PkgConfig::GST PkgConfig::GST_RTP)
add_definitions(${GST_CFLAGS_OTHER})
add_definitions(${GST_RTP_CFLAGS_OTHER})



add_subdirectory(estimator ${CMAKE_BINARY_DIR}/simple_rtp_client/gstgccanalysis/estimator)
target_link_libraries(gstgccanalysis PRIVATE estimator)




