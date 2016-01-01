#include <gl_helper.h>
#include <math/vector/vsx_vector3_helper.h>

#include "vsx_widget_profiler_thread.h"
#include "time_scale.h"




void vsx_widget_profiler_thread::init()
{
  support_interpolation = true;
  allow_resize_x = true;
  allow_resize_y = true;
  set_size(vsx_vector3<>(20.0f,0.3f));
  set_pos(vsx_vector3<>(0,0));
  size_min.x = 0.2;
  size_min.y = 0.2;

  profiler = vsx_profiler_manager::get_instance()->get_profiler();

  title = "thread";

  allow_move_x = false;

  this->interpolate_size();
  init_run = true;
}


void vsx_widget_profiler_thread::load_thread(uint64_t id)
{
  vsx_printf(L"load profile thread %ld\n", id);
  vsx_profiler_consumer::get_instance()->get_thread(0.0, 5.0, id, consumer_chunks);
  update();
}

void vsx_widget_profiler_thread::update()
{
  update_vbo();
  update_tag_draw_chunks();
}


void vsx_widget_profiler_thread::update_vbo()
{
  draw_bucket.vertices.reset_used();
  draw_bucket.faces.reset_used();

  vsx_printf(L"time scale: %f\n", time_scale::get_instance()->time_scale_x);

  size_t max_depth = 1;

  for (size_t i = 0; i < consumer_chunks.size(); i++)
  {
    vsx_profiler_consumer_chunk& chunk = consumer_chunks[i];

    double cts = (chunk.time_start ) * time_scale::get_instance()->time_scale_x;
    double cte = (chunk.time_end ) * time_scale::get_instance()->time_scale_x;


    if (chunk.depth > max_depth)
      max_depth = chunk.depth;

    float depth = -(chunk.depth + 1.0) * chunk_height;
    cts += time_scale::get_instance()->time_offset;
    cte += time_scale::get_instance()->time_offset;

    if (cte < -15.0 && cts < -15.0)
      continue;

    if (cte > 15.0 && cts > 15.0)
      continue;

    if (cte - cts < 0.002)
      continue;

    draw_bucket.vertices.push_back( vsx_vector3<>( cts, depth ) );
    draw_bucket.vertices.push_back( vsx_vector3<>( cts, depth - chunk_height ) );
    draw_bucket.vertices.push_back( vsx_vector3<>( cte, depth - chunk_height ) );
    draw_bucket.vertices.push_back( vsx_vector3<>( cte, depth ) );

    draw_bucket.vertex_colors.push_back( vsx_color<>(1.0, 1.0, 1.0, 0.5) );
    draw_bucket.vertex_colors.push_back( vsx_color<>(1.0, 1.0, 1.0, 0.5) );
    draw_bucket.vertex_colors.push_back( vsx_color<>(1.0, 1.0, 1.0, 0.5) );
    draw_bucket.vertex_colors.push_back( vsx_color<>(1.0, 1.0, 1.0, 0.5) );

    time_scale::get_instance()->chunk_time_end = chunk.time_end;
    time_scale::get_instance()->time_size_x = chunk.time_end * time_scale::get_instance()->time_scale_x;

    line_index idx;
    idx.a = draw_bucket.vertices.size() - 4;
    idx.b = draw_bucket.vertices.size() - 3;
    draw_bucket.faces.push_back( idx );

    idx.a = draw_bucket.vertices.size() - 3;
    idx.b = draw_bucket.vertices.size() - 2;
    draw_bucket.faces.push_back( idx );

    idx.a = draw_bucket.vertices.size() - 2;
    idx.b = draw_bucket.vertices.size() - 1;
    draw_bucket.faces.push_back( idx );
  }

  time_scale::get_instance()->one_div_chunk_time_end = 1.0 / time_scale::get_instance()->chunk_time_end;
  time_scale::get_instance()->one_div_time_size_x = 1.0 / time_scale::get_instance()->time_size_x;

  draw_bucket.invalidate_vertices();
  draw_bucket.invalidate_colors();
  draw_bucket.update();

  set_size(vsx_vector3<>(20.0f,chunk_height * (float)(max_depth+3)));
}

void vsx_widget_profiler_thread::update_tag_draw_chunks()
{
  tag_draw_chunks.reset_used();

  for (size_t i = 0; i < consumer_chunks.size(); i++)
  {
    if
    (
      consumer_chunks[i].time_start * time_scale::get_instance()->time_scale_x + time_scale::get_instance()->time_offset > -10.0
      &&
      (consumer_chunks[i].time_end * time_scale::get_instance()->time_scale_x) - (consumer_chunks[i].time_start * time_scale::get_instance()->time_scale_x ) > 0.02
    )
    {
      tag_draw_chunks.push_back( &consumer_chunks[i] );
    }

    if (consumer_chunks[i].time_start * time_scale::get_instance()->time_scale_x + time_scale::get_instance()->time_offset > 10.0)
      break;
  }
}

void vsx_widget_profiler_thread::draw_tags()
{
  font.color.r = 1.0;
  font.color.g = 1.0;
  font.color.b = 1.0;
  font.color.a = 1.0;

  for (size_t i = 0; i < tag_draw_chunks.size(); i++)
  {
    float chunk_depth = (float)(tag_draw_chunks[i]->depth + 1);
    vsx_vector3<> dpos(
          tag_draw_chunks[i]->time_start * time_scale::get_instance()->time_scale_x + time_scale::get_instance()->time_offset,
          -chunk_depth * chunk_height - 0.005
    );
    //vsx_vector3_helper::dump<float>(dpos);
    font.print( dpos, tag_draw_chunks[i]->tag, 0.005 );
    dpos.y -= 0.005;
    font.print( dpos, vsx_string_helper::i2s(tag_draw_chunks[i]->cycles_end-tag_draw_chunks[i]->cycles_start) + " CPU cycles", 0.005 );
    dpos.y -= 0.005;
    font.print( dpos, vsx_string_helper::f2s(tag_draw_chunks[i]->time_end - tag_draw_chunks[i]->time_start) + " seconds", 0.005 );
  }
  profiler->sub_end();
}

void vsx_widget_profiler_thread::i_draw()
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

    draw_bucket.output();
    profiler->sub_end();

    // green timeline
    glColor4f(0,1,0,1);
    glBegin(GL_LINES);
      glVertex2f( - 1000.0, -chunk_height + 0.001);
      glVertex2f(   1000.0, -chunk_height + 0.001);
    glEnd();



  if (selected_chunk != 0x0)
  {
    float depth = -(selected_chunk->depth + 1.0) * chunk_height;

    float cts = (selected_chunk->time_start ) * time_scale::get_instance()->time_scale_x + time_scale::get_instance()->time_offset;
    float cte = (selected_chunk->time_end )   * time_scale::get_instance()->time_scale_x + time_scale::get_instance()->time_offset;

    glColor4f(1, 0.25,0.25,0.95);
    glBegin(GL_LINES);
      glVertex2f( cts, depth);
      glVertex2f( cts, depth - chunk_height );

      glVertex2f( cts, depth - chunk_height );
      glVertex2f( cte, depth - chunk_height );

      glVertex2f( cte, depth - chunk_height );
      glVertex2f( cte, depth );

      glVertex2f( cte, depth );
      glVertex2f( cts, depth);
    glEnd();

  }
  draw_tags();
  glPopMatrix();
  vsx_widget::i_draw();
}

void vsx_widget_profiler_thread::event_mouse_wheel(float y)
{
  parent->event_mouse_wheel(y);
}



void vsx_widget_profiler_thread::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  double x = coords.world_global.x;
  double factor = time_scale::get_instance()->world_to_time_factor(x);

  // decide which way to iterate
//  long index = (long)(factor * consumer_chunks.size());


  selected_chunk = 0x0;

  int mouse_depth = depth_from_mouse_position();

  vsx_printf(L"mouse depth: %d\n", mouse_depth);

  if (mouse_depth < 0)
  {
    vsx_widget::event_mouse_down(distance, coords, button);
    return;
  }

  for (size_t i = 0; i < consumer_chunks.size(); i++)
  {
    if
    (
      factor < consumer_chunks[i].time_end * time_scale::get_instance()->one_div_chunk_time_end
      &&
      factor > consumer_chunks[i].time_start * time_scale::get_instance()->one_div_chunk_time_end
      &&
      mouse_depth == (int)consumer_chunks[i].depth
    )
    {
      selected_chunk = &consumer_chunks[i];
      return;
    }
  }
  vsx_widget::event_mouse_down(distance, coords, button);
}

void vsx_widget_profiler_thread::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(distance);
  //mouse_pos = coords.world_global - get_pos_p();
  mouse_pos = distance.center;
  parent->event_mouse_move_passive(distance, coords);
}

void vsx_widget_profiler_thread::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
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
  update_tag_draw_chunks();
}

void vsx_widget_profiler_thread::interpolate_size()
{
  vsx_widget::interpolate_size();
}
