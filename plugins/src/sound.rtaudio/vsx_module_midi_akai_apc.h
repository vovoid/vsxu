/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Patrick Jacobs, Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__)
  #include <windows.h>
  #include <conio.h>
#else
#endif
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsxfst.h"
#include <RtMidi/RtMidi.h>
#include <sstream>



#define CC_ABSOLUTE 0
#define CC_ID_FADER 7
#define CC_ID_MASTER 0xE
#define CC_ID_CUE_LVEL_KNOB 47
#define CC_ID_DEVICE_CONTROL_1 0x10
#define CC_ID_DEVICE_CONTROL_2 0x11
#define CC_ID_DEVICE_CONTROL_3 0x12
#define CC_ID_DEVICE_CONTROL_4 0x13
#define CC_ID_DEVICE_CONTROL_5 0x14
#define CC_ID_DEVICE_CONTROL_6 0x15
#define CC_ID_DEVICE_CONTROL_7 0x16
#define CC_ID_DEVICE_CONTROL_8 0x17
#define NOTE_ON 1
#define NOTE_OFF 2
#define ON 1.0f
#define OFF 0.0f

class vsx_module_midi_akai_apc_40_controller : vsx_module
{
  unsigned int current_port;
  unsigned int port_count;
  vsx_module_param_int* midi_source;
  RtMidiIn* m_midi_in;
  vsx_module_param_float* multi_clip_launch[8][8][5];
  vsx_module_param_float* clip_launch[9][5];
  vsx_module_param_float* scene_launch[5];
  vsx_module_param_float* clip_stop[8];
  vsx_module_param_float* track_select[8];
  vsx_module_param_float* activator[8];
  vsx_module_param_float* solo[8];
  vsx_module_param_float* record_arm[8];
  vsx_module_param_float* fader[10];
  vsx_module_param_float* stop_all_clips;
  vsx_module_param_float* knob;


  // internal
  unsigned char prev_command[3];

  size_t track_selection;

public:

  vsx_module_midi_akai_apc_40_controller()
  {
    current_port=0;
    port_count=0;
    track_selection = 0;

    try
    {
      m_midi_in = new RtMidiIn(RtMidi::UNSPECIFIED,"vsxu_midi",1024*6);
      m_midi_in->ignoreTypes(false,false,false);
    }
    catch (RtError &error)
    {
      error.printMessage();
    }
  }

  ~vsx_module_midi_akai_apc_40_controller()
  {
    delete m_midi_in;
  }


  void module_info(vsx_module_info* info)
  {
    std::stringstream in_buff;
    in_buff << "midi_source:enum?";
    port_count = m_midi_in->getPortCount();
    for(int i=0;i<port_count;i++)
    {
      in_buff << i << "__" << remove_spaces(m_midi_in->getPortName(i)).c_str();
      if(i<port_count-1)
        in_buff << "|";
    }
    in_buff << "";
    info->in_param_spec = in_buff.str().c_str();
    info->identifier =
      "sound;midi;akai_apc_40_controller";

    info->out_param_spec =
      "track_select_multi:complex"
      "{"
        "mtrack1:complex{"
          "multi_clip_launch_1_1_1:float,"
          "multi_clip_launch_1_1_2:float,"
          "multi_clip_launch_1_1_3:float,"
          "multi_clip_launch_1_1_4:float,"
          "multi_clip_launch_1_1_5:float,"
          "multi_clip_launch_1_2_1:float,"
          "multi_clip_launch_1_2_2:float,"
          "multi_clip_launch_1_2_3:float,"
          "multi_clip_launch_1_2_4:float,"
          "multi_clip_launch_1_2_5:float,"
          "multi_clip_launch_1_3_1:float,"
          "multi_clip_launch_1_3_2:float,"
          "multi_clip_launch_1_3_3:float,"
          "multi_clip_launch_1_3_4:float,"
          "multi_clip_launch_1_3_5:float,"
          "multi_clip_launch_1_4_1:float,"
          "multi_clip_launch_1_4_2:float,"
          "multi_clip_launch_1_4_3:float,"
          "multi_clip_launch_1_4_4:float,"
          "multi_clip_launch_1_4_5:float,"
          "multi_clip_launch_1_5_1:float,"
          "multi_clip_launch_1_5_2:float,"
          "multi_clip_launch_1_5_3:float,"
          "multi_clip_launch_1_5_4:float,"
          "multi_clip_launch_1_5_5:float,"
          "multi_clip_launch_1_6_1:float,"
          "multi_clip_launch_1_6_2:float,"
          "multi_clip_launch_1_6_3:float,"
          "multi_clip_launch_1_6_4:float,"
          "multi_clip_launch_1_6_5:float,"
          "multi_clip_launch_1_7_1:float,"
          "multi_clip_launch_1_7_2:float,"
          "multi_clip_launch_1_7_3:float,"
          "multi_clip_launch_1_7_4:float,"
          "multi_clip_launch_1_7_5:float,"
          "multi_clip_launch_1_8_1:float,"
          "multi_clip_launch_1_8_2:float,"
          "multi_clip_launch_1_8_3:float,"
          "multi_clip_launch_1_8_4:float,"
          "multi_clip_launch_1_8_5:float"
        "},"
        "mtrack2:complex{"
          "multi_clip_launch_2_1_1:float,"
          "multi_clip_launch_2_1_2:float,"
          "multi_clip_launch_2_1_3:float,"
          "multi_clip_launch_2_1_4:float,"
          "multi_clip_launch_2_1_5:float,"
          "multi_clip_launch_2_2_1:float,"
          "multi_clip_launch_2_2_2:float,"
          "multi_clip_launch_2_2_3:float,"
          "multi_clip_launch_2_2_4:float,"
          "multi_clip_launch_2_2_5:float,"
          "multi_clip_launch_2_3_1:float,"
          "multi_clip_launch_2_3_2:float,"
          "multi_clip_launch_2_3_3:float,"
          "multi_clip_launch_2_3_4:float,"
          "multi_clip_launch_2_3_5:float,"
          "multi_clip_launch_2_4_1:float,"
          "multi_clip_launch_2_4_2:float,"
          "multi_clip_launch_2_4_3:float,"
          "multi_clip_launch_2_4_4:float,"
          "multi_clip_launch_2_4_5:float,"
          "multi_clip_launch_2_5_1:float,"
          "multi_clip_launch_2_5_2:float,"
          "multi_clip_launch_2_5_3:float,"
          "multi_clip_launch_2_5_4:float,"
          "multi_clip_launch_2_5_5:float,"
          "multi_clip_launch_2_6_1:float,"
          "multi_clip_launch_2_6_2:float,"
          "multi_clip_launch_2_6_3:float,"
          "multi_clip_launch_2_6_4:float,"
          "multi_clip_launch_2_6_5:float,"
          "multi_clip_launch_2_7_1:float,"
          "multi_clip_launch_2_7_2:float,"
          "multi_clip_launch_2_7_3:float,"
          "multi_clip_launch_2_7_4:float,"
          "multi_clip_launch_2_7_5:float,"
          "multi_clip_launch_2_8_1:float,"
          "multi_clip_launch_2_8_2:float,"
          "multi_clip_launch_2_8_3:float,"
          "multi_clip_launch_2_8_4:float,"
          "multi_clip_launch_2_8_5:float"
        "},"
        "mtrack3:complex{"
          "multi_clip_launch_3_1_1:float,"
          "multi_clip_launch_3_1_2:float,"
          "multi_clip_launch_3_1_3:float,"
          "multi_clip_launch_3_1_4:float,"
          "multi_clip_launch_3_1_5:float,"
          "multi_clip_launch_3_2_1:float,"
          "multi_clip_launch_3_2_2:float,"
          "multi_clip_launch_3_2_3:float,"
          "multi_clip_launch_3_2_4:float,"
          "multi_clip_launch_3_2_5:float,"
          "multi_clip_launch_3_3_1:float,"
          "multi_clip_launch_3_3_2:float,"
          "multi_clip_launch_3_3_3:float,"
          "multi_clip_launch_3_3_4:float,"
          "multi_clip_launch_3_3_5:float,"
          "multi_clip_launch_3_4_1:float,"
          "multi_clip_launch_3_4_2:float,"
          "multi_clip_launch_3_4_3:float,"
          "multi_clip_launch_3_4_4:float,"
          "multi_clip_launch_3_4_5:float,"
          "multi_clip_launch_3_5_1:float,"
          "multi_clip_launch_3_5_2:float,"
          "multi_clip_launch_3_5_3:float,"
          "multi_clip_launch_3_5_4:float,"
          "multi_clip_launch_3_5_5:float,"
          "multi_clip_launch_3_6_1:float,"
          "multi_clip_launch_3_6_2:float,"
          "multi_clip_launch_3_6_3:float,"
          "multi_clip_launch_3_6_4:float,"
          "multi_clip_launch_3_6_5:float,"
          "multi_clip_launch_3_7_1:float,"
          "multi_clip_launch_3_7_2:float,"
          "multi_clip_launch_3_7_3:float,"
          "multi_clip_launch_3_7_4:float,"
          "multi_clip_launch_3_7_5:float,"
          "multi_clip_launch_3_8_1:float,"
          "multi_clip_launch_3_8_2:float,"
          "multi_clip_launch_3_8_3:float,"
          "multi_clip_launch_3_8_4:float,"
          "multi_clip_launch_3_8_5:float"
        "},"
        "mtrack4:complex{"
          "multi_clip_launch_4_1_1:float,"
          "multi_clip_launch_4_1_2:float,"
          "multi_clip_launch_4_1_3:float,"
          "multi_clip_launch_4_1_4:float,"
          "multi_clip_launch_4_1_5:float,"
          "multi_clip_launch_4_2_1:float,"
          "multi_clip_launch_4_2_2:float,"
          "multi_clip_launch_4_2_3:float,"
          "multi_clip_launch_4_2_4:float,"
          "multi_clip_launch_4_2_5:float,"
          "multi_clip_launch_4_3_1:float,"
          "multi_clip_launch_4_3_2:float,"
          "multi_clip_launch_4_3_3:float,"
          "multi_clip_launch_4_3_4:float,"
          "multi_clip_launch_4_3_5:float,"
          "multi_clip_launch_4_4_1:float,"
          "multi_clip_launch_4_4_2:float,"
          "multi_clip_launch_4_4_3:float,"
          "multi_clip_launch_4_4_4:float,"
          "multi_clip_launch_4_4_5:float,"
          "multi_clip_launch_4_5_1:float,"
          "multi_clip_launch_4_5_2:float,"
          "multi_clip_launch_4_5_3:float,"
          "multi_clip_launch_4_5_4:float,"
          "multi_clip_launch_4_5_5:float,"
          "multi_clip_launch_4_6_1:float,"
          "multi_clip_launch_4_6_2:float,"
          "multi_clip_launch_4_6_3:float,"
          "multi_clip_launch_4_6_4:float,"
          "multi_clip_launch_4_6_5:float,"
          "multi_clip_launch_4_7_1:float,"
          "multi_clip_launch_4_7_2:float,"
          "multi_clip_launch_4_7_3:float,"
          "multi_clip_launch_4_7_4:float,"
          "multi_clip_launch_4_7_5:float,"
          "multi_clip_launch_4_8_1:float,"
          "multi_clip_launch_4_8_2:float,"
          "multi_clip_launch_4_8_3:float,"
          "multi_clip_launch_4_8_4:float,"
          "multi_clip_launch_4_8_5:float"
        "},"
        "mtrack5:complex{"
          "multi_clip_launch_5_1_1:float,"
          "multi_clip_launch_5_1_2:float,"
          "multi_clip_launch_5_1_3:float,"
          "multi_clip_launch_5_1_4:float,"
          "multi_clip_launch_5_1_5:float,"
          "multi_clip_launch_5_2_1:float,"
          "multi_clip_launch_5_2_2:float,"
          "multi_clip_launch_5_2_3:float,"
          "multi_clip_launch_5_2_4:float,"
          "multi_clip_launch_5_2_5:float,"
          "multi_clip_launch_5_3_1:float,"
          "multi_clip_launch_5_3_2:float,"
          "multi_clip_launch_5_3_3:float,"
          "multi_clip_launch_5_3_4:float,"
          "multi_clip_launch_5_3_5:float,"
          "multi_clip_launch_5_4_1:float,"
          "multi_clip_launch_5_4_2:float,"
          "multi_clip_launch_5_4_3:float,"
          "multi_clip_launch_5_4_4:float,"
          "multi_clip_launch_5_4_5:float,"
          "multi_clip_launch_5_5_1:float,"
          "multi_clip_launch_5_5_2:float,"
          "multi_clip_launch_5_5_3:float,"
          "multi_clip_launch_5_5_4:float,"
          "multi_clip_launch_5_5_5:float,"
          "multi_clip_launch_5_6_1:float,"
          "multi_clip_launch_5_6_2:float,"
          "multi_clip_launch_5_6_3:float,"
          "multi_clip_launch_5_6_4:float,"
          "multi_clip_launch_5_6_5:float,"
          "multi_clip_launch_5_7_1:float,"
          "multi_clip_launch_5_7_2:float,"
          "multi_clip_launch_5_7_3:float,"
          "multi_clip_launch_5_7_4:float,"
          "multi_clip_launch_5_7_5:float,"
          "multi_clip_launch_5_8_1:float,"
          "multi_clip_launch_5_8_2:float,"
          "multi_clip_launch_5_8_3:float,"
          "multi_clip_launch_5_8_4:float,"
          "multi_clip_launch_5_8_5:float"
        "},"
        "mtrack6:complex{"
          "multi_clip_launch_6_1_1:float,"
          "multi_clip_launch_6_1_2:float,"
          "multi_clip_launch_6_1_3:float,"
          "multi_clip_launch_6_1_4:float,"
          "multi_clip_launch_6_1_5:float,"
          "multi_clip_launch_6_2_1:float,"
          "multi_clip_launch_6_2_2:float,"
          "multi_clip_launch_6_2_3:float,"
          "multi_clip_launch_6_2_4:float,"
          "multi_clip_launch_6_2_5:float,"
          "multi_clip_launch_6_3_1:float,"
          "multi_clip_launch_6_3_2:float,"
          "multi_clip_launch_6_3_3:float,"
          "multi_clip_launch_6_3_4:float,"
          "multi_clip_launch_6_3_5:float,"
          "multi_clip_launch_6_4_1:float,"
          "multi_clip_launch_6_4_2:float,"
          "multi_clip_launch_6_4_3:float,"
          "multi_clip_launch_6_4_4:float,"
          "multi_clip_launch_6_4_5:float,"
          "multi_clip_launch_6_5_1:float,"
          "multi_clip_launch_6_5_2:float,"
          "multi_clip_launch_6_5_3:float,"
          "multi_clip_launch_6_5_4:float,"
          "multi_clip_launch_6_5_5:float,"
          "multi_clip_launch_6_6_1:float,"
          "multi_clip_launch_6_6_2:float,"
          "multi_clip_launch_6_6_3:float,"
          "multi_clip_launch_6_6_4:float,"
          "multi_clip_launch_6_6_5:float,"
          "multi_clip_launch_6_7_1:float,"
          "multi_clip_launch_6_7_2:float,"
          "multi_clip_launch_6_7_3:float,"
          "multi_clip_launch_6_7_4:float,"
          "multi_clip_launch_6_7_5:float,"
          "multi_clip_launch_6_8_1:float,"
          "multi_clip_launch_6_8_2:float,"
          "multi_clip_launch_6_8_3:float,"
          "multi_clip_launch_6_8_4:float,"
          "multi_clip_launch_6_8_5:float"
        "},"
        "mtrack7:complex{"
          "multi_clip_launch_7_1_1:float,"
          "multi_clip_launch_7_1_2:float,"
          "multi_clip_launch_7_1_3:float,"
          "multi_clip_launch_7_1_4:float,"
          "multi_clip_launch_7_1_5:float,"
          "multi_clip_launch_7_2_1:float,"
          "multi_clip_launch_7_2_2:float,"
          "multi_clip_launch_7_2_3:float,"
          "multi_clip_launch_7_2_4:float,"
          "multi_clip_launch_7_2_5:float,"
          "multi_clip_launch_7_3_1:float,"
          "multi_clip_launch_7_3_2:float,"
          "multi_clip_launch_7_3_3:float,"
          "multi_clip_launch_7_3_4:float,"
          "multi_clip_launch_7_3_5:float,"
          "multi_clip_launch_7_4_1:float,"
          "multi_clip_launch_7_4_2:float,"
          "multi_clip_launch_7_4_3:float,"
          "multi_clip_launch_7_4_4:float,"
          "multi_clip_launch_7_4_5:float,"
          "multi_clip_launch_7_5_1:float,"
          "multi_clip_launch_7_5_2:float,"
          "multi_clip_launch_7_5_3:float,"
          "multi_clip_launch_7_5_4:float,"
          "multi_clip_launch_7_5_5:float,"
          "multi_clip_launch_7_6_1:float,"
          "multi_clip_launch_7_6_2:float,"
          "multi_clip_launch_7_6_3:float,"
          "multi_clip_launch_7_6_4:float,"
          "multi_clip_launch_7_6_5:float,"
          "multi_clip_launch_7_7_1:float,"
          "multi_clip_launch_7_7_2:float,"
          "multi_clip_launch_7_7_3:float,"
          "multi_clip_launch_7_7_4:float,"
          "multi_clip_launch_7_7_5:float,"
          "multi_clip_launch_7_8_1:float,"
          "multi_clip_launch_7_8_2:float,"
          "multi_clip_launch_7_8_3:float,"
          "multi_clip_launch_7_8_4:float,"
          "multi_clip_launch_7_8_5:float"
        "},"
        "mtrack8:complex{"
          "multi_clip_launch_8_1_1:float,"
          "multi_clip_launch_8_1_2:float,"
          "multi_clip_launch_8_1_3:float,"
          "multi_clip_launch_8_1_4:float,"
          "multi_clip_launch_8_1_5:float,"
          "multi_clip_launch_8_2_1:float,"
          "multi_clip_launch_8_2_2:float,"
          "multi_clip_launch_8_2_3:float,"
          "multi_clip_launch_8_2_4:float,"
          "multi_clip_launch_8_2_5:float,"
          "multi_clip_launch_8_3_1:float,"
          "multi_clip_launch_8_3_2:float,"
          "multi_clip_launch_8_3_3:float,"
          "multi_clip_launch_8_3_4:float,"
          "multi_clip_launch_8_3_5:float,"
          "multi_clip_launch_8_4_1:float,"
          "multi_clip_launch_8_4_2:float,"
          "multi_clip_launch_8_4_3:float,"
          "multi_clip_launch_8_4_4:float,"
          "multi_clip_launch_8_4_5:float,"
          "multi_clip_launch_8_5_1:float,"
          "multi_clip_launch_8_5_2:float,"
          "multi_clip_launch_8_5_3:float,"
          "multi_clip_launch_8_5_4:float,"
          "multi_clip_launch_8_5_5:float,"
          "multi_clip_launch_8_6_1:float,"
          "multi_clip_launch_8_6_2:float,"
          "multi_clip_launch_8_6_3:float,"
          "multi_clip_launch_8_6_4:float,"
          "multi_clip_launch_8_6_5:float,"
          "multi_clip_launch_8_7_1:float,"
          "multi_clip_launch_8_7_2:float,"
          "multi_clip_launch_8_7_3:float,"
          "multi_clip_launch_8_7_4:float,"
          "multi_clip_launch_8_7_5:float,"
          "multi_clip_launch_8_8_1:float,"
          "multi_clip_launch_8_8_2:float,"
          "multi_clip_launch_8_8_3:float,"
          "multi_clip_launch_8_8_4:float,"
          "multi_clip_launch_8_8_5:float"
        "}"
      "},"
      "tracks:complex"
      "{"
        "track1:complex{"
          "clip_launch1_1:float,"
          "clip_launch1_2:float,"
          "clip_launch1_3:float,"
          "clip_launch1_4:float,"
          "clip_launch1_5:float,"
          "clip_stop1:float,"
          "track_select1:float,"
          "activator1:float,"
          "solo1:float,"
          "record_arm1:float,"
          "fader1:float"
        "},"
        "track2:complex{"
          "clip_launch2_1:float,"
          "clip_launch2_2:float,"
          "clip_launch2_3:float,"
          "clip_launch2_4:float,"
          "clip_launch2_5:float,"
          "clip_stop2:float,"
          "track_select2:float,"
          "activator2:float,"
          "solo2:float,"
          "record_arm2:float,"
          "fader2:float"
        "},"
        "track3:complex{"
          "clip_launch3_1:float,"
          "clip_launch3_2:float,"
          "clip_launch3_3:float,"
          "clip_launch3_4:float,"
          "clip_launch3_5:float,"
          "clip_stop3:float,"
          "track_select3:float,"
          "activator3:float,"
          "solo3:float,"
          "record_arm3:float,"
          "fader3:float"
        "},"
        "track4:complex{"
          "clip_launch4_1:float,"
          "clip_launch4_2:float,"
          "clip_launch4_3:float,"
          "clip_launch4_4:float,"
          "clip_launch4_5:float,"
          "clip_stop4:float,"
          "track_select4:float,"
          "activator4:float,"
          "solo4:float,"
          "record_arm4:float,"
          "fader4:float"
        "},"
        "track5:complex{"
          "clip_launch5_1:float,"
          "clip_launch5_2:float,"
          "clip_launch5_3:float,"
          "clip_launch5_4:float,"
          "clip_launch5_5:float,"
          "clip_stop5:float,"
          "track_select5:float,"
          "activator5:float,"
          "solo5:float,"
          "record_arm5:float,"
          "fader5:float"
        "},"
        "track6:complex{"
          "clip_launch6_1:float,"
          "clip_launch6_2:float,"
          "clip_launch6_3:float,"
          "clip_launch6_4:float,"
          "clip_launch6_5:float,"
          "clip_stop6:float,"
          "track_select6:float,"
          "activator6:float,"
          "solo6:float,"
          "record_arm6:float,"
          "fader6:float"
        "},"
        "track7:complex{"
          "clip_launch7_1:float,"
          "clip_launch7_2:float,"
          "clip_launch7_3:float,"
          "clip_launch7_4:float,"
          "clip_launch7_5:float,"
          "clip_stop7:float,"
          "track_select7:float,"
          "activator7:float,"
          "solo7:float,"
          "record_arm7:float,"
          "fader7:float"
        "},"
        "track8:complex{"
          "clip_launch8_1:float,"
          "clip_launch8_2:float,"
          "clip_launch8_3:float,"
          "clip_launch8_4:float,"
          "clip_launch8_5:float,"
          "track_select8:float,"
          "clip_stop8:float,"
          "activator8:float,"
          "solo8:float,"
          "record_arm8:float,"
          "fader8:float"
        "},"
        "scene_launch:complex{"
          "scene_launch1:float,"
          "scene_launch2:float,"
          "scene_launch3:float,"
          "scene_launch4:float,"
          "scene_launch5:float,"
          "stop_all_clips:float,"
          "knob_dir:float,"
          "fader9:float"
        "}"
      "}"
    ;
    info->component_class = "system";
    info->description = "This represents the Akai APC20 midi\ncontroller.";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    std::stringstream intbuf;
    for (int track_sel_i = 0; track_sel_i < 8; track_sel_i++)
    {
      for (int column_i = 0; column_i < 8; column_i++)
      {
        for (int row_i = 0; row_i < 5; row_i++)
        {
          intbuf.str("");
          intbuf << "multi_clip_launch_" << track_sel_i+1 << "_" << column_i+1 << "_" << row_i+1;

          multi_clip_launch[track_sel_i][column_i][row_i] =  (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, intbuf.str().c_str() );
          multi_clip_launch[track_sel_i][column_i][row_i]->set(0.0f);
          intbuf.str("");
        }
      }
    }

    for(int i=1;i<10;i++)
    {
      intbuf << "fader" << i;
      fader[i-1]=(vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,intbuf.str().c_str());
      fader[i-1]->set(0.0f);
      intbuf.str("");
      if ( i == 9 )
      {
        for(int j=1;j<6;j++)
        {
          intbuf << "scene_launch" << j;
          scene_launch[j-1] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,intbuf.str().c_str());
          scene_launch[j-1]->set(0.0f);
          intbuf.str("");
        }
        stop_all_clips = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"stop_all_clips");
        knob = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"knob_dir");
        stop_all_clips->set(0.0);
        knob->set(0.0);
        continue;
      }
      for(int j=1;j<6;j++)
      {
        intbuf << "clip_launch" << i;
        intbuf << "_";
        intbuf << j;
        clip_launch[i-1][j-1] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,intbuf.str().c_str());
        clip_launch[i-1][j-1]->set(0.0f);
        intbuf.str("");
      }
      intbuf.str("");
      intbuf << "track_select" << i;
      track_select[i-1] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,intbuf.str().c_str());
      track_select[i-1]->set(0.0f);

      intbuf.str("");
      intbuf << "clip_stop" << i;
      clip_stop[i-1] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,intbuf.str().c_str());
      clip_stop[i-1]->set(0.0f);

      intbuf.str("");
      intbuf << "activator" << i;
      activator[i-1] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,intbuf.str().c_str());
      activator[i-1]->set(0.0f);

      intbuf.str("");
      intbuf << "solo" << i;
      solo[i-1] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,intbuf.str().c_str());
      solo[i-1]->set(0.0f);

      intbuf.str("");
      intbuf << "record_arm" << i;
      record_arm[i-1] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,intbuf.str().c_str());
      record_arm[i-1]->set(0.0f);
      intbuf.str("");
    }
    redeclare_in_params(in_parameters);
    m_midi_in->openPort(current_port);

    loading_done = true;
  }

  void redeclare_in_params(vsx_module_param_list& in_param)
  {
    midi_source = (vsx_module_param_int*)in_param.create(VSX_MODULE_PARAM_ID_FLOAT,"midi_source");
    midi_source->set(0);
  }

  void run()
  {
    if ( current_port != get_port() )
    {
      int new_src = midi_source->get();
      if ( new_src > port_count-1 )
      {
        new_src = port_count-1;
      }
      try
      {
        current_port = new_src;
        m_midi_in->closePort();
        m_midi_in->openPort(current_port);
      }
      catch (RtError &error)
      {
        message = error.getMessage().c_str();
      }
    }

    if ( port_count != m_midi_in->getPortCount())
    {
      port_count = m_midi_in->getPortCount();
      redeclare_in = true;
    }

    handle_messages();
  }

  void handle_messages()
  {
    std::vector<unsigned char> mess;
    m_midi_in->getMessage(&mess);

    if(mess.size() > 0)
    {
      int byte1,byte2,byte3,chan,type;
      byte1=mess.at(0);
      byte2=mess.at(1);
      byte3=mess.at(2);

      //vsx_printf("%X %X %X\n", byte1, byte2, byte3);

      if(byte1-0xB0>=0)
      {
       chan = byte1-0xB0;
       type = CC_ABSOLUTE;
      }
      else
      if (byte1-0x90>=0)
      {
       chan = byte1-0x90;
       type = NOTE_ON;
      }
      else
      if (byte1-0x80>=0)
      {
       chan = byte1-0x80;
       type = NOTE_OFF;
      }

      switch(type)
      {
        case NOTE_ON:
          switch ( byte2 )
          {
            case 48:
              record_arm[chan]->set(ON);
            break;

            case 49:
              solo[chan]->set(ON);
            break;

            case 50:
                  activator[chan]->set(ON);
            break;

            case 51:
              track_select[chan]->set(ON);
            break;

            case 52:
              clip_stop[chan]->set(ON);
            break;

            case 53:
            case 54:
            case 55:
            case 56:
            case 57:
              byte2 -= 53;
              clip_launch[chan][byte2]->set(ON);
              if (chan < 8)
              {
                multi_clip_launch[track_selection][chan][byte2]->set(ON);
              }
            break;

            case 81:
              stop_all_clips->set(ON);
            break;

            case 82:
            case 83:
            case 84:
            case 85:
            case 86:
              scene_launch[byte2-82]->set(ON);
            break;
          }
        break;

        case NOTE_OFF:
          switch(byte2)
          {
            case 48:
              record_arm[chan]->set(OFF);
            break;

            case 49:
              solo[chan]->set(OFF);
            break;

            case 50:
              activator[chan]->set(OFF);
            break;

            case 51:
              track_select[chan]->set(OFF);
            break;

            case 52:
              clip_stop[chan]->set(OFF);
            break;

            case 53:
            case 54:
            case 55:
            case 56:
            case 57:
              byte2 -= 53;
              clip_launch[chan][byte2]->set(OFF);
              if (chan < 8)
              {
                multi_clip_launch[track_selection][chan][byte2]->set(OFF);
              }
            break;

            case 81:
                stop_all_clips->set(OFF);
            break;

            case 82:
            case 83:
            case 84:
            case 85:
            case 86:
              scene_launch[byte2-82]->set(OFF);
            break;
          }
        break;
        case CC_ABSOLUTE:
          float val = (float)(byte3/127.0f);

          // the per-channel faders
          if(byte2 == CC_ID_FADER)
          {
            fader[chan]->set(val);
            break;
          }

          // master fader
          if (byte2 == CC_ID_MASTER)
          {
            fader[8]->set(val);
            break;
          }

          // cue level knob
          if (byte2 == CC_ID_CUE_LVEL_KNOB)
          {
            knob->set((float)byte3);
            break;
          }

          switch (byte2)
          {
            case CC_ID_DEVICE_CONTROL_8:
              if (prev_command[1] == CC_ID_DEVICE_CONTROL_7)
              {
                track_selection = byte1 - 0xB0;
              }
            break;
          }
        break; // CC_ABSOLUTE
      }

      prev_command[0] = byte1;
      prev_command[1] = byte2;
      prev_command[2] = byte3;

      handle_messages();
    }
  }
private:
  vsx_string remove_spaces(std::string st)
  {
    vsx_string str = st.c_str();
    for(size_t i=0;i<st.length();i++)
    {
      if(str[i]==' '||str[i]==':')
      {
        str[i]='_';
      }
    }
    return str;
  }

  int get_port()
  {
    int i = midi_source->get();
    if(i > port_count)
    {
     return port_count;
    }
    return i;
  }

};


