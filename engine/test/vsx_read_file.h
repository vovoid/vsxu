#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include "../include/vsx_profiler.h"

#ifndef PROFILE_SEGMENT_SIZE
#define PROFILE_SEGMENT_SIZE sizeof (vsx_profile_chunk)
#endif

unsigned int get_filesize (FILE **file)
{
  unsigned int size;

  fseek (*file , 0 , SEEK_END);
  size = ftell (*file);
  rewind (*file);

  return size;
}

unsigned int get_amount_file_segments (unsigned int filesize)
{
  unsigned int file_segments;
  file_segments = get_filesize(filesize) / PROFILE_SEGMENT_SIZE;
  return file_segments;
}

char *get_file_buffer (FILE **file, unsigned int filesize)
{
  char *buffer = (char*) malloc (filesize + 1);

  fread (buffer, filesize, 1, *file);

  return buffer;
}

unsigned int read_file_to_memory (char path[], char **buffer)
{
  unsigned int filesize;

  FILE *file = fopen (path, "rb");
  if (file != NULL)
  {
    filesize = get_filesize (&file);
    *buffer = get_file_buffer (&file, filesize);
    fclose (file);

    return filesize;
  }
  else
  {
    *buffer = NULL;

    return -1;
  }
}
