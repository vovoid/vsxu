#pragma once

#include <vsx_font_outline.h>
#include <tools/vsx_managed_singleton.h>

class outline_font_cache
  : public vsx::managed_singleton<outline_font_cache>
{
  vsx_nw_vector< vsx_string<> > names;
  vsx_nw_vector<font_outline_holder*> pointers;

  bool is_name_in_cache(vsx_string<>name)
  {
    for (size_t i = 0; i < names.size(); i++)
      if (names[i] == name)
        return true;
    return false;
  }

  bool is_pointer_in_cache(font_outline_holder* pointer)
  {
    for (size_t i = 0; i < pointers.size(); i++)
      if (pointers[i] == pointer)
        return true;
    return false;
  }

  font_outline_holder* get_pointer_by_name(vsx_string<>name)
  {
    for (size_t i = 0; i < names.size(); i++)
      if (names[i] == name)
        return pointers[i];
    return 0x0;
  }

  font_outline_holder* find_and_bind(vsx_string<>font_file_name)
  {
    if (!is_name_in_cache(font_file_name))
      return 0x0;

    font_outline_holder* font_holder = get_pointer_by_name(font_file_name);
    font_holder->bind();
    return font_holder;
  }

  font_outline_holder* create_and_bind(vsx::filesystem* filesystem, vsx_string<>path, vsx_font_outline_render_type type)
  {
    font_outline_holder* font_holder = new font_outline_holder;

    font_holder->load(filesystem, path, type);
    font_holder->bind();
    names.push_back(path);
    pointers.push_back(font_holder);
    return font_holder;
  }

public:

  font_outline_holder* get_font( vsx::filesystem* filesystem, vsx_string<>path, vsx_font_outline_render_type type)
  {
    font_outline_holder* font_holder = find_and_bind( path );
    if (font_holder)
      return font_holder;

    return create_and_bind(filesystem, path, type);
  }

  void recycle( font_outline_holder* holder )
  {
    if (!is_pointer_in_cache(holder))
      return;

    holder->unbind();
    if (holder->should_be_destroyed())
    {
      names.remove_value( holder->get_file_name() );
      pointers.remove_value( holder );
      delete holder;
    }
  }
};

