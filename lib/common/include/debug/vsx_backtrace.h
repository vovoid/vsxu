#ifndef VSX_BACKTRACE_H
#define VSX_BACKTRACE_H

#include <vsx_platform.h>


#if (PLATFORM_FAMILY != PLATFORM_FAMILY_WINDOWS)
  #include <stdio.h>
  #include <errno.h>
  #include <execinfo.h>
  #include <unistd.h>
  #include <string.h>
  #include <malloc.h>
#endif

#if (PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS)
  #include <cstddef>
#endif

namespace vsx_backtrace
{

inline void full_write(int fd, const char *buf, size_t len)
{
#if !(PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS)
  while (len > 0) {
  ssize_t ret = write(fd, buf, len);

  if ((ret == -1) && (errno != EINTR))
  break;

  buf += (size_t) ret;
  len -= (size_t) ret;
  }
#endif
}


inline void print_backtrace(void)
{
#if !(PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS)
  static const char start[] = "BACKTRACE ------------\n";
  static const char end[] = "----------------------\n";

  void *bt[1024];
  int bt_size;
  char **bt_syms;
  int i;

  bt_size = backtrace(bt, 1024);
  bt_syms = backtrace_symbols(bt, bt_size);
  full_write(STDERR_FILENO, start, strlen(start));
  for (i = 1; i < bt_size; i++)
  {
    size_t len = strlen(bt_syms[i]);
    full_write(STDERR_FILENO, bt_syms[i], len);
    full_write(STDERR_FILENO, "\n", 1);
  }
  full_write(STDERR_FILENO, end, strlen(end));
  free(bt_syms);
#endif
}

}

#endif
