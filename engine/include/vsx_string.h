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

// Vovoid Light String Class
// (c) 2005 Vovoid Software & Multimedia, All rights reserved

#ifndef VSX_STRING_H
#define VSX_STRING_H

#include "vsx_avector.h"
#include <cstring>
#include <ctype.h>
 
class vsx_string {
  mutable vsx_avector<char> data;

  // deal with the terminating 0 character
  bool zero_test() const {
    return (data[data.get_used()-1]);
  }
  void zero_add() const {
    //printf("data_get_used: %d\n",data.get_used());
    if (!data.size()) {
      data.push_back((char)0);
    } else
    if (zero_test())
    data.push_back((char)0);
  }
public:

  char* get_pointer() const
  {
  	return data.get_pointer();
  }
  void zero_remove() const {
    if (data.size())
    if (!data[data.size()-1])
    data.reset_used(data.size()-1);
  }
  // data manipulation
  const char* c_str() const {
    zero_add();
    return data.get_pointer();
  }
  
  char* c_copy() const {
  	zero_add();
  	char* n = new char[this->size()+1];
  	for (size_t i = 0; i <= this->size(); i++) {
  		n[i] = data[i];
  	}
  	return n;
  }
  /*const char* get_clean() const {
    zero_remove();
    return data.get_pointer();
  }*/
  inline void push_back(const char p) {
    zero_remove();
    data.push_back(p);
  }

  inline void push_back_(const char p) {
    data.push_back(p);
  }

  char pop_back() const {
    zero_remove();
    if (data.size()) {
      char f = data[data.size()-1];
      data.reset_used(data.size()-1);
      return f;
    }
    return 0;
  }

  size_t size() const {
    if (!data.size()) return 0;
    if (data[data.get_used()-1])
    return data.get_used(); else
    return data.get_used()-1; 
  }

  char& operator[](int index) const {
  	if (index < 0) index = 0;
    return data[index];
  }

	void make_lowercase() {
		for (unsigned long i = 0; i < data.size(); ++i) {
			data[i] = tolower(data[i]);
		}
	}
  
  // constructors
  vsx_string() {};

//----------------------------------------------------------------------------
// OTHER VSX_STRING REFERENCE
//----------------------------------------------------------------------------
 
  vsx_string(const vsx_string& foo) {
//    printf("copy constructor\n");
    *this = foo;
  };
//#

//----------------------------------------------------------------------------
// NULLTERMINATED CHAR*
//----------------------------------------------------------------------------
  vsx_string(const char* ss) {
    *this = ss;
  };

  const vsx_string& operator=(const vsx_string& ss) {
    if (&ss != this) {
      //printf("copying with =\n");
      data.clear();
      int sss = ss.size();
      if (sss > 0)
      data[sss-1] = 0;
			char* dp = data.get_pointer();
			char* sp = ss.get_pointer();
      for (int i = 0; i < sss; ++i) {
        //printf("%d+",ss[i]);
        dp[i] = sp[i];
      }
      return *this;
    }
    return *this;
  };
//#
  const vsx_string operator+(const vsx_string& right) const {
    //printf("a1\n");
    vsx_string n;
    unsigned long i = 0;
    while (i < data.get_used()) {
      if (data[i] != 0) n.push_back_(data[i]);
      ++i;
    }
    //right.zero_remove();
    //i = 0;
    //while (i < right.size()) {
//      n.push_back_(right[i]);
//      ++i;
//    }
    
    for (i = 0; i < (unsigned long)right.size(); ++i) {
      n.push_back_(right[i]);
    }
    return n;
  };
//#
  const vsx_string& operator+=(const vsx_string& right) {
    //printf("a5\n");
    zero_remove();
    size_t i = 0;
    while (i < right.size()) {
      data.push_back(right[i]);
      ++i;
    }
    return *this;
  };

//#
//#ifndef VSX_STRING_TINY
  const vsx_string& operator=(const char* ss) {
    data.clear();
    while (ss && *ss != 0) {
      data.push_back(*ss);
      ++ss;
    }
    return *this;
  };
//#
  const vsx_string operator+(const char* const right) const {
    //printf("a2\n");
    vsx_string n;
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
  };
//#endif
//#
  const vsx_string& operator+=(const char* right) {
    //printf("a4\n");
    zero_remove();
    int i = 0;
    while (right[i] != 0) {
      data.push_back(right[i]);
      ++i;
    }
    return *this;
  };


//----------------------------------------------------------------------------
// SINGLE CHAR
//----------------------------------------------------------------------------

  vsx_string(const char& ss) {
    *this = ss;
  };
//#
  const vsx_string& operator=(const char& ss) {
    data.clear();
    data.push_back(ss);
    return *this;
  };
//#
  const vsx_string operator+(const char& right) const {
    vsx_string n = *this;
    n.zero_remove();
    n.push_back_(right);
    return n;
  }
//#
  const vsx_string& operator+=(const char& right) {
    //printf("a6\n");
    push_back(right);
    return *this;
  };
  
  friend const vsx_string operator+(const char* left, const vsx_string& right) {
    vsx_string n;
    size_t i = 0;
    while (left[i] != 0) {
      n.push_back_(left[i]);
      ++i;
    }
    i = 0;
    while (i < right.size()) {
      n.push_back_(right[i]);
      ++i;
    }
    return n;
  };

  friend int operator==(const vsx_string& left,const vsx_string& right) {
    if (left.size() != right.size()) return 0;
    return !strcmp(left.c_str(), right.c_str());
  }

  friend int operator<(const vsx_string& left,const vsx_string& right) {
    return strcmp(left.c_str(), right.c_str())<0;
  }

  friend int operator<=(const vsx_string& left,const vsx_string& right) {
    return strcmp(left.c_str(), right.c_str())<=0;
  }

  friend int operator>(const vsx_string& left,const vsx_string& right) {
    return strcmp(left.c_str(), right.c_str())>0;
  }

  friend int operator>=(const vsx_string& left,const vsx_string& right) {
    return strcmp(left.c_str(), right.c_str())>=0;
  }

  friend int operator!=(const vsx_string& left,const vsx_string& right) {
    if (left.size() != right.size()) return 1;
    return strcmp(left.c_str(), right.c_str());
  }
 
  int find(const vsx_string& search, int start = 0) const {
    if (search.size() == 0) return 0;
    size_t found = 0; // if this is equal to search.size() we're done
    if (start > (int)size()) return -1;
		if (start < 0) return -1;
    //printf("search_a size: %d\n",size());
    //printf("search_a text: %s\n",search.c_str());
    for (; start < (int)size(); ++start) {
      if (data[start] == search[found]) {
      //printf("start: %d\n",start);
        ++found;
        if (found == search.size()) 
        {
          int f = start-(found-1);
          if (f < 0) return 0; else
          return f;
        }
      } else found = 0;
    }
    return -1;
  }

  const vsx_string substr(int start, int length = -1) {
    vsx_string n;
    zero_remove();
    if (length == -1) length = size();
    if (start < 0) {
    	int s = -start;
    	start = size()-s;
    	if (length == -1)
    	length = s;
    	else
    	if (length > s) length = s;
    }
    if (start < (int)size()) {
      while (start < (int)size() && length) {
        n.push_back(data[start]);
        ++start;
        --length;
      }
    }
    return n;
  }
  
  void insert(int pos, char key) {
    //vsx_string n;
    *this = substr(0,pos) + key + substr(pos);
    //return n;
  }

  void erase(int pos, int num) {
    //vsx_string n;
    *this = substr(0,pos) + substr(pos+num);
    //return n;
  }

};


#endif
