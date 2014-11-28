#if PLATFORM == PLATFORM_LINUX
#include <sys/ioctl.h>

#include <linux/joystick.h>
#define JOY_DEV "/dev/input/js"
#define JOY_DEV1 "/dev/input/js0"
#define JOY_DEV2 "/dev/input/js1"

#endif

#if (PLATFORM == PLATFORM_LINUX)
struct js_event js;

class joystick_info {
public:
  vsx_nw_vector<vsx_module_param_float*> axes;
  vsx_nw_vector<vsx_module_param_float*> buttons;
  vsx_module_param_string* name;

  int joy_fd;
  int *axis;
  int num_of_axis;
  int num_of_buttons;
  int x;
  char *button, name_of_joystick[80];
  joystick_info()
    : joy_fd(-2),
      axis(0x0),
      num_of_axis(0),
      num_of_buttons(0),
      x(0),
      button(0)
  {
    memset(&name_of_joystick,0,80);
  }

  ~joystick_info()
  {
    if (axis)
      free(axis);

    if (button)
      free(button);
  }
};

class module_system_joystick : public vsx_module
{
  vsx_nw_vector<joystick_info> joysticks;
  int joystick_count;

public:
  
  module_system_joystick()
  {
    joystick_count = 0;
  }

  void on_delete()
  {
  }
  
  bool init_joysticks()
  {
    for (int i = 0; i < 10; i++)
    {
      joystick_info ji;
      if (joysticks[i].joy_fd == -2) // uninitialized!
      if ( ( ji.joy_fd = open( (vsx_string<>(JOY_DEV)+vsx_string_helper::i2s(i)).c_str() , O_RDONLY)) != -1 )
      {
        // enumerate joystick
        fcntl( ji.joy_fd, F_SETFL, O_NONBLOCK );

        ioctl( ji.joy_fd, JSIOCGAXES, &ji.num_of_axis );
        ioctl( ji.joy_fd, JSIOCGBUTTONS, &ji.num_of_buttons );
        ioctl( ji.joy_fd, JSIOCGNAME(80), &ji.name_of_joystick );
        joysticks[i] = ji;
        
        joystick_count++;
      }
    }
    return true;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "system;joystick";

    info->description =
      "Joystick input";

    info->in_param_spec = "";

    info->component_class = "system";

    init_joysticks();

    if (joystick_count == 0)
    {
      info->out_param_spec = "no_joystick_detected:complex{}";
      return;
    }

    for (int j = 0; j < joystick_count; j++)
    {
      if (j != 0) info->out_param_spec += ",";
      info->out_param_spec += "joystick_"+vsx_string_helper::i2s(j)+":complex{j_"+vsx_string_helper::i2s(j)+"_name:string,axes:complex{";
      for (int i = 0; i < joysticks[j].num_of_axis; i++) {
        if (i != 0) info->out_param_spec += ",";
        info->out_param_spec += "j_"+vsx_string_helper::i2s(j)+"_axis"+vsx_string_helper::i2s(i)+":float";
      }
      info->out_param_spec += "},";
      info->out_param_spec += "buttons:complex{";
      for (int i = 0; i < joysticks[j].num_of_buttons; i++) {
        if (i != 0) info->out_param_spec += ",";
        info->out_param_spec += "j_"+vsx_string_helper::i2s(j)+"_button"+vsx_string_helper::i2s(i)+":float";
      }
      info->out_param_spec += "}}";
    }

    info->output = 1;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(in_parameters);
    loading_done = true;

    init_joysticks();

    for (int j = 0; j < joystick_count; j++)
    {
      joysticks[j].axis = (int *) calloc( joysticks[j].num_of_axis, sizeof( int ) );
      joysticks[j].button = (char *) calloc( joysticks[j].num_of_buttons, sizeof( char ) );

      joysticks[j].name = (vsx_module_param_string*)out_parameters.create(VSX_MODULE_PARAM_ID_STRING, vsx_string<>("j_"+vsx_string_helper::i2s(j)+"_name").c_str());
      joysticks[j].name->set(vsx_string<>(joysticks[j].name_of_joystick));
      for (int i = 0; i < joysticks[j].num_of_buttons; i++) {
        joysticks[j].buttons[i] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,(vsx_string<>("j_"+vsx_string_helper::i2s(j)+"_button")+vsx_string_helper::i2s(i)).c_str());
        joysticks[j].buttons[i]->set(0.0f);
      }
      for (int i = 0; i < joysticks[j].num_of_axis; i++) {
        joysticks[j].axes[i] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,(vsx_string<>("j_"+vsx_string_helper::i2s(j)+"_axis")+vsx_string_helper::i2s(i)).c_str());
        joysticks[j].axes[i]->set(0.0f);
      }
    }
  }

  void run()
  {
    for (int j = 0; j < joystick_count; j++)
    {
      for (int k = 0; k < 10; k++)
      {
        js.type = 0;
        /* read the joystick state */
        if (read(joysticks[j].joy_fd, &js, sizeof(struct js_event)))
        {

          /* see what to do with the event */
          switch (js.type & ~JS_EVENT_INIT)
          {
            case JS_EVENT_AXIS:
              if (js.number < joysticks[j].num_of_axis)
              joysticks[j].axis   [ js.number ] = js.value;
            break;
            case JS_EVENT_BUTTON:
              if (js.number < joysticks[j].num_of_buttons)
              joysticks[j].button [ js.number ] = js.value;
            break;
          }

          for (int i = 0; i < joysticks[j].num_of_buttons; i++) {
            joysticks[j].buttons[i]->set( (float) joysticks[j].button[i]);
          }

          for (int i = 0; i < joysticks[j].num_of_axis; i++) {
            joysticks[j].axes[i]->set( (float) joysticks[j].axis[i] / 32768.0f);
          }
        }
      }
    }
  }

};
#endif


