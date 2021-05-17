
#ifndef __utils_h_
#define __utils_h_

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define GET_SYS_MS()		(su_get_sys_time() / 1000)
int64_t su_get_sys_time();
#define SU_MAX(a, b) ((a) > (b) ? (a) : (b))
#define SU_MIN(a, b) ((a) < (b) ? (a) : (b))
#define SU_ABS(a, b) ((a) > (b) ? ((a) - (b)) : ((b) - (a)))
#define SIM_HEADER_SIZE 6
#define SIM_SEGMENT_HEADER_SIZE (SIM_HEADER_SIZE + 24)
#define MIN_BITRATE 80000    /*10KB*/
#define MAX_BITRATE 16000000 /*2MB*/
#define START_BITRATE 800000 /*100KB*/
#endif
