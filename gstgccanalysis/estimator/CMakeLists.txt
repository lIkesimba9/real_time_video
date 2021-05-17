project(estimator LANGUAGES C)
cmake_minimum_required(VERSION 3.20)

add_library(estimator SHARED)

target_sources(estimator PRIVATE
 source/utils.c
 source/aimd_rate_control.c
 source/estimator_common.c
 source/inter_arrival.c
 source/kalman_filter.c
 source/overuse_detector.c
 source/rate_stat.c
 source/remote_bitrate_estimator.c
)
target_sources(estimator PUBLIC
 header/utils.h
 header/aimd_rate_control.h
 header/estimator_common.h
 header/inter_arrival.h
 header/kalman_filter.h
 header/overuse_detector.h
 header/rate_stat.h
 header/remote_bitrate_estimator.h
)

target_link_libraries(estimator PRIVATE m)
