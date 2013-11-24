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
  vsx_avector<vsx_module_param_float*> axes;
  vsx_avector<vsx_module_param_float*> buttons;
  vsx_module_param_string* name;

  int joy_fd;
  int *axis;
  int num_of_axis;
  int num_of_buttons;
  int x;
  char *button, name_of_joystick[80];
  joystick_info() {
    joy_fd = -2;
    num_of_buttons = 0;
    num_of_axis = 0;
    memset(&name_of_joystick,0,80);
  }
};

class module_system_joystick : public vsx_module
{
  vsx_avector<joystick_info> joysticks;
  int joystick_count;
public:
  
  module_system_joystick() {
    joystick_count = 0;
  }
  
  bool init_joysticks()
  {
//    printf("start init joysticks\n");
    for (int i = 0; i < 10; i++)
    {
      joystick_info ji;
      if (joysticks[i].joy_fd == -2) // uninitialized!
      if ( ( ji.joy_fd = open( (vsx_string(JOY_DEV)+i2s(i)).c_str() , O_RDONLY)) != -1 )
      {
        // enumerate joystick
        fcntl( ji.joy_fd, F_SETFL, O_NONBLOCK );

        ioctl( ji.joy_fd, JSIOCGAXES, &ji.num_of_axis );
        ioctl( ji.joy_fd, JSIOCGBUTTONS, &ji.num_of_buttons );
        ioctl( ji.joy_fd, JSIOCGNAME(80), &ji.name_of_joystick );
        /*printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
          , ji.name_of_joystick
          , ji.num_of_axis
          , ji.num_of_buttons );*/
        joysticks[i] = ji;
//        printf("ji.joy_fd: %d\n", ji.joy_fd);
        
        joystick_count++;
      }
    }
/*    joy_fd = -1;
    num_of_axis = 0;
    num_of_buttons = 0;
    if( ( joy_fd = open( JOY_DEV1 , O_RDONLY)) == -1 )
    {
      printf( "Couldn't open joystick\n" );
      return false;
    }
    fcntl( joy_fd, F_SETFL, O_NONBLOCK );

    ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
    ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
    ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );
    printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
      , name_of_joystick
      , num_of_axis
      , num_of_buttons );*/
    return true;
  }

void module_info(vsx_module_info* info)
{
  info->output = 1;
  info->identifier = "system;joystick";
  info->description = "Joystick input";
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
    info->out_param_spec += "joystick_"+i2s(j)+":complex{j_"+i2s(j)+"_name:string,axes:complex{";
    for (int i = 0; i < joysticks[j].num_of_axis; i++) {
      if (i != 0) info->out_param_spec += ",";
      info->out_param_spec += "j_"+i2s(j)+"_axis"+i2s(i)+":float"; 
    }
    info->out_param_spec += "},";
    info->out_param_spec += "buttons:complex{"; 
//    printf("num of buttons: %d\n", joysticks[j].num_of_buttons);
    for (int i = 0; i < joysticks[j].num_of_buttons; i++) {
      if (i != 0) info->out_param_spec += ",";
      info->out_param_spec += "j_"+i2s(j)+"_button"+i2s(i)+":float"; 
    }
    info->out_param_spec += "}}";
  }
//  printf("%s\n", info->out_param_spec.c_str());
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  VSX_UNUSED(in_parameters);
  loading_done = true;
  
  init_joysticks();

  //if (!init_joystick()) return;  
  //close( joy_fd );
  //joy_fd = -1;
  for (int j = 0; j < joystick_count; j++)
  {  
    printf("FOO num of axes: %d\n", joysticks[j].num_of_axis);
    joysticks[j].axis = (int *) calloc( joysticks[j].num_of_axis, sizeof( int ) );
    joysticks[j].button = (char *) calloc( joysticks[j].num_of_buttons, sizeof( char ) );

    joysticks[j].name = (vsx_module_param_string*)out_parameters.create(VSX_MODULE_PARAM_ID_STRING, vsx_string("j_"+i2s(j)+"_name").c_str());
    joysticks[j].name->set(vsx_string(joysticks[j].name_of_joystick));
    for (int i = 0; i < joysticks[j].num_of_buttons; i++) {
      joysticks[j].buttons[i] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,(vsx_string("j_"+i2s(j)+"_button")+i2s(i)).c_str());
      joysticks[j].buttons[i]->set(0.0f);
    }
    for (int i = 0; i < joysticks[j].num_of_axis; i++) {
      joysticks[j].axes[i] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,(vsx_string("j_"+i2s(j)+"_axis")+i2s(i)).c_str());
      joysticks[j].axes[i]->set(0.0f);
    }
  }
  //float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"shutdown");
  //float_in->set(0);
  //render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  //--------------------------------------------------------------------------------------------------  
}

void run() {
  //if (joy_fd == -1)
//    if (!init_joystick())
    //return;
  for (int j = 0; j < joystick_count; j++)
  {
//    printf("reading from : %d\n", joysticks[j].joy_fd);
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
            //printf("js axis-number: %d\n", js.number);
            if (js.number < joysticks[j].num_of_axis)
            joysticks[j].axis   [ js.number ] = js.value;
          break;
          case JS_EVENT_BUTTON:
            //printf("js button-number: %d\n", js.number);
            if (js.number < joysticks[j].num_of_buttons)
            joysticks[j].button [ js.number ] = js.value;
          break;
        }
        //return;
      
        for (int i = 0; i < joysticks[j].num_of_buttons; i++) {
          joysticks[j].buttons[i]->set( (float) joysticks[j].button[i]);
        }
    
        for (int i = 0; i < joysticks[j].num_of_axis; i++) {
          joysticks[j].axes[i]->set( (float) joysticks[j].axis[i] / 32768.0f);
        }
      }
    }
  }
  
  /* print the results */
/*printf( "X: %6d  Y: %6d  ", axis[0], axis[1] );

if( num_of_axis > 2 )
  printf("Z: %6d  ", axis[2] );
  
if( num_of_axis > 3 )
  printf("R: %6d  ", axis[3] );

if( num_of_axis > 4 )
  printf("R1: %6d  ", axis[4] );

if( num_of_axis > 5 )
  printf("R2: %6d  ", axis[5] );
  
for( x=0 ; x<num_of_buttons ; ++x )
  printf("B%d: %d  ", x, button[x] );
printf("\n");
*/  
}

};
#endif


