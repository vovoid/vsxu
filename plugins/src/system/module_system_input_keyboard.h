#include <module/vsx_module.h>
#include <input/states/vsx_input_state_keyboard.h>
#include <input/vsx_input_event_queue_reader.h>


class module_system_input_keyboard : public vsx_module
{
  vsx_input_state_keyboard state_keyboard;

public:

  // out - buttons
  vsx_module_param_float* button_left;
  vsx_module_param_float* button_right;
  vsx_module_param_float* button_up;
  vsx_module_param_float* button_down;

  vsx_module_param_float* button_a;
  vsx_module_param_float* button_b;
  vsx_module_param_float* button_c;
  vsx_module_param_float* button_d;
  vsx_module_param_float* button_e;
  vsx_module_param_float* button_f;
  vsx_module_param_float* button_g;
  vsx_module_param_float* button_h;
  vsx_module_param_float* button_i;
  vsx_module_param_float* button_j;
  vsx_module_param_float* button_k;
  vsx_module_param_float* button_l;
  vsx_module_param_float* button_m;
  vsx_module_param_float* button_n;
  vsx_module_param_float* button_o;
  vsx_module_param_float* button_p;
  vsx_module_param_float* button_q;
  vsx_module_param_float* button_r;
  vsx_module_param_float* button_s;
  vsx_module_param_float* button_t;
  vsx_module_param_float* button_u;
  vsx_module_param_float* button_v;
  vsx_module_param_float* button_w;
  vsx_module_param_float* button_x;
  vsx_module_param_float* button_y;
  vsx_module_param_float* button_z;

  vsx_module_param_float* button_f1;
  vsx_module_param_float* button_f2;
  vsx_module_param_float* button_f3;
  vsx_module_param_float* button_f4;
  vsx_module_param_float* button_f5;
  vsx_module_param_float* button_f6;
  vsx_module_param_float* button_f7;
  vsx_module_param_float* button_f8;
  vsx_module_param_float* button_f9;
  vsx_module_param_float* button_f10;
  vsx_module_param_float* button_f11;
  vsx_module_param_float* button_f12;

  vsx_module_param_float* button_esc;
  vsx_module_param_float* button_tab;
  vsx_module_param_float* button_enter;
  vsx_module_param_float* button_space;
  vsx_module_param_float* button_backspace;
  vsx_module_param_float* button_pgup;
  vsx_module_param_float* button_pgdn;
  vsx_module_param_float* button_shift_left;
  vsx_module_param_float* button_shift_right;
  vsx_module_param_float* button_ctrl_left;
  vsx_module_param_float* button_ctrl_right;
  vsx_module_param_float* button_alt_left;
  vsx_module_param_float* button_alt_right;


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "system;input;input_keyboard";

    info->description =
      "Keyboard input.\n"
      "Note that it only works in fullscreen\n"
      "mode!"
      ;

    info->in_param_spec = "";
    info->out_param_spec =
        "arrow_keys:complex"
        "{"
          "button_left:float,"
          "button_right:float,"
          "button_up:float,"
          "button_down:float"
        "},"
        "buttons_a_z:complex"
        "{"
          "button_a:float,"
          "button_b:float,"
          "button_c:float,"
          "button_d:float,"
          "button_e:float,"
          "button_f:float,"
          "button_g:float,"
          "button_h:float,"
          "button_i:float,"
          "button_j:float,"
          "button_k:float,"
          "button_l:float,"
          "button_m:float,"
          "button_n:float,"
          "button_o:float,"
          "button_p:float,"
          "button_q:float,"
          "button_r:float,"
          "button_s:float,"
          "button_t:float,"
          "button_u:float,"
          "button_v:float,"
          "button_w:float,"
          "button_x:float,"
          "button_y:float,"
          "button_z:float"
        "},"
        "function_keys:complex"
        "{"
          "button_f1:float,"
          "button_f2:float,"
          "button_f3:float,"
          "button_f4:float,"
          "button_f5:float,"
          "button_f6:float,"
          "button_f7:float,"
          "button_f8:float,"
          "button_f9:float,"
          "button_f10:float,"
          "button_f11:float,"
          "button_f12:float"
        "},"
        "misc_keys:complex"
        "{"
          "button_esc:float,"
          "button_tab:float,"
          "button_enter:float,"
          "button_space:float,"
          "button_backspace:float,"
          "button_pgup:float,"
          "button_pgdn:float,"
          "button_shift_left:float,"
          "button_shift_right:float,"
          "button_ctrl_left:float,"
          "button_ctrl_right:float,"
          "button_alt_left:float,"
          "button_alt_right:float"
        "}"

      ;

    info->component_class = "system";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(in_parameters);
    loading_done = true;

    button_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_left");
    button_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_right");
    button_up = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_up");
    button_down = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_down");

    button_a = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_a");
    button_b = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_b");
    button_c = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_c");
    button_d = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_d");
    button_e = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_e");
    button_f = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f");
    button_g = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_g");
    button_h = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_h");
    button_i = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_i");
    button_j = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_j");
    button_k = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_k");
    button_l = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_l");
    button_m = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_m");
    button_n = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_n");
    button_o = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_o");
    button_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_p");
    button_q = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_q");
    button_r = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_r");
    button_s = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_s");
    button_t = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_t");
    button_u = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_u");
    button_v = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_v");
    button_w = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_w");
    button_x = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_x");
    button_y = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_y");
    button_z = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_z");

    button_f1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f1");
    button_f2 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f2");
    button_f3 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f3");
    button_f4 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f4");
    button_f5 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f5");
    button_f6 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f6");
    button_f7 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f7");
    button_f8 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f8");
    button_f9 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f9");
    button_f10 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f10");
    button_f11 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f11");
    button_f12 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_f12");

    button_esc = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_esc");
    button_tab = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_tab");
    button_enter = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_enter");
    button_space = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_space");
    button_backspace = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_backspace");
    button_pgup = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_pgup");
    button_pgdn = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_pgdn");
    button_shift_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_shift_left");
    button_shift_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_shift_right");
    button_ctrl_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_ctrl_left");
    button_ctrl_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_ctrl_right");
    button_alt_left = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_alt_left");
    button_alt_right = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "button_alt_right");
  }

  void run()
  {
    vsx_input_event_queue_reader_consume(engine_state->event_queue, state_keyboard);

    button_left->set( state_keyboard.pressed( VSX_SCANCODE_LEFT ) );
    button_right->set( state_keyboard.pressed( VSX_SCANCODE_RIGHT ) );
    button_up->set( state_keyboard.pressed( VSX_SCANCODE_UP ) );
    button_down->set( state_keyboard.pressed( VSX_SCANCODE_DOWN ) );

    button_a->set( state_keyboard.pressed( VSX_SCANCODE_A ) );
    button_b->set( state_keyboard.pressed( VSX_SCANCODE_B ) );
    button_c->set( state_keyboard.pressed( VSX_SCANCODE_C ) );
    button_d->set( state_keyboard.pressed( VSX_SCANCODE_D ) );
    button_e->set( state_keyboard.pressed( VSX_SCANCODE_E ) );
    button_f->set( state_keyboard.pressed( VSX_SCANCODE_F ) );
    button_g->set( state_keyboard.pressed( VSX_SCANCODE_G ) );
    button_h->set( state_keyboard.pressed( VSX_SCANCODE_H ) );
    button_i->set( state_keyboard.pressed( VSX_SCANCODE_I ) );
    button_j->set( state_keyboard.pressed( VSX_SCANCODE_J ) );
    button_k->set( state_keyboard.pressed( VSX_SCANCODE_K ) );
    button_l->set( state_keyboard.pressed( VSX_SCANCODE_L ) );
    button_m->set( state_keyboard.pressed( VSX_SCANCODE_M ) );
    button_n->set( state_keyboard.pressed( VSX_SCANCODE_N ) );
    button_o->set( state_keyboard.pressed( VSX_SCANCODE_O ) );
    button_p->set( state_keyboard.pressed( VSX_SCANCODE_P ) );
    button_q->set( state_keyboard.pressed( VSX_SCANCODE_Q ) );
    button_r->set( state_keyboard.pressed( VSX_SCANCODE_R ) );
    button_s->set( state_keyboard.pressed( VSX_SCANCODE_S ) );
    button_t->set( state_keyboard.pressed( VSX_SCANCODE_T ) );
    button_u->set( state_keyboard.pressed( VSX_SCANCODE_U ) );
    button_v->set( state_keyboard.pressed( VSX_SCANCODE_V ) );
    button_w->set( state_keyboard.pressed( VSX_SCANCODE_W ) );
    button_x->set( state_keyboard.pressed( VSX_SCANCODE_X ) );
    button_y->set( state_keyboard.pressed( VSX_SCANCODE_Y ) );
    button_z->set( state_keyboard.pressed( VSX_SCANCODE_Z ) );

    button_f1->set( state_keyboard.pressed( VSX_SCANCODE_F1 ) );
    button_f2->set( state_keyboard.pressed( VSX_SCANCODE_F2 ) );
    button_f3->set( state_keyboard.pressed( VSX_SCANCODE_F3 ) );
    button_f4->set( state_keyboard.pressed( VSX_SCANCODE_F4 ) );
    button_f5->set( state_keyboard.pressed( VSX_SCANCODE_F5 ) );
    button_f6->set( state_keyboard.pressed( VSX_SCANCODE_F6 ) );
    button_f7->set( state_keyboard.pressed( VSX_SCANCODE_F7 ) );
    button_f8->set( state_keyboard.pressed( VSX_SCANCODE_F8 ) );
    button_f9->set( state_keyboard.pressed( VSX_SCANCODE_F9 ) );
    button_f10->set( state_keyboard.pressed( VSX_SCANCODE_F10 ) );
    button_f11->set( state_keyboard.pressed( VSX_SCANCODE_F11 ) );
    button_f12->set( state_keyboard.pressed( VSX_SCANCODE_F12 ) );

    button_esc->set( state_keyboard.pressed( VSX_SCANCODE_ESCAPE ) );
    button_tab->set( state_keyboard.pressed( VSX_SCANCODE_TAB ) );
    button_enter->set( state_keyboard.pressed( VSX_SCANCODE_RETURN) );
    button_space->set( state_keyboard.pressed( VSX_SCANCODE_SPACE) );
    button_backspace->set( state_keyboard.pressed( VSX_SCANCODE_BACKSPACE) );
    button_pgup->set( state_keyboard.pressed( VSX_SCANCODE_PAGEUP) );
    button_pgdn->set( state_keyboard.pressed( VSX_SCANCODE_PAGEDOWN ) );
    button_shift_left->set( state_keyboard.pressed( VSX_SCANCODE_LSHIFT ) );
    button_shift_right->set( state_keyboard.pressed( VSX_SCANCODE_RSHIFT ) );
    button_ctrl_left->set( state_keyboard.pressed( VSX_SCANCODE_LCTRL ) );
    button_ctrl_right->set( state_keyboard.pressed( VSX_SCANCODE_RCTRL ) );
    button_alt_left->set( state_keyboard.pressed( VSX_SCANCODE_LALT ) );
    button_alt_right->set( state_keyboard.pressed( VSX_SCANCODE_RALT ) );
  }

};
