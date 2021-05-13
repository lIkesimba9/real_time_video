project(estimator LANGUAGES CXX C)
cmake_minimum_required(VERSION 3.16)

set(SOURCE_LIB source/aimd_rate_control.c
 source/estimator_common.c
 source/inter_arrival.c
 source/kalman_filter.c
 source/overuse_detector.c
 source/rate_stat.c
 source/remote_bitrate_estimator.c
 )

set(HEADER_LIB header/aimd_rate_control.h
 header/estimator_common.h
 header/inter_arrival.h
 header/kalman_filter.h
 header/overuse_detector.h
 header/rate_stat.h
 header/remote_bitrate_estimator.h
 header/utils.h
 )

add_library(${PROJECT_NAME} STATIC ${SOURCE_LIB} ${HEADER_LIB})



set (CMAKE_INSTALL_PREFIX ../install)



target_include_directories(${PROJECT_NAME}  PUBLIC
    "$<BUILD_INTERFACE:"
    "${CMAKE_CURRENT_SOURCE_DIR}/header/remote_bitrate_estimator.h;"
        ">"
        )


set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER
    "header/remote_bitrate_estimator.h;"
    )
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${PROJECT_NAME}
        ARCHIVE DESTINATION ${PROJECT_NAME}
        PUBLIC_HEADER DESTINATION ${PROJECT_NAME}/include
    )

install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Config.cmake
    DESTINATION ${PROJECT_NAME}/cmake
    )



