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

#include "input_audio_midi.h"

input_audio_midi::input_audio_midi():
  vsx_module(),
  m_midiin(0),
  m_isValid(false)
{

  try {
    m_midiin = new RtMidiIn( RtMidi::UNSPECIFIED,std::string("VSXu Midi input") );
  }
  catch ( RtError &error ) {
    error.printMessage();
  }

}

input_audio_midi::~input_audio_midi()
{
  if(m_midiin)
    delete m_midiin;
}

bool input_audio_midi::can_run()
{
  return m_midiin? m_midiin->getPortCount() >= 1:false;
}

bool input_audio_midi::init()
{
  if( !m_midiin )
    return false;

  try {
    m_midiin->openPort( 0 );
    // Don't ignore sysex, timing, or active sensing messages.
    m_midiin->ignoreTypes( false, false, false );
    m_isValid = true;
  }
  catch ( RtError &error ) {
    m_isValid = false;
    error.printMessage();
  }

  return m_isValid;
}

void input_audio_midi::on_delete()
{
  if(m_isValid)
    m_midiin->closePort();
}

void input_audio_midi::module_info(vsx_module_info* info)
{
  //TODO: Re create the input params based on the available device capabilities for module configuration.
  info->output = 1;
  info->identifier = "audio;input;midi_in";
#ifndef VSX_NO_CLIENT
  info->description = "This module represents the state of a Midi Input Device";
  info->in_param_spec = "";
  info->out_param_spec = "";
  info->component_class = "output";
#endif

}

void input_audio_midi::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  VSX_UNUSED(in_parameters);
  VSX_UNUSED(out_parameters);
  loading_done = true;
}

void input_audio_midi::run()
{
  if( !m_isValid )
    return;

  std::vector<unsigned char> message;
  double stamp = m_midiin->getMessage( &message );
  for ( size_t i = 0; i < message.size(); i++ )
    std::cout << "Byte " << i << " = " << (int)message[i] << ", ";

  if ( message.size() > 0 )
    std::cout << "stamp = " << stamp << std::endl;
}
