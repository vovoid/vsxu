#ifndef VSX_STRING_HELPER_H
#define VSX_STRING_HELPER_H

#include <stdlib.h>
#include <wchar.h>

#include <string/vsx_string.h>
#include <vsxfst.h>
#include <tools/vsx_req.h>

namespace vsx_string_helper
{
  /**
   * @brief s2i
   * @param in
   * @return
   */
  inline int s2i(const vsx_string<>& in)
  {
    return atoi( in.c_str() );
  }

  /**
   * @brief i2s
   * @param in
   * @return
   */
  inline vsx_string<>i2s(int in)
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
  inline vsx_string<wchar_t>i2sw(int in)
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
  inline bool s2b(const vsx_string<>& in)
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
  inline vsx_string<>b2s(bool in)
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
  inline float s2f(const vsx_string<>& in)
  {
    return atof(in.c_str());
  }

  /**
   * @brief f2s
   * @param in
   * @return
   */
  inline vsx_string<>f2s(float in)
  {
    char string_res[256] = "";
    sprintf(string_res,"%f",(double)in);
    return vsx_string<>(string_res);
  }

  /**
   * @brief f2s
   * @param in
   * @param decimals
   * @return
   */
  inline vsx_string<>f2s(float in, int decimals)
  {
    char string_res[256] = "";
    sprintf(string_res,vsx_string<>("%."+vsx_string_helper::i2s(decimals)+"f").c_str(),(double)in);
    return vsx_string<>(string_res);
  }

  /**
   * @brief write_to_file
   * @param filename
   * @param payload
   */
  inline void write_to_file(vsx_string<>filename, vsx_string<> payload)
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
  inline void write_to_file(vsx_string<>filename, vsx_string<>* payload)
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
  inline void write_to_file_p(vsx_string<>filename, vsx_string<wchar_t>* payload)
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

    char buf[buf_size];

    while ( fgets(buf,buf_size,fp) )
    {
      vsx_string<> line(buf);
      result += line;
    }
    fclose(fp);
    return result;
  }

  /**
   * @brief read_from_file_filesystem
   * @param filename
   * @param filesystem
   * @return
   */
  template < int buf_size = 1024 >
  inline vsx_string<> read_from_file(vsx_string<> filename, vsxf* filesystem)
  {
    vsx_string<> result;
    vsxf_handle* fp = filesystem->f_open(filename.c_str(), "r");
    if (!fp)
      return "";

    char buf[buf_size] = "";

    while ( filesystem->f_gets(buf,buf_size,fp) )
    {
      vsx_string<> line(buf);
      result += line;
    }
    filesystem->f_close(fp);
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
   * @brief explode_single Splits a string by a single character useful for splitting on 0x0A (newline) or 0x22 (space)
   * @param input string to work on
   * @param delimiter_char
   * @param parts
   */
  template < typename W = char >
  inline void explode_single( vsx_string<W> input, W delimiter_char, vsx_nw_vector< vsx_string<W> >& parts )
  {
    VSX_REQ_TRUE( input.size() );
    vsx_string<W> temp;
    for (size_t i = 0; i < input.size(); i++)
      if (input[i] == delimiter_char)
      {
        parts.push_back( temp );
        temp.clear();
      } else
        temp.push_back( input[i] );

    if (temp.size() || input[ input.size() - 1 ] == delimiter_char)
      parts.push_back(temp);
  }

  /**
   * @brief utf8_string_to_wchar_string
   * @param src
   * @return
   */
  inline vsx_string<wchar_t> utf8_string_to_wchar_string(const vsx_string<char>& src)
  {
    vsx_string<wchar_t> result;
    wchar_t w = 0;
    int bytes = 0;
    wchar_t err = L'�';
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

    return result;
  }

  /**
   * @brief wchar_string_to_utf8_string
   * @param src
   * @return
   */
  inline vsx_string<char> wchar_string_to_utf8_string(const vsx_string<wchar_t>& src)
  {
    vsx_string<char> result;
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
        result.push_back(0xc0 | ((w >> 6)& 0x1f));
        result.push_back(0x80| (w & 0x3f));
        continue;
      }

      if (w <= 0xffff)
      {
        result.push_back(0xe0 | ((w >> 12)& 0x0f));
        result.push_back(0x80| ((w >> 6) & 0x3f));
        result.push_back(0x80| (w & 0x3f));
        continue;
      }

      if (w <= 0x10ffff)
      {
        result.push_back(0xf0 | ((w >> 18)& 0x07));
        result.push_back(0x80| ((w >> 12) & 0x3f));
        result.push_back(0x80| ((w >> 6) & 0x3f));
        result.push_back(0x80| (w & 0x3f));
        continue;
      }

      result.push_back('?');
    }
    return result;
  }


  inline vsx_string<>base64_encode(vsx_string<>data)
  {
    int i;
    char               c;
    int len = data.size();
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

  inline vsx_string<>base64_decode(vsx_string<>data)
  {
    int i;
    char               c;
    char               c1;
    int len = data.size();
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


}

#endif
