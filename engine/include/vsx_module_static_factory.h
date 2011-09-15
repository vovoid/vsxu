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

// This is a bit messy, but saves loads of work when maintaining modules.
// Factory methods are created automatically by cmake, and when loading
// it's fast enough


//vsx module static factory info class

class vsxm_sf_info
{
public:
  vsx_string name;
  void* cm; //create module func
  void* dm; //delete module func
  void* nm; //number of modules
  vsxm_sf_info(vsx_string n_name, void *n_cm, void* n_dm, void* n_nm)
  {
    name = n_name;
    cm = n_cm;
    dm = n_dm;
    nm = n_nm;
  }
};

//vsx module static factory manager class

class vsxm_sf
{
  private:
  vsx_avector<vsxm_sf_info*> modules;
    
  
  public:
    // will fill out internal std::maps with pointers to factory functions
    void vsx_module_static_factory();

    void* dlopen(const char *filename);

    // fake dlsym, returns function pointers to static functions
    void* dlsym(void *handle, const char *symbol);

    // returns a list of factory names to query
    void get_factory_names(std::list<vsx_string>* mfiles);

    vsxm_sf();

};

extern vsxm_sf static_holder;

// WAH HAX
#ifdef VSXU_ENGINE_STATIC
  #define dlopen(a,b) static_holder.dlopen(a)
  #define dlsym(a,b) static_holder.dlsym(a,b)
#endif
