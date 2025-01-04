#include <sys/time.h>
#include "helpers.h"

double time_in_seconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6; // Convert seconds and microseconds to seconds
}