#include <module/vsx_module.h>
#include <input/states/vsx_input_state_mouse.h>
#include <input/vsx_input_event_queue_reader.h>


class module_system_input_mouse: public vsx_module
{
  vsx_input_state_mouse state_mouse;

  // out - buttons
  vsx_module_param_float* button_left;
  vsx_module_param_float* button_middle;
  vsx_module_param_float* button_right;
  vsx_module_param_float* button_x1;
  vsx_module_param_float* button_x2;
  vsx_module_param_float* button_0;
  vsx_module_param_float* button_1;
  vsx_module_param_float* button_2;
  vsx_module_param_float* button_3;
  vsx_module_param_float* button_4;
  vsx_module_param_float* button_5;
  vsx_module_param_float* button_6;
  vsx_module_param_float* button_7;
  vsx_module_param_float* button_8;
  vsx_module_param_float* button_9;
  vsx_module_param_float* button_10;
  vsx_module_param_float* button_11;
  vsx_module_param_float* button_12;
  vsx_module_param_float* button_13;
  vsx_module_param_float* button_14;
  vsx_module_param_float* button_15;
  vsx_module_param_float* button_16;
  vsx_module_param_float* button_17;
  vsx_module_param_float* button_18;
  vsx_module_param_float* button_19;
  vsx_module_param_float* button_20;
  vsx_module_param_float* button_21;
  vsx_module_param_float* button_22;
  vsx_module_param_float* button_23;
  vsx_module_param_float* button_24;
  vsx_module_param_float* button_25;
  vsx_module_param_float* button_26;
  vsx_module_param_float* button_27;
  vsx_module_param_float* button_28;
  vsx_module_param_float* button_29;
  vsx_module_param_float* button_30;
  vsx_module_param_float* button_31;

  vsx_module_param_float* pos_x;
  vsx_module_param_float* pos_y;
  vsx_module_param_float* pos_screen_x;
  vsx_module_param_float* pos_screen_y;
  vsx_module_param_float* wheel_x;
  vsx_module_param_float* wheel_y;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "system;input;input_mouse";

    info->description =
      "Mouse input.\n"
      "Note that it only works in fullscreen\n"
      "mode!"
      ;

    info->in_param_spec = "";
    info->out_param_spec =
        "buttons:complex"
        "{"
          "button_left:float,"
          "button_middle:float,"
          "button_right:float,"
          "button_x1:float,"
          "button_x2:float"
        "},"
        "buttons_list:complex"
        "{"
          "button_0:float,"
          "button_1:float,"
          "button_2:float,"
          "button_3:float,"
          "button_4:float,"
          "button_5:float,"
          "button_6:float,"
          "button_7:float,"
          "button_8:float,"
          "button_9:float,"
          "button_10:float,"
          "button_11:float,"
          "button_12:float,"
          "button_13:float,"
          "button_14:float,"
          "button_15:float,"
          "button_16:float,"
          "button_17:float,"
          "button_18:float,"
          "button_19:float,"
          "button_20:float,"
          "button_21:float,"
          "button_22:float,"
          "button_23:float,"
          "button_24:float,"
          "button_25:float,"
          "button_26:float,"
          "button_27:float,"
          "button_28:float,"
          "button_29:float,"
          "button_30:float,"
          "button_31:float"
        "},"
        "movement:complex"
        "{"
          "pos_x:float,"
          "pos_y:float,"
          "pos_screen_x:float,"
          "pos_screen_y:float,"
          "wheel_x:float,"
          "wheel_y:float"
        "}"
      ;

    info->component_class = "system";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(in_parameters);
    loading_done = true;

    button_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_left");
    button_middle = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_middle");
    button_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_right");
    button_x1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_x1");
    button_x2 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_x2");

    button_0 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_0");
    button_1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_1");
    button_2 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_2");
    button_3 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_3");
    button_4 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_4");
    button_5 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_5");
    button_6 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_6");
    button_7 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_7");
    button_8 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_8");
    button_9 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_9");
    button_10 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_10");
    button_11 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_11");
    button_12 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_12");
    button_13 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_13");
    button_14 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_14");
    button_15 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_15");
    button_16 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_16");
    button_17 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_17");
    button_18 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_18");
    button_19 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_19");
    button_20 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_20");
    button_21 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_21");
    button_22 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_22");
    button_23 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_23");
    button_24 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_24");
    button_25 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_25");
    button_26 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_26");
    button_27 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_27");
    button_28 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_28");
    button_29 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_29");
    button_30 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_30");
    button_31 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_31");

    pos_x = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "pos_x");
    pos_y = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "pos_y");
    pos_screen_x = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "pos_screen_x");
    pos_screen_y = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "pos_screen_y");
    wheel_x = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "wheel_x");
    wheel_y = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "wheel_y");
  }

  void run()
  {
    vsx_input_event_queue_reader_consume(engine_state->event_queue, state_mouse);

    button_left->set( state_mouse.button_left );
    button_middle->set( state_mouse.button_middle );
    button_right->set( state_mouse.button_right );
    button_x1->set( state_mouse.button_x1 );
    button_x2->set( state_mouse.button_x2 );

    button_0->set( state_mouse.buttons[0] );
    button_1->set( state_mouse.buttons[1] );
    button_2->set( state_mouse.buttons[2] );
    button_3->set( state_mouse.buttons[3] );
    button_4->set( state_mouse.buttons[4] );
    button_5->set( state_mouse.buttons[5] );
    button_6->set( state_mouse.buttons[6] );
    button_7->set( state_mouse.buttons[7] );
    button_8->set( state_mouse.buttons[8] );
    button_9->set( state_mouse.buttons[9] );
    button_10->set( state_mouse.buttons[10] );
    button_11->set( state_mouse.buttons[11] );
    button_12->set( state_mouse.buttons[12] );
    button_13->set( state_mouse.buttons[13] );
    button_14->set( state_mouse.buttons[14] );
    button_15->set( state_mouse.buttons[15] );
    button_16->set( state_mouse.buttons[16] );
    button_17->set( state_mouse.buttons[17] );
    button_18->set( state_mouse.buttons[18] );
    button_19->set( state_mouse.buttons[19] );

    pos_x->set( state_mouse.position.x );
    pos_y->set( state_mouse.position.y );

    pos_screen_x->set( state_mouse.position_screen.x );
    pos_screen_y->set( state_mouse.position_screen.y );

    wheel_x->set( state_mouse.wheel.x );
    wheel_y->set( state_mouse.wheel.y );
  }

};
