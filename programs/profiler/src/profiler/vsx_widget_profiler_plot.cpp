#include <gl_helper.h>
#include <widgets/vsx_widget_popup_menu.h>
#include <math/vector/vsx_vector3_helper.h>

#include "vsx_widget_profiler_plot.h"
#include "time_scale.h"

void vsx_widget_profiler_plot::init()
{
  support_interpolation = true;

  allow_resize_x = true;
  allow_resize_y = true;
  set_size(vsx_vector3<>(20.0f,0.3f));
  set_pos(vsx_vector3<>(0,0));
  this->interpolate_size();

  size_min.x = 0.2;
  size_min.y = 0.2;

  allow_move_x = false;
  set_pos(vsx_vector3<>(0,0));

  plot_type = v_t;
  plot_tuple_depth = one_dimension;

  profiler = vsx_profiler_manager::get_instance()->get_profiler();

  title = "plot";

  // Menu
  menu = add(new vsx_widget_popup_menu, ".comp_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "close", "menu_close", "");
  menu->commands.adds(VSX_COMMAND_MENU, "Plot Type;v_t", "plot_type", "v_t");
  menu->size.x = size.x * 0.4;
  menu->init();

  init_run = true;
}


void vsx_widget_profiler_plot::load_plot(uint64_t id)
{
  vsx_printf(L"load profile plot %ld\n", id);
  vsx_profiler_consumer::get_instance()->get_plot( id, consumer_chunks);
  update();
}

void vsx_widget_profiler_plot::update()
{
  update_vbo();
}

void vsx_widget_profiler_plot::update_plot_v_t()
{

  vsx_printf(L"time scale: %f\n", time_scale::get_instance()->time_scale_x);

  draw_bucket_a.vertices.reset_used();
  draw_bucket_a.faces.reset_used();

  for (size_t i = 0; i < consumer_chunks.size(); i++)
  {
    vsx_profiler_consumer_plot& plot = consumer_chunks[i];

    double ct = (plot.time ) * time_scale::get_instance()->time_scale_x;

    ct += time_scale::get_instance()->time_offset;

    if (ct < -15.0)
      continue;

    if (ct > 15.0)
      continue;

    draw_bucket_a.vertices.push_back( vsx_vector3<>( ct, plot.v.x ) );

    draw_bucket_a.vertex_colors.push_back( vsx_color<>(1.0, 0.5, 0.5, 0.5) );


    vsx_face1 idx;
    idx.a = draw_bucket_a.vertices.size() - 1;
    draw_bucket_a.faces.push_back( idx );
  }

  draw_bucket_b.vertices.reset_used();
  draw_bucket_b.faces.reset_used();

  for (size_t i = 0; i < consumer_chunks.size(); i++)
  {
    vsx_profiler_consumer_plot& plot = consumer_chunks[i];

    double ct = (plot.time ) * time_scale::get_instance()->time_scale_x;

    ct += time_scale::get_instance()->time_offset;

    if (ct < -15.0)
      continue;

    if (ct > 15.0)
      continue;

    draw_bucket_b.vertices.push_back( vsx_vector3<>( ct, plot.v.y ) );

    draw_bucket_b.vertex_colors.push_back( vsx_color<>(0.5, 0.5, 1.0, 0.5) );


    vsx_face1 idx;
    idx.a = draw_bucket_b.vertices.size() - 1;
    draw_bucket_b.faces.push_back( idx );
  }
}

void vsx_widget_profiler_plot::update_plot_x_y()
{

}

void vsx_widget_profiler_plot::update_vbo()
{
  draw_bucket_a.vertices.reset_used();
  draw_bucket_a.faces.reset_used();

  draw_bucket_b.vertices.reset_used();
  draw_bucket_b.faces.reset_used();

  vsx_printf(L"time scale: %f\n", time_scale::get_instance()->time_scale_x);

  switch (plot_type)
  {
    case v_t:
      update_plot_v_t();
      break;
    case x_y:
      update_plot_x_y();
      break;
  }


  time_scale::get_instance()->one_div_chunk_time_end = 1.0 / time_scale::get_instance()->chunk_time_end;
  time_scale::get_instance()->one_div_time_size_x = 1.0 / time_scale::get_instance()->time_size_x;

  draw_bucket_a.invalidate_vertices();
  draw_bucket_a.invalidate_colors();
  draw_bucket_a.update();

  draw_bucket_b.invalidate_vertices();
  draw_bucket_b.invalidate_colors();
  draw_bucket_b.update();


  set_size(vsx_vector3<>(20.0f,0.3f));
}

void vsx_widget_profiler_plot::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "plot_type")
  {
    if (t->cmd_data == "v_t")
    {
      plot_type = v_t;
    }
    if (t->cmd_data == "x_y")
    {
      plot_type = x_y;
    }

    update();
    return;
  }
}

void vsx_widget_profiler_plot::i_draw()
{
  vsx_vector3<> parentpos = get_pos_p();
  glBegin(GL_QUADS);
    vsx_widget_skin::get_instance()->set_color_gl(1);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
  glEnd();
  vsx_widget_skin::get_instance()->set_color_gl(0);
  draw_box_border(vsx_vector3<>(parentpos.x-size.x*0.5,parentpos.y-size.y*0.5f), vsx_vector3<>(size.x,size.y), dragborder);

  glColor4f(1,1,1,1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
    glLoadIdentity();

    glTranslatef( 0, parentpos.y + size.y * 0.5, 0.0 );
    profiler->sub_begin("draw vbo bucket");

    draw_bucket_a.output();
    draw_bucket_b.output();
    profiler->sub_end();


  glPopMatrix();
  vsx_widget::i_draw();
}

void vsx_widget_profiler_plot::event_mouse_wheel(float y)
{
  parent->event_mouse_wheel(y);
}



void vsx_widget_profiler_plot::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  selected_chunk = 0x0;

  vsx_widget::event_mouse_down(distance, coords, button);
}

void vsx_widget_profiler_plot::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(distance);
  mouse_pos = distance.center;
  parent->event_mouse_move_passive(distance, coords);
}

void vsx_widget_profiler_plot::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  VSX_UNUSED(button);
  if (!selected_chunk)
    return;

  double tdiff = selected_chunk->time_end - selected_chunk->time_start;
  vsx_printf(L"tdiff. %f\n", tdiff);
  time_scale::get_instance()->time_scale_x = 0.5 / (tdiff) ;
  time_scale::get_instance()->time_offset = -time_scale::get_instance()->time_scale_x * (selected_chunk->time_start + 0.5 * tdiff);

  camera.set_pos( vsx_vector3<>(0.0, 0.0, 1.9) );
  update_vbo();
}

void vsx_widget_profiler_plot::interpolate_size()
{
  vsx_widget::interpolate_size();
}
