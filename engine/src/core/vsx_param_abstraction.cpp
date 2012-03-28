#include "vsx_param.h"
#include "vsx_module_dll_info.h"
#include "vsx_module.h"
#include "vsx_command.h"
class vsx_engine_param_list;
#include "vsx_comp_abs.h"
#include "vsx_comp_channel.h"
#include "vsx_param_abstraction.h"
#include "vsx_timer.h"
#include "vsx_comp_channel.h"
#include "vsx_param_interpolation.h"
#include "scripting/vsx_param_vsxl.h"
#include "vsx_log.h"

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

vsx_engine_param::vsx_engine_param() {
  sequence = false;
  alias = false;
  alias_parent = 0;
//  alias_child = 0;
  channel = 0;
}

int vsx_engine_param::connect_abs(vsx_engine_param* src, vsx_channel_connection_info* channel_connection, int order, int position) {
//  printf("vsx_engine_param::connect_abs %s %s\n",src->name.c_str(),name.c_str());
  // find this connection already
  for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    //printf("connection channel id: %d\n",(*it)->connection_order);
    if ((*it)->src == src && (*it)->dest == this)
    return 0;
  }

  vsx_engine_param_connection* engine_conn = new vsx_engine_param_connection;
  engine_conn->src = src;
  engine_conn->dest = this;
  //printf("order: %d\n",order);
  engine_conn->connection_order = order;
  engine_conn->owner = this;
  engine_conn->alias_connection = false;
  channel_connection->engine_connection = engine_conn;
  engine_conn->channel_connection = channel_connection;
  //BUUUUUUUU!
  src->connections.push_back(engine_conn);
  if (position == -1 || !connections.size())
  connections.insert(connections.begin(),engine_conn);
  else if (position == -2)
  connections.push_back(engine_conn);
  else
  //connections.push_back(engine_conn);
  //else
  //if (position == 0)
  //connections.insert(connections.begin(),engine_conn);

  {


    if (position < (int)connections.size()) {
      if (connections[position]->connection_order == position || connections[position]->connection_order == -1)
      {
        // m�ste l�gga till EFTER denna eftersom den redan finns.
        //printf("going the hard way1,..\n");
        int c = 0;
        std::vector<vsx_engine_param_connection*>::iterator it = connections.begin();
        bool run = true;
        while (run) {
          //printf("c IIIII %d\n",c);
          if (c == position+1) {
            connections.insert(it, engine_conn);
            run = false;
          } else
          if (it == connections.end()) {
            connections.insert(it, engine_conn);
            run = false;
    //        param_id_list.push_back(new_param);
          }
          ++it;
          ++c;
        }
      }
      else {
        // vi kan l�gga till den F�RE positionen
        //printf("going the hard way2,..\n");
        int c = 0;
        std::vector<vsx_engine_param_connection*>::iterator it = connections.begin();
        bool run = true;
        while (run) {
          //printf("c IIIII %d\n",c);
          if (c == position) {
            connections.insert(it, engine_conn);
            run = false;
          } else
          if (it == connections.end()) {
            connections.insert(it, engine_conn);
            run = false;
    //        param_id_list.push_back(new_param);
          }
          ++it;
          ++c;
        }
      }
    } else {
      // vi kan l�gga till den F�RE positionen
      //printf("going the hard way,..\n");
      int c = 0;
      std::vector<vsx_engine_param_connection*>::iterator it = connections.begin();
      bool run = true;
      while (run) {
        //printf("c IIIII %d\n",c);
        if (c == position) {
          connections.insert(it, engine_conn);
          run = false;
        } else
        if (it == connections.end()) {
          connections.insert(it, engine_conn);
          run = false;
  //        param_id_list.push_back(new_param);
        }
        ++it;
        ++c;
      }

    }


    //connections.insert(connections.begin(),engine_conn);
    //connections.push_back(engine_conn);
    /*// find where to insert
    std::vector<vsx_engine_param_connection*>::iterator it = connections.begin();
    int c = 0;
    while (++c < position) ++it;
    connections.insert(it,engine_conn);*/
  }
  std::vector<int> new_channel_order;
  alias_owner->rebuild_orders(&new_channel_order);
  for (unsigned long i = 0; i < new_channel_order.size(); ++i) {
    //printf("value: %d\n",new_channel_order[i]);
  }
  alias_owner->channel->connections_order(&new_channel_order);
  return order;
}

int vsx_engine_param::connect(vsx_engine_param* src) {
//  printf("vsx_engine_param::connect\n");
  // 1. create the low-level link on component level
    // for that we need to access that
    vsx_engine_param* real_dest_param;
    vsx_engine_param* real_src_param;
    if (alias) real_dest_param = alias_owner; else real_dest_param = this;
    if (src->alias) real_src_param = src->alias_owner; else real_src_param = src;

    // clear the value if needed
    clean_up_module_param(real_dest_param->module_param);

    vsx_channel_connection_info* channel_connection = real_dest_param->channel->connect(real_src_param);
    if (!channel_connection) return -1;


  vsx_engine_param_connection_info cinfo;
  cinfo.connection_order = real_dest_param->channel->connections.size()-1;
  cinfo.localorder = connections.size();
  cinfo.num_dest_connections = 0;
  cinfo.src = src;
  cinfo.dest = this;
  cinfo.channel_connection = channel_connection;
  return connect_far_abs(&cinfo,-2);
/*  // 2. create a connection and add it to our list
    vsx_engine_param_connection* engine_conn = new vsx_engine_param_connection;
    // push the connection on both source and dest for traversing later
    src->connections.push_back(engine_conn);
    connections.push_back(engine_conn);
    // set the channel's connection to our connection for cross reference
    channel_connection->engine_connection = engine_conn;

    // MEKKARA BEEEEAMU!!!
    engine_conn->src = src;
    engine_conn->dest = this;
    engine_conn->connection_order = real_dest_param->channel->connections.size()-1; // as new connections are always added in the end we can do this :>
    engine_conn->owner = this;
    engine_conn->alias_connection = false;
    engine_conn->channel_connection = channel_connection;

    std::vector<int> new_channel_order;
    alias_owner->rebuild_orders(&new_channel_order);
    // 3. tell channel to rebuild with new list
    alias_owner->channel->connections_order(&new_channel_order);


  return connections.size()-1;//engine_conn->connection_order;
  */
}

vsx_engine_param* vsx_engine_param::alias_to_level(vsx_engine_param* dest) {
//#ifndef VSX_NO_CLIENT
//  printf("alias_to_level> %s\n",dest->name.c_str());
  // this will only work for params belonging to a io == 1 list
  if (owner->io == 1) {
    vsx_string src_name = owner->component->name;
    vsx_string dest_name = dest->owner->component->name;
//    printf("src_name = %s\n",src_name.c_str());
//    printf("dest_name = %s\n",dest_name.c_str());
    str_remove_equal_prefix(&src_name, &dest_name,".");

    vsx_string deli = ".";
    std::vector<vsx_string> dest_name_parts;
    explode(dest_name,deli,dest_name_parts);
    dest_name_parts.pop_back();
    dest_name = implode(dest_name_parts,deli);

    std::vector<vsx_string> src_name_parts;
    explode(src_name,deli,src_name_parts);
    src_name_parts.pop_back();
    dest_name = implode(src_name_parts,deli);
//    printf("src_name_f = %s\n",src_name.c_str());
//    printf("dest_name_f = %s\n",dest_name.c_str());

    if (dest_name_parts.size() == 0) {
      if (src_name_parts.size() == 0) return this; else {
        // go through our connections to see if there is an alias already

        for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
          if ((*it)->alias_connection) return (*it)->dest->alias_to_level(dest);
        }

        vsx_string new_name = owner->component->parent->get_params_out()->alias_get_unique_name("alias_"+name);
        //printf("new alias_to_level %s\n",new_name.c_str());
        owner->component->parent->get_params_out()->alias(this,new_name,-1);
        return owner->component->parent->get_params_out()->get_by_name(new_name)->alias_to_level(dest);
      }
    } else return this;
  }
  return 0;
//#endif
}

int vsx_engine_param::connect_far_abs(vsx_engine_param_connection_info* info,int order,vsx_engine_param* referrer) {
//#ifndef VSX_NO_CLIENT
  // bygg �t v�rt h�ll, bygg fr�n andra h�llet, koppla ihop med vanlig connection
  //printf("........\nconnect_far_abs src: %s dest:%s localorder %d\n",info->src->owner->component->name.c_str(),name.c_str(),order);
  // low level link already exists or is not our concern.
  // 1. find out and remove the part of the name that is a common base
  if (owner->io == -1) {
    vsx_string src_name = info->src->owner->component->name;
    vsx_string dest_name = owner->component->name;
//    printf("src_name = %s\n",src_name.c_str());
//    printf("dest_name = %s\n",dest_name.c_str());
    str_remove_equal_prefix(&src_name, &dest_name, ".");
//    printf("src_name = %s\n",src_name.c_str());
//    printf("dest_name = %s\n",dest_name.c_str());
//    printf("'''''''''''\n");
    if (src_name == "" && info->src->alias) {
      //printf("moment 22:1\n");
      info->src = info->src->alias_parent;
      return connect_far_abs(info,order);
    } else
    if (dest_name == "" && alias) {
      // p� v�nstra sidan, stopp
      //printf("moment 22:2\n");


      // v�ran connection skall in efter aliasets connection
      // ta reda p� vilket ID v�r parent har och sl�ng in order d�refter
      int c = 0;
      int norder = -1;
      for (std::vector<vsx_engine_param_connection*>::iterator it = alias_parent->connections.begin(); it != alias_parent->connections.end(); ++it) {
        if ((*it)->dest == this) {
          // found our match
          //printf("found it!\n");
          norder = c;
          //it = alias_parent->connections.end();
        }
        ++c;
      }

      //printf("norder == %d\n",norder);

      if ((info->localorder+1) > info->num_dest_connections/2)
      {
        //printf("second half\n");
        return alias_parent->connect_far_abs(info,norder,this);
      }
      else
      {
        //printf("first half\n");
        return alias_parent->connect_far_abs(info,norder-1,this);
      }
    }

    vsx_string deli = ".";
    std::vector<vsx_string> dest_name_parts;
    explode(dest_name,deli,dest_name_parts);
    dest_name_parts.pop_back();
    dest_name = implode(dest_name_parts,deli);

    std::vector<vsx_string> src_name_parts;
    explode(src_name,deli,src_name_parts);
    src_name_parts.pop_back();
    src_name = implode(src_name_parts,deli);


    if (dest_name_parts.size() == 0) {
      // check if we can make a clean connection to the src
      // if not, ask src to build to this level.
      vsx_engine_param* new_source = info->src;
      if (src_name != "") {
        new_source = info->src->alias_to_level(this);
      }
      // we need the src to alias itself down to our level
      if (new_source) {
        //printf("info->src->name: %s\n",info->src->name.c_str());
        //printf("order: %d\n",order);
        // enda s�ttet att veta att det inte hoppats fr�n n�t ALIAS
        if (!referrer) {
          if (info->connection_order > (int)connections.size()+1000) info->connection_order = connections.size();
          return connect_abs(new_source, info->channel_connection, info->connection_order, order);

        } else {
          if (order == -1) {
            // insert in the beginning
            if (info->connection_order > (int)connections.size()+1000) info->connection_order = connections.size();
            return connect_abs(new_source, info->channel_connection, info->connection_order, -1);
          } else {
            if (info->connection_order > (int)connections.size()+1000) info->connection_order = connections.size();
            return connect_abs(new_source, info->channel_connection, info->connection_order, order);
          }
        }
      }
    } else {
      // go through our list of connections to find an alias
      vsx_engine_param_connection* our_alias = 0;
      std::vector<vsx_engine_param_connection*>::iterator it = connections.begin();
      bool drun = true;
      int count = 0;
      if (connections.size())
      while (drun) {
//        printf("a connections size %d\n",connections.size());
        if ((*it)->alias_connection) {
          our_alias = *it;
//          printf("drun = false\n");
          drun = false;
          ++count;
        }
        if (drun) {
          ++count;
          ++it;
          if (it == connections.end()) drun = false;
        }
      }
      if (our_alias) {
        // alias already exists, send our request further down the chain
        int neworder;
        //printf("FFFFFFFFFFFF info->localorder: %d\n",info->localorder);
        // om det �r andra hoppet ut i ett alias
        if (info->localorder != -1) {
          // om �vre halvan
          if (count > info->localorder) neworder = -1; // l�gg till i b�rjan (ute i aliaset)
          else
          // l�gg till i slutet efter att hoppa ut (till v�nster)
          neworder = -2;
//          printf("ordering %d %d %d\n",count, info->localorder, neworder);
          // f�r att beh�lla uppe- eller nere-statusen
          info->localorder = -1;
        } else neworder = order;

        our_alias->dest->connect_far_abs(info,neworder,this);
      } else {
        // we need to create this alias
//        printf("creating alias %s\n",("alias_"+name).c_str());
        vsx_string new_name = owner->component->parent->get_params_in()->alias_get_unique_name("alias_"+name);
        // we're the first iteration here
        if (order == -1) {
          owner->component->parent->get_params_in()->alias(this,new_name,-1);
        } else
        {
          //printf("kabrompfelpoff: %d\n",info->localorder);
          owner->component->parent->get_params_in()->alias(this,new_name,info->localorder);
        }

        return owner->component->parent->get_params_in()->get_by_name(new_name)->connect_far_abs(info,order,this);
      }
    }
  }
  return 0;
//#endif
}

int vsx_engine_param::disconnect(vsx_engine_param* src, bool lowlevel) {

  if (!lowlevel) {
      vsx_engine_param_connection* engine_conn = get_conn_by_src(src);

      src->delete_conn(engine_conn);
      delete_conn(engine_conn);
      delete engine_conn;
  } else
  {
    // 1. disconnect the low level stuff
    vsx_engine_param* real_dest_param;
    vsx_engine_param* real_src_param;

    if (alias) real_dest_param = alias_owner; else {
      real_dest_param = this;
    }
    if (src->alias) real_src_param = src->alias_owner; else real_src_param = src;
    if (real_dest_param->channel->disconnect(real_src_param)) {
    // 2. that went well, so disconnect our connection as well
      //printf("disconn OK\n");
      vsx_engine_param_connection* engine_conn = get_conn_by_src(src);
      src->delete_conn(engine_conn);
      delete_conn(engine_conn);
      delete engine_conn;

      channel->update_connections_order();
    } else return -1;
  }
  return 1;

}

bool vsx_engine_param::disconnect() {

  std::vector<vsx_engine_param_connection*> temp_conn = connections;
  for (std::vector<vsx_engine_param_connection*>::iterator it = temp_conn.begin(); it != temp_conn.end(); ++it) {
    if ((*it)->alias_connection)
    {
      (*it)->dest->disconnect();
      (*it)->dest->unalias();
    } else
    {
      (*it)->dest->disconnect((*it)->src);
    }
  }
  return true;
}

bool vsx_engine_param::unalias() {

  std::vector<vsx_engine_param_connection*> temp_conn = connections;
  for (std::vector<vsx_engine_param_connection*>::iterator it = temp_conn.begin(); it != temp_conn.end(); ++it) {
//    printf("1");
    if ((*it)->alias_connection) {
//      printf("2");
      (*it)->dest->unalias();
//      printf("deleting alias %s\n",name.c_str());
      // 1. delete the param from our list
      (*it)->src->delete_conn(*it);
      // 2. delete the param itself
      (*it)->dest->owner->delete_param((*it)->dest);
      // 3. delete the conn itself
      delete *it;
    }
  }
  return true;
}

void vsx_engine_param::disconnect_abs_connections() {

  std::vector<vsx_engine_param_connection*> temp_conn = connections;
  for (std::vector<vsx_engine_param_connection*>::iterator it = temp_conn.begin(); it != temp_conn.end(); ++it) {
    if ((*it)->alias_connection) {
      (*it)->dest->disconnect_abs_connections();
//      printf("deleting alias %s\n",name.c_str());
      // 1. delete the param from our list
      (*it)->src->delete_conn(*it);
      // 2. delete the param itself
      (*it)->dest->owner->delete_param((*it)->dest);
      // 3. delete the conn itself
      delete *it;

    } else
    {
      (*it)->dest->disconnect((*it)->src, false);
    }
  }
  unalias();

}


void vsx_engine_param::dump_aliases(vsx_string base_macro, vsx_command_list* command_result) {
#ifndef VSX_NO_CLIENT
  for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
//    printf("1");
    if ((*it)->alias_connection && (*it)->dest->owner->component->name.find(base_macro) == 0)
    {
//      printf("2");

      //        0          1           2            3           4              5                 6
      //   param_alias [p_def] [-1=in / 1=out] [component] [parameter] [source_component] [source_parameter]

      (*it)->dest->dump_aliases(base_macro, command_result);
//      printf("dumping alias %s\n",name.c_str());
      command_result->add_raw("param_alias "+
      (*it)->dest->name+":"+(*it)->dest->spec+" "+
      i2s(owner->io)+" "+
      str_replace(base_macro,"$$name",str_replace(base_macro+".","$$name.",(*it)->dest->owner->component->name,1,0),1,0)+" "+
      (*it)->dest->name+" "+
      str_replace(base_macro,"$$name",str_replace(base_macro+".","$$name.",(*it)->src->owner->component->name,1,0),1,0)+" "+
      (*it)->src->name
      );
    }
  }
#endif
}

void vsx_engine_param::dump_aliases_rc(vsx_command_list* command_result) {
#ifndef VSX_NO_CLIENT
  for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
//    printf("1");
    if ((*it)->alias_connection)
    {
      //        0          1           2            3           4              5                 6
      //   param_alias [p_def] [-1=in / 1=out] [component] [parameter] [source_component] [source_parameter]
      (*it)->dest->dump_aliases_rc(command_result);
      command_result->add_raw("param_alias_ok "+
      (*it)->dest->name+":"+(*it)->dest->spec+" "+
      i2s(owner->io)+" "+
      (*it)->dest->owner->component->name+" "+
      (*it)->dest->name+" "+
      (*it)->src->owner->component->name+" "+
      (*it)->src->name+" "+
      i2s((*it)->connection_order)
      );
    }
  }
#endif
}

void vsx_engine_param::dump_aliases_and_connections(vsx_string base_macro, vsx_command_list* command_result) {
#ifndef VSX_NO_CLIENT
  for (std::vector<vsx_engine_param_connection*>::reverse_iterator it = connections.rbegin(); it != connections.rend(); ++it) {
    if ((*it)->alias_connection && (*it)->dest->owner->component->name.find(base_macro) == 0)
    {
      //        0          1           2            3           4              5                 6
      //   param_alias [p_def] [-1=in / 1=out] [component] [parameter] [source_component] [source_parameter]
      (*it)->dest->dump_aliases_and_connections(base_macro, command_result);
      command_result->add_raw("param_alias "+
      (*it)->dest->name+":"+(*it)->dest->spec+" "+
      i2s(owner->io)+" "+
      str_replace(base_macro,"$$name",str_replace(base_macro+".","$$name.",(*it)->dest->owner->component->name,1,0),1,0)+" "+
      (*it)->dest->name+" "+
      str_replace(base_macro,"$$name",str_replace(base_macro+".","$$name.",(*it)->src->owner->component->name,1,0),1,0)+" "+
      (*it)->src->name
      );
    } else
    if (((*it)->src->owner->component->name.find(base_macro+".") == 0
    || ((*it)->src->owner->component->name.find(base_macro) == 0
    && base_macro.size() == (*it)->src->owner->component->name.size())) ||
      base_macro.size() == 0)
    {
      //       0            1          2          3          4
      //  param_connect [in-comp] [in-param] [out-comp] [out-param]
      command_result->add_raw("param_connect "+
      str_replace(base_macro,"$$name",str_replace(base_macro+".","$$name.",(*it)->dest->owner->component->name,1,0),1,0)+" "+
      (*it)->dest->name+" "+
      str_replace(base_macro,"$$name",str_replace(base_macro+".","$$name.",(*it)->src->owner->component->name,1,0),1,0)+" "+
      (*it)->src->name
      );
    }
  }
#endif
}

void vsx_engine_param::dump_aliases_and_connections_rc(vsx_command_list* command_result) {
#ifndef VSX_NO_CLIENT
  int c = 0;
  for (std::vector<vsx_engine_param_connection*>::reverse_iterator it = connections.rbegin(); it != connections.rend(); ++it) {
    if ((*it)->alias_connection)
    {
      //        0          1           2            3           4              5                 6
      //   param_alias [p_def] [-1=in / 1=out] [component] [parameter] [source_component] [source_parameter]
      command_result->add_raw("param_alias_ok "+
      (*it)->dest->name+":"+(*it)->dest->spec+" "+
      i2s(owner->io)+" "+
      (*it)->dest->owner->component->name+" "+
      (*it)->dest->name+" "+
      (*it)->src->owner->component->name+" "+
      (*it)->src->name+" "+
      i2s((connections.size() - c) -1)//connections.size()-1-c)
      );
      (*it)->dest->dump_aliases_and_connections_rc(command_result);
    } else
    {
      //       0            1          2          3          4
      //  param_connect [in-comp] [in-param] [out-comp] [out-param]
      command_result->add_raw("param_connect_ok "+
      (*it)->dest->owner->component->name+" "+
      (*it)->dest->name+" "+
      (*it)->src->owner->component->name+" "+
      (*it)->src->name+" "+
      i2s((connections.size() - c) -1)//connections.size()-1-c)
      );
    }
    ++c;
  }
#endif
}


void vsx_engine_param::dump_connections(vsx_string base_macro, vsx_command_list* command_result) {
#ifndef VSX_NO_CLIENT
  for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    if ((*it)->alias_connection && (*it)->dest->owner->component->name.find(base_macro) == 0) {
      (*it)->dest->dump_connections(base_macro,command_result);
    } else
    if ((*it)->src->owner->component->name.find(base_macro) == 0)
    {
      //       0            1          2          3          4
      //  param_connect [in-comp] [in-param] [out-comp] [out-param]
      command_result->add_raw("param_connect "+
      str_replace(base_macro,"$$name",str_replace(base_macro+".","$$name.",(*it)->dest->owner->component->name,1,0),1,0)+" "+
      (*it)->dest->name+" "+
      str_replace(base_macro,"$$name",str_replace(base_macro+".","$$name.",(*it)->src->owner->component->name,1,0),1,0)+" "+
      (*it)->src->name
      );
    }
  }
#endif
}


void vsx_engine_param::get_abs_connections(std::list<vsx_engine_param_connection_info*>* abs_connections, vsx_engine_param* dest) {
#ifndef VSX_NO_CLIENT
  //printf("get_abs_connections %s\n",name.c_str());
  int count = connections.size()-1;
  //int count = 0;
  for (std::vector<vsx_engine_param_connection*>::reverse_iterator it = connections.rbegin(); it != connections.rend(); ++it) {
    if ((*it)->alias_connection) {
      (*it)->dest->get_abs_connections(abs_connections,dest);
    } else {
      vsx_engine_param_connection_info* param_connection_info = new vsx_engine_param_connection_info;
      if (owner->io == -1) {
        param_connection_info->src = (*it)->src;
        param_connection_info->src_name = (*it)->src->name;
        param_connection_info->dest = dest;
        param_connection_info->dest_name = dest->name;
        param_connection_info->localorder = count;
        param_connection_info->num_dest_connections = connections.size();
      } else {
        param_connection_info->src_name = (*it)->src->name;
        param_connection_info->src = dest;
        param_connection_info->dest = (*it)->dest;
        param_connection_info->dest_name = dest->name;
        std::vector<vsx_engine_param_connection*>::iterator it2 = (*it)->dest->connections.begin();
        bool drun = true;
        int c = 0;
        while (it2 != (*it)->dest->connections.end() && drun) {
          if (*it2 == *it) {
            drun = false;
            param_connection_info->localorder = c;
          }
          ++c;
          ++it2;
        }
        param_connection_info->num_dest_connections = (*it)->dest->connections.size();
      }
      param_connection_info->connection_order = (*it)->connection_order;
      param_connection_info->channel_connection = (*it)->channel_connection;
      abs_connections->push_front(param_connection_info);
    }
    --count;
  }
#endif
}

void vsx_engine_param::rebuild_orders(std::vector<int>* new_order) {
//#ifndef VSX_NO_CLIENT
  for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    if ((*it)->alias_connection) {
      (*it)->dest->rebuild_orders(new_order);
    } else
    {
      new_order->push_back((*it)->connection_order);
    }
  }
//#endif
}


// manage connections
bool vsx_engine_param::delete_conn(vsx_engine_param_connection* conn) {
  for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); it++) {
    if ((*it) == conn) {
      connections.erase(it);
      return true;
    }
  }
  return false;
}

vsx_engine_param_connection* vsx_engine_param::get_conn_by_src(vsx_engine_param* src) {
  for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); it++) {
    if ((*it)->src == src) {
      return (*it);
    }
  }
  return false;
}

vsx_engine_param_connection* vsx_engine_param::get_conn_by_dest(vsx_engine_param* dest) {
  for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); it++) {
    if ((*it)->dest == dest) {
      return (*it);
    }
  }
  return false;
}

vsx_engine_param::~vsx_engine_param() {
  for (std::vector<vsx_engine_param_connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    if ((*it)->dest == this) {
      disconnect();
      (*it)->src->delete_conn(*it);
    }
    delete *it;
  }
}



vsx_string vsx_engine_param::get_type_name() {
  vsx_string a;
  switch (module_param->type) {
    case VSX_MODULE_PARAM_ID_FLOAT: {
      a = "float";
    } break;
    case VSX_MODULE_PARAM_ID_QUATERNION: {
      a = "quaternion";
    } break;
  }
  return a;
}


// HANDLE VALUES

char res[256];

vsx_string vsx_engine_param::get_string() {

  if (alias) return alias_owner->get_string();
  vsx_string p;
  switch (module_param->type) {
    case VSX_MODULE_PARAM_ID_INT: {
      sprintf(res,"%d",((vsx_module_param_int*)module_param)->param_data_suggestion[0]);
      p = res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_FLOAT: {
      sprintf(res,"%.20f",((vsx_module_param_float*)module_param)->param_data_suggestion[0]);
      p = res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_DOUBLE: {
      sprintf(res,"%.20f",((vsx_module_param_double*)module_param)->param_data_suggestion[0]);
      p = res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_FLOAT3: {
      sprintf(res,"%.20f",((vsx_module_param_float3*)module_param)->param_data_suggestion[0]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float3*)module_param)->param_data_suggestion[1]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float3*)module_param)->param_data_suggestion[2]);
      p += res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_FLOAT4: {
      sprintf(res,"%.20f",((vsx_module_param_float4*)module_param)->param_data_suggestion[0]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float4*)module_param)->param_data_suggestion[1]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float4*)module_param)->param_data_suggestion[2]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float4*)module_param)->param_data_suggestion[3]);
      p += res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_QUATERNION: {
      sprintf(res,"%.20f",((vsx_module_param_quaternion*)module_param)->param_data_suggestion[0]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_quaternion*)module_param)->param_data_suggestion[1]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_quaternion*)module_param)->param_data_suggestion[2]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_quaternion*)module_param)->param_data_suggestion[3]);
      p += res;
      return p;
    }

    case VSX_MODULE_PARAM_ID_STRING: {
      return *((vsx_module_param_string*)module_param)->param_data;
    }
    case VSX_MODULE_PARAM_ID_RESOURCE: {
      return *((vsx_module_param_resource*)module_param)->param_data;
    }
    case VSX_MODULE_PARAM_ID_SEQUENCE: {
      //printf("finding value: %f\n",((vsx_module_param_sequence*)module_param)->param_data[0].items[0].value);
      //printf("pointer: %d\n",((vsx_module_param_sequence*)module_param)->get_addr());
      return ((vsx_module_param_sequence*)module_param)->param_data[0].get_string();
    }
    case VSX_MODULE_PARAM_ID_MESH: {
      /*
      vsx_mesh* m = ((vsx_module_param_mesh*)module_param)->param_data;
      if (m->data)
      sprintf(res,"%d vertices %d faces",(int)m->data->vertices.size(), (int)m->data->faces.size());
      return vsx_string(res);*/
    }
  }
  return "";
}

vsx_string vsx_engine_param::get_default_string() {
  if (alias) return alias_owner->get_default_string();
  vsx_string p;
  switch (module_param->type) {
    case VSX_MODULE_PARAM_ID_INT: {
      sprintf(res,"%d",((vsx_module_param_int*)module_param)->param_data_default[0]);
      p = res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_FLOAT: {
      sprintf(res,"%.20f",((vsx_module_param_float*)module_param)->param_data_default[0]);
      p = res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_DOUBLE: {
      sprintf(res,"%.20f",((vsx_module_param_double*)module_param)->param_data_default[0]);
      p = res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_FLOAT3: {
      sprintf(res,"%.20f",((vsx_module_param_float3*)module_param)->param_data_default[0]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float3*)module_param)->param_data_default[1]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float3*)module_param)->param_data_default[2]);
      p += res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_FLOAT4: {
      sprintf(res,"%.20f",((vsx_module_param_float4*)module_param)->param_data_default[0]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float4*)module_param)->param_data_default[1]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float4*)module_param)->param_data_default[2]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_float4*)module_param)->param_data_default[3]);
      p += res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_QUATERNION: {
      sprintf(res,"%.20f",((vsx_module_param_quaternion*)module_param)->param_data_default[0]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_quaternion*)module_param)->param_data_default[1]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_quaternion*)module_param)->param_data_default[2]);
      p += res;
      sprintf(res,",%.20f",((vsx_module_param_quaternion*)module_param)->param_data_default[3]);
      p += res;
      return p;
    }
    case VSX_MODULE_PARAM_ID_STRING: {
      return vsx_string("");//((vsx_module_param_string*)module_param)->param_data_default[0];
    }
    case VSX_MODULE_PARAM_ID_RESOURCE: {
      return vsx_string("");//((vsx_module_param_string*)module_param)->param_data_default[0];
    }
  }
  return "";
}

void vsx_engine_param::set_compound_string(vsx_string data)
{
  if (!data.size()) return;
  switch (module_param->type) {
    case VSX_MODULE_PARAM_ID_INT:
    case VSX_MODULE_PARAM_ID_FLOAT:
    case VSX_MODULE_PARAM_ID_DOUBLE:
    case VSX_MODULE_PARAM_ID_STRING:
    case VSX_MODULE_PARAM_ID_RESOURCE:
    case VSX_MODULE_PARAM_ID_SEQUENCE:
    case VSX_MODULE_PARAM_ID_FLOAT_ARRAY:
    set_string(data);
    return;
    case VSX_MODULE_PARAM_ID_FLOAT3:
    case VSX_MODULE_PARAM_ID_FLOAT4:
    case VSX_MODULE_PARAM_ID_QUATERNION:
    break;
  }
  vsx_string deli = ",";
  std::vector<vsx_string> data_parts;
  explode(data,deli,data_parts);
  for (size_t i = 0; i < data_parts.size(); i++)
  {
    set_string(data_parts[i],i);
  }

}

void vsx_engine_param::set_string(vsx_string data, int index) {
  if (alias) {
    alias_owner->set_string(data,index);
    return;
  }
  ++module->param_updates;
  ++module_param->updates;
  switch (module_param->type) {
    case VSX_MODULE_PARAM_ID_INT: {
      ((vsx_module_param_int*)module_param)->set_internal(atoi(data.c_str()));
      return;
    }
    case VSX_MODULE_PARAM_ID_FLOAT: {
      ((vsx_module_param_float*)module_param)->set_internal((float)atof(data.c_str()));
      return;
    }
    case VSX_MODULE_PARAM_ID_DOUBLE: {
      ((vsx_module_param_double*)module_param)->set_internal(atof(data.c_str()));
      return;
    }
    case VSX_MODULE_PARAM_ID_FLOAT3: {
      ((vsx_module_param_float3*)module_param)->set_internal((float)atof(data.c_str()),index);
      return;
    }
    case VSX_MODULE_PARAM_ID_FLOAT4: {
      ((vsx_module_param_float4*)module_param)->set_internal((float)atof(data.c_str()),index);
      return;
    }
    case VSX_MODULE_PARAM_ID_QUATERNION: {
      ((vsx_module_param_quaternion*)module_param)->set_internal((float)atof(data.c_str()),index);
      return;
    }
    case VSX_MODULE_PARAM_ID_STRING: {
      ((vsx_module_param_string*)module_param)->check_free();
      ((vsx_module_param_string*)module_param)->param_data[0] = data;
      return;
    }
    case VSX_MODULE_PARAM_ID_RESOURCE: {
      ((vsx_module_param_resource*)module_param)->check_free();
      data = str_replace("//", "/", data);
      ((vsx_module_param_resource*)module_param)->param_data[0] = data;
      return;
    }
    case VSX_MODULE_PARAM_ID_SEQUENCE: {
      //printf("setting sequence in engine\n");
      ((vsx_module_param_sequence*)module_param)->param_data[index].set_string(data);
      return;
    }
    case VSX_MODULE_PARAM_ID_FLOAT_ARRAY: {
      clean_up_module_param(module_param);
      if (!((vsx_module_param_float_array*)module_param)->valid) {
        // data in our param is most likely pointing at invalid memory.
        vsx_float_array nn;
        nn.data = new vsx_array<float>;
        ((vsx_module_param_float_array*)module_param)->param_data[0] = nn;
      }

      vsx_string deli = ";";
      vsx_avector<vsx_string> parts;
      explode(data,deli,parts);
      ((vsx_module_param_float_array*)module_param)->param_data[0].data->clear();
      for (unsigned long i = 0; i < parts.size(); ++i) {
        (*((vsx_module_param_float_array*)module_param)->param_data[0].data).push_back(s2f(parts[i]));
      }
      ((vsx_module_param_float_array*)module_param)->valid = true;
      return;
    }
  }
}

void vsx_engine_param::clean_up_module_param(vsx_module_param_abs* param) {
  if (alias) {
    alias_owner->clean_up_module_param(param);
    return;
  }
  if (!param->valid) return;
  // we don't wanna clean out a module's internal value, let the module handle
  // that so it can set it back if it wants to.
  if (param->value_from_module) {
    return;
  } else
  switch (param->type) {
    case VSX_MODULE_PARAM_ID_FLOAT_ARRAY: {
      delete ((vsx_module_param_float_array*)module_param)->param_data[0].data;
    }
    break;
  }
  param->valid = false;
  param->value_from_module = false;
}


//-------------------------------------------------------------------------------------------------------------------

void vsx_engine_param_list::init(vsx_module_param_list* module_param_list) {
  this->module_param_list = module_param_list;
  for (unsigned long i = 0; i < module_param_list->id_vec.size(); ++i) {
//  for (std::vector<vsx_module_param_abs*>::iterator it = module_param_list->id_vec.begin(); it != module_param_list->id_vec.end(); it++) {
	  vsx_engine_param* new_param = new vsx_engine_param;
	  vsx_string name = module_param_list->id_vec[i]->name;
	  LOG("initing vepl with "+name)
    param_name_list[name] = new_param;
    param_id_list.push_back(new_param);
    new_param->alias_owner = new_param;
    LOG("vsx_engine_param_list::init 2")
    new_param->module_param = module_param_list->id_vec[i];
    new_param->module = component->module;
    LOG("vsx_engine_param_list::init 3")
    new_param->name = name;
    new_param->owner = this;
    new_param->spec = single_param_spec(name);
    LOG("vsx_engine_param_list::init 4")
    new_param->critical = module_param_list->id_vec[i]->critical;
    new_param->all_required = module_param_list->id_vec[i]->all_required;
    LOG("vsx_engine_param_list::init finished")
	}
}

void vsx_engine_param_list::delete_param(vsx_engine_param* param) {
  param_name_list.erase(param->name);
  delete param;
  for (std::vector<vsx_engine_param*>::iterator it = param_id_list.begin(); it != param_id_list.end(); ++it)
  if (*it == param) {
//    param_name_list.erase((*it)->name);
    param_id_list.erase(it);
    return;
  }
}

void vsx_engine_param_list::dump_aliases(vsx_string base_macro, vsx_command_list* command_result) {
  for (std::vector<vsx_engine_param*>::iterator it = param_id_list.begin(); it != param_id_list.end(); ++it) {
    (*it)->dump_aliases(base_macro, command_result);
  }
}

void vsx_engine_param_list::dump_aliases_rc(vsx_command_list* command_result) {
  for (std::vector<vsx_engine_param*>::iterator it = param_id_list.begin(); it != param_id_list.end(); ++it) {
    (*it)->dump_aliases_rc(command_result);
  }
}



void vsx_engine_param_list::dump_aliases_and_connections(vsx_string base_macro, vsx_command_list* command_result) {
  for (std::vector<vsx_engine_param*>::iterator it = param_id_list.begin(); it != param_id_list.end(); ++it) {
    (*it)->dump_aliases_and_connections(base_macro, command_result);
  }
}

void vsx_engine_param_list::dump_aliases_and_connections_rc(vsx_command_list* command_result) {
  for (std::vector<vsx_engine_param*>::iterator it = param_id_list.begin(); it != param_id_list.end(); ++it) {
    (*it)->dump_aliases_and_connections_rc(command_result);
  }
}


void vsx_engine_param_list::dump_connections(vsx_string base_macro, vsx_command_list* command_result) {
  for (std::vector<vsx_engine_param*>::iterator it = param_id_list.begin(); it != param_id_list.end(); ++it) {
    (*it)->dump_connections(base_macro, command_result);
  }
}

void vsx_engine_param_list::dump_param_values(vsx_string my_name, vsx_command_list* command_result) {
#ifndef VSX_NO_CLIENT
  for (unsigned long i = 0; i < param_id_list.size(); ++i) {
    bool run = true;
    if (param_id_list[i]->channel)
      if ((param_id_list[i]->channel->connections.size()))
      run = false;
    if (run) {
      // dump the value plz
      //cout << comp->channels[i]->get_param_name() << ":::" << comp->channels[i]->my_param->module_param->get_default_string() << ":::" << comp->channels[i]->my_param->module_param->get_string() << endl;
      vsx_engine_param* param = param_id_list[i];
      //printf("name:%s\nval: %s\ndef: %s\n",param->module_param->name.c_str(),param->module_param->get_string().c_str(),param->module_param->get_default_string().c_str());
      vsx_string pval = param->get_string();
      if (!param->alias) {
        if (
          pval !=
          param->get_default_string()
        )
        {
          if (param->module_param->type == VSX_MODULE_PARAM_ID_STRING) {
            command_result->add_raw(vsx_string("ps64 ")+my_name+" "+param->name+" "+base64_encode(pval));
          } else
          command_result->add_raw(vsx_string("param_set ")+my_name+" "+param->name+" "+pval);
        }
      }
      // dump the param vsxl filter as well
			#ifndef VSXE_NO_GM
      if (param->module_param->vsxl_modifier) {
        vsx_param_vsxl_driver_abs* driver;
        driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->get_driver();
        command_result->add_raw(vsx_string("vsxl_pfi ")+my_name+" "+param->name+" "+base64_encode(driver->script));
      }
			#endif
    }
  }
#endif
}


void vsx_engine_param_list::get_abs_connections(std::list<vsx_engine_param_connection_info*>* abs_connections) {
  for (std::vector<vsx_engine_param*>::iterator it = param_id_list.begin(); it != param_id_list.end(); ++it) {
    (*it)->get_abs_connections(abs_connections,*it);
  }
}

int vsx_engine_param_list::alias(vsx_engine_param* src, vsx_string name, int order) {
  // 1. create our param here in our list
  vsx_engine_param* new_param = new vsx_engine_param;
  param_name_list[name] = new_param;
  int c = 0;
  if (order == -1)
  param_id_list.push_back(new_param);
  else {
    //printf("going the hard way,..\n");
    std::vector<vsx_engine_param*>::iterator it = param_id_list.begin();
    bool run = true;
    if (order == 0) {
      param_id_list.insert(param_id_list.begin(), new_param);
    } else
    while (run) {
      if (c == order) {
        //printf("c is %d,..\n",c);
        //it++;
        param_id_list.insert(it, new_param);
        run = false;
      }
      if (it == param_id_list.end()) {
        param_id_list.insert(it, new_param);

        run = false;
//        param_id_list.push_back(new_param);
      }
      ++it;
      ++c;
    }
  }

  // 2. copy the info from the source
  new_param->alias_owner = src->alias_owner;
  new_param->module_param = new_param->alias_owner->module_param;
  new_param->module = new_param->alias_owner->module;
  new_param->name = name;
  new_param->owner = this;
  new_param->spec = new_param->alias_owner->spec;
  new_param->critical = new_param->alias_owner->critical;
  new_param->all_required = new_param->alias_owner->all_required;
  new_param->channel = new_param->alias_owner->channel;
  new_param->alias_parent = src;
  new_param->alias = true;
  // 3. create a connection in the source to us for traversal purpouses
  vsx_engine_param_connection* new_conn = new vsx_engine_param_connection;
  if (order == -1)
  src->connections.push_back(new_conn);
  else {
    c = 0;
    std::vector<vsx_engine_param_connection*>::iterator it = src->connections.begin();
    bool run = true;
    if (order == 0) {
      src->connections.insert(src->connections.begin(), new_conn);
    } else
    while (run) {
      if (c == order) {
        //++it;
        src->connections.insert(it, new_conn);
        run = false;
      }
      if (it == src->connections.end()) {
        run = false;
        src->connections.insert(it, new_conn);
//        src->connections.push_back(new_conn);
      }
      ++it;
      ++c;
    }
  }
  new_conn->src = src;
  new_conn->dest = new_param;
  new_conn->connection_order = -1;
  new_conn->owner = src;
  new_conn->alias_connection = true;

  if (order == -1)
  return src->connections.size()-1;
  else
  return c;
}

bool vsx_engine_param_list::unalias(vsx_string name) {
  vsx_engine_param* param = get_by_name(name);
  if (param) {
    if (param->alias)
    if (param->alias_parent != param)
    param->alias_parent->delete_conn(param->alias_parent->get_conn_by_dest(param));

    param->disconnect();
//    printf("unalias 1 %s\n",param->name.c_str());
    param->unalias();
//    printf("unalias 2\n");

    delete_param(param);
  } else return false;
  return true;
}

vsx_string vsx_engine_param_list::alias_get_unique_name(vsx_string base_name, int tried) {
  if (tried == 0) {
    // first run!
    if (param_name_list.find(base_name) == param_name_list.end()) {
      return base_name;
    } else {
      // oops, trouble!
      return alias_get_unique_name(base_name,tried+1);
    }
  } else {
    // ok, we already know we need to do stuff
    if (param_name_list.find(base_name+"_"+i2s(tried)) ==  param_name_list.end()) {
      return base_name+"_"+i2s(tried);
    } else {
      // oops, trouble!
      return alias_get_unique_name(base_name,tried+1);
    }
  }
}

bool vsx_engine_param_list::alias_rename(vsx_string ren_name,vsx_string to_name) {
#ifndef VSX_NO_CLIENT
  if (param_name_list.find(ren_name) != param_name_list.end()) {
    if (param_name_list.find(to_name) == param_name_list.end()) {
      vsx_engine_param* b = param_name_list[ren_name];
      if (b->alias) {
        b->name = to_name;
        param_name_list.erase(ren_name);
        param_name_list[to_name] = b;
      } else return false;
    } else return false;
  } else return false;
#endif
  return true;
}

/////

void vsx_engine_param_list::disconnect_abs_connections() {
  std::map<vsx_string, vsx_engine_param*> temp_list = param_name_list;
  for (std::map<vsx_string, vsx_engine_param*>::iterator it = temp_list.begin(); it != temp_list.end(); it++) {
    ((*it).second)->disconnect_abs_connections();
  }
}


/*void vsx_engine_param_list::alias(vsx_param_anchor* param_to_alias, vsx_string name) {
  printf("internal alias##########################\n");
  if (param_to_alias->alias_child == 0) {
    vsx_param_anchor* new_anchor = new vsx_param_anchor;
    new_anchor->param = param_to_alias->param;
    new_anchor->comp = param_to_alias->comp;
    new_anchor->alias = true;
    new_anchor->owner = this;
    new_anchor->name = name;
    new_anchor->alias_parent = param_to_alias;
    param_to_alias->alias_child = new_anchor;
    param_name_list[name] = new_anchor;
    param_id_list.push_back(param_to_alias->param);
    //source->aliased[param_to_alias] = this;
  } else printf("vsx_param_list::alias() ERR: alias for this anchor already present!\n");
}*/


/*void vsx_param_list::unalias(vsx_string name)
{
  vsx_param_abs* a = param_name_list[name];
  param_name_list.erase(name);

  for (std::vector<vsx_param_abs*>::iterator it = param_id_list.begin(); it != param_id_list.end(); it++) {
    if ((*it) == a) {
      param_id_list.erase(it);
      return;
    }
  }
}

void vsx_param_list::unalias(vsx_param_anchor* param_to_unalias)
{
  printf("unalias run for %s\n",((vsx_comp*)component)->name.c_str());
  vsx_param_abs* param_found = 0;
  bool run = true;
  if (param_id_list.size()) {
    std::vector<vsx_param_abs*>::iterator it = param_id_list.begin();
    while (run) {
      if ((*it) == param_to_unalias) {
        param_found = (*it);
        printf("__alias_name: %s\n",param_found->get_name().c_str());
        param_id_list.erase(it);
        run = false;
      }
      if (run)
      if (it != param_id_list.end()) it++; else run = false;
    }
    if (param_found) {
//      printf(" size: %d",param_name_list.size());
//      printf(" name: %s",param_found->get_name().c_str());
      //param_name_list.erase(param_found->get_name());
      for (std::map<vsx_string, vsx_param_abs*>::iterator it2 = param_name_list.begin(); it2 != param_name_list.end(); it2++) {
        if ((*it2).second == param_found) {
          param_name_list.erase((*it2).first);
          printf(":found - list name: %s\n",(*it2).first.c_str());
          if (aliased.find(param_found) != aliased.end()) {
            printf("removing iterative aliases\n");
            aliased[param_found]->unalias(param_found);
            aliased.erase(param_found);
          }

          return;
        }
      }
    }
  }
  printf("c\n");
}
*/

void vsx_engine_param_list::unalias_aliased() {
  //printf("unalias size: %d\n",param_name_list.size());
  //if (param_name_list.size())
  std::map<vsx_string, vsx_engine_param*> temp_list = param_name_list;
  for (std::map<vsx_string, vsx_engine_param*>::iterator it = temp_list.begin(); it != temp_list.end(); ++it) {
    //printf("u: %s\n",(*it).first.c_str());
    unalias((*it).first);
//    (*it).second->unalias((*it).first);
  }
}


int vsx_engine_param_list::order(vsx_string param, vsx_string new_order) {
//#ifndef VSX_NO_CLIENT
  vsx_engine_param* my_param = get_by_name(param);
  if (my_param) {
    // 0. parse the new_order string that our nice client sent us
    vsx_string deli = ",";
    std::vector<int> order_list;
    std::vector<vsx_string> order_source;
    explode(new_order, deli, order_source);
    // 1. Re-order our own little connection list
    std::vector<vsx_engine_param_connection*> new_connection_list;
    for (std::vector<vsx_string>::iterator it = order_source.begin(); it != order_source.end(); ++it) {
      new_connection_list.push_back(my_param->connections[s2i(*it)]);
    }
    my_param->connections = new_connection_list;

    // 2. recurse to find the new connection id order
    std::vector<int> new_channel_order;
    my_param->alias_owner->rebuild_orders(&new_channel_order);
    // 3. tell channel to rebuild with new list
    my_param->alias_owner->channel->connections_order(&new_channel_order);
    return 1;
  } else return -2;
//#endif
}

//.........................................................................................

vsx_engine_param* vsx_engine_param_list::get_by_name(vsx_string name) {
  if (param_name_list.find(name) != param_name_list.end())
  {
    //printf("vsx_engine_param_list::get_by_name -- found %s\n",name.c_str());
  	return param_name_list[name];
  }
	return 0;
}

vsx_string vsx_engine_param_list::get_name_by_param(vsx_engine_param* param) {
  vsx_string h = "";
  for (std::map<vsx_string, vsx_engine_param*>::iterator it = param_name_list.begin(); it != param_name_list.end(); it++) {
//	    printf("a\n");
    if ((*it).second == param) return (*it).first;
  }
	return h;
}



vsx_string vsx_engine_param_list::single_param_spec(vsx_string param_name, int startpos) {
  //printf("parsing start\n");
  //if (module) {
    vsx_string sin;
    if (io == 1)
    sin = component->out_param_spec;
    else
    sin = component->in_param_spec;
    
#ifdef VSXU_DEBUG
    //printf("vsx_engine_param_list::single_param_spec, sin= %s\n", sin.c_str() );
#endif
    
    if (sin.size() == 0) return "";
    //unsigned int loc = startpos;
    int loc = startpos;
    bool nfound = true;
    while (nfound) {
      loc = sin.find(param_name, loc);
      //std::cout << " loc = " << loc;
			if (loc >= startpos && loc != -1) {
				if (loc == 0 ||
				 sin[loc-1] == ',' ||
				 sin[loc-1] == '{') {
					//printf("parsing_inner\n");
					vsx_string res = "";
					int spos = loc;
					int size = sin.size();
					//printf("sin: %s\n", sin.c_str());
					while (spos < size) {
						res+=sin[spos];
						++spos;
            if (spos == size ||
                sin[spos] == ',' ||
								sin[spos] == '}'
								)
						{
							std::vector<vsx_string> plist;
							vsx_string deli = ":";
							//int explode(vsx_string& input, vsx_string& delimiter, std::vector<vsx_string>& results, int max_parts = 0);
							explode(res, deli, plist,2);
							deli = "[";
							std::vector<vsx_string> plist2;

							explode(plist[1], deli, plist2);
							if (plist2[0] == "complex")
							return single_param_spec(param_name, spos);
							else
							return plist[1];
						}
					}
					return res;
				} else ++loc;
			} else return "";
    }
//U  } else return "";
  return "";

}

vsx_engine_param_list::~vsx_engine_param_list() {
  for (std::vector<vsx_engine_param*>::iterator it = param_id_list.begin(); it != param_id_list.end(); ++it) {
    (*it)->disconnect();
    (*it)->unalias();
    delete *it;
  }
}

