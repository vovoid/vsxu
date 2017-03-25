#include "vsx_read_file.h"

void show_file_buffer (char *buffer, unsigned int filesize)
{
  for (unsigned int i = 0; i < filesize; i++)
    printf ("%x ", ((char *)buffer)[i]);
}

int main ()
{
  char path[] = "/tmp/vsx_profile.dat";
  char *buffer;
  unsigned int filesize, file_segments;
  filesize = read_file_to_memory (path, &buffer);
  //show_file_buffer (buffer, filesize);

  //printf ("%d \n", PROFILE_SEGMENT_SIZE);
  free (buffer);
  return 0;
}
