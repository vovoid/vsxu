#ifndef VSX_COMMAND_H
#define VSX_COMMAND_H
#include <vsx_platform.h>
#include <map>
#include <list>
#include <vector>
#include "vsxfst.h"


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
//    cmd_data_bin << t->cmd_data_bin.str();
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

class vsx_command_list {
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
      commands.push_back(t);
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
        commands.push_back(cmd_);
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
        commands.push_front(cmd_);
        return cmd_;
      }
    } else return 0;
    return 0;
  }
	// add a command by specifying command and command data
  VSX_COMMAND_DLLIMPORT void add(vsx_string cmd, vsx_string cmd_data);

  void add(vsx_string cmd, int cmd_data) {
    if (!accept_commands) return;
    vsx_command_s* t = new vsx_command_s;
    t->cmd = cmd;
    /*std::stringstream f;
    f << cmd_data;*///Implementing new conversion functions
    t->cmd_data = i2s(cmd_data);//f.str();
    commands.push_back(t);
  }

  VSX_COMMAND_DLLIMPORT void adds(int tp, vsx_string titl,vsx_string cmd, vsx_string cmd_data);

  VSX_COMMAND_DLLIMPORT void clear(bool del = false);

  vsx_command_s* reset() {
    iter = commands.begin();
    return *iter;
  }

  vsx_command_s* get_cur() {
    if (iter != commands.end()) {
      return *iter;
    } else return 0;
  }
  vsx_command_s* get() {
    if (iter != commands.end()) {
    vsx_command_s* h = *iter;
    ++iter;
    return h;
    }
    else
    return 0;
  }

  bool pop(vsx_command_s **t) {
    if (commands.size()) {
      *t = commands.front();
      commands.pop_front();
      return true;
    }
    //*t = 0;
    return false;
  }
  vsx_command_s *pop() {
    if (commands.size()) {
      vsx_command_s *t = commands.front();
      commands.pop_front();
      return t;
    } else return 0;
  }

  vsx_command_s *pop_back() {
    if (commands.size()) {
      vsx_command_s *t = commands.back();
      commands.pop_back();
      return t;
    } else return 0;
  }
  // loads from file and puts the lines in vsx_command_s::raw.
  // The default is not to parse.
  VSX_COMMAND_DLLIMPORT void load_from_file(vsx_string filename, bool parse = false,int type = 0);
  VSX_COMMAND_DLLIMPORT void save_to_file(vsx_string filename);

  VSX_COMMAND_DLLIMPORT void token_replace(vsx_string search, vsx_string replace);
  VSX_COMMAND_DLLIMPORT void parse();

  void set_type(int new_type);
  int count() {
    return commands.size();
  }
  vsx_command_list() : filesystem(0),accept_commands(1) {}
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

/*
class vsx_command_timing {
  std::map<vsx_string,vsx_command_timing_container*> vsx_command_timing;
  vsx_command_timing_container *t;
  double startt;
  vsx_string cmd;

public:

  void start(vsx_string cmd_) {
    cmd = cmd_;
    startt = ntime();
  }

  double stop() {
    double end = ntime();
    startt = end-startt;
    if (startt == 0) startt = 0.00001;
    t = vsx_command_timing[cmd];
    if (!t) { t = vsx_command_timing[cmd] = new vsx_command_timing_container; }
    t->totaltime+=startt;
    ++t->count;
    t->average = t->totaltime/((double)t->count);
    return startt;
  }

  double average(vsx_string cmd_) {
    vsx_command_timing_container *t = 0;
    t = vsx_command_timing[cmd_];
    if (t) return t->average;
    else return 0.00001;
  }

};
*/


#endif
