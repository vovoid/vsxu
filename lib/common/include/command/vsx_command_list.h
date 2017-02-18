#ifndef VSX_COMMAND_LIST_H
#define VSX_COMMAND_LIST_H

#include <vsx_platform.h>
#include <map>
#include <list>
#include <vector>
#include <container/vsx_ma_vector.h>

#include "vsx_command.h"

// thread safety notice:
//  an instance of this class shouldn't be shared among more than 2 threads hence it's a simple mutex
//  combined with provider/consumer FIFO or LIFO buffer (pop/push, pop_front/push_front)

template<class T>
class vsx_command_buffer_broker
{
  vsx_lock lock;

  void get_lock()
  {
    lock.aquire();
  }

  void release_lock()
  {
    lock.release();
  }

  vsx::filesystem* filesystem = 0x0;

  int accept_commands = 1;  // 1 accepts, 0 won't accept
  vsx_nw_vector <T*> commands; // results of commands
  size_t commands_iterator = 0;
  bool delete_commands_on_delete = false;

public:

  void set_filesystem(vsx::filesystem* new_filesystem)
  {
    filesystem = new_filesystem;
  }

  void set_accept_commands(int new_value)
  {
    accept_commands = new_value;
  } 

  // add copy of command at the end of the list
  T* addc(T* cmd, bool garbage_collect = false)
  {
    if (!accept_commands)
      return 0;

    // make a copy of the command
    T *t = new T;
    t->copy(cmd);

    if (garbage_collect)
      t->gc();

    get_lock();
      commands.push_back(t);
    release_lock();
    return t;
  }


  // add copy of command at the beginning of the list
  bool addc_front(T *cmd)
  {
    if (!accept_commands)
      return 0;

    T *t = new T;
    t->copy(cmd);

    // add to front
    add_front(t);

    return true;
  }


  // add & parse a command to the end of the list
  T* add_raw(vsx_string<>r, bool garbage_collect = false)
  {
    if (!accept_commands)
      return 0;

    return
      add
      (
        vsx_command_parse<T>
        (
          r, garbage_collect
        )
      )
    ;
  }



  // add & parse a command to the beginning of the list
  T* add_raw_front(vsx_string<>r)
  {
    if (!accept_commands)
      return 0;

    return
      add_front
      (
        vsx_command_parse<T>
        (
          r
        )
      )
    ;
  }



  // add a command by pointer to the end of the list
  T* add(T* cmd)
  {
    if (!accept_commands)
      return 0;

    if (!cmd)
      return 0;

    get_lock();
      commands.push_back(cmd);
    release_lock();

    return cmd;
  }



  // add a command by pointer to the beginning of the list
  T* add_front(T* cmd)
  {
    if (!accept_commands)
      return 0;

    if (!cmd)
      return 0;

    get_lock();
      commands.push_front(cmd);
    release_lock();

    return cmd;
  }



  // add a command by specifying command and command data
  void add(vsx_string<>cmd, vsx_string<>cmd_data)
  {
    if (!accept_commands)
      return;

    T* t = new T;
    t->cmd = cmd;
    t->cmd_data = cmd_data;
    t->parts.push_back(cmd);
    t->parts.push_back(cmd_data);
    t->raw = cmd+" "+cmd_data;
    commands.push_back(t);
  }


  // adds a command
  // Thread safety: YES
  void add(vsx_string<>cmd, int cmd_data, bool garbage_collect = false)
  {
    if (!accept_commands)
      return;

    T* t = new T;
    t->cmd = cmd;
    t->cmd_data = vsx_string_helper::i2s(cmd_data);

    if (garbage_collect)
      t->gc();

    get_lock();
      commands.push_back(t);
    release_lock();
  }


  void adds(int tp, vsx_string<>title, vsx_string<>cmd, vsx_string<>cmd_data)
  {
    if (!accept_commands)
      return;

    T* t = new T;
    t->type = tp;
    t->title = std::move(title);
    t->cmd = cmd;
    t->cmd_data = std::move(cmd_data);

    t->parts.move_back(std::move(cmd));
    vsx_string<> deli = " ";
    vsx_nw_vector< vsx_string<> > pp;
    vsx_string_helper::explode(t->cmd_data, deli, pp);

    for (size_t i = 0; i < pp.size(); ++i)
      t->parts.move_back(std::move(pp[i]));

    t->raw = t->cmd+" "+t->cmd_data;

    commands.push_back(t);
  }


  void clear_normal()
  {
    commands.clear();
  }

  void clear_delete()
  {
    foreach(commands, i)
      if (!commands[i]->garbage_collected)
        delete commands[i];
    commands.clear();
  }

  void garbage_collect()
  {
    foreach(commands, i)
      commands[i]->gc();
  }

  void reset()
  {
    get_lock();
      commands_iterator = 0;
    release_lock();
  }


  // gets the current command from internal iterator
  // Thread safety: NO
  T* get_cur()
  {
    reqrv(commands.size(), 0x0);
    reqrv(commands_iterator != commands.size(), 0x0);
    return commands[commands_iterator];
  }

  // gets the current command from internal iterator and advancing iterator
  // Thread safety: NO
  T* get()
  {
    reqrv(commands.size(), 0x0);
    reqrv(commands_iterator != commands.size(), 0x0);

    T* h = commands[commands_iterator];
    ++commands_iterator;
    return h;
  }


  // returns and removes the command first in the list
  // suitable for while loops
  // Thread safety: YES
  bool pop(T **t)
  {
    get_lock();
      if (commands.size())
      {
        *t = *commands.front();
        commands.pop_front();
        release_lock();
        return true;
      }
    release_lock();
    return false;
  }



  // returns and removes the command first in the list
  // Thread safety: YES
  T *pop()
  {
    get_lock();
    if (commands.size())
    {
        T* t = *commands.front();
        commands.pop_front();

      release_lock();
      return t;
    }
    release_lock();
    return 0;
  }



  // returns and removes the command last in the list
  // Thread safety: YES
  T *pop_back()
  {
    get_lock();
    if (commands.size())
    {
      T *t = *commands.back();
      commands.pop_back();
      release_lock();
      return t;
    }
    release_lock();
    return 0;
  }



  // loads from file and puts the lines in T::raw.
  // The default is not to parse.
  // Thread safety: NO
  void load_from_file(vsx_string<>filename, bool parse = false, int type = 0 )
  {
    if (!filesystem)
      filesystem = vsx::filesystem::get_instance();

    vsx::file* fp = filesystem->f_open(filename.c_str());
    req(fp);

    char buf[65535];
    vsx_string<>line;
    while (filesystem->f_gets((char*)&buf,65535,fp))
    {
      line = buf;
      line.trim_lf();

      if (!line.size())
        continue;

      if (parse)
      {
        add_raw(line);
        if (commands.back())
          (*(*commands.back())).type = type;
        continue;
      }

      T* t = new T;
      t->raw = line;
      t->type = type;
      commands.push_back(t);
    }
    filesystem->f_close(fp);
  }



  // Thread safety: NO
  vsx_string<> get_as_string()
  {
    vsx_string<> result;

    foreach(commands, i)
      result += commands[i]->raw + "\n";

    return result;
  }



  // Thread safety: NO
  void token_replace(vsx_string<>search, vsx_string<>replace)
  {
    foreach (commands, i)
    {
      T& command = *commands[i];
      if (command.parsed)
      {
        foreach(command.parts, i)
          command.parts[i].replace(search, replace);

          command.raw.replace(search, replace);
        continue;
      }
      command.raw.replace(search, replace);
    }
  }



  // Thread safety: NO
  void parse()
  {
    foreach(commands, i)
      commands[i]->parse();
  }

  void set_type(int new_type)
  {
    foreach(commands, i)
      commands[i]->type = new_type;
  }



  // Thread safety: YES
  size_t count()
  {
    get_lock();
      size_t j = commands.size();
    release_lock();
    return j;
  }

  vsx_command_buffer_broker(bool delete_commands)
  :
  delete_commands_on_delete(delete_commands)
  {
  }

  ~vsx_command_buffer_broker()
  {
    if (delete_commands_on_delete)
      clear_delete();
  }

};



typedef vsx_command_buffer_broker<vsx_command_s> vsx_command_list;



#endif // VSX_COMMAND_LIST_H
