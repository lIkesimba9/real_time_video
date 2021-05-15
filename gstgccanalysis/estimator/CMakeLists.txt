project(estimator LANGUAGES C)
cmake_minimum_required(VERSION 3.16)

add_library(estimator SHARED)
# §Ù§Ñ§Ü§Ú§ß§å§Ý §Ó§ã§Ö §ã§í§â§è§í §Ó PRIVATE, §Ó§ã§Ö §ç§ï§Õ§Ö§â§í §Ó PUBLIC
# §¯§µ§¨§¯§° §à§ä§ã§à§â§ä§Ú§â§à§Ó§Ñ§ä§î §ä§Ñ§Ü, §Ü§Ñ§Ü §â§Ö§Ñ§Ý§î§ß§à §Õ§à§Ý§Ø§ß§à §Ò§í§ä§î §Ó §á§â§à§Ö§Ü§ä§Ö
target_sources(estimator PRIVATE
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
# §Ý§Ú§ß§Ü§å§Ö§Þ§ã§ñ §ã m
target_link_libraries(estimator PRIVATE m)
