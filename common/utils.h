#define _BSD_SOURCE
#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>




#define GET_SYS_MS()		(su_get_sys_time() / 1000)
inline int64_t su_get_sys_time()
{

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + (int64_t)tv.tv_sec * 1000 * 1000;
}
#define SU_MAX(a, b)		((a) > (b) ? (a) : (b))
#define SU_MIN(a, b)		((a) < (b) ? (a) : (b))
#define SU_ABS(a, b)		((a) > (b) ? ((a) - (b)) : ((b) - (a)))
#define SIM_HEADER_SIZE			6
#define SIM_SEGMENT_HEADER_SIZE (SIM_HEADER_SIZE + 24)
#define MIN_BITRATE		80000					/*10KB*/
#define MAX_BITRATE		16000000				/*2MB*/
#define START_BITRATE	800000					/*100KB*/
#endif // UTILS_H
