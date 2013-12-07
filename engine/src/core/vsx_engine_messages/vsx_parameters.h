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


if (cmd == "pa_ren")
{
  vsx_comp* dest = get_component_by_name(c->parts[1]);
  if (!dest)
    goto process_message_queue_end;

  bool ok;
  if (c->parts[4] == "-1") {
    ok = dest->get_params_in()->alias_rename(c->parts[2],c->parts[3]);
  } else {
    ok = dest->get_params_out()->alias_rename(c->parts[2],c->parts[3]);
  }
  if (ok)
    cmd_out->add_raw("pa_ren_ok "+c->parts[1]+" "+c->parts[2]+" "+c->parts[3]+" "+c->parts[4]);
  else
    cmd_out->add_raw("alert_fail "+base64_encode(c->raw)+" Error "+base64_encode("Either param is not alias, was changed by someone else on this server or other error."));
  goto process_message_queue_end;
}






if (cmd == "param_get" || cmd == "pgo")
{
  // syntax:
  //  param_get [component] [param] [extra_info]
  if (!c->parts.size() >= 3)
    goto process_message_queue_end;

  vsx_comp* dest = get_component_by_name(c->parts[1]);
  if (!dest)
    goto process_message_queue_end;

  vsx_engine_param* param;

  if (cmd == "pgo")
    param = dest->get_params_out()->get_by_name(c->parts[2]);
  else
    param = dest->get_params_in()->get_by_name(c->parts[2]);

  if (!param)
    goto process_message_queue_end;

  vsx_string _value = param->get_string();


  if (!_value.size())
    goto process_message_queue_end;

  if (_value.size() > 100)
    _value = _value.substr(0,100)+"...";

  vsx_string value = base64_encode(_value);
  vsx_string extra = "";

  if (c->parts.size() == 4)
    extra = " "+c->parts[3];

  // syntax:
  //  param_get_ok [component] [param] [value] [extra_info]
  cmd_out->add_raw("param_get_ok "+c->parts[1]+" "+c->parts[2]+" "+value+extra);

  goto process_message_queue_end;
}







if (cmd == "pg64")
{
  // syntax:
  //  param_get [component] [param] [extra_info]
  if (c->parts.size() >= 3)
  {
    vsx_comp* dest = get_component_by_name(c->parts[1]);
    if (dest) {
      vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);
      if (param) {
        vsx_string value = base64_encode(param->get_string());
        vsx_string extra = "";
        if (c->parts.size() == 4) extra = " "+c->parts[3];
        // syntax:
        //  param_get_ok [component] [param] [value] [extra_info]
        cmd_out->add_raw("pg64_ok "+c->parts[1]+" "+c->parts[2]+" "+value+extra);
      }
    }
  }
  goto process_message_queue_end;
}








if (cmd == "ps64")
{
  // syntax:
  //  param_set [component] [param] [value]
  if (c->parts.size() == 4)
  {
    vsx_comp* dest = get_component_by_name(c->parts[1]);
    if (dest) {
      vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);
      if (param) {
        param->set_string(base64_decode(c->parts[3]));
        param->module->param_set_notify(c->parts[2]);
        if (param->module->redeclare_in) {
          redeclare_in_params(dest,cmd_out);
        }
      }
      else cmd_out->add_raw("alert_fail "+base64_encode(c->raw)+" Error "+base64_encode("Param does not exist!"));
    }
    else cmd_out->add_raw("alert_fail "+base64_encode(c->raw)+" Error "+base64_encode("Component "+c->parts[1]+" does not exist!"));
  }
  goto process_message_queue_end;
}







if (cmd == "ps")
{
  // syntax:
  //  ps [component] [param] [value]
  if (c->parts.size() == 4)
  {
    vsx_comp* dest = get_component_by_name(c->parts[1]);
    if (dest) {
      vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);
      if (param) {
        param->set_string(c->parts[3]);
        param->module->param_set_notify(c->parts[2]);
        if (param->module->redeclare_in) {
          redeclare_in_params(dest,cmd_out);
        }
      }
      else cmd_out->add_raw("alert_fail "+base64_encode(c->raw)+" Error "+base64_encode("Param does not exist!"));
    }
    else cmd_out->add_raw("alert_fail "+base64_encode(c->raw)+" Error "+base64_encode("Component "+c->parts[1]+" does not exist!"));
  }
  goto process_message_queue_end;
}







if (cmd == "param_set")
{
  // this is for float3 and such where multiple arity values have to be set in one command.
  // as the last argument is a comma-separated list of values the character "," is banned
  // from values.
  // syntax:
  //   param_set [component] [parameter] [value],[value],
  if (c->parts.size() == 4)
  {
    vsx_comp* dest = get_component_by_name(c->parts[1]);
    if (dest) {
      vsx_engine_param* ep = dest->get_params_in()->get_by_name(c->parts[2]);
      if (ep) {
        vsx_string a = c->parts[3];
        vsx_string deli = ",";
        std::vector<vsx_string> pp;
        explode(a,deli,pp);
        if (!pp.size()) pp.push_back(c->parts[3]);
        int cc = 0;

        interpolation_list.remove(ep);

        for (std::vector<vsx_string>::iterator it = pp.begin(); it != pp.end(); ++it) {
          ep->set_string(*it,cc);
          ++cc;
        }
        ep->module->param_set_notify(c->parts[2]);
        if (ep->module->redeclare_in) {
          redeclare_in_params(dest,cmd_out);
        }
      }
    }
  }
  goto process_message_queue_end;
}







if (cmd == "param_set_interpolate")
{
  // this is for float3 and such where multiple arity values have to be set in one command.
  // as the last argument is a comma-separated list of values the character "," is banned
  // from values.
  // syntax:
  //   param_set_interpolate [component] [parameter] [value],[value],... [speed]
  if (c->parts.size() >= 5)
  {
    vsx_comp* dest = get_component_by_name(c->parts[1]);
    if (dest) {
      vsx_engine_param* e_param = dest->get_params_in()->get_by_name(c->parts[2]);
      if (e_param) {
        if (!e_param->sequence) {
          vsx_string a = c->parts[3];
          vsx_string deli = ",";
          std::vector<vsx_string> pp;
          split_string(a,deli,pp);
          int cc = 0;
          if (!pp.size()) pp.push_back(c->parts[3]);

          float interp_time = 16;
          if (c->parts.size() == 5)
          interp_time = s2f(c->parts[4]);

          for (std::vector<vsx_string>::iterator it = pp.begin(); it != pp.end(); ++it) {
            interpolation_list.set_target_value(e_param, *it, cc, interp_time);
            ++cc;
          }
        } else cmd_out->add_raw("alert_fail "+base64_encode(c->raw)+" Error "+base64_encode("Param is controlled by sequencer!"));
      } else cmd_out->add_raw("alert_fail "+base64_encode(c->raw)+" Error "+base64_encode("Param does not exist!"));
    }
  }
  goto process_message_queue_end;
}








if (cmd == "param_set_default")
{
  vsx_comp* dest = get_component_by_name(c->parts[1]);

  // sanity
  if (!dest)
    goto process_message_queue_end;

  vsx_module_param_abs* param = dest->get_params_in()->get_by_name(c->parts[2])->module_param;

  // sanity
  if (!param)
    goto process_message_queue_end;

  param->set_default();

  goto process_message_queue_end;
}








// parameter flag - boolean toggles for the param
// syntax:
//  0     1           2           3     4
//  pflag [component] [parameter] [key] [value]
// example:
//  pflag simple angle external_expose 1
if (cmd == "pflag")
{
  vsx_comp* dest = get_component_by_name(c->parts[1]);
  if (dest)
  {
    vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);
    if (param)
    {
      if (c->parts[3] == "external_expose")
      {
        param->external_expose = vsx_string_aux::s2i( c->parts[4] );
      }
    }
  }
  goto process_message_queue_end;
}



