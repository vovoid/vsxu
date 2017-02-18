#pragma once

namespace vsx
{
class filesystem_archive_writer_base
{
public:
  virtual void create(const char* filename) = 0;
  virtual void add_file(vsx_string<> filename, vsx_string<> disk_filename, bool deferred_multithreaded) = 0;
  virtual void add_string(vsx_string<> filename, vsx_string<> payload, bool deferred_multithreaded) = 0;
  virtual void close() = 0;

  virtual ~filesystem_archive_writer_base()
  {}
};
}
