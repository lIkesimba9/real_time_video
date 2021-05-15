cmake_minimum_required(VERSION 3.16)
project(gstgccanalysis LANGUAGES C)
set(CMAKE_CXX_STANDARD 17)

# §Õ§à§Ò§Ñ§Ó§Ý§ñ§ä§î -Wl --no-as-needed §Ü§à §Ó§ã§Ö§Þ §ä§Ñ§â§Ô§Ö§ä§Ñ§Þ
# §Õ§Ý§ñ §à§ä§Õ§Ö§Ý§î§ß§í§ç §ä§Ñ§â§Ô§Ö§ä§à§Ó §Þ§à§Ø§ß§à §Ù§Ñ§Õ§Ñ§Ó§Ñ§ä§î §ã §á§à§Þ§à§ë§î§ð target_set_property
set(CMAKE_LINK_WHAT_YOU_USE ON)

# §ß§Ñ§Û§Õ§Ö§Þ §ß§å§Ø§ß§í§Ö §á§Ñ§Ü§Ö§ä§í
find_package(PkgConfig REQUIRED)
# §Õ§à§Ý§Ø§ß§à §Ò§í§ä§î GSTREAMER, §Ñ §ß§Ö GST, §ß§à §ß§Ö §Ò§Ö§â§å§ã§î §å§ä§Ó§Ö§â§Ø§Õ§Ñ§ä§î
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GST_RTP REQUIRED gstreamer-rtp-1.0)

# §ã§à§Ù§Õ§Ñ§Ý§Ú §ä§Ñ§â§Ô§Ö§ä, §Ù§Ñ§Õ§Ñ§Ý§Ú §Ö§Ô§à §Ú§ã§ç§à§Õ§ß§í§Ö §æ§Ñ§Û§Ý§í
add_library(gstgccanalysis SHARED)
target_sources(gstgccanalysis PRIVATE gstgccanalysis.c)
target_sources(gstgccanalysis PUBLIC gstgccanalysis.h)

# §Õ§à§Ò§Ñ§Ó§Ú§Þ §Ù§Ñ§Ô§â§å§Ø§Ö§ß§ß§í§Ö §á§Ö§â§Ö§Þ§Ö§ß§ß§í§Ö gst §Ó §ä§Ñ§â§Ô§Ö§ä
# §ï§ä§à§Ô§à §Þ§å§ã§à§â§Ñ §Þ§à§Ø§ß§à §Ò§í§Ý§à §Ò§í §Ú§Ù§Ò§Ö§Ø§Ñ§ä§î,
# §Ö§ã§Ý§Ú §Ò§í gstreamer §á§â§Ö§Õ§à§ã§ä§Ñ§Ó§Ý§ñ§Ý §ß§à§â§Þ§Ñ§Ý§î§ß§í§Û cmake §æ§Ñ§Û§Ý
target_include_directories(gstgccanalysis PRIVATE ${GST_INCLUDE_DIRS})
target_link_directories(gstgccanalysis PRIVATE ${GST_LIBRARY_DIRS})
target_compile_definitions(gstgccanalysis PRIVATE ${GST_CFLAGS_OTHER})
# §Ñ§ß§Ñ§Ý§à§Ô§Ú§é§ß§à §Õ§Ý§ñ gst-rtp
target_include_directories(gstgccanalysis PRIVATE ${GST_RTP_INCLUDE_DIRS})
target_link_directories(gstgccanalysis PRIVATE ${GST_RTP_LIBRARY_DIRS})
target_compile_definitions(gstgccanalysis PRIVATE ${GST_RTP_CFLAGS_OTHER})

# §à§Ò§â§Ñ§Ò§à§ä§Ñ§Ö§Þ §á§à§Õ§Õ§Ú§â§Ö§Ü§ä§à§â§Ú§ð estimator, §ï§ä§à §Ù§Ñ§Ô§â§å§Ù§Ú§ä §ä§Ñ§â§Ô§Ö§ä estimator
add_subdirectory(estimator ${CMAKE_BINARY_DIR}/simple_rtp_client/gstgccanalysis/estimator)
# §Ü§à§Ô§Õ§Ñ §Ó target_link_libraries §á§à§Õ§Ñ§Ö§ä§ã§ñ §ß§Ö §ß§Ñ§Ù§Ó§Ñ§ß§Ú§Ö §Ò§Ú§Ò§Ý§Ú§à§ä§Ö§Ü§Ú (.so, .a), §Ñ
# §ß§Ñ§Ù§Ó§Ñ§ß§Ú§Ö §ä§Ñ§â§Ô§Ö§ä§Ñ, cmake §Ñ§Ó§ä§à§Þ§Ñ§ä§Ú§é§Ö§ã§Ü§Ú §Õ§à§Ò§Ñ§Ó§Ú§ä §Ó§ã§Ö §ß§Ö§à§Ò§ç§à§Õ§Ú§Þ§í§Ö §á§Ö§â§Ö§Þ§Ö§ß§ß§í§Ö §Ú§Ù §ß§Ö§Ô§à
target_link_libraries(gstgccanalysis PRIVATE estimator)
