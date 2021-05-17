#include "../header/utils.h"


int64_t su_get_sys_time()
{

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + (int64_t)tv.tv_sec * 1000 * 1000;
}
