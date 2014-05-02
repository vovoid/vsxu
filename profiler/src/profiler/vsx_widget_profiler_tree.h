#ifndef VSX_WIDGET_SEQUENCE_TREE_H
#define VSX_WIDGET_SEQUENCE_TREE_H

#include <vsx_widget_window.h>
#include <widgets/vsx_widget_base_edit.h>

class vsx_widget_profiler_tree : public vsx_widget_editor
{
  vsx_widget_profiler* profiler;

public:

  void set_profiler( vsx_widget_profiler* p )
  {
    profiler = p;
  }

  void extra_init()
  {
    editor->enable_syntax_highlighting = false;
    editor->mirror_mouse_double_click_object = this;
  }

  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);
    VSX_UNUSED(button);

    profiler->load_profile( editor->selected_line );
  }
};

class vsx_widget_profiler_tree_window : public vsx_widget_window
{
  vsx_widget_profiler_tree* tree;

public:

  virtual void init()
  {
    vsx_widget_window::init();

    allow_resize_x = true;
    allow_resize_y = true;

    tree = (vsx_widget_profiler_tree*)add((vsx_widget*)(new vsx_widget_profiler_tree),"tree");
    tree->init();

    font_size = 0.03;
    tree->editor->set_font_size(0.008f);
    tree->editor->editing_enabled = false;
    tree->editor->selected_line_highlight = true;
    tree->editor->enable_syntax_highlighting = false;
    tree->editor->enable_line_action_buttons = true;
//    tree->coord_type = VSX_WIDGET_COORD_CORNER;


    tree->set_render_type(render_2d);
    tree->show();
  }

  virtual void i_draw()
  {
    vsx_widget_window::i_draw();

//    tree->set_pos(vsx_vector<>(size.x * 0.5,size.y * 0.5 - font_size + dragborder*0.5f));
//    tree->set_size(vsx_vector<>(size.x - dragborder * 2, size.y - 0.2 - font_size * 2.0 - dragborder * 2.0));


//    tree->set_size( vsx_vector<>( size.x , size.y - font_size - dragborder * 2)  );
//    tree->set_pos(vsx_vector<>(size.x * 0.5, (size.y - font_size) * 0.5));
//    timeline->target_pos.y = timeline->pos.y =  size.y*0.5f - dragborder-timeline->size.y*0.5f;

  }

  void set_profiler( vsx_widget_profiler* p )
  {
//    tree->set_profiler( p );
  }

  void set_string( vsx_string s )
  {
//    tree->set_string( s );
  }


};



#endif // VSX_WIDGET_SEQUENCE_TREE_H
