#include "vsxfst.h"
#include "vsx_param.h"
#include "vsx_module_dll_info.h"
#include "vsx_module.h"
#include "vsx_timer.h"
#include <list>
#include "vsx_command.h"
#include "vsx_comp_abs.h"
#include "vsx_param_abstraction.h"
#include "vsx_param_sequence.h"
#include "vsx_master_sequence_channel.h"
#include "vsx_param_sequence_list.h"
#include "vsx_param_interpolation.h"
#include "vsx_sequence_pool.h"

#include "vsx_engine.h"
#include "vsx_quaternion.h"
#include "vsx_bezier_calc.h"

using namespace std;

vsx_sequence_master_channel_item::vsx_sequence_master_channel_item()
{
	pool_sequence_list = 0x0;
	time_sequence.set_string("1.000000;1.000000;"+base64_encode("0.0")+"|0.000000;1.000000;"+base64_encode("1.0"));
	//printf("vsx_sequence_master_channel_item::constructor\n");
}

void vsx_sequence_master_channel_item::run(float abs_time)
{
	//we're getting time in between 0.0f and 1.0f, REAL TIME
//	vsx_sequence time_sequence; // how to interpret the time of the sequence list
//	float length;

	//	vsx_param_sequence_list* pool_sequence_list;

	if (pool_sequence_list)
	{
		// first get the time from the time_sequence
		// say abs = 4.0 and length = 5.0
		if (length == 0.0f) return;
		float time_sequence_pos = abs_time / length;
		float time_pos = 0.0f;
		if (time_sequence_pos < 1.0f)
		time_pos = time_sequence.execute_absolute(time_sequence_pos);
		//if (time_pos != time_pos) time_pos = 0.0f;
		//printf("abs_time: %f  length: %f\n",abs_time,length);
		/*printf("time_pos: %f  time_sequence_pos: %f  seq.i_time: %f  line_cur: %d   line_time: %f length: %f\n",
						time_pos,
						time_sequence_pos,
						time_sequence.i_time,
						time_sequence.line_cur,
						time_sequence.line_time,
						length
					);
		*/

		//printf("sequence length: %f\n", pool_sequence_list->calculate_total_time());
		//printf("inner run sequence: %f\n", pool_sequence_list->calculate_total_time() * time_pos);

		pool_sequence_list->run_absolute( pool_sequence_list->calculate_total_time() * time_pos);
//		pool_sequence_list->run(0.0f);
	}
}

void vsx_master_sequence_channel::run(float dtime)
{
	line_time += dtime;
	i_vtime += dtime;


	while (line_time < 0)
	{
		--line_cur;
		if (line_cur < 0) {
			//printf("__f__ %d ",c);
			line_cur = 0;
			line_time = 0;
			//cur_val = items[line_cur].value;
			//cur_delay = items[line_cur].delay;
			//cur_interpolation = items[line_cur].interpolation;
			//to_val = items[line_cur+1].value;
		} else {
			//printf("__a__ %d ",c);
			line_time +=  items[line_cur]->total_length;
			//cur_val = items[line_cur].value;
			//cur_delay = items[line_cur].delay;
			//cur_interpolation = items[line_cur].interpolation;
			//to_val = items[line_cur+1].value;
		}
	}
	/*while (line_time < 0.0f && line_cur) {
		line_cur--;
		line_time += items[line_cur]->total_length + line_time;
	}*/

	if ((size_t)line_cur+1 < items.size())
	{// if not, no need to do anything..
		if (line_time >= items[line_cur]->total_length)
		{
			line_time -= items[line_cur]->total_length;
			line_cur++;
		}
	}
	//printf("line_cur: %d ", line_cur);
	//printf("line_time: %f\n", line_time);
	if (line_time <= items[line_cur]->length)
	items[line_cur]->run(line_time);
}



//*********************************************************************************************************
//*********************************************************************************************************
//*********************************************************************************************************

//void vsx_master_sequence_channel::add_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix)
//{
	// 0=mseq_channel 1=row 2=add 3=[name] 4=[time]
	//total_time = 0.0f; // reset total time for re-calculation
//  vsx_sequence_master_channel_item ma;
  //pa.value = cmd_in->parts[5];
//  ma.length = s2f(cmd_in->parts[6]);
  //ma.interpolation = s2i(cmd_in->parts[7]);
  //lines.push_back(ma);
  //dest->add_raw(cmd_prefix+"pseq_r_ok add "+cmd_in->parts[4]+" a "+cmd_in->parts[5]+" "+cmd_in->parts[6]+" "+cmd_in->parts[7]);
//}


void vsx_master_sequence_channel::update_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix)
{
	//total_time = 0.0f; // reset total time for re-calculation
	// ____COMMAND STRUCTURE______________________
	// 0=mseq_channel
	// 1=row
	// 2=update
	// 3=[channel_name]
	// 4=[item_action_id]
	// 5=[local_time_distance if -1, don't set it]
	// 6=[length - if -1 don't set it]

	float p_vtime = i_vtime;

  //printf("UPDATE_MASTER_LINE in engine %s\n",cmd_in->raw.c_str());
  vsx_sequence_master_channel_item* pa = (items[ s2i(cmd_in->parts[4]) ]);

  // time distance (to next item)
  float local_time_distance;
  if ( (local_time_distance = s2f(cmd_in->parts[5])) != -1.0f)
  {
  	pa->total_length = local_time_distance;
  }

  // time length (internal length)
  float local_length;
  if ( (local_length = s2f(cmd_in->parts[6]))  !=  -1.0f )
  {
  	pa->length = local_length;
  }
  //dest->add_raw(cmd_prefix+"mseq_channel_ok row update "+cmd_in->parts[3]+" "+cmd_in->parts[4]+" "+cmd_in->parts[5]+" "+cmd_in->parts[6]);
  line_cur = 0;
  last_time = 0.0f;
  line_time = 0.0f;
	i_vtime = 0.0f;
	//printf("line time: %f\n",line_time);
	//printf("line_cur:  %d   items size: %d\n", line_cur, items.size());
	run(p_vtime);
	//printf("line time: %f\n",line_time);
	//printf("line_cur:  %d   items size: %d\n", line_cur, items.size());

}

// 0=mseq_channel 1=row 2=insert 3=[channel_name] 4=[item_action_id] 5=[local_time_distance] 6=[length] 7=[seq_pool_name]
void vsx_master_sequence_channel::insert_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix)
{
#ifndef VSX_NO_CLIENT
	float p_vtime = i_vtime;
  long after_this_id = s2i(cmd_in->parts[4]); // item_action_id
  float time_diff = s2f(cmd_in->parts[5]);
  vsx_sequence_master_channel_item* pa;
  std::vector<vsx_sequence_master_channel_item*>::iterator it = items.begin();
  float item_length = 0.1f;
  if (after_this_id == (long)items.size()-1)
  {
    //printf("last position\n");
    items[items.size()-1]->total_length = time_diff;
    pa = new vsx_sequence_master_channel_item;
    pa->pool_sequence_list = ((vsx_engine*)engine)->sequence_pool.get_sequence_list_by_name(cmd_in->parts[7]);
    item_length = pa->pool_sequence_list->calculate_total_time(true);
    if (item_length == 0.0f) item_length = 0.1f;
    pa->total_length = item_length;
    pa->length = item_length;
    pa->time_sequence.set_string("1.000000;1.000000;"+base64_encode("0.0")+"|0.000000;1.000000;"+base64_encode("1.0"));
    items.push_back(pa);
    //for (size_t i = 0; i < items.size(); i++)
    //{
    //	items[i]->time_sequence.set_string("1.000000;1.000000;"+base64_encode("0.0")+"|0.000000;1.000000;"+base64_encode("1.0"));
    //}
  } else {
    int i;
    for (i = 0; i < after_this_id; ++i) {
    	++it;
    }
    //printf("i : %d\n",i);
    pa = new vsx_sequence_master_channel_item();
    pa->pool_sequence_list = ((vsx_engine*)engine)->sequence_pool.get_sequence_list_by_name(cmd_in->parts[7]);
    item_length = pa->pool_sequence_list->calculate_total_time(true);
    pa->total_length = (*it)->total_length - time_diff;
    if (pa->total_length > item_length)
    {
    	pa->length = item_length;
    }
    else
    {
    	pa->length = pa->total_length;
    }
    (*it)->total_length = time_diff;
    ++it;
    // TODO: maybe: cut off the time of the default sequence here rather than scaling it?
    items.insert(it, pa);
  }
  pa->pool_name = cmd_in->parts[7];

  line_cur = 0;
  last_time = 0.0f;
  line_time = 0.0f;
	i_vtime = 0.0f;
	run(p_vtime);


  // 0=mseq_channel_ok 1=row 2=insert 3=[channel_name] 4=[item_action_id] 5=[local_time_distance] 6=[length] 7=[name]
  dest->add_raw(
  			cmd_prefix+
  			"mseq_channel_ok "+
  			"row "+
  			"insert "+
  			cmd_in->parts[3]+" "+
  			cmd_in->parts[4]+" "+
  			cmd_in->parts[5]+" "+
  			f2s(pa->length) +" "+ // [length]
  			cmd_in->parts[7]//+" "+

	);
  //printf("pseql_r insert %s\n",base64_decode(cmd_in->parts[4]).c_str());
#endif
}

void vsx_master_sequence_channel::i_remove_line(int pos)
{
	float p_vtime = i_vtime;
  std::vector<vsx_sequence_master_channel_item*>::iterator it = items.begin();
  if (pos != 0)
  {
    for (size_t i = 0; i < (size_t)pos; ++i) ++it;
    //++it;
    //(*it).delay -= delay;
    if (pos < (long)items.size()-1)
    {
      items[pos-1]->total_length += items[pos]->total_length;
    }
    items.erase(it);
  }
  line_cur = 0;
  last_time = 0.0f;
  line_time = 0.0f;
	i_vtime = 0.0f;
	run(p_vtime);
}

// 0=mseq_channel 1=row 2=remove 3=[channel_name] 4=[item_action_id]
void vsx_master_sequence_channel::remove_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix)
{
  //printf("REMOVE_LINE in engine %s\n",cmd_in->raw.c_str());
  long pos = s2i(cmd_in->parts[4]);

	i_remove_line(pos);

	if (dest)
  dest->add_raw(cmd_prefix+"mseq_channel_ok row remove "+cmd_in->parts[3]+" "+cmd_in->parts[4]);
}

void vsx_master_sequence_channel::remove_all_lines_referring_to_sequence_list(vsx_param_sequence_list* list)
{
	int run = 1;
	size_t i = 0;
	while (run)
	{
		if (items[i]->pool_sequence_list == list)
		{
			i_remove_line(i);
			i = 0;
		} else
		{
			i++;
		}
		if (i == items.size()) run = 0;
	}
}

// 0=mseq_channel 1=row 2=time_sequence 3=[name] 4=[item_id] 5=[get]/[set] 6(optional)=[sequence_dump]
void vsx_master_sequence_channel::time_sequence(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix)
{
  //printf("TIME_SEQUENCE_MASTER_LINE in engine %s\n",cmd_in->raw.c_str());
  vsx_sequence_master_channel_item* pa = (items[ s2i(cmd_in->parts[4]) ]);

  if (cmd_in->parts[5] == "set")
  {
  	pa->time_sequence.set_string(cmd_in->parts[6]);
  } else
  if (cmd_in->parts[5] == "get")
  {
  	dest->add_raw(
  				cmd_prefix+
  				"mseq_channel_ok "
  				"row "
  				"time_sequence "+
  				cmd_in->parts[3]+" "+
  				cmd_in->parts[4]+" "+
  				cmd_in->parts[5]+" "+
  				pa->time_sequence.get_string()
  	);
  }
  //dest->add_raw(cmd_prefix+"mseq_channel_ok row update "+cmd_in->parts[3]+" "+cmd_in->parts[4]+" "+cmd_in->parts[5]+" "+cmd_in->parts[6]);
}




void vsx_master_sequence_channel::set_time(float new_time)
{

}

vsx_string vsx_master_sequence_channel::dump()
{
#ifndef VSX_NO_CLIENT
  vsx_string res = "";
  std::list<vsx_string> ml;
  size_t i = 0;
  for (std::vector<vsx_sequence_master_channel_item*>::iterator it = items.begin(); it != items.end(); ++it)
  {
    //printf("adding dumpstring: %s\n",(f2s((*it).delay)+";"+f2s((*it).interpolation)+";"+base64_encode((*it).value)).c_str());
    ml.push_back(
    			f2s((*it)->total_length)+";"+
    			f2s((*it)->length)+";"+
    			base64_encode((*it)->pool_name)+";"+
    			//(i!=0?
    					base64_encode((*it)->time_sequence.get_string())
    					//:"")
    );
    i++;
  }
  vsx_string deli = "|";
  res = implode(ml, deli);
  return res;
#else
	return "";
#endif
}


void vsx_master_sequence_channel::inject(vsx_string inject_string)
{
	//printf("********BEGIN INJECT********************\n%s\n", inject_string.c_str());
	// the reverse of the dump method above
	delete (vsx_sequence_master_channel_item*)(items[0]);
	items.clear();
	vsx_string deli = "|";
	vsx_avector<vsx_string> parts;
	explode(inject_string, deli, parts);
	for (size_t i = 0; i < parts.size(); i++)
	{
		vsx_string ideli = ";";
		vsx_avector<vsx_string> iparts;
		explode(parts[i], ideli, iparts);
		// 0 = total_length, 1 = internal_length, 2 = pool_name, 3 = time_sequence_data
		//printf("inject at last position %s\n",parts[i].c_str());
		vsx_sequence_master_channel_item* pa = new vsx_sequence_master_channel_item;
		pa->total_length = s2f(iparts[0]);
		pa->length = s2f(iparts[1]);
		//printf("pa length: %f\n", pa->length);
		pa->pool_sequence_list = ((vsx_engine*)engine)->sequence_pool.get_sequence_list_by_name(base64_decode(iparts[2]));
		pa->pool_name = base64_decode(iparts[2]);
		// TODO: remove the following line, it's obsolete
		if (iparts.size() > 3)
		pa->time_sequence.set_string(base64_decode(iparts[3]));
		items.push_back(pa);
		//for (size_t i = 0; i < items.size(); i++)
		//{
			//items[i]->time_sequence.set_string("1.000000;1.000000;"+base64_encode("0.0")+"|0.000000;1.000000;"+base64_encode("1.0"));
		//}
	}
}

vsx_master_sequence_channel::vsx_master_sequence_channel()
{
	i_vtime = 0.0f;
	line_cur = 0;
	line_time = 0.0f;
	vsx_sequence_master_channel_item* item = new vsx_sequence_master_channel_item;
	item->total_length = 1.0f;
	item->length = 0.0f;
	items.push_back(item);
}
