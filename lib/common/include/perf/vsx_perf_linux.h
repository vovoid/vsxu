#pragma once

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#include "sys/types.h"
#include "sys/sysinfo.h"


class vsx_perf
{

  static long
  perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                  int cpu, int group_fd, unsigned long flags)
  {
      int ret;

      ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                     group_fd, flags);
      return ret;
  }

  int fd = 0;

  int parseLine(char* line)
  {
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
  }


public:

  // cache misses
  void cache_misses_start(bool include_kernel = false)
  {
    if (fd)
      return;

    struct perf_event_attr pe;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CACHE_MISSES;
    pe.disabled = 1;
    pe.exclude_kernel = include_kernel?0:1;
    pe.exclude_hv = 1;

    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
       fprintf(stderr, "Error opening leader %llx\n", pe.config);
    }
  }

  void cache_misses_begin()
  {
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
  }

  long long cache_misses_end()
  {
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    long long count;
    ssize_t r = read(fd, &count, sizeof(long long));
    VSX_UNUSED(r);
    return count;
  }

  void cache_misses_stop()
  {
    close(fd);
    fd = 0;
  }

  // cpu instructions
  void cpu_instructions_start()
  {
    if (fd)
      return;

    struct perf_event_attr pe;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
       fprintf(stderr, "Error opening leader %llx\n", pe.config);
    }

  }

  inline void cpu_instructions_begin()
  {
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
  }

  inline long long cpu_instructions_end()
  {
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    long long count;
    ssize_t r = read(fd, &count, sizeof(long long));
    VSX_UNUSED(r);
    return count;
  }

  void cpu_instructions_stop()
  {
    close(fd);
    fd = 0;
  }

  /**
   * @brief memory_currently_used
   * Returns number of megabytes
   * @return
   */
  int memory_currently_used()
  {
    return memory_currently_used_bytes() / (1024 * 1024);
  }

  int memory_currently_used_bytes()
  {
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result * 1024;
  }

};
