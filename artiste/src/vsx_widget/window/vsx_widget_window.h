#ifndef VSX_WIDGET_WINDOW_H_
#define VSX_WIDGET_WINDOW_H_

class vsx_widget_window : public vsx_widget {
	vsx_widget* button_close;
public:
	vsx_widget_window();
  virtual void init();
  virtual void i_draw();
};



#endif /*VSX_WIDGET_WINDOW_H_*/
