#include <stdlib.h>
#include <stdio.h>
#include <tools/vsx_req.h>

size_t f_get_size(FILE* handle)
{
  long prev_pos = ftell(handle);
  fseek( handle, 0, SEEK_END);
  long size = ftell(handle);
  fseek( handle, prev_pos, SEEK_SET);
  return size;
}

int main(int argc,char* argv[])
{
  reqrv(argc == 3, 1);
  printf("Converting %s from RAW to WAV (%s)\n", argv[1], argv[2]);
  FILE* ip = fopen(argv[1], "rb");
  FILE* op = fopen(argv[2], "wb");

  size_t header_size = 0x2c;
  u_int32_t data_size = f_get_size(ip);
  u_int32_t write_size = header_size + data_size - 8;

  char* source_data = (char*)malloc(data_size);
  size_t bytes_read = fread(source_data, 1, data_size, ip);
  printf("Read %lx bytes from source data...\n", bytes_read);

  char a;

#define writechar(c) \
  a = c; \
  fwrite(&a, 1, 1, op)

  writechar('R');
  writechar('I');
  writechar('F');
  writechar('F');
  fwrite(&write_size,sizeof(u_int32_t), 1, op);
  writechar('W');
  writechar('A');
  writechar('V');
  writechar('E');
  writechar('f');
  writechar('m');
  writechar('t');
  writechar(0x20);
  writechar(0x10);
  writechar(0x00);
  writechar(0x00);
  writechar(0x00);
  writechar(0x01);
  writechar(0x00);
  writechar(0x01);
  writechar(0x00);
  writechar(0x44);
  writechar(0xac);
  writechar(0x00);
  writechar(0x00);
  writechar(0x88);
  writechar(0x58);
  writechar(0x01);
  writechar(0x00);
  writechar(0x02);
  writechar(0x00);
  writechar(0x10);
  writechar(0x00);
  writechar('d');
  writechar('a');
  writechar('t');
  writechar('a');
  fwrite(&data_size, sizeof(u_int32_t), 1, op);

  fwrite(source_data, 1, data_size, op);


  fclose(ip);
  fclose(op);

  return 0;
}
