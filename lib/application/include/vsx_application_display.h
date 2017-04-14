#pragma once

#include <tools/vsx_singleton.h>
#include <container/vsx_nw_vector.h>
#include <math/vector/vsx_vector2.h>
#include <tools/vsx_req.h>

class vsx_application_display
  : public vsx::singleton<vsx_application_display>
{

public:

  class display_info
  {
  public:
    int offset_x = -1;
    int offset_y = -1;
    int width = -1;
    int height = -1;

    display_info() {}

    display_info(int offset_x, int offset_y, int width, int height)
    {
      this->offset_x = offset_x;
      this->offset_y = offset_y;
      this->width = width;
      this->height = height;
    }
  };

  vsx_nw_vector<display_info> displays;

  /**
   * Calculates the relative position of a centered window.
   *
   * @brief get_local_pos
   * @param display
   * @param width
   * @param height
   * @return
   */
  std::pair<int, int> get_local_pos(size_t display, int width, int height)
  {
    if (!displays.size())
      return {0,0};

    if (display >= displays.size())
      return {0,0};

    return
    {
      (displays[display].width - width) / 2,
      (displays[display].height - height) / 2
    };
  }

};
