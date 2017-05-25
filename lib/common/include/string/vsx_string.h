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

#pragma once

#include <container/vsx_nw_vector.h>
#include <cstring>
#include <ctype.h>
#include <vsx_platform.h>
#include <wchar.h>
#include <tools/vsx_foreach.h>
#include <iomanip>

 
template<typename W = char> 
class vsx_string
{
  mutable vsx_nw_vector<W> data;

  // deal with the terminating 0 character
  inline bool zero_test() const VSX_ALWAYS_INLINE
  {
    if (!data.size())
      return false;
    return (data[data.get_used()-1]) == 0;
  }

  inline void zero_add() const VSX_ALWAYS_INLINE
  {
    if (!data.size())
      data.push_back((W)0);
    else
      if (!zero_test())
        data.push_back((W)0);
  }

  inline void zero_remove() const VSX_ALWAYS_INLINE
  {
    if (data.size())
      if (!data[data.size()-1])
        data.reset_used(data.size()-1);
  }

public:

  inline W* get_pointer() const VSX_ALWAYS_INLINE
  {
  	return data.get_pointer();
  }

  inline void allocate(size_t size)
  {
    req(size);
    data.allocate(size - 1);
  }

  void reset_used()
  {
    data.reset_used();
  }

  inline const W* c_str() const VSX_ALWAYS_INLINE
  {
    zero_add();
    return data.get_pointer();
  }
  
  inline void push_back(const W p) VSX_ALWAYS_INLINE
  {
    zero_remove();
    data.push_back(p);
  }

  inline void push_back_(const W p) VSX_ALWAYS_INLINE
  {
    data.push_back(p);
  }

  inline W pop_back() const VSX_ALWAYS_INLINE
  {
    zero_remove();

    if (!data.size())
      return 0;

    W f = data[data.size()-1];
    data.reset_used(data.size()-1);
    return f;
  }

  inline size_t size() const VSX_ALWAYS_INLINE
  {
    if (!data.size())
      return 0;

    if (data[data.get_used()-1])
      return data.get_used();

    return data.get_used()-1;
  }

  inline void clear() VSX_ALWAYS_INLINE
  {
    data.reset_used();
  }

  inline void empty() VSX_ALWAYS_INLINE
  {
    data.reset_used();
  }

  void set_volatile_data(W* data_pointer, size_t length)
  {
    data.set_volatile();
    data.set_data(data_pointer, length);
  }

  W& operator[](size_t index) const VSX_ALWAYS_INLINE
  {
    return data[index];
  }

  void make_lowercase() VSX_ALWAYS_INLINE
  {
    foreach(data, i)
      data[i] = (char)tolower(data[i]);
	}
  
  // constructors
  vsx_string<W>()
  :
  data()
  {
  }

//----------------------------------------------------------------------------
// OTHER vsx_string<W>REFERENCE
//----------------------------------------------------------------------------
 
  vsx_string<W> (const vsx_string<W>& other_string)
  :
  data()
  {
    *this = other_string;
  }

//----------------------------------------------------------------------------
// NULLTERMINATED *
//----------------------------------------------------------------------------
  vsx_string<W> (const W* other_string)
  :
  data()
  {
    *this = other_string;
  }

  vsx_string<W>(const W* other_string, size_t max_len)
  :
  data()
  {
    for (size_t i = 0; i < max_len; i++)
    {
      push_back( other_string[i] );
    }
    zero_add();
  }

  ~vsx_string<W>()
  {

  }

  inline const vsx_string<W>& operator=(const vsx_string<W>& other_string) VSX_ALWAYS_INLINE
  {
    data.clear();
    size_t other_string_size = other_string.size();
    if (other_string_size > 0)
      data[other_string_size-1] = 0;
    W* dp = data.get_pointer();
    W* sp = other_string.get_pointer();
    for (size_t i = 0; i < other_string_size; ++i)
      dp[i] = sp[i];
    return *this;
  }

  inline const vsx_string<W>operator+(const vsx_string<W>& right) const VSX_ALWAYS_INLINE
  {
    vsx_string<W>n;
    unsigned long i = 0;
    while (i < data.get_used()) {
      if (data[i] != 0) n.push_back_(data[i]);
      ++i;
    }
    for (i = 0; i < (unsigned long)right.size(); ++i)
    {
      n.push_back_(right[i]);
    }
    return n;
  }

  inline const vsx_string<W>& operator+=(const vsx_string<W>& right) VSX_ALWAYS_INLINE
  {
    zero_remove();
    size_t i = 0;
    while (i < right.size()) {
      data.push_back(right[i]);
      ++i;
    }
    return *this;
  }


  inline const vsx_string<W>& operator=(const W* ss) VSX_ALWAYS_INLINE
  {
    data.clear();
    W* si = (W*)ss;
    while (si && *si != 0) {
      data.push_back(*si);
      ++si;
    }
    return *this;
  }


  inline const vsx_string<W>operator+(const W* const right) const VSX_ALWAYS_INLINE
  {
    vsx_string<W>n;
    unsigned long i = 0;
    while (i < data.get_used() && data[i] != 0) {
      n.push_back_(data[i]);
      ++i;
    }
    i = 0;
    while (right[i] != 0) {
      n.push_back_(right[i]);
      ++i;
    }
    return n;
  }


  const vsx_string<W>& operator+=(const W* right) VSX_ALWAYS_INLINE
  {
    zero_remove();
    int i = 0;
    while (right[i] != 0)
    {
      data.push_back(right[i]);
      ++i;
    }
    return *this;
  }


//----------------------------------------------------------------------------
// SINGLE
//----------------------------------------------------------------------------

  inline vsx_string<W>(const W& ss) VSX_ALWAYS_INLINE
  :
  data()
  {
    *this = ss;
  }

  inline const vsx_string<W>& operator=(const W& ss) VSX_ALWAYS_INLINE
  {
    data.clear();
    data.push_back(ss);
    return *this;
  }

  inline vsx_string<W>& operator=(vsx_string<W>&& other) VSX_ALWAYS_INLINE
  {
    data = std::move(other.data);
    return *this;
  }


  inline const vsx_string<W>operator+(const W& right) const VSX_ALWAYS_INLINE
  {
    vsx_string<W>n = *this;
    n.zero_remove();
    n.push_back_(right);
    return n;
  }

  inline const vsx_string<W>& operator+=(const W& right) VSX_ALWAYS_INLINE
  {
    push_back(right);
    return *this;
  }
  
  inline friend const vsx_string<W>operator+(const W* left, const vsx_string<W>& right) VSX_ALWAYS_INLINE
  {
    vsx_string<W>n;
    size_t i = 0;
    while (left[i] != 0)
    {
      n.push_back_(left[i]);
      ++i;
    }
    i = 0;
    while (i < right.size()) {
      n.push_back_(right[i]);
      ++i;
    }
    return n;
  }

  inline static bool s_equals(const vsx_string<W>& left, const vsx_string<W>& right)
  {
    if (right.size() != left.size())
      return false;

    W* op = right.get_pointer();
    W* ip = left.get_pointer();

    for (size_t i = 0; i < left.size(); i++)
    {
      if (*op != *ip)
        return false;

      op++;
      ip++;
    }

    return true;
  }


  inline friend bool operator==(const vsx_string<W>& left, const vsx_string<W>& right)
  {
    return vsx_string<W>::s_equals(left, right);
  }

  inline friend bool operator<(const vsx_string<W>& left,const vsx_string<W>& right) VSX_ALWAYS_INLINE
  {
    return strcmp(left.c_str(), right.c_str())<0;
  }

  inline friend bool operator<=(const vsx_string<W>& left,const vsx_string<W>& right) VSX_ALWAYS_INLINE
  {
    return strcmp(left.c_str(), right.c_str())<=0;
  }

  inline friend bool operator>(const vsx_string<W>& left,const vsx_string<W>& right) VSX_ALWAYS_INLINE
  {
    return strcmp(left.c_str(), right.c_str())>0;
  }

  inline friend bool operator>=(const vsx_string<W>& left,const vsx_string<W>& right) VSX_ALWAYS_INLINE
  {
    return strcmp(left.c_str(), right.c_str())>=0;
  }

  inline friend bool operator!=(const vsx_string<W>& left,const vsx_string<W>& right) VSX_ALWAYS_INLINE
  {
    if (left.size() != right.size())
      return true;

    return !vsx_string<W>::s_equals(left, right);
  }
 
  inline int find(const vsx_string<W>& search, int start = 0) const VSX_ALWAYS_INLINE
  {
    if (search.size() == 0)
      return 0;

    size_t found = 0; // if this is equal to search.size() we're done
    if (start > (int)size())
      return -1;

    if (start < 0)
      return -1;

    for (; start < (int)size(); ++start)
    {
      if (data[start] == search[found])
      {
        ++found;
        if (found == search.size()) 
        {
          int f = start-((int)found-1);
          if (f < 0)
            return 0;
          else
            return f;
        }
      }
      else found = 0;
    }
    return -1;
  }

  /**
   * @brief match_partial Looks for search in the current string
   * @param search
   * @return
   */
  inline bool match_partial(const vsx_string<W>& search) const
  {
    if (search.size() > size())
      return false;

    foreach(search, i)
      if (search[i] != data[i])
        return false;
    return true;
  }

  inline const vsx_string<W> substr(int start, int length = -1) VSX_ALWAYS_INLINE
  {
    vsx_string<W> result;
    zero_remove();

    if (length == -1)
      length = (int)size();

    if (start < 0)
    {
    	int s = -start;
    	start = (int)size()-s;

    	if (length == -1)
        length = s;
    	else
        if (length > s)
          length = s;
    }
    if (start < (int)size())
    {
      while (start < (int)size() && length)
      {
        result.push_back(data[start]);
        ++start;
        --length;
      }
    }
    return result;
  }

  inline void replace(
      vsx_string<W> search,
      vsx_string<W> replace,
      int max_replacements = 0,
      int required_pos = -1)
  {
    zero_remove();

    req(search.size());

    vsx_string<W> n = *this;
    int loc = 1;
    int replacements = 0;
    while ((loc = n.find(search, loc-1)) != -1)
    {
      if (loc <= required_pos || required_pos == -1)
      {
        if (replace.size())
        {
          n = n.substr(0,loc) + replace + n.substr(loc + (int)search.size());
          loc += (int)replace.size();
        }
        else
        {
          n = n.substr(0,loc) + n.substr(loc + (int)search.size());
          ++loc;
        }
      }
      else
      {
        *this = n;
        return;
      }

      if (max_replacements) {
        replacements++;
        if (replacements >= max_replacements)
        {
          *this = n;
          return;
        }
      }
    }
    *this = n;
  }
  
  inline void insert(int pos, W key) VSX_ALWAYS_INLINE
  {
    *this = substr(0,pos) + key + substr(pos);
  }

  inline void erase(int pos, int num) VSX_ALWAYS_INLINE
  {
    *this = substr(0,pos) + substr(pos+num);
  }

  inline void trim_lf() VSX_ALWAYS_INLINE
  {
    req(this->size());
    if ( (*this)[this->size()-1] == 0x0A )
      this->pop_back();

    req(this->size());
    if ( (*this)[this->size()-1] == 0x0D )
      this->pop_back();

    req(this->size());
    if ( (*this)[this->size()-1] == 0x0A )
      this->pop_back();
  }
};

#include "vsx_printf.h"
