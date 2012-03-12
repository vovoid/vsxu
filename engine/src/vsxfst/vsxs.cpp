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

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <io.h>
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#endif

#include "vsxfst.h"



char string_res[256] = "";

int s2i(const vsx_string& in)
{
  return atoi(in.c_str());
}

vsx_string i2s(int in)
{
  sprintf(string_res,"%d",in);
  return vsx_string(string_res);
}

float s2f(const vsx_string& in)
{
  return atof(in.c_str());
}

vsx_string f2s(float in)
{
  sprintf(string_res,"%f",in);
  return vsx_string(string_res);
}

vsx_string f2s(float in, int decimals)
{
  sprintf(string_res,vsx_string("%."+i2s(decimals)+"f").c_str(),in);
  return vsx_string(string_res);
}

bool crlf(char *buffer,int len) {
	bool cr = 0;
	bool lf = 0;
	int i = len-1;
	while (i+1 && (!cr || !lf)) {
		if (!cr) cr=(buffer[i] == 10)?1:0;
		if (!lf) lf=(buffer[i] == 13)?1:0;
		i--;
	}
	return (cr && lf);
}




vsx_string str_pad(const vsx_string& str, const vsx_string& chr, size_t t_len, int pad_type, int overflow_adjust) {
  vsx_string ps = "";
  if (str.size() > t_len) 
  {
    if (overflow_adjust == STR_PAD_OVERFLOW_LEFT) {
      for (size_t i = 0; i < t_len; ++i) {
        ps = vsx_string(str[str.size()-i-1]) + ps;
      }  
      return ps;
    } else
    {
      for (size_t i = 0; i < t_len; ++i) {
        ps += str[i];
      }
      return ps;
    }
  } else  
  {  
    for (size_t i = 0; i < (t_len-str.size()); ++i) {
      ps += chr;
    }
    if (pad_type == STR_PAD_LEFT)
    ps = ps+str;
    else
    ps = str+ps;
    if (ps.size() > t_len) return str_pad(str,chr,t_len,pad_type,overflow_adjust);
    else
    return ps;
  }  
}  

/*const vsx_string str_replace_token_prefix(vsx_string& token, vsx_string& search, vsx_string& replace, vsx_string& subject) {
  //1 explode our vsx_string
  vsx_string deli = token;
  vsx_avector<vsx_string> tokens;
  explode(subject,deli, tokens);
  for (unsigned long i = 0; i < tokens.size(); ++i) {
  //for (std::vector<vsx_string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
    tokens[i] = str_replace(search, replace, tokens[i],1);
  }
  return implode(tokens, token);
} */


vsx_string str_replace(vsx_string search, vsx_string replace, vsx_string subject, int max_replacements, int required_pos) {
  //printf("------------\nsubject coming in: %s\n",subject.c_str());
  vsx_string n = subject;
  if (search == "") return n;
  int loc = 1;
  int replacements = 0;
  while ((loc = n.find(search, loc-1)) != -1) {
//      printf("loc: %d\n",loc);
    if (loc <= required_pos || required_pos == -1) {
//      printf("subject changed 0: %s\n",subject.c_str());
      if (replace.size()) {
        n = n.substr(0,loc) + replace + n.substr(loc+search.size());
        loc += replace.size();
      }
      else
      {
        n = n.substr(0,loc) + n.substr(loc+search.size());
        ++loc;
      }
//        printf("subject changed 1: %s\n",n.c_str());
      
    } else return n; 
    if (max_replacements) {
      replacements++;
      if (replacements >= max_replacements) {
        return n;
      }  
    }  
  }
//  printf("reached end %s\n",n.c_str());
  return n;
}  

const vsx_string str_replace_char_pad(vsx_string search, vsx_string replace, vsx_string subject, vsx_string subject_r, int max_replacements, int required_pos) {
  //printf("------------\nsubject coming in: %s\n",subject.c_str());
  //vsx_string hh = "";
  if (subject.size() != subject_r.size()) return subject_r;
  vsx_string rep;
  for (size_t i = 0; i < search.size(); ++i) rep.push_back(replace[0]);
  vsx_string n;
  //printf("first\n");
  if (search == "") return subject_r;
  int loc = 1;
  int replacements = 0;
  while ((loc = subject.find(search, loc-1)) != -1) {
  if (loc <= required_pos || required_pos == -1) {
 //      printf("loc: %d\n",loc);
      subject = subject.substr(0,loc) + rep + subject.substr(loc+search.size());
      subject_r = subject_r.substr(0,loc) + rep + subject_r.substr(loc+search.size());
         //printf("subject changed 1: %s\n",subject.c_str());
      loc += replace.size();
    } else return subject_r; 
    if (max_replacements) {
      replacements++;
      if (replacements >= max_replacements) {
        return subject_r;
      }  
    }  
  }
  //printf("last\n");
  return subject_r;
}  

int explode(vsx_string& input, vsx_string& delimiter, vsx_avector<vsx_string>& results, int max_parts) {
  results.clear();
  if (input == delimiter) {
    results.push_back(input);
    return 1;
  }
  //printf("splitting vsx_string: %s\n",input.c_str());
  vsx_string res;
  size_t fpos = 0;
  int count = 0;
  char lastchar = 0;
  for (size_t i = 0; i < input.size(); ++i)
  {
    if (input[i] == delimiter[fpos] && lastchar != '\\') {
      ++fpos;
    } else {
      res.push_back(input[i]);
      fpos = 0;
    }
    if (fpos == delimiter.size() || i == input.size()-1)
    {
      fpos = 0;
      results.push_back(res);
      res = "";
      //res.clear();
      ++count;
    }
    lastchar = input[i];
  }
  if (count == 0) results.push_back(input);
  return count;
}

vsx_string implode(vsx_avector<vsx_string>& in,vsx_string& delimiter) {
  if (in.size() == 0) return "";
  if (in.size() == 1) return in[0];
  vsx_string h;
  for (unsigned long i = 0; i < in.size()-1; ++i) {
    h += in[i]+delimiter;
  }
  h += in[in.size()-1];
  return h;
}





















void str_remove_equal_prefix(vsx_string* str1, vsx_string* str2, vsx_string delimiter) {
//#ifndef VSX_NO_CLIENT
  vsx_string to_remove = "";
  vsx_string deli = delimiter;
  std::list<vsx_string> str1_;
  std::list<vsx_string> str2_;
  explode(*str1, deli, str1_);
  explode(*str2, deli, str2_);
  while (str1_.size() && str2_.size() && str1_.front() == str2_.front()) {
    str1_.pop_front();
    str2_.pop_front();
  }
  *str1 = implode(str1_,deli);
  *str2 = implode(str2_,deli);
//#endif
  //while (p < str1->size() && p < str2->size() && (*str1)[p] == (*str2)[p]) {
    //printf("to_remove %s\n",to_remove.c_str());
    //to_remove+=(*str1)[p];
    //++p;
  //}
  //if (to_remove == *str1) *str1 = ""; else
  //*str1 = str_replace(to_remove, "", *str1,1,0);
  //if (to_remove == *str2) *str2 = ""; else
  //*str2 = str_replace(to_remove, "", *str2,1,0);
}  



int split_string(vsx_string& input, vsx_string& delimiter, std::vector<vsx_string>& results, int max_parts) {
  results.clear();
  if (input == delimiter) {
    results.push_back(input);
    return 1;
  }
  //printf("splitting vsx_string: %s\n",input.c_str());
  vsx_string res;
  size_t fpos = 0;
  int count = 0;
  char lastchar = 0;
  for (size_t i = 0; i < input.size(); ++i)
  {
    if (input[i] == delimiter[fpos] && lastchar != '\\')
    {
      ++fpos;
    } else {
      res.push_back(input[i]);
      fpos = 0;
    }
    if (fpos == delimiter.size() || i == input.size()-1)
    {
      fpos = 0;
      results.push_back(res);
      res = "";
      //res.clear();
      ++count;
    }
    lastchar = input[i];
  }
  if (count == 0) results.push_back(input);
  return count;
}
int explode(vsx_string& input, vsx_string& delimiter, std::vector<vsx_string>& results, int max_parts)
{
  return split_string(input, delimiter,results,max_parts);
}  

int split_string(vsx_string& input, vsx_string& delimiter, std::list<vsx_string>& results, int max_parts) {
  if (input == delimiter) {
    results.push_back(input);
    return 1;
  }
  //printf("splitting vsx_string: %s\n",input.c_str());
  vsx_string res;
  size_t fpos = 0;
  int count = 0;
  for (size_t i = 0; i < input.size(); ++i)
  {
    if (input[i] == delimiter[fpos]) {
      ++fpos;
    } else {
      res.push_back(input[i]);
      fpos = 0;
    }
    if (fpos == delimiter.size() || i == input.size()-1)
    {
      fpos = 0;
      results.push_back(res);
      res = "";
      //res.str("");
      //res.clear();
      ++count;
    }
  }
  if (count == 0 && input != "") results.push_back(input);
  return count;
}
int explode(vsx_string& input, vsx_string& delimiter, std::list<vsx_string>& results, int max_parts)
{
  return split_string(input, delimiter,results,max_parts);
}



 
vsx_string implode(std::vector<vsx_string> in,vsx_string delimiter) {
  if (in.size() == 0) return "";
  if (in.size() == 1) return in[0];
  vsx_string h;
  for (std::vector<vsx_string>::iterator it = in.begin(); it != in.end()-1; ++it) {
  //for (int i = 0; i < in.size()-1; ++i) {
    h += *it+delimiter;
//    h += in[i]+delimiter;
  }
//  h += in[in.size()-1];
  h += in.back();
  return h;
}

vsx_string implode(std::list<vsx_string> in,vsx_string delimiter) {
  if (in.size() == 0) return "";
  if (in.size() == 1) return in.front();
  vsx_string h;
  std::list<vsx_string>::iterator it = in.begin();
  h += *it;
  ++it;
  for (; it != in.end(); ++it) {
  //  printf(" imploding on %s\n",(*it).c_str());
    h += delimiter+*it;
  }
//  h += delimiter + in.back();
  return h;
}

bool verify_filesuffix(vsx_string& input, const char* type) {
	std::vector<vsx_string> parts;
	vsx_string deli = ".";
	explode(input,deli,parts);
	if (parts.size()) {
		vsx_string a = parts[parts.size()-1];
		a.make_lowercase();
		vsx_string t = type;
		t.make_lowercase();
		if (t == a) return true;
	}
	return false;
}

#ifndef _WIN32
void get_files_recursive_(vsx_string startpos, std::list<vsx_string>* filenames,vsx_string include_filter,vsx_string exclude_filter);
void get_files_recursive(vsx_string startpos, std::list<vsx_string>* filenames,vsx_string include_filter,vsx_string exclude_filter) {
	get_files_recursive_(startpos,filenames,include_filter,exclude_filter);
	(*filenames).sort();
}
void get_files_recursive_(vsx_string startpos, std::list<vsx_string>* filenames,vsx_string include_filter,vsx_string exclude_filter) {
#else
void get_files_recursive(vsx_string startpos, std::list<vsx_string>* filenames,vsx_string include_filter,vsx_string exclude_filter) {
#endif
	
#ifdef _WIN32
  _finddata_t fdp;
#else
  DIR* dir;
  dirent* dp;
#endif

  long fhandle = 0;
  bool run = true;   
#ifdef _WIN32
  vsx_string fstring = startpos+"/*";
  fhandle=_findfirst(fstring.c_str(),&fdp);
#else
  vsx_string fstring = startpos;
  dir = opendir(startpos.c_str());
  if (!dir) return;
  dp = readdir(dir);
  if (!dp) return;
#endif
  bool exclude;
  bool include;
#ifdef VSXS_DEBUG
  FILE* fp = fopen("get_files_recursive.log","a");
  fprintf(fp,"Starting traversion in : %s\n",fstring.c_str());
#endif
  if (fhandle != -1)
  while (run) {
  vsx_string ss;
#ifdef _WIN32
    ss = fdp.name;
#else
    ss = dp->d_name;
    // stat the file to see if it's a dir or not
    struct stat stbuf;
    vsx_string full_path = fstring + "/" + ss;
    stat(full_path.c_str(), &stbuf);
#endif
#ifdef VSXS_DEBUG
    fprintf(fp,"File found: %s\n",full_path.c_str());
#endif    
    if (include_filter == "") include = true;
    else {
#ifdef _WIN32
      if (!(fdp.attrib & _A_SUBDIR))
#else
      if (!S_ISDIR(stbuf.st_mode) && !S_ISLNK(stbuf.st_mode))
#endif
      {
        if (ss.find(include_filter) != -1) 
        {
          include = true;
        }
        else 
        {
          include = false;
        }
      }
    }
	//printf("get_files_recursive:%d\n",__LINE__);
    
    if (exclude_filter == "")
    {
      exclude = false;
    }
    else {
      vsx_avector<vsx_string> parts;
      vsx_string deli = " ";
      explode(exclude_filter,deli, parts);
      exclude = false;
      unsigned long i = 0;
      while (i < parts.size() && !exclude)
      {
        if (ss.find(parts[i]) != -1) exclude = true;
        ++i;
				//printf("get_files_recursive:%d\n",__LINE__);
      }
    }

    if ((ss != ".") && (ss != "..") && !exclude)
    {
      #ifdef VSXS_DEBUG
        fprintf(fp,"ss = %s  ___ include %d exclude %d  link: %d \n",ss.c_str(),include,exclude,(int)(stbuf.st_mode));
        fprintf(fp,"S_ISREG: %d\n", S_ISREG(stbuf.st_mode) );
        fprintf(fp,"S_ISDIR: %d\n", S_ISDIR(stbuf.st_mode) );
      #endif
#ifdef _WIN32
      if (fdp.attrib & _A_SUBDIR)
#else
      if (S_ISDIR(stbuf.st_mode) || S_ISLNK(stbuf.st_mode))
#endif
      {
        get_files_recursive(startpos+"/"+ss,filenames,include_filter,exclude_filter);
      } else {
      	if (include)
      	{
        	filenames->push_back(startpos+"/"+ss);
        	//printf("adding %s\n", vsx_string(startpos+"/"+ss).c_str());
      	}
      }
    }
#ifdef _WIN32
    if (_findnext(fhandle,&fdp) == -1) run = false;
#else
  dp = readdir(dir);
    if (dp == 0) 
    	run = false;
#endif
  }
#ifdef _WIN32
  _findclose(fhandle);
#else
  closedir(dir);
#endif
  #ifdef VSXS_DEBUG
  fclose(fp);
  #endif  
}  



//----------------------------------------------------------------



const char          fillchar = '=';
                        // 00000000001111111111222222
                        // 01234567890123456789012345
const vsx_string       cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

                        // 22223333333333444444444455
                        // 67890123456789012345678901
                          "abcdefghijklmnopqrstuvwxyz"

                        // 555555556666
                        // 234567890123
                          "0123456789+/";

vsx_string base64_encode(vsx_string data)
{
  int i;
    //auto     string::size_type  i;
    char               c;
    //auto     string::size_type  len = data.length();
    int len = data.size();
    vsx_string             ret;

    for (i = 0; i < len; ++i)
    {
        c = (data[i] >> 2) & 0x3f;
        ret.push_back(cvt[c]);
        c = (data[i] << 4) & 0x3f;
        if (++i < len)
            c |= (data[i] >> 4) & 0x0f;

        ret.push_back(cvt[c]);
        if (i < len)
        {
            c = (data[i] << 2) & 0x3f;
            if (++i < len)
                c |= (data[i] >> 6) & 0x03;

            ret.push_back( cvt[c]);
        }
        else
        {
            ++i;
            ret.push_back(fillchar);
        }

        if (i < len)
        {
            c = data[i] & 0x3f;
            ret.push_back(cvt[c]);
        }
        else
        {
            ret.push_back(fillchar);
        }
    }

    return(ret);
}

vsx_string base64_decode(vsx_string data)
{
    //auto     string::size_type  i;
    int i;
    char               c;
    char               c1;
    //auto     string::size_type  len = data.length();
    int len = data.size();
    vsx_string             ret;

    for (i = 0; i < len; ++i)
    {
        c = (char) cvt.find(data[i]);
        ++i;
        c1 = (char) cvt.find(data[i]);
        c = (c << 2) | ((c1 >> 4) & 0x3);
        ret.push_back(c);
        if (++i < len)
        {
            c = data[i];
            if (fillchar == c)
                break;

            c = (char) cvt.find(c);
            c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
            ret.push_back(c1);
        }

        if (++i < len)
        {
            c1 = data[i];
            if (fillchar == c1)
                break;

            c1 = (char) cvt.find(c1);
            c = ((c << 6) & 0xc0) | c1;
            ret.push_back(c);
        }
    }

    return(ret);
}


