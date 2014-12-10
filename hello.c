#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>

double gettime(struct timespec *timer)
{
    struct timespec t;
    if (! timer)
        timer = &t;
    clock_gettime(CLOCK_REALTIME, timer);
    return (double)timer->tv_sec + 1e-9*(double)timer->tv_nsec;
}

void print_time()
{
    printf("%.6f\n", gettime(NULL));
}

int main(void)
{
#pragma omp parallel
    {
        print_time();
        /* print_time(); */
    }
    return 0;
}
