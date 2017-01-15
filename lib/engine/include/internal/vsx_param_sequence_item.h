#pragma once


class vsx_param_sequence_item
{
public:
  float accum_time; // the time on wich this row starts
  float total_length = 1.0f; // in seconds (float)
  vsx_string<> value;
  int interpolation = 1;
  vsx_vector3<> handle1;
  vsx_vector3<> handle2;

  vsx_string<>get_value()
  {
    if (interpolation == 4)
    {
      return value+":"+vsx_string_helper::f2s(handle1.x)+","+vsx_string_helper::f2s(handle1.y)+":"+vsx_string_helper::f2s(handle2.x)+","+vsx_string_helper::f2s(handle2.y);
    }
    return value;
  }

  vsx_param_sequence_item& operator=(const vsx_param_sequence_item& other)
  {
    accum_time = other.accum_time;
    total_length = other.total_length;
    value = other.value;
    interpolation = other.interpolation;
    handle1 = other.handle1;
    handle2 = other.handle2;
    return *this;
  }

  vsx_param_sequence_item& operator=(const vsx_param_sequence_item&& other)
  {
    accum_time = other.accum_time;
    total_length = other.total_length;
    value = std::move(other.value);
    interpolation = other.interpolation;
    handle1 = other.handle1;
    handle2 = other.handle2;
    return *this;
  }

  vsx_param_sequence_item(const vsx_param_sequence_item& other)
  {
    accum_time = other.accum_time;
    total_length = other.total_length;
    value = other.value;
    interpolation = other.interpolation;
    handle1 = other.handle1;
    handle2 = other.handle2;
  }

  vsx_param_sequence_item(const vsx_param_sequence_item&& other)
  {
    accum_time = other.accum_time;
    total_length = other.total_length;
    value = std::move(other.value);
    interpolation = other.interpolation;
    handle1 = other.handle1;
    handle2 = other.handle2;
  }

  vsx_param_sequence_item() {}
};
