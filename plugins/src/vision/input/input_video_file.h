/**
* @author Dinesh Manajipet, Vovoid Media Technologies - Copyright (C) 2012-2020
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

#ifndef INPUT_VIDEO_FILE_H
#define INPUT_VIDEO_FILE_H

#include "input_video.h"

class input_video_file : public module_video_input {
  /**
   * The File Backend of the Video Capture Class
   * TODO: Allow setting an external time source.
   * TODO: Add options like repeat forever/play once, reload etc...
   */
  vsx_module_param_string* m_filename;

  bool isValid();
  void worker();

public:
  input_video_file();
  void module_info(vsx_module_specification* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void param_set_notify(const vsx_string<>& name);
};

#endif // INPUT_VIDEO_FILE_H
