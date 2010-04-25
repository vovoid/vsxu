#ifndef VSX_WIDGET_CONTROLLER_SEQ_H
#define VSX_WIDGET_CONTROLLER_SEQ_H


class vsx_widget_controller_sequence : public vsx_widget_base_controller {
private:
  vsx_widget* seq_chan;
  int size_controlled_from_outside;
  int draw_base;
  int parent_removal;
public:
  void init();
  virtual void vsx_command_process_b(vsx_command_s *t);
  void set_size_controlled_from_outside(int new_value) { size_controlled_from_outside = new_value; }
  void set_span(float, float);
  void set_view_time(float, float);
  void set_draw_base(int new_val)
  {
  	draw_base = new_val;
  }
  void set_parent_removal(int new_val)
  {
  	parent_removal = new_val;
  }
  virtual void set_size(vsx_vector new_size);

  virtual void i_draw();
  vsx_widget_controller_sequence() :
  		seq_chan(0x0),
  		size_controlled_from_outside(0),
  		draw_base(1),
  		parent_removal(0)
	{};
};




#endif

