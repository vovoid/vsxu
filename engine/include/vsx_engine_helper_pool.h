/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#ifndef VSX_ENGINE_HELPER_POOL_H
#define VSX_ENGINE_HELPER_POOL_H


template <int num_engines = 64>
class vsx_engine_helper_pool
{
protected:
  vsx_engine_helper* engines[num_engines];
  int engines_idle[num_engines];

  inline bool verify_handle( const int & handle )
  {
    if (handle == -1) return false;
    if (handle >= num_engines) return false;
    return true;
  }

public:

  void init(vsx_string state_name, vsx_module_list_abs* module_list)
  {
    for (size_t i = 0; i < num_engines; i++)
    {
      engines[i] = new vsx_engine_helper(
            state_name,
            module_list
      );
      this->on_create(i);
      engines[i]->render(100.0f);

      engines_idle[i] = 100;
    }
  }



  virtual void on_create(const int &handle)
  {
    (void)handle;
  }

  virtual void on_return_handle(const int &handle)
  {
    (void)handle;
  }

  int get_handle()
  {
    for (size_t i = 0; i < num_engines; i++)
    {
      if (engines_idle[i] > 10)
      {
        on_return_handle(i);
        return i;
      }
    }
    // none available
    printf("WARNING: engine pool underrun!\n");
    return -1;
  }

  virtual void on_render(const int &handle)
  {
    (void)handle;
  }

  virtual void render(const int &handle )
  {
    if (!verify_handle(handle)) return;
    // reset idle counter
    engines_idle[handle] = 0;

    on_render(handle);

    engines[handle]->render(100.0f);
  }

  void per_frame_maintenance()
  {
    for (size_t i = 0; i < num_engines; i++)
    {
      engines_idle[i]++;
    }
  }
};

#endif
