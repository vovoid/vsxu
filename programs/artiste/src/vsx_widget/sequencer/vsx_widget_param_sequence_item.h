#ifndef VSX_WIDGET_PARAM_SEQUENCE_ITEM
#define VSX_WIDGET_PARAM_SEQUENCE_ITEM

#include <string/vsx_string_helper.h>

#define VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_NONE 0
#define VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_LINEAR 1
#define VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_COSINE 2
#define VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_RESERVED 3
#define VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER 4
// 0 = no interpolation
// 1 = linear interpolation
// 2 = cosine interpolation
// 3 = reserved
// 4 = bezier


enum sequence_param_type_t
{
  pt_float,
  pt_string
};

class vsx_widget_param_sequence_item
{
  int type = 0; // 0 = param, 1 = master
  sequence_param_type_t sequence_param_type = pt_float;
  float total_length = 1.0f; // in seconds (float)
  float length = 0.1f; // for master channel, this is the length of the block
  float accum_time = 0.0f;
  vsx_string<> value; // if master, this is the name of the sequence_pool
  size_t interpolation = 0;
  vsx_vector3<> handle1;
  vsx_vector3<> handle2;

  // child controller for setting the sequence for a master channel item
  vsx_widget* master_channel_time_sequence = 0x0;
  vsx_string<>pool_name;

public:

  vsx_widget_param_sequence_item()
  {
    handle1.x = 0.1f;
    handle1.y = 1.5f;
    handle2.x = 0.9f;
    handle2.y = 1.5f;
  }


  int get_type()
  {
    return type;
  }

  void set_type(int v)
  {
    type = v;
  }

  sequence_param_type_t get_param_type()
  {
    return sequence_param_type;
  }

  void set_param_type(sequence_param_type_t t)
  {
    sequence_param_type = t;
  }

  vsx_string<> get_value_by_interpolation()
  {
    if (interpolation == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
      return value+":"+
          vsx_string_helper::f2s(handle1.x)+","+
          vsx_string_helper::f2s(handle1.y)+":"+
          vsx_string_helper::f2s(handle2.x)+","+
          vsx_string_helper::f2s(handle2.y);
    return value;
  }






  float get_total_length()
  {
    return total_length;
  }

  void set_total_length(float v)
  {
    if (v < 0.0)
    {
      total_length = 0.0;
      return;
    }
    total_length = v;
  }

  void increase_total_length(float v)
  {
    total_length += v;
  }






  float get_length()
  {
    return length;
  }

  void set_length(float v)
  {
    if (v < 0.0)
    {
      length = 0.0;
      return;
    }
    length = v;
  }

  void increase_length(const float v)
  {
    length += v;
    if (length > total_length)
      length = total_length;
    if (length < 0.0f)
      length = 0.0f;
  }


  float get_accum_time()
  {
    return accum_time;
  }

  void set_accum_time(float n) {
    accum_time = n;
  }




  vsx_string<>get_value()
  {
    return value;
  }


  void set_value(vsx_string<>v)
  {
    value = v;
  }

  inline float convert_to_float(vsx_string<> v)
  {
    if (sequence_param_type == pt_string)
      return 0.0;
    return vsx_string_helper::s2f(v);
  }




  size_t get_interpolation()
  {
    return interpolation;
  }

  void set_interpolation(const size_t v, vsx_widget_param_sequence_item* next_value = 0x0)
  {
    if (sequence_param_type == pt_string)
      interpolation = VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_NONE;

    // if we change to bezier, we want special handling.
    if
    (
      interpolation != VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER
      &&
      v == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER
    )
    {
      switch (interpolation)
      {
        case VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_NONE:
        case VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_LINEAR:
          handle1.x = 0.1;
          handle2.x = 0.9;
          if (next_value)
          {
            float diff = vsx_string_helper::s2f(next_value->get_value() ) - vsx_string_helper::s2f(value);

            handle1.y = 0.1 *  diff;
            handle2.y = -0.1 *  diff;
          }
          break;
        case VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_COSINE:
          handle1.y = 0;
          handle2.y = 0;
          handle1.x = 0.6;
          handle2.x = 0.4;
          break;

      }
    }

    interpolation = v;
  }

  void set_bezier_handles_factor(float factor)
  {
    handle1.y = 0;
    handle2.y = 0;
    handle1.x = factor;
    handle2.x = factor;
  }






  const vsx_vector3<>& get_handle1()
  {
    return handle1;
  }

  void set_handle1(const vsx_vector3<> &v)
  {
    handle1 = v;
    if (handle1.x < 0.0)
      handle1.x = 0.0;
    if (handle1.x > 1.0)
      handle1.x = 1.0;
  }






  const vsx_vector3<>& get_handle2()
  {
    return handle2;
  }

  void set_handle2(const vsx_vector3<> &v)
  {
    handle2 = v;
    if (handle1.x < 0.0)
      handle2.x = 0.0;
    if (handle1.x > 1.0)
      handle2.x = 1.0;
  }


  vsx_string<>get_pool_name()
  {
    return pool_name;
  }

  void set_pool_name(vsx_string<>v)
  {
    pool_name = v;
  }





  vsx_widget*& get_master_channel_time_sequence()
  {
    return master_channel_time_sequence;
  }

  void set_master_channel_time_sequence(vsx_widget* v)
  {
    master_channel_time_sequence = v;
  }


};





#endif
