project(gstgccanalysis LANGUAGES CXX)
cmake_minimum_required(VERSION 3.16)

set(SOURCE_LIB gstgccanalysis.cpp)
set(HEADER_LIB gstgccanalysis.h)





find_package(PkgConfig REQUIRED)

pkg_check_modules(GST REQUIRED gstreamer-1.0)
include_directories(${GST_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS})
add_definitions(${GST_CFLAGS_OTHER})

pkg_check_modules(GST_RTP REQUIRED gstreamer-rtp-1.0)
include_directories(${GST_RTP_INCLUDE_DIRS})
link_directories(${GST_RTP_LIBRARY_DIRS})
add_definitions(${GST_RTP_CFLAGS_OTHER})

set(LIBS ${LIBS} ${GST_RTP_LIBRARIES} ${GST_LIBRARIES})




set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_FLAGS "-Wl,--no-as-needed")
#set(CMAKE_C_FLAGS "-Wl,--no-as-needed")


add_library(gstgccanalysis SHARED ${SOURCE_LIB} ${HEADER_LIB})
add_dependencies(gstgccanalysis estimator)

#set(estimator_DIR ${CMAKE_BINARY_DIR}/../install/estimator/cmake/)


target_link_libraries(gstgccanalysis PUBLIC estimator ${LIBS})






