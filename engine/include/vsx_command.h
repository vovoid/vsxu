/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSX_COMMAND_H
#define VSX_COMMAND_H
#include <vsx_platform.h>
#include <map>
#include <list>
#include <vector>
#include "vsxfst.h"
#include <pthread.h>


#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSX_COMMAND_DLLIMPORT
#else
  #if defined(VSX_ENG_DLL)
    #define VSX_COMMAND_DLLIMPORT __declspec (dllexport)
  #else 
    #define VSX_COMMAND_DLLIMPORT __declspec (dllimport)
  #endif
#endif


// different types of commands
#define VSX_COMMAND_PARAM_SET 1
#define VSX_COMMAND_MENU 2
#define VSX_COMMAND 3

#define VSX_COMMAND_MAX_ITERATIONS 100
#define VSX_COMMAND_DELETE_ITERATIONS 120
// just to psych the hell out of everyone looking at the code!
#define intarnet int
#define LOL float
#define lamer bool
#define flamewar vsx_string
#define yes true
#define no false

//lamer you = yes;
//intarnet internet = 0xDEADBEEF;
//flamewar = you?"WINDOWS IS BETTER THAN LINUX":"LINUX IS BETTER THAN WINDOWS";
//float www = (LOL)intarnet;


// command specification (container class)
// this is used within the whole system. that is both the client, who has both a vsx_engine and an instance of the gui
// widget system, and in the server.
// it works somewhat the way windows messages works, like "do this"

// Each instance of vsxu program running has a global command processing queue.
// The vsx_command_list is there for this, it acts as a FIFO buffer for commands as well as reading and managing
// possible commands that appear in menus etc.

// cut'n'paste examples:
// commands.adds(VSX_COMMAND_MENU, "Connect to server", "desktop.server.connect","");
//            ^-- adds sys item ^-- type menu

//**********************************************************************************************************************

VSX_COMMAND_DLLIMPORT class vsx_command_s {
  VSX_COMMAND_DLLIMPORT static std::list<vsx_command_s*> garbage_list; // the parts of the command
  VSX_COMMAND_DLLIMPORT static std::list<vsx_command_s*>::iterator it;
  VSX_COMMAND_DLLIMPORT static std::list<vsx_command_s*>::iterator it_2;
public:
  VSX_COMMAND_DLLIMPORT void process_garbage();
  std::list<vsx_command_s*>* garbage_pointer;
  VSX_COMMAND_DLLIMPORT static int id;
  bool parsed;
  int owner; // for color-coding this command
  int type; // type of command
  int iterations;
  vsx_string title; // Title - for internal GUI stuff like menus and stuff
  vsx_string cmd; // the first part of the command, the actual command
  vsx_string cmd_data; // the second parameter (for simple commands)
  vsx_avector<char> cmd_data_bin; // the binary part of the command
  vsx_string raw; // the unparsed command, empty when binary command
  std::vector <vsx_string> parts; // the parts of the command


  void copy (vsx_command_s *t) {
    owner = t->owner;
    type = t->type;
    title = t->title;
    cmd = t->cmd;
    cmd_data = t->cmd_data;
    raw = t->raw;
    parts = t->parts;
    iterations = t->iterations;
  }
  vsx_string str() {
    if (raw.size())
    return raw; else
    return cmd + " " + cmd_data;
  }
  VSX_COMMAND_DLLIMPORT void parse();
  vsx_command_s() {
    parsed = false;
    type = 0;
    iterations = 0;
    ++id;
    //#ifndef VSX_CMD_GARBAGE_DISABLED
    garbage_list.push_back(this);
    garbage_pointer = &garbage_list;
    //#endif
  }

  // returns a string like "part1 part2 part3" if start was 1 and end was 3
  VSX_COMMAND_DLLIMPORT vsx_string get_parts(int start = 0, int end = -1);

  void dump_to_stdout()
  {
  	for (size_t i = 0; i < parts.size(); i++)
  	printf("%s ",parts[i].c_str());
  	printf("\n");
  }

  VSX_COMMAND_DLLIMPORT ~vsx_command_s();
};

VSX_COMMAND_DLLIMPORT vsx_command_s* vsx_command_parse(vsx_string& cmd_raw);

// thread safety notice:
//  an instance of this class shouldn't be shared among more than 2 threads hence it's a simple mutex
//  combined with provider/consumer FIFO or LIFO buffer (pop/push, pop_front/push_front)
class vsx_command_list {
  int mutex; // thread safety, 1 = locked, 0 = unlocked, ready to lock
  pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

  void get_lock() {
    pthread_mutex_lock( &mutex1 );
  }
  void release_lock() {
    pthread_mutex_unlock( &mutex1 );
  }
 
public:
#ifdef VSX_ENG_DLL
  vsxf* filesystem;
#else
  void* filesystem;
#endif
  int accept_commands;  // 1 accepts, 0 won't accept
  std::list <vsx_command_s*> commands; // results of commands
  std::list <vsx_command_s*>::const_iterator iter;

  // add copy of command at the end of the list
  vsx_command_s* addc(vsx_command_s *cmd) {
    if (!accept_commands) return 0;
    if (cmd->iterations < VSX_COMMAND_MAX_ITERATIONS) {
      ++cmd->iterations;
      vsx_command_s *t = new vsx_command_s;
      t->copy(cmd);
      get_lock();
        commands.push_back(t);
      release_lock();
      return t;
    }
    return 0;
  }
  // add copy of command at the beginning of the list
  bool addc_front(vsx_command_s *cmd) {
    if (!accept_commands) return 0;
    if (cmd->iterations < VSX_COMMAND_MAX_ITERATIONS) {
      ++cmd->iterations;
      vsx_command_s *t = new vsx_command_s;
      t->copy(cmd);
      add_front(t);
    }
    return true;
  }
	// add & parse a command to the end of the list
  vsx_command_s* add_raw(vsx_string r) {
    if (!accept_commands) return 0;
    return add(vsx_command_parse(r));
  }
	// add & parse a command to the beginning of the list
  vsx_command_s* add_raw_front(vsx_string r) {
    if (!accept_commands) return 0;
    return add_front(vsx_command_parse(r));
  }
	// add a command by pointer to the end of the list
  vsx_command_s* add(vsx_command_s* cmd_) {
    if (!accept_commands) return 0;
    if (cmd_) {
      if (cmd_->iterations < VSX_COMMAND_MAX_ITERATIONS) {
        ++cmd_->iterations;
        get_lock();
          commands.push_back(cmd_);
        release_lock();
        return cmd_;
      }
    } else return 0;
    return 0;
  }
	// add a command by pointer to the beginning of the list
  vsx_command_s* add_front(vsx_command_s* cmd_) {
    if (!accept_commands) return 0;
    if (cmd_) {
      if (cmd_->iterations < VSX_COMMAND_MAX_ITERATIONS) {
        ++cmd_->iterations;
        get_lock();
          commands.push_front(cmd_);
        release_lock();
        return cmd_;
      }
    } else return 0;
    return 0;
  }
	// add a command by specifying command and command data
  VSX_COMMAND_DLLIMPORT void add(vsx_string cmd, vsx_string cmd_data);

  // adds a command
  // Thread safety: YES

  void add(vsx_string cmd, int cmd_data) {
    if (!accept_commands) return;
    vsx_command_s* t = new vsx_command_s;
    t->cmd = cmd;
    t->cmd_data = i2s(cmd_data);//f.str();
    get_lock();
      commands.push_back(t);
    release_lock();
  }

  VSX_COMMAND_DLLIMPORT void adds(int tp, vsx_string titl,vsx_string cmd, vsx_string cmd_data);

  VSX_COMMAND_DLLIMPORT void clear(bool del = false);

  vsx_command_s* reset() {
    //printf("reset command list %p\n", this);
    get_lock();
      iter = commands.begin();
    release_lock();
    return *iter;
  }

  // gets the current command from internal iterator
  // Thread safety: NO
  vsx_command_s* get_cur() {
    if (iter != commands.end()) {
      return *iter;
    } else return 0;
  }
  
  // gets the current command from internal iterator and advancing iterator
  // Thread safety: NO
  vsx_command_s* get() {
    if (iter != commands.end()) {
    vsx_command_s* h = *iter;
    ++iter;
    return h;
    }
    else
    return 0;
  }

  // returns and removes the command first in the list
  // Thread safety: YES
  bool pop(vsx_command_s **t) {
    get_lock();
    if (commands.size()) {
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
  vsx_command_s *pop() {
    get_lock();
    if (commands.size()) {
      vsx_command_s *t = commands.front();
      commands.pop_front();
      release_lock();
      return t;
    }
    release_lock();
    return 0;
  }

  // returns and removes the command last in the list
  // Thread safety: YES
  vsx_command_s *pop_back() {
    get_lock();
    if (commands.size()) {
      vsx_command_s *t = commands.back();
      commands.pop_back();
      release_lock();
      return t;
    }
    release_lock();
    return 0;
  }

  // loads from file and puts the lines in vsx_command_s::raw.
  // The default is not to parse.
  // Thread safety: NO
  VSX_COMMAND_DLLIMPORT void load_from_file(vsx_string filename, bool parse = false,int type = 0);
  // Thread safety: NO
  VSX_COMMAND_DLLIMPORT void save_to_file(vsx_string filename);

  // Thread safety: NO
  VSX_COMMAND_DLLIMPORT void token_replace(vsx_string search, vsx_string replace);
  // Thread safety: NO
  VSX_COMMAND_DLLIMPORT void parse();

  void set_type(int new_type);

  // Thread safety: YES
  int count() {
    get_lock();
    int j = commands.size();
    release_lock();
    return j;
  }
  VSX_COMMAND_DLLIMPORT vsx_command_list();
  ~vsx_command_list()
  {
    //for (std::list <vsx_command_s*>::iterator it = commands.begin(); it != commands.end(); ++it) {
     //delete *it;
      //*it = 0;
    //}
  };
};


// timing the vsx commands

class vsx_command_timing_container {
public:
    double totaltime; // in milliseconds
    int count; // divider
    double average; // keeping the average for this entry
    vsx_command_timing_container() : totaltime(0), count(1) {}
};

double ntime();

#endif
