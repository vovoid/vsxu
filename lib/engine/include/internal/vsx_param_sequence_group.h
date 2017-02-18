#pragma once

#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>

class vsx_param_sequence_group
{
  vsx_nw_vector< vsx_string<> > component_params_list;

  int get_index_by_name(vsx_string<>& name)
  {
    foreach(component_params_list, i)
      if (component_params_list[i] == name)
        return (int)i;
    return -1;
  }

public:

  void add(vsx_string<> name)
  {
    foreach (component_params_list, i)
    {
      if (component_params_list[i] == "_")
      {
        component_params_list[i] = name;
        return;
      }
      if (component_params_list[i] == name)
        return;
    }
    component_params_list.push_back(name);
  }

  void remove(vsx_string<> name)
  {
    int index = get_index_by_name(name);
    req(index >= 0);
    component_params_list[(size_t)index] = "_";
  }

  void clear()
  {
    component_params_list.clear();
  }

  vsx_string<> dump()
  {
    return vsx_string_helper::implode_single<char>( component_params_list, '*' );
  }

  void load(vsx_string<>& list)
  {
    component_params_list.clear();
    vsx_string_helper::explode_single(list, '*', component_params_list);
  }

};
