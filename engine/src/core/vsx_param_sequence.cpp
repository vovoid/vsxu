#include "vsx_engine.h"
#include "vsx_param_sequence.h"


using namespace std;
//std::list<vsx_param_sequence*> vsx_param_sequence_list::ppl;
//std::map<vsx_engine_param*,vsx_param_sequence*> vsx_param_sequence_list::ppm;

vsx_bezier_calc bez_calc;

vsx_param_sequence_item::vsx_param_sequence_item() {
  total_length = 1.0f;
  value = "";
  interpolation = 1;
}

/*void vsx_param_sequence::execute_time(float stime) {
	line_cur = 0;
	while (stime > 0.0f)
	{
		if (stime > lines[line_cur].delay)
		{
			stime -= lines[line_cur].delay;
			line_cur++;
		} else {
			line_time = stime;
			stime = 0.0f;
		}
	}
	execute(0.0f);
}*/

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

void vsx_param_sequence::execute(float ptime) {
  if (items.size() < 2) {
    if (items.size()) {
      //if (param->type == VSX_MODULE_PARAM_ID_FLOAT) {
        param->set_string(items[0].value);
      //}
    }
  }
  //printf("vsx_param_sequence::execute %f\n", ptime);
  /*if (comp->local_engine_info.dtime+comp->local_engine_info.vtime == 0) {
    line_time = 0;
    line_cur = 0;
    return;
  } */
  //float ptime = comp->local_engine_info.dtime;  // time to go in one direction
  if (line_time == 0 && line_cur == 0) {
    if (cur_val == "") {
      cur_val = items[0].value;
      cur_delay = items[0].total_length;
      cur_interpolation = items[0].interpolation;
      if (items.size() > 1)
      to_val = items[1].value;
    }
  }

  int c = 0;

  line_time += ptime;
  //printf("ptime: %f  line_time: %f line_cur: %d\n",ptime,line_time,line_cur);

  if (ptime < 0) {
    //c = 1;
    //line_time += p_time;

    if ((line_time) < 0 && line_cur != 0) {
      //while (line_time + ptime < lines[line_cur].delay && !stop) {
      while (line_time < 0) {
        ++c;// = 10;
        //line_time = 0;
        --line_cur;
        if (line_cur < 0) {
          line_cur = 0;
          line_time = 0;
//          cur_val = lines[0].value;
          //stop = true;
          //cur_val = lines[line_cur].value;
          //cur_delay = lines[line_cur].delay;
          //cur_interpolation = lines[line_cur].interpolation;
          //to_val = lines[line_cur+1].value;
        } else {
          line_time +=  items[line_cur].total_length;
          cur_val = items[line_cur].value;
          cur_delay = items[line_cur].total_length;
          cur_interpolation = items[line_cur].interpolation;
          //if (lines.size() > 1)
          to_val = items[line_cur+1].value;
        }

      }
    }
  } else {
    //printf("ptime: %f ltime: %f\n",ptime,line_time);
    //printf("line_cur: %d\n",line_cur);
    //if (line_cur < lines.size()) {
      if (cur_delay != -1)
      while (line_time > cur_delay && cur_delay != -1) {
        ++c;
        //printf("while %s\n",lines[line_cur].value.c_str());

        //if (lines[line_cur].value != "")
        //if (lines[line_cur].interpolation) {
          //  vsx_string a = lines[line_cur].value;
//            vsx_string deli = ",";
//            std::vector<vsx_string> pp;
//            split_string(a,deli,pp);
//            int cc = 0;
  //          if (!pp.size()) pp.push_back(lines[line_cur].value);

//            for (std::vector<vsx_string>::iterator it = pp.begin(); it != pp.end(); ++it) {
//              ((vsx_engine*)engine)->interpolation_list.set_target_value(param, *it, cc, lines[line_cur].interpolation);
//              ++cc;
//            }
//        } else {
          //printf("setting param: %s\n",lines[line_cur].value.c_str());
          //param->set_string(lines[line_cur].value);
//        }
        //printf("line_cur: %d\n",line_cur);
        line_time -= items[line_cur].total_length;//lines[line_cur].delay;
        //ptime -= lines[line_cur].delay;
        ++line_cur;
        cur_interpolation = items[line_cur].interpolation;
        cur_delay = items[line_cur].total_length;
        cur_val = to_val;
        if (line_cur >= (int)items.size()-1) {
          //line_cur = 0;
          cur_delay = -1;
        } else {
          if (line_cur >= (int)items.size()-1)
          to_val = items[line_cur].value;
          else
          to_val = items[line_cur+1].value;
        }
      }
  }
  //printf("line_cur: %d  cur_val: %s to_val: %s line_time: %f curdel: \n",line_cur, cur_val.c_str(),to_val.c_str(),line_time,cur_delay);
    if (to_val.size() && cur_val.size()) {
      float t = (line_time/cur_delay);
      if (param->module_param->type == VSX_MODULE_PARAM_ID_FLOAT) {
        ++param->module->param_updates;
        ++((vsx_module_param_quaternion*)param->module_param)->updates;
        float cv = s2f(cur_val);
        float ev = s2f(to_val);
        float dv = ev-cv;
        if (cur_interpolation == 4) {
          //printf("handle1.x: %f\n",lines[line_cur].handle1.x);
          bez_calc.x0 = 0.0f;
          bez_calc.y0 = cv;
          bez_calc.x1 = items[line_cur].handle1.x;
          bez_calc.y1 = cv+items[line_cur].handle1.y;
          bez_calc.x2 = items[line_cur].handle2.x;
          bez_calc.y2 = ev+items[line_cur].handle2.y;
          bez_calc.x3 = 1.0f;
          bez_calc.y3 = ev;
          bez_calc.init();
          float tt = bez_calc.t_from_x(t);
          float rv = bez_calc.y_from_t(tt);
          //printf("rv: %f\n",rv);
          
          //param->set_string(f2s(rv));
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(rv);
        } else
        {

					//printf("sequence execute: %s %f\n", cur_val.c_str(), cv);
          // 0 = no interpolation
          // 1 = linear interpolation
          // 2 = cosine interpolation
          // 3 = no interpolation + param_interpolator

          if (cur_interpolation == 0) {
            ((vsx_module_param_quaternion*)param->module_param)->set_internal(cv);
            //param->set_string(f2s(cv));
          } else
          if (cur_interpolation == 1) {
            ((vsx_module_param_quaternion*)param->module_param)->set_internal(cv+dv*t);
            //param->set_string(f2s(cv+dv*t));
          } else
          if (cur_interpolation == 2) {
            float ft = t*pi_float;
            float f = (1 - cos(ft)) * 0.5f;
            ((vsx_module_param_quaternion*)param->module_param)->set_internal(cv*(1-f) + ev*f);
            //param->set_string(f2s(cv*(1-f) + ev*f));
          }
          #ifndef VSX_NO_CLIENT
          else
          if (cur_interpolation == 3) {
            //((vsx_engine*)engine)->interpolation_list.set_target_value(param, f2s(cv), 0, interp_time);
          }
          #endif
        }
      } else
      if (param->module_param->type == VSX_MODULE_PARAM_ID_QUATERNION) {
        vsx_quaternion cv, ev;
        cv.from_string(cur_val);
        ev.from_string(to_val);

        // 0 = no interpolation
        // 1 = linear interpolation
        // 2 = cosine interpolation
        // 3 = no interpolation + param_interpolator
        if (cur_interpolation == 0) {
          cv.normalize();
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(cv.x,0);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(cv.y,1);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(cv.z,2);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(cv.w,3);
        } else
        if (cur_interpolation == 1) {
          cv.normalize();
          ev.normalize();
          vsx_quaternion iq;
          iq.slerp(cv, ev, t);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(iq.x,0);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(iq.y,1);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(iq.z,2);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(iq.w,3);
        } else
        if (cur_interpolation == 2) {
          cv.normalize();
          ev.normalize();
          vsx_quaternion iq;
          iq.cos_slerp(cv, ev, t);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(iq.x,0);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(iq.y,1);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(iq.z,2);
          ((vsx_module_param_quaternion*)param->module_param)->set_internal(iq.w,3);
        }
      } else
      if (param->module_param->type == VSX_MODULE_PARAM_ID_FLOAT3) {
      }       //} else
      //param->set_string(f2s(ev));
    }
    //if (!c)
  //}
}

float vsx_param_sequence::calculate_total_time(bool no_cache)
{
	if (no_cache)
  total_time = 0.0f;
	else
	if (total_time != 0.0f) return total_time;

  float last_length;
  for (std::vector<vsx_param_sequence_item>::iterator it = items.begin(); it != items.end(); ++it) {
  	//printf("adding to total_time: %f\n", (*it).total_length);
    total_time += (*it).total_length;
    last_length = (*it).total_length;
  }
  total_time -= last_length;
  return total_time;
}

vsx_string vsx_param_sequence::dump()
{
#ifndef VSX_NO_CLIENT
  vsx_string res = "";
  std::list<vsx_string> ml;
  for (std::vector<vsx_param_sequence_item>::iterator it = items.begin(); it != items.end(); ++it)
  {
    //printf("adding dumpstring: %s\n",(f2s((*it).delay)+";"+f2s((*it).interpolation)+";"+base64_encode((*it).value)).c_str());
    ml.push_back(f2s((*it).total_length)+";"+i2s((*it).interpolation)+";"+base64_encode((*it).get_value()));
  }
  vsx_string deli = "|";
  res = implode(ml, deli);
  return res;
#else
	return "";
#endif
}

void vsx_param_sequence::inject(vsx_string ij) {
	total_time = 0.0f; // reset total time for re-calculation
  items.clear();
  vsx_string deli = "|";
  std::list<vsx_string> pl;
  explode(ij, deli, pl);
  for (std::list<vsx_string>::iterator it = pl.begin(); it != pl.end(); ++it) {
    std::vector<vsx_string> pld;
    vsx_string pdeli = ";";
    explode((*it),pdeli,pld);
    vsx_param_sequence_item pa;
    pa.total_length = s2f(pld[0]);
    pa.interpolation = s2i(pld[1]);
    if (pa.interpolation < 4) {
      pa.value = base64_decode(pld[2]);
    } else
    if (pa.interpolation == 4) {
      std::vector<vsx_string> pld_l;
      vsx_string pdeli_l = ":";
      vsx_string vtemp = base64_decode(pld[2]);
      //printf("value: %s\n",vtemp.c_str());
      explode(vtemp,pdeli_l,pld_l);
      pa.value = pld_l[0];
      pa.handle1.from_string(pld_l[1]);
      pa.handle2.from_string(pld_l[2]);
    }
    items.push_back(pa);
    //printf("inject delay: %s\n",pld[0].c_str());
  }
}

vsx_param_sequence::vsx_param_sequence(int p_type,vsx_engine_param* param) {
  interp_time = 10;
  cur_val = to_val = "";
  last_time = 0.0f;
  line_time = 0.0f;
  line_cur = 0;
  p_time = 0;
  vsx_param_sequence_item pa;
  pa.total_length = 3;

  switch (p_type) {
    case VSX_MODULE_PARAM_ID_FLOAT:
    {
      pa.interpolation = 1;
      pa.value =  f2s(((vsx_module_param_float*)param->module_param)->get());//f2s(0.0f);
      items.push_back(pa);
      items.push_back(pa);
    } break;
    case VSX_MODULE_PARAM_ID_QUATERNION:
    {
      pa.interpolation = 0;
      pa.value = param->get_string();
      //pa.value = "0.0,0.0,0.0,1.0";
      items.push_back(pa);
      items.push_back(pa);
    } break;
  }
}

void vsx_param_sequence::rescale_time(float start, float scale) {
	total_time = 0.0f; // reset total time for re-calculation
  float accum_time = 0.0f;
  bool first = true;
  for (unsigned long i = 0; i < items.size(); ++i) {
    accum_time += items[i].total_length;
    if (accum_time > start) {
      // we found our target position yay
      if (first) {
        // cut in half
        float second_half = accum_time-start;
        float first_half = items[i].total_length - second_half;
        items[i].total_length = first_half + second_half*scale;
        first = false;
      } else {
        items[i].total_length *= scale;
      }
    }
  }
}

vsx_param_sequence::vsx_param_sequence() {
  interp_time = 10;
  cur_val = to_val = "";
  last_time = 0.0f;
  line_time = 0.0f;
  line_cur = 0;
  p_time = 0;
	total_time = 0.0f;
}

/*void vsx_param_sequence::add_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix) {
#ifndef VSX_NO_CLIENT
	total_time = 0.0f; // reset total time for re-calculation
  vsx_param_sequence_item pa;
  pa.value = cmd_in->parts[5];
  pa.delay = s2f(cmd_in->parts[6]);
  pa.interpolation = s2i(cmd_in->parts[7]);
  lines.push_back(pa);
  dest->add_raw(cmd_prefix+"pseq_r_ok add "+cmd_in->parts[4]+" a "+cmd_in->parts[5]+" "+cmd_in->parts[6]+" "+cmd_in->parts[7]);
#endif
  //printf("pseq_r_ok add %s\n",cmd_in->parts[4].c_str());
}*/

void vsx_param_sequence::update_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix)
{
#ifndef VSX_NO_CLIENT
	total_time = 0.0f; // reset total time for re-calculation
#ifdef VSXU_DEBUG
  printf("UPDATE_LINE in engine %s\n",cmd_in->raw.c_str());
#endif
  vsx_param_sequence_item pa;
  pa.total_length = s2f(cmd_in->parts[5]);
  pa.interpolation = s2i(cmd_in->parts[6]);
  if (pa.interpolation < 4)
  {
    pa.value = base64_decode(cmd_in->parts[4]);
  	//printf("value in string format: %s\n", pa.value.c_str());
  }
  else
  if (pa.interpolation == 4) {
    std::vector<vsx_string> pld_l;
    vsx_string pdeli_l = ":";
    vsx_string vtemp = base64_decode(cmd_in->parts[4]);
    //printf("value: %s\n",vtemp.c_str());
    explode(vtemp,pdeli_l,pld_l);
    pa.value = pld_l[0];
    pa.handle1.from_string(pld_l[1]);
    pa.handle2.from_string(pld_l[2]);
  }

  /*for (size_t i = 0; i < cmd_in->parts.size(); i++) {
  	printf("i %d: %s\n", i, cmd_in->parts[i].c_str());
  }*/

  items[s2i(cmd_in->parts[7])] = pa;
  //dest->add_raw(cmd_prefix+"pseq_r_ok update "+cmd_in->parts[2]+" "+cmd_in->parts[3]+" "+cmd_in->parts[4]+" "+cmd_in->parts[5]+" "+cmd_in->parts[6]+" "+cmd_in->parts[7]);
  cur_val = to_val = "";
  last_time = 0.0;
  line_time = 0.0;
  line_cur = 0;
  p_time = 0;
//  printf("pseql_r a %s\n",cmd_in->parts[4].c_str());
#endif
}

void vsx_param_sequence::insert_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix) {
#ifndef VSX_NO_CLIENT
	total_time = 0.0f; // reset total time for re-calculation
  //printf("INSERT_LINE in engine %s\n",cmd_in->raw.c_str());
  long after_pos = s2i(cmd_in->parts[7]);
  float delay = s2f(cmd_in->parts[5]);
  //printf("delay: %f\n",delay);
  std::vector<vsx_param_sequence_item>::iterator it = items.begin();
  if (after_pos == (long)items.size()-1) {
    printf("last position, interpolation type: %s\n",cmd_in->parts[6].c_str());
    items[items.size()-1].total_length = delay;
    vsx_param_sequence_item pa;
    pa.value = base64_decode(cmd_in->parts[4]);
    pa.total_length = 1;
    pa.interpolation = s2i(cmd_in->parts[6]);
    items.push_back(pa);
  } else {
    int i;
    for (i = 0; i < after_pos; ++i) {++it;
    //  printf("incrementing it\n");
    }

    //printf("i : %d\n",i);

    vsx_param_sequence_item pa;
    pa.total_length = (*it).total_length-delay;
    (*it).total_length = delay;
    pa.interpolation = s2i(cmd_in->parts[6]);
    if (pa.interpolation < 4) {
      pa.value = base64_decode(cmd_in->parts[4]);
    } else
    if (pa.interpolation == 4) {
      std::vector<vsx_string> pld_l;
      vsx_string pdeli_l = ":";
      vsx_string vtemp = base64_decode(cmd_in->parts[4]);
      //printf("value: %s\n",vtemp.c_str());
      explode(vtemp,pdeli_l,pld_l);
      pa.value = pld_l[0];
      pa.handle1.from_string(pld_l[1]);
      pa.handle2.from_string(pld_l[2]);
    }

    ++it;
    items.insert(it, pa);
  }
  cur_val = to_val = "";
  last_time = 0.0;
  line_time = 0.0;
  line_cur = 0;
  p_time = 0;
//  lines[s2i(cmd_in->parts[8])] = pa;
  dest->add_raw(cmd_prefix+"pseq_r_ok insert "+cmd_in->parts[2]+" "+param->name+" "+cmd_in->parts[4]+" "+cmd_in->parts[5]+" "+cmd_in->parts[6]+" "+cmd_in->parts[7]);
  //printf("pseql_r insert %s\n",base64_decode(cmd_in->parts[4]).c_str());
#endif
}

void vsx_param_sequence::remove_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix) {
#ifndef VSX_NO_CLIENT
	total_time = 0.0f; // reset total time for re-calculation
  //printf("REMOVE_LINE in engine %s\n",cmd_in->raw.c_str());
  long pos = s2i(cmd_in->parts[4]);
  std::vector<vsx_param_sequence_item>::iterator it = items.begin();
  if (pos != 0) {
    for (int i = 0; i < pos; ++i) ++it;
    //++it;
    //(*it).delay -= delay;
    if (pos < (long)items.size()-1) {
      items[pos-1].total_length += items[pos].total_length;
    }
    items.erase(it);
  }
  cur_val = to_val = "";
  last_time = 0.0;
  line_time = 0.0;
  line_cur = 0;
  p_time = 0;

  dest->add_raw(cmd_prefix+"pseq_r_ok remove "+cmd_in->parts[2]+" "+cmd_in->parts[3]+" "+cmd_in->parts[4]);
#endif
}

//----------------------------------------------------------------------

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

