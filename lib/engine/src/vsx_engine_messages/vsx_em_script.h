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


#ifndef VSXE_NO_GM

// COMPONENT VSXL
// gui asks for the contents of a vsxl filter (and or creating a new one)
if (cmd == "vsxl_cfl")
{
  vsx_comp* dest = get_by_name(c->parts[1]);
  if (dest)
  {
    vsx_comp_vsxl_driver_abs* driver;
    if (!dest->vsxl_modifier)
    {
      dest->vsxl_modifier = (vsx_comp_vsxl*)(new vsx_comp_vsxl());
      // load with default script
      driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)dest->vsxl_modifier)->load(dest->in_module_parameters,"");
      driver->comp = (void*)dest;
      driver->run();
      // notify the client!
      cmd_out->add_raw("vsxl_cfi_ok "+c->parts[1], VSX_COMMAND_GARBAGE_COLLECT);
    }
    else
    {
      driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)dest->vsxl_modifier)->get_driver();
    }
    if (driver)
    cmd_out->add_raw("vsxl_cfl_s "+c->parts[1]+" "+base64_encode(driver->script), VSX_COMMAND_GARBAGE_COLLECT);

  }
  goto process_message_queue_end;
}






// COMPONENT VSXL
// init component vsxl filter, run from macros
if (cmd == "vsxl_cfi")
{
  vsx_comp* dest = get_by_name(c->parts[1]);
  if (!dest)
    goto process_message_queue_end;

  vsx_comp_vsxl_driver_abs* driver;
  if (!dest->vsxl_modifier)
  {
    dest->vsxl_modifier = (vsx_comp_vsxl*)(new vsx_comp_vsxl());
    // load with default script
    driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)dest->vsxl_modifier)->load(dest->in_module_parameters,base64_decode(c->parts[2]));
    driver->comp = (void*)dest;
    driver->run();
    // notify the client!
    cmd_out->add_raw("vsxl_cfi_ok "+c->parts[1], VSX_COMMAND_GARBAGE_COLLECT);
  }
  else
    driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)dest->vsxl_modifier)->load((dest->in_module_parameters),base64_decode(c->parts[2]));
  goto process_message_queue_end;
}



// remove vsxl filter from the engine
if (cmd == "vsxl_cfr")
{
  vsx_comp* dest = get_by_name(c->parts[1]);
  if (dest)
  {
    //vsx_param_vsxl_driver_abs* driver;
    if (dest->vsxl_modifier)
    {
      ((vsx_comp_vsxl*)dest->vsxl_modifier)->unload();
      delete (vsx_comp_vsxl*)(dest->vsxl_modifier);
      dest->vsxl_modifier = 0;
      // send status to client
      cmd_out->add_raw("vsxl_cfr_ok "+c->parts[1], VSX_COMMAND_GARBAGE_COLLECT);
    }
  }
  goto process_message_queue_end;
}






// PARAMETER VSXL
// gui asks for the contents of a vsxl filter (and or creating a new one)
if (cmd == "vsxl_pfl")
{
  printf("pfl\n");
  vsx_comp* dest = get_by_name(c->parts[1]);
  if (dest)
  {
    vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);
    vsx_param_vsxl_driver_abs* driver = 0;
    if (!param->module_param->vsxl_modifier)
    {
      param->module_param->vsxl_modifier = (vsx_param_vsxl_abs*)(new vsx_param_vsxl());
      ((vsx_param_vsxl*)(param->module_param->vsxl_modifier))->engine = this;

      // load with default script
      driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->load(param->module_param,"");
      driver->interpolation_list = &interpolation_list;
      driver->comp = (void*)dest;
      driver->run();
      // notify the client!
      cmd_out->add_raw("vsxl_pfi_ok "+c->parts[1]+" "+c->parts[2], VSX_COMMAND_GARBAGE_COLLECT);
    }
    else
    {
      driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->get_driver();
    }

    if (driver)
    cmd_out->add_raw("vsxl_pfl_s "+c->parts[1]+" "+c->parts[2]+" "+base64_encode(driver->script), VSX_COMMAND_GARBAGE_COLLECT);
  }
  goto process_message_queue_end;
}






// init vsxl filter, run from macros
if (cmd == "vsxl_pfi")
{
  vsx_comp* dest = get_by_name(c->parts[1]);
  if (dest)
  {
    vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);
    vsx_param_vsxl_driver_abs* driver;
    if (!param->module_param->vsxl_modifier) {
      param->module_param->vsxl_modifier = (vsx_param_vsxl_abs*)(new vsx_param_vsxl());
      ((vsx_param_vsxl*)(param->module_param->vsxl_modifier))->engine = this;
      driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->load(param->module_param,base64_decode(c->parts[4]),s2i(c->parts[3]));

      driver->interpolation_list = &interpolation_list;
      driver->comp = (void*)dest;
      driver->run();

      // send status to client
      cmd_out->add_raw("vsxl_pfi_ok "+c->parts[1]+" "+c->parts[2], VSX_COMMAND_GARBAGE_COLLECT);
    } else
      driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->load(param->module_param,base64_decode(c->parts[4]));
    driver->run();
  }
  goto process_message_queue_end;
}






// remove vsxl filter from the engine
if (cmd == "vsxl_pfr")
{
  vsx_comp* dest = get_by_name(c->parts[1]);

  if (!dest)
    goto process_message_queue_end;

  vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);

  if (!param->module_param->vsxl_modifier)
    goto process_message_queue_end;


  // actual deletion
  ((vsx_param_vsxl*)param->module_param->vsxl_modifier)->unload();
  delete (vsx_param_vsxl_abs*)(param->module_param->vsxl_modifier);
  param->module_param->vsxl_modifier = 0;

  // send status to client
  cmd_out->add_raw("vsxl_pfr_ok "+c->parts[1]+" "+c->parts[2], VSX_COMMAND_GARBAGE_COLLECT);

  goto process_message_queue_end;
}

#endif // NO GM


