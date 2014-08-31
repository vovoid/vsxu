/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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


if (cmd == "note_create")
{
  static unsigned long note_counter = 0;
  c->parts[1] = "n"+vsx_string_helper::i2s(note_counter);
  vsx_note new_note;
  note_counter++;
  if (new_note.set(c))
  {
    note_map[c->parts[1]] = new_note;
    cmd_out->add_raw(new_note.serialize(), VSX_COMMAND_GARBAGE_COLLECT);
  }
  goto process_message_queue_end;
}




if (cmd == "note_update")
{
  vsx_note new_note;
  if (new_note.set(c))
  {
    note_map[c->parts[1]] = new_note;
  }
  goto process_message_queue_end;
}




if (cmd == "note_delete")
{
  note_iter = note_map.find(c->parts[1]);
  if (note_iter != note_map.end()) {
    note_map.erase(c->parts[1]);
    cmd_out->add_raw("note_delete_ok "+c->parts[1], VSX_COMMAND_GARBAGE_COLLECT);
  }
  goto process_message_queue_end;
}
