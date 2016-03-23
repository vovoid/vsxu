#include <module/vsx_module.h>
#include <input/states/game/vsx_input_state_game_controller.h>
#include <input/vsx_input_event_queue_reader.h>


class module_system_input_game_controler : public vsx_module
{
  vsx_input_state_game_controller state_game_controller;

public:

  // out - analog
  vsx_module_param_float* analog_left_x;
  vsx_module_param_float* analog_left_y;
  vsx_module_param_float* analog_right_x;
  vsx_module_param_float* analog_right_y;
  vsx_module_param_float* analog_trigger_left;
  vsx_module_param_float* analog_trigger_right;

  // out - d-pad
  vsx_module_param_float* d_pad_left;
  vsx_module_param_float* d_pad_right;
  vsx_module_param_float* d_pad_up;
  vsx_module_param_float* d_pad_down;
  vsx_module_param_float* d_pad_direction_x;
  vsx_module_param_float* d_pad_direction_y;

  // out - buttons
  vsx_module_param_float* button_left;
  vsx_module_param_float* button_right;
  vsx_module_param_float* button_up;
  vsx_module_param_float* button_down;
  vsx_module_param_float* button_analog_left;
  vsx_module_param_float* button_analog_right;
  vsx_module_param_float* button_shoulder_left;
  vsx_module_param_float* button_shoulder_right;
  vsx_module_param_float* button_start;
  vsx_module_param_float* button_select;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "system;input;input_game_controller";

    info->description =
      "Game controller";

    info->in_param_spec = "";
    info->out_param_spec =
        "analog:complex"
        "{"
          "left:complex"
          "{"
            "analog_left_x:float,"
            "analog_left_y:float"
          "},"
          "right:complex"
          "{"
            "analog_right_x:float,"
            "analog_right_y:float"
          "},"
          "trigger:complex"
          "{"
            "analog_trigger_left:float,"
            "analog_trigger_right:float"
          "}"
        "},"
        "d_pad:complex"
        "{"
          "direction:complex"
          "{"
            "d_pad_direction_x:float,"
            "d_pad_direction_y:float"
          "},"
          "d_pad_left:float,"
          "d_pad_right:float,"
          "d_pad_up:float,"
          "d_pad_down:float"
        "},"
        "buttons:complex"
        "{"
          "button_left:float,"
          "button_right:float,"
          "button_up:float,"
          "button_down:float,"
          "button_analog_left:float,"
          "button_analog_right:float,"
          "button_shoulder_left:float,"
          "button_shoulder_right:float,"
          "button_start:float,"
          "button_select:float,"
        "}"
      ;

    info->component_class = "system";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(in_parameters);
    loading_done = true;

    analog_left_x = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "analog_left_x");
    analog_left_y = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "analog_left_y");
    analog_right_x = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "analog_right_x");
    analog_right_y = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "analog_right_y");
    analog_trigger_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "analog_trigger_left");
    analog_trigger_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "analog_trigger_right");

    d_pad_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "d_pad_left");
    d_pad_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "d_pad_right");
    d_pad_up = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "d_pad_up");
    d_pad_down = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "d_pad_down");
    d_pad_direction_x = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "d_pad_direction_x");
    d_pad_direction_y = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "d_pad_direction_y");

    button_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_left");
    button_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_right");
    button_up = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_up");
    button_down = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_down");
    button_analog_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_analog_left");
    button_analog_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_analog_right");
    button_shoulder_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_shoulder_left");
    button_shoulder_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_shoulder_right");
    button_start = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_start");
    button_select = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_select");
  }

  void run()
  {
    vsx_input_event_queue_reader_consume(engine_state->event_queue, state_game_controller);

    analog_left_x->set( state_game_controller.analog_left.x );
    analog_left_y->set( state_game_controller.analog_left.y );
    analog_right_x->set( state_game_controller.analog_right.x );
    analog_right_y->set( state_game_controller.analog_right.y );
    analog_trigger_left->set( state_game_controller.trigger_left );
    analog_trigger_right->set( state_game_controller.trigger_right );
    d_pad_left->set( state_game_controller.d_pad_left);
    d_pad_right->set( state_game_controller.d_pad_right);
    d_pad_up->set( state_game_controller.d_pad_up);
    d_pad_down->set( state_game_controller.d_pad_down);
    d_pad_direction_x->set( state_game_controller.d_pad_direction().x);
    d_pad_direction_y->set( state_game_controller.d_pad_direction().y);

    button_left->set( state_game_controller.button_left);
    button_right->set( state_game_controller.button_right);
    button_up->set( state_game_controller.button_up);
    button_down->set( state_game_controller.button_down);
    button_analog_left->set( state_game_controller.button_analog_left);
    button_analog_right->set( state_game_controller.button_analog_right);
    button_shoulder_left->set( state_game_controller.button_shoulder_left);
    button_shoulder_right->set( state_game_controller.button_shoulder_right);
    button_start->set( state_game_controller.button_start);
    button_select->set( state_game_controller.button_select);
  }

};


