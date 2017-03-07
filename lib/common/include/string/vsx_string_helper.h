#pragma once

#include <stdlib.h>
#include <wchar.h>

#include <string/vsx_string.h>
#include <filesystem/vsx_filesystem.h>
#include <tools/vsx_req.h>

namespace vsx_string_helper
{
  /**
   * @brief s2i
   * @param in
   * @return
   */
  inline int s2i(const vsx_string<> &in)
  {
    return atoi( in.c_str() );
  }

  /**
   * @brief i2s
   * @param in
   * @return
   */
  inline vsx_string<>i2s(const int &in)
  {
    char string_res[256] = "";
    sprintf(string_res,"%d",in);
    return vsx_string<>(string_res);
  }

  /**
   * @brief i2s
   * @param in
   * @return
   */
  inline vsx_string<>ui642s(const uint64_t &in)
  {
    char string_res[256] = "";
    #if COMPILER == COMPILER_VISUAL_STUDIO
    sprintf(string_res,"%llu",in);
    #else
    sprintf(string_res,"%lu",in);
    #endif
    return vsx_string<>(string_res);
  }

  /**
  * @brief i2s
  * @param in
  * @return
  */
  inline vsx_string<>st2s(const size_t &in)
  {
    char string_res[256] = "";
    sprintf(string_res, "%zu", in);
    return vsx_string<>(string_res);
  }

  /**
   * @brief i2x
   * @param in
   * @return
   */
  inline vsx_string<>i2x(const uint64_t &in)
  {
    char string_res[256] = "";
    #if COMPILER == COMPILER_VISUAL_STUDIO
      sprintf(string_res,"%llx",in);
    #else
      sprintf(string_res,"%lx",in);
    #endif
    return vsx_string<>(string_res);
  }

  /**
   * @brief i2s
   * @param in
   * @return
   */
  inline vsx_string<wchar_t>i2sw(const int &in)
  {
    wchar_t string_res[256] = L"";
    swprintf(string_res, 255, L"%d", in);
    return vsx_string<wchar_t>(string_res);
  }


  /**
   * @brief s2b
   * @param in
   * @return
   */
  inline bool s2b(const vsx_string<> &in)
  {
    if (in == "true")
      return true;
    return false;
  }

  /**
   * @brief b2s
   * @param in
   * @return
   */
  inline vsx_string<>b2s(const bool &in)
  {
    char string_res[8] = "";
    if (in)
      sprintf(string_res,"%s","true");
    else
      sprintf(string_res,"%s","false");
    return vsx_string<>(string_res);
  }

  /**
   * @brief s2f
   * @param in
   * @return
   */
  inline float s2f(const vsx_string<> &in)
  {
    return (float)atof(in.c_str());
  }

  /**
   * @brief f2s
   * @param in
   * @return
   */
  inline vsx_string<>f2s(const float &in)
  {
    char string_res[64] = "";
    sprintf(string_res,"%f",(double)in);
    return vsx_string<>(string_res);
  }

  /**
   * @brief i2s
   * @param in
   * @return
   */
  inline vsx_string<wchar_t>f2sw(const float &in)
  {
    wchar_t string_res[64] = L"";
    swprintf(string_res, 64, L"%f", in);
    return vsx_string<wchar_t>(string_res);
  }

  /**
   * @brief f2s
   * @param in
   * @param decimals
   * @return
   */
  inline vsx_string<>f2s(const float &in, const int &decimals)
  {
    char string_res[64] = "";
    sprintf(string_res,vsx_string<>("%."+vsx_string_helper::i2s(decimals)+"f").c_str(),(double)in);
    return vsx_string<>(string_res);
  }

  /**
   * @brief write_to_file
   * @param filename
   * @param payload
   */
  inline void write_to_file(vsx_string<> filename, const vsx_string<> &payload)
  {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp)
      return;
    fputs( payload.c_str(), fp );
    fclose( fp );
  }

  /**
   * @brief write_to_file_p
   * @param filename
   * @param payload
   */
  inline void write_to_file(vsx_string<> filename, vsx_string<>* payload)
  {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp)
      return;
    fputs( payload->c_str(), fp );
    fclose( fp );
  }

  /**
   * @brief write_to_file
   * @param filename
   * @param payload
   */
  inline void write_to_file(vsx_string<>filename, vsx_string<wchar_t> payload)
  {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp)
      return;
    fputws( payload.c_str(), fp );
    fclose( fp );
  }

  /**
   * @brief write_to_file_p
   * @param filename
   * @param payload
   */
  inline void write_to_file_p(vsx_string<> filename, vsx_string<wchar_t>* payload)
  {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp)
      return;
    fputws( payload->c_str(), fp );
    fclose( fp );
  }

  /**
   * @brief read_from_file
   * @param filename
   * @return
   */
  template < int buf_size = 1024 >
  inline vsx_string<> read_from_file(vsx_string<> filename)
  {
    vsx_string<> result;
    FILE* fp = fopen(filename.c_str(), "r");
    if (!fp)
      return "";

    char* buf = (char*)malloc(buf_size);

    while ( fgets(buf, buf_size, fp) )
    {
      vsx_string<> line(buf);
      result += line;
    }
    fclose(fp);
    free(buf);
    return result;
  }

  /**
   * @brief read_from_file_filesystem
   * @param filename
   * @param filesystem
   * @return
   */
  template < int buf_size = 1024 >
  inline vsx_string<> read_from_file(vsx_string<> filename, vsx::filesystem* filesystem)
  {
    vsx_string<> result;
    vsx::file* fp = filesystem->f_open(filename.c_str());
    if (!fp)
      return "";

    char* buf = (char*)malloc(buf_size);

    while ( filesystem->f_gets(buf,buf_size,fp) )
    {
      vsx_string<> line(buf);
      result += line;
    }
    filesystem->f_close(fp);
    free(buf);
    return result;
  }

  /**
   * @brief string_convert converts between char and wchar_t or any other character storage type you like (templates)
   * @param s
   * @return
   */
  template < typename FROM = char, typename TO = wchar_t >
  inline vsx_string<TO> string_convert(vsx_string<FROM> s)
  {
    vsx_string<TO> result;
    for (size_t i = 0; i < s.size(); i++)
      result.push_back( (TO)s[i] );
    return result;
  }


  /**
   * @brief vsx_string_helper::explode_single Splits a string by a single character useful for splitting on 0x0A (newline) or 0x22 (space)
   * @param input string to work on
   * @param delimiter_char
   * @param parts
   */
  template < typename W = char >
  inline void explode_single(
    vsx_string<W> input,
    W delimiter_char,
    vsx_nw_vector< vsx_string<W> >& parts
  )
  {
    VSX_REQ_TRUE( input.size() );
    vsx_string<W> temp;
    for (size_t i = 0; i < input.size(); i++)
      if (input[i] == delimiter_char)
      {
        temp.trim_lf();
        parts.push_back( temp );
        temp.clear();
      } else
        temp.push_back( input[i] );

    if (temp.size() || input[ input.size() - 1 ] == delimiter_char)
      parts.push_back(temp);
  }


  /**
   * @brief vsx_string_helper::explode
   * @param input
   * @param delimiter
   * @param results
   * @param max_parts
   * @return
   */
  template < typename W = char >
  inline int explode(
    const vsx_string<W>& input,
    const vsx_string<W>& delimiter,
    vsx_nw_vector< vsx_string<W> >& results,
    int max_parts = 0
  )
  {
    results.clear();
    if (input == delimiter)
    {
      results.push_back(input);
      return 1;
    }

    vsx_string<>res;
    size_t fpos = 0;
    int count = 0;
    char lastchar = 0;
    for (size_t i = 0; i < input.size(); ++i)
    {
      if (input[i] == delimiter[fpos] && lastchar != '\\')
        ++fpos;
      else {
        res.push_back(input[i]);
        fpos = 0;
      }

      if (fpos == delimiter.size() || i == input.size()-1)
      {
        fpos = 0;
        results.push_back(res);
        res = "";
        ++count;
      }

      if (count >= max_parts && max_parts > 0)
        return count;

      lastchar = input[i];
    }

    if (count == 0)
      if (input.size())
      {
        results.push_back(input);
        return 1;
      }

    return count;
  }


  /**
   * @brief vsx_string_helper::implode
   * @param in
   * @param delimiter
   * @param start_index
   * @param shave_off_at_end
   * @return
   */
  template < typename W = char >
  inline vsx_string<W> implode(
    vsx_nw_vector< vsx_string<W> >& in,
    const vsx_string<W>& delimiter,
    size_t start_index = 0,
    size_t shave_off_at_end = 0
  )
  {
    reqrv(in.size(), "");

    long calculated_end_index = (long)in.size() - ((long)shave_off_at_end + 1);
    reqrv(calculated_end_index >= 0, "");

    if (in.size() == 1)
      return in[0];

    reqrv(start_index <= in.size()-1, "");

    size_t end_index = (size_t)calculated_end_index;

    vsx_string<> result;
    for (size_t i = start_index; i < end_index; ++i)
      result += in[i] + delimiter;

    result += in[end_index];
    return result;
  }

  template < typename W = char >
  inline vsx_string<W> implode_single(
      vsx_nw_vector< vsx_string<W> >& in,
      const W delimiter
  )
  {
    vsx_string<W> result;
    foreach(in, i)
    {
      if (i)
        result += delimiter;
      result += in[i];
    }
    return result;
  }

  template < typename W = char >
  inline vsx_string<W> pad_left(
      vsx_string<W> source,
      W character,
      size_t limit
    )
  {
    vsx_string<W> result;
    while ((int)result.size() < (int)limit - (int)source.size())
      result += character;
    return result + source;
  }

  template < typename W = char >
  inline vsx_string<W> pad_right(
      vsx_string<W> source,
      W character,
      size_t limit
    )
  {
    while (source.size() < limit)
      source += character;
    return source;
  }


  /**
   * @brief str_replace
   * @param search
   * @param replace
   * @param subject
   * @param max_replacements
   * @param required_pos
   * @return
   */
  template < typename W = char >
  inline vsx_string<W> str_replace(
    vsx_string<W> search,
    vsx_string<W> replace,
    vsx_string<W> subject,
    int max_replacements = 0,
    int required_pos = -1
  )
  {
    subject.replace(search, replace, max_replacements, required_pos);
    return subject;
  }


  /**
   * @brief str_replace_char_pad
   * @param search
   * @param replace
   * @param subject
   * @param subject_r
   * @param max_replacements
   * @param required_pos
   * @return
   */
  template < typename W = char >
  inline const vsx_string<W> str_replace_char_pad(
    vsx_string<W> search,
    vsx_string<W> replace,
    vsx_string<W> subject,
    vsx_string<W> subject_r,
    int max_replacements = 0,
    int required_pos = -1
  )
  {
    reqrv(search.size(), subject_r);
    reqrv(subject.size() == subject_r.size(), subject_r);

    vsx_string<> rep;
    for (size_t i = 0; i < search.size(); ++i)
      rep.push_back(replace[0]);

    int loc = 1;
    int replacements = 0;
    while ((loc = subject.find(search, loc-1)) != -1)
    {
      if (loc <= required_pos || required_pos == -1)
      {
          subject = subject.substr(0,loc) + rep + subject.substr(loc + (int)search.size());
          subject_r = subject_r.substr(0,loc) + rep + subject_r.substr(loc + (int)search.size());
          loc += (int)replace.size();
      }
      else
        return subject_r;

      if (max_replacements)
      {
        replacements++;
        if (replacements >= max_replacements)
          return subject_r;
      }
    }
    return subject_r;
  }


  /**
   * @brief str_remove_equal_prefix
   * @param str1
   * @param str2
   * @param delimiter
   */
  inline void str_remove_equal_prefix(
    vsx_string<>& str1,
    vsx_string<>& str2,
    const vsx_string<> delimiter
  )
  {
//    vsx_string<> deli = delimiter;
    vsx_nw_vector< vsx_string<> > str1_parts;
    vsx_nw_vector< vsx_string<> > str2_parts;
    vsx_string_helper::explode(str1, delimiter, str1_parts);
    vsx_string_helper::explode(str2, delimiter, str2_parts);

    req(str1_parts.front());
    req(str2_parts.front());

    while (
      str1_parts.size() && str2_parts.size()
      &&
      *str1_parts.front() == *str2_parts.front()
    )
    {
      str1_parts.pop_front();
      str2_parts.pop_front();
    }

    str1 = vsx_string_helper::implode(str1_parts, delimiter);
    str2 = vsx_string_helper::implode(str2_parts, delimiter);
  }


  /**
   * @brief verify_filesuffix
   * @param input
   * @param type
   * @return
   */
  inline bool verify_filesuffix(
    vsx_string<char>& input,
    const char* type
  )
  {
    vsx_nw_vector <vsx_string<char> > parts;
    vsx_string<char> deli = ".";
    vsx_string_helper::explode(input,deli,parts);
    reqrv(parts.size(), false);

    vsx_string<> a = parts[parts.size()-1];
    a.make_lowercase();
    vsx_string<> t = type;
    t.make_lowercase();
    if (t == a)
      return true;

    return false;
  }

  /**
   * @brief path_from_filename
   * @param filename
   * @param override_directory_separator
   * @return
   */
  inline vsx_string<> path_from_filename(vsx_string<> filename, char override_directory_separator = 0)
  {
    vsx_string<> deli = DIRECTORY_SEPARATOR;
    if (override_directory_separator)
      deli = override_directory_separator;
    vsx_nw_vector <vsx_string<> > results;
    vsx_string_helper::explode(filename, deli, results);
    return vsx_string_helper::implode(results, deli, 0, 1);
  }

  inline vsx_string<> filename_from_path(vsx_string<> filename, char override_directory_separator = 0)
  {
    vsx_string<> deli = DIRECTORY_SEPARATOR;
    if (override_directory_separator)
      deli = override_directory_separator;
    vsx_nw_vector <vsx_string<> > results;
    vsx_string_helper::explode(filename, deli, results);
    reqrv(results.size(), "");
    return results[results.size()-1];
  }

  inline vsx_string<> add_filename_suffix(vsx_string<> filename, vsx_string<> suffix)
  {
    vsx_nw_vector< vsx_string<> > parts;
    vsx_string_helper::explode_single<char>(filename, '.', parts);
    reqrv(parts.size() > 1, filename);
    parts[parts.size() - 2 ] += "_" + suffix;
    return vsx_string_helper::implode_single<char>(parts, '.');
  }


  /**
   * @brief utf8_string_to_wchar_string
   * @param src
   * @return
   */
  inline void utf8_string_to_wchar_string(vsx_string<wchar_t>& result, const vsx_string<char>& src)
  {
    result.reset_used();
    wchar_t w = 0;
    int bytes = 0;
    wchar_t err = L'?';
    for (size_t i = 0; i < src.size(); i++)
    {
      unsigned char c = (unsigned char)src[i];
      if (c <= 0x7f)
      {
        if (bytes)
        {
          result.push_back(err);
          bytes = 0;
        }
        result.push_back((wchar_t)c);
        continue;
      }

      if (c <= 0xbf)
      {
        if (bytes)
        {
          w = ((w << 6)|(c & 0x3f));
          bytes--;
          if (bytes == 0)
            result.push_back(w);
          continue;
        }
        result.push_back(err);
        continue;
      }

      if (c <= 0xdf)
      {
        bytes = 1;
        w = c & 0x1f;
        continue;
      }

      if (c <= 0xef)
      {
        bytes = 2;
        w = c & 0x0f;
        continue;
      }

      if (c <= 0xf7)
      {
        bytes = 3;
        w = c & 0x07;
        continue;
      }

      result.push_back(err);
      bytes = 0;
    }

    if (bytes)
      result.push_back(err);
  }

  /**
   * @brief utf8_string_to_wchar_string
   * @param src
   * @return
   */
  inline vsx_string<wchar_t> utf8_string_to_wchar_string(const vsx_string<char>& src)
  {
    vsx_string<wchar_t> result;
    utf8_string_to_wchar_string(result, src);
    return result;
  }


  /**
   * @brief wchar_string_to_utf8_string
   * @param src
   * @return
   */
  inline void wchar_string_to_utf8_string(vsx_string<char>& result, const vsx_string<wchar_t>& src)
  {
    result.reset_used();
    for (size_t i = 0; i < src.size(); i++)
    {
      wchar_t w = src[i];
      if (w <= 0x7f)
      {
        result.push_back((char)w);
        continue;
      }

      if (w <= 0x7ff)
      {
        result.push_back((char)(0xc0 | ((w >> 6)& 0x1f)));
        result.push_back((char)(0x80| (w & 0x3f)));
        continue;
      }

      if ((uint64_t)w <= 0xffff)
      {
        result.push_back((char)(0xe0 | ((w >> 12)& 0x0f)));
        result.push_back((char)(0x80 | ((w >> 6) & 0x3f)));
        result.push_back((char)(0x80 | (w & 0x3f)));
        continue;
      }

      if ((uint64_t)w <= 0x10ffff)
      {
        result.push_back((char)(0xf0 | (((uint64_t)w >> 18)& 0x07)) );
        result.push_back((char)(0x80 | (((uint64_t)w >> 12) & 0x3f)) );
        result.push_back((char)(0x80 | (((uint64_t)w >> 6) & 0x3f)) );
        result.push_back((char)(0x80 | ((uint64_t)w & 0x3f)) );
        continue;
      }

      result.push_back('?');
    }
  }

  /**
   * @brief wchar_string_to_utf8_string
   * @param src
   * @return
   */
  inline vsx_string<char> wchar_string_to_utf8_string(const vsx_string<wchar_t>& src)
  {
    vsx_string<char> result;
    vsx_string_helper::wchar_string_to_utf8_string(result, src);
    return result;
  }


  /**
   * @brief base64_encode
   * @param data
   * @return
   */
#ifdef __GNUC__
#if __GNUC__ > 3
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#endif
  inline vsx_string<> base64_encode(vsx_string<> data)
  {
    char               c;
    size_t len = data.size();
    vsx_string<>            ret;
    const char          fillchar = '=';
    // 00000000001111111111222222
    // 01234567890123456789012345
    const vsx_string<>      cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

    // 22223333333333444444444455
    // 67890123456789012345678901
      "abcdefghijklmnopqrstuvwxyz"

    // 555555556666
    // 234567890123
      "0123456789+/";


    for (size_t i = 0; i < len; ++i)
    {
      c = (data[i] >> 2) & 0x3f;
      ret.push_back(cvt[c]);
      c = (data[i] << 4) & 0x3f;
      if (++i < len)
        c |=  (
                data[i]
                >>
                4
              )
              &
              0x0f
            ;

      ret.push_back(cvt[c]);
      if (i < len)
      {
        c = (data[i] << 2) & 0x3f;
        if (++i < len)
          c |= (char)((data[i] >> (char)6) & (char)0x03);

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

    return ret;
  }

  /**
   * @brief base64_decode
   * @param data
   * @return
   */
  inline vsx_string<>base64_decode(vsx_string<>data)
  {
    char               c;
    char               c1;
    size_t len = data.size();
    vsx_string<>            ret;
    const char          fillchar = '=';
    // 00000000001111111111222222
    // 01234567890123456789012345
    const vsx_string<>      cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

    // 22223333333333444444444455
    // 67890123456789012345678901
      "abcdefghijklmnopqrstuvwxyz"

    // 555555556666
    // 234567890123
      "0123456789+/";


    for (size_t i = 0; i < len; ++i)
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

    return ret;
  }

  inline void ensure_trailing_dir_separator(vsx_string<>& str)
  {
    req(str.size());
    req(str[str.size()-1] != DIRECTORY_SEPARATOR_CHAR);
    str.push_back( DIRECTORY_SEPARATOR_CHAR );
  }

}

#ifdef __GNUC__
#if __GNUC__ > 3

#pragma GCC diagnostic pop
#endif
#endif
