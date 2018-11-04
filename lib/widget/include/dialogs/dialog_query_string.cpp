#include "dialog_query_string.h"

void dialog_query_string::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "cancel")
  {
    command_q_b.add(name+"_cancel","cancel");
    parent->vsx_command_queue_b(this);
    visible = 0;
    return;
  }

  vsx_string<> first_res;

  vsx_nw_vector< vsx_string<> > callback_result_list;

  vsx_nw_vector< vsx_string<> > result_list;
  for (unsigned long i = 0; i < edits.size(); ++i)
  {
    callback_result_list.push_back(((vsx_widget_base_edit*)(edits[i]))->get_string());

    if (!first_res.size())
    {
      first_res = ((vsx_widget_base_edit*)(edits[i]))->get_string();
      continue;
    }

    result_list.push_back( ((vsx_widget_base_edit*)(edits[i]))->get_string() );
    ((vsx_widget_base_edit*)(edits[i]))->set_string("");
  }

  visible = 0;
  req(on_ok(callback_result_list));

  vsx_string<> i("|");
  vsx_string<> result_string = vsx_string_helper::implode(result_list, i);
  vsx_string<> cmd = name+" "+first_res;

  if (result_string.size())
    cmd += " "+vsx_string_helper::base64_encode(result_string);

  if (extra_value != "")
    cmd += " " + extra_value;

  command_q_b.add_raw(cmd);
}

void dialog_query_string::set_title(vsx_string<> new_title)
{
  title = new_title;
}

void dialog_query_string::set_label(size_t index, vsx_string<> title)
{
  if (index >= labels.size())
    return;
  labels[index]->title = title;
}

void dialog_query_string::set_value(vsx_string<> value)
{
  ((vsx_widget_base_edit*)edit1)->set_string(value);
  ((vsx_widget_base_edit*)edit1)->caret_goto_end();
}

void dialog_query_string::show(vsx_string<> value)
{
  ((vsx_widget_base_edit*)edit1)->caret_goto_end();
  show();
  ((vsx_widget_base_edit*)edit1)->set_string(value);
}

void dialog_query_string::show()
{
  // set keyboard focus to current editor widget
  a_focus = k_focus = edit1;

  for (unsigned long i = 0; i < edits.size(); ++i)
  {
    ((vsx_widget_base_edit*)(edits[i]))->set_string("");
    ((vsx_widget_base_edit*)(edits[i]))->caret_goto_end();
  }

  // set ourselves visible
  visible = 1;
  set_pos(vsx_vector3<>(0.5f-size.x/2, 0.5-size.y/2,0));
  parent->front(this);
}

void dialog_query_string::set_allowed_chars(vsx_string<> ch)
{
  ((vsx_widget_base_edit*)edit1)->allowed_chars = ch;
}

dialog_query_string::dialog_query_string(vsx_string<> window_title, vsx_string<> in_fields)
{
  // buttons, always needed
  vsx_widget *button1 = add(new vsx_widget_button,".b1");
  vsx_widget *button2 = add(new vsx_widget_button,".b2");
  vsx_widget_window::init();

  // now for the edit fields
  if (in_fields != "")
  {
    vsx_nw_vector< vsx_string<> > f_parts;
    vsx_string<>deli = "|";
    vsx_string_helper::explode(in_fields, deli, f_parts);
    if (f_parts.size() == 0) f_parts.push_back("");
    set_size(vsx_vector3<>(0.45f, 0.10f+(float)(f_parts.size())*0.04f));
    float yp = target_size.y - 0.04f-0.02f;
    edit1 = 0;
    for (unsigned int i = 0; i < f_parts.size(); ++i)
    {
      // add an editor
      vsx_widget_base_edit *e = (vsx_widget_base_edit*)add(new vsx_widget_base_edit,"e");
      if (!edit1)
        edit1 = (vsx_widget*)e;

      edits.push_back((vsx_widget*)e);
      e->init();
      e->set_size(vsx_vector3<>(size.x-0.04f, 0.02f));
      e->set_pos(vsx_vector3<>(size.x-e->target_size.x*0.5-0.02,yp));
      e->set_font_size(0.02f);
      e->size_from_parent = true;
      e->single_row = true;
      e->set_string("");
      e->caret_goto_end();
      e->allowed_chars = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!#�%&()=+?-_.,:/;";

      // add a label for this editor
      vsx_widget_label *l = (vsx_widget_label*)add(new vsx_widget_label,"l");
      labels.push_back(l);
      l->init();
      l->halign = a_left;
      l->set_pos( vsx_vector3<>(e->pos.x, yp + 0.02f) );
      l->set_size( vsx_vector3f(size.x - 0.04f, 0.02f) );
      l->set_font_size(0.015f);
      l->title = f_parts[i];

      yp -= 0.04f;
    }
  }
  title = window_title;

  button1->title = "ok";
  button1->set_pos(vsx_vector3<>(0.055,0.03));
  button1->commands.adds(4,"ok","ok","");

  button2->title = "cancel";
  button2->set_pos(vsx_vector3<>(size.x-0.005-button2->size.x,0.03));
  button2->commands.adds(4,"cancel","cancel","cancel");
  init_run = true;
  coord_related_parent = false;
  visible = 0;
}
