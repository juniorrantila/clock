#include "base.h"
#include "try.h"

#include <stdio.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <time.h>
#include <spawn.h>
#include <stdlib.h>

typedef enum : u8  {
    State_Milliseconds,
    State_Seconds,
    State_Minutes,
    State_Hours,
} State;

typedef struct {
    u16 milliseconds;
    u8 seconds;
    u8 minutes;
    u8 hours;
    State last_used;
} TimeParts;

static i128 clock_gettime_as_u128(clockid_t);
static u128 timespec_to_u128(struct timespec);
static TimeParts time_parts(u128 nanoseconds);
static i32 time_printable(TimeParts, char* buf, u32 buf_size);

int main(int argc, char* argv[], char* envp[])
{
    if (argc < 2) {
        (void)fprintf(stderr, "USAGE: %s <command>\n", argv[0]);
        return -1;
    }

    u128 real_start = (u128)MUST(clock_gettime_as_u128(CLOCK_REALTIME));
    u128 cpu_start = (u128)MUST(clock_gettime_as_u128(CLOCK_PROCESS_CPUTIME_ID));

    pid_t pid = 0;
    if (posix_spawnp(&pid, argv[1], null, null, &argv[1], envp) != 0) {
        perror("posix_spawnp");
        return -1;
    }
    int result = 0;
    MUST(waitpid(pid, &result, 0));

    u128 real_end = MUST(clock_gettime_as_u128(CLOCK_REALTIME));
    u128 cpu_end = MUST(clock_gettime_as_u128(CLOCK_PROCESS_CPUTIME_ID));

    TimeParts real_parts = time_parts(real_end - real_start);
    {
        char buf[1024];
        i32 buf_size = MUST(time_printable(real_parts, buf, sizeof(buf)));
        (void)fprintf(stderr, "wall: %.*s\n", buf_size, buf);
    }

    TimeParts cpu_parts = time_parts(cpu_end - cpu_start);
    {
        char buf[1024];
        i32 buf_size = MUST(time_printable(cpu_parts, buf, sizeof(buf)));
        (void)fprintf(stderr, "cpu:  %.*s\n", buf_size, buf);
    }

    return WEXITSTATUS(result);
}

static i128 clock_gettime_as_u128(clockid_t id)
{
    struct timespec ts;
    TRY(clock_gettime(id, &ts));
    return (i128)timespec_to_u128(ts);
}

static u128 timespec_to_u128(struct timespec ts)
{
    return (((u128)ts.tv_sec) * (u128)1000000000) + (u128)ts.tv_nsec;
}

static TimeParts time_parts(const u128 nanoseconds)
{
    u128 duration = nanoseconds / 1000000;
    State state = State_Milliseconds;
    u16 milliseconds = duration % 1000;

    u8 seconds = 0;
    if (duration >= 1000) {
        state = State_Seconds;
        duration /= 1000;
        seconds = duration % 60;
    }

    u8 minutes = 0;
    if (duration >= 60) {
        state = State_Minutes;
        duration /= 60;
        minutes = duration % 60;
    }

    u8 hours = 0;
    if (duration >= 60) {
        state = State_Hours;
        duration /= 60;
        hours = duration % 255; // Assume no longer 255 hours.
    }

    return (TimeParts) {
        .milliseconds = milliseconds,
        .seconds = seconds,
        .minutes = minutes,
        .hours = hours,
        .last_used = state,
    };
}

static i32 time_printable(TimeParts parts, char* buf, u32 buf_size)
{
    i32 used_size = 0;
    switch (parts.last_used) {
    case State_Hours:
        if ((i64)used_size == (i64)buf_size)
            return errno = EOVERFLOW, -1;
        used_size += TRY(snprintf(&buf[used_size], buf_size - used_size, "%uh", parts.hours));

    case State_Minutes:
        if ((i64)used_size == (i64)buf_size)
            return errno = EOVERFLOW, -1;
        used_size += TRY(snprintf(&buf[used_size], buf_size - used_size, "%um", parts.minutes));

    case State_Seconds:
        if ((i64)used_size == (i64)buf_size)
            return errno = EOVERFLOW, -1;
        used_size += TRY(snprintf(&buf[used_size], buf_size - used_size, "%us", parts.seconds));

    case State_Milliseconds:
        if ((i64)used_size == (i64)buf_size)
            return errno = EOVERFLOW, -1;
        used_size += TRY(snprintf(&buf[used_size], buf_size - used_size, "%ums", parts.milliseconds));

    }
    return used_size;
}
