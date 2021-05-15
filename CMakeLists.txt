cmake_minimum_required(VERSION 3.20)

project(real_time_video C CXX)


add_subdirectory(gstgccanalysis ${CMAKE_BINARY_DIR}/simple_rtp_client/gstgccanalysis)
add_subdirectory(simple_rtp_client/ ${CMAKE_BINARY_DIR}/simple_rtp_client)
add_subdirectory(simple_rtp_server/ ${CMAKE_BINARY_DIR}/simple_rtp_server)


