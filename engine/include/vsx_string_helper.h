#ifndef VSX_STRING_HELPER_H
#define VSX_STRING_HELPER_H

#include <stdlib.h>
#include <wchar.h>

#include <vsx_string.h>
#include <vsx_req.h>

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
    sprintf(string_res,"%f",in);
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
    sprintf(string_res,vsx_string<>("%."+vsx_string_helper::i2s(decimals)+"f").c_str(),in);
    return vsx_string<>(string_res);
  }

  /**
   * @brief write_to_file
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
  inline void write_to_file(vsx_string<>filename, vsx_string<wchar_t>* payload)
  {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp)
      return;
    fputws( payload->c_str(), fp );
    fclose( fp );
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
  inline void explode_single( vsx_string<W> input, W delimiter_char, vsx_avector< vsx_string<W> >& parts )
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

    if (temp.size())
      parts.push_back(temp);
  }
}

#endif
