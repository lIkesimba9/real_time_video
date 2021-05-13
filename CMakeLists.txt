cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project(real_time_video C CXX)

add_subdirectory(simple_rtp_client/ ${CMAKE_BINARY_DIR}/simple_rtp_client)
add_subdirectory(simple_rtp_server/ ${CMAKE_BINARY_DIR}/simple_rtp_server)


