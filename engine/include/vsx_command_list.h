#ifndef VSX_COMMAND_LIST_H
#define VSX_COMMAND_LIST_H

#include <vsx_platform.h>
#include <map>
#include <list>
#include <vector>

#include "vsx_command.h"
#include <pthread.h>

// thread safety notice:
//  an instance of this class shouldn't be shared among more than 2 threads hence it's a simple mutex
//  combined with provider/consumer FIFO or LIFO buffer (pop/push, pop_front/push_front)



template<class T>
class vsx_command_buffer_broker
{
  pthread_mutex_t mutex1;

  void get_lock()
  {
    pthread_mutex_lock( &mutex1 );
  }

  void release_lock()
  {
    pthread_mutex_unlock( &mutex1 );
  }

  vsxf* filesystem;

  int accept_commands;  // 1 accepts, 0 won't accept
  typename std::list <T*> commands; // results of commands
  typename std::list <T*>::const_iterator iter;


public:

  void set_filesystem(vsxf* new_filesystem)
  {
    filesystem = new_filesystem;
  }

  void set_accept_commands(int new_value)
  {
    accept_commands = new_value;
  }



  // add copy of command at the end of the list
  T* addc(T* cmd)
  {
    if (!accept_commands)
      return 0;

    if (cmd->iterations >= VSX_COMMAND_MAX_ITERATIONS)
      return 0;

    ++cmd->iterations;

    // make a copy of the command
    T *t = new T;
    t->copy(cmd);

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

    if (cmd->iterations >= VSX_COMMAND_MAX_ITERATIONS)
      return 0;

    // make a copy of the command
    ++cmd->iterations;
    T *t = new T;
    t->copy(cmd);

    // add to front
    add_front(t);

    return true;
  }


  // add & parse a command to the end of the list
  T* add_raw(vsx_string r)
  {
    if (!accept_commands)
      return 0;

    return
      add
      (
        vsx_command_parse
        (
          r
        )
      )
    ;
  }



  // add & parse a command to the beginning of the list
  T* add_raw_front(vsx_string r)
  {
    if (!accept_commands)
      return 0;

    return
      add_front
      (
        vsx_command_parse
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

    if (cmd->iterations >= VSX_COMMAND_MAX_ITERATIONS)
      return 0;

    ++cmd->iterations;

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

    if (cmd->iterations >= VSX_COMMAND_MAX_ITERATIONS)
      return 0;

    ++cmd->iterations;

    get_lock();
      commands.push_front(cmd);
    release_lock();

    return cmd;
  }



  // add a command by specifying command and command data
  void add(vsx_string cmd, vsx_string cmd_data)
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
  void add(vsx_string cmd, int cmd_data)
  {
    if (!accept_commands)
      return;

    T* t = new T;
    t->cmd = cmd;
    t->cmd_data = i2s(cmd_data);
    get_lock();
      commands.push_back(t);
    release_lock();
  }


  void adds(int tp, vsx_string title, vsx_string cmd, vsx_string cmd_data)
  {
    if (!accept_commands)
      return;

    T* t = new T;
    t->type = tp;
    t->title = title;
    t->cmd = cmd;
    t->cmd_data = cmd_data;
    t->parts.push_back(cmd);
    vsx_string deli = " ";
    vsx_avector<vsx_string> pp;
    explode(cmd_data,deli,pp);

    for (size_t i = 0; i < pp.size(); ++i)
    {
      t->parts.push_back(pp[i]);
    }

    t->raw = cmd+" "+cmd_data;

    commands.push_back(t);
  }


  void clear(bool del = false)
  {
    commands.clear();

    if (!del)
      return;

    for (typename std::list <T*>::iterator it = commands.begin(); it != commands.end(); ++it)
    {
      (*it)->garbage_pointer->remove(*it);
      delete *it;
    }
  }



  void reset()
  {
    get_lock();
      iter = commands.begin();
    release_lock();
  }


  // gets the current command from internal iterator
  // Thread safety: NO
  T* get_cur()
  {
    if (iter != commands.end())
      return *iter;

    return 0;
  }

  // gets the current command from internal iterator and advancing iterator
  // Thread safety: NO
  T* get()
  {
    if (iter != commands.end())
    {
      T* h = *iter;
      ++iter;
      return h;
    }
    return 0;
  }


  // returns and removes the command first in the list
  // suitable for while loops
  // Thread safety: YES
  bool pop(T **t)
  {
    get_lock();
      if (commands.size())
      {
        *t = commands.front();
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
      T *t = commands.front();
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
      T *t = commands.back();
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
  void load_from_file(vsx_string filename, bool parse = false, int type = 0 )
  {
    if (!filesystem)
    {
      filesystem = new vsxf;
    }

    vsxf_handle* fp;
    if ((fp = filesystem->f_open(filename.c_str(), "r")) == NULL)
      return;

    char buf[65535];
    vsx_string line;
    while (filesystem->f_gets((char*)&buf,65535,fp))
    {
      line = buf;
      line.trim_lf();

      if (!line.size())
        continue;

      if (parse)
      {
        add_raw(line);
        (*commands.back()).type = type;
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
  void save_to_file(vsx_string filename)
  {
    if (!filesystem)
      filesystem = new vsxf;

    vsxf_handle* fp;
    if ((fp = filesystem->f_open(filename.c_str(), "w")) == NULL)
      return;

    for (typename std::list <T*>::iterator it = commands.begin(); it != commands.end(); ++it)
    {
      filesystem->f_puts(
        (
          (*it)->raw + vsx_string("\n")
        ).c_str(),
        fp
      );
    }
    filesystem->f_close(fp);
  }



  // Thread safety: NO
  void token_replace(vsx_string search, vsx_string replace)
  {
    for (typename std::list <T*>::iterator it = commands.begin(); it != commands.end(); ++it)
    {
      if ((*it)->parsed)
      {
        for (size_t i = 0; i < (*it)->parts.size(); ++i)
        {
          (*it)->parts[i] = str_replace(search, replace, (*it)->parts[i]);
        }
        (*it)->raw = str_replace(search, replace, (*it)->raw);
        continue;
      }
      (*it)->raw = str_replace(search, replace, (*it)->raw);
    }
  }



  // Thread safety: NO
  void parse()
  {
    for (typename std::list <T*>::iterator it = commands.begin(); it != commands.end(); ++it)
    {
      (*it)->parse();
    }
  }

  void set_type(int new_type)
  {
    for (typename std::list <T*>::iterator it = commands.begin(); it != commands.end(); ++it)
    {
      (*it)->type = new_type;
    }
  }



  // Thread safety: YES
  size_t count()
  {
    get_lock();
      size_t j = commands.size();
    release_lock();
    return j;
  }

  vsx_command_buffer_broker()
    :
    filesystem(0),
    accept_commands(1)
  {
    pthread_mutex_init(&mutex1, NULL);
  }

};



typedef vsx_command_buffer_broker<vsx_command_s> vsx_command_list;



#endif // VSX_COMMAND_LIST_H
