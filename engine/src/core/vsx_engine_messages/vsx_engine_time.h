#ifndef VSX_DEMO_MINI
    // ***************************************
    // Set time loop point
    // ***************************************
    // 0=seq_pool 1=time_set_loop_point 2=[time:float]
    if (cmd == "time_set_loop_point")
    {
      loop_point_end = s2f(c->parts[1]);
    } else

    if (cmd == "play") {
      time_play();
    } else
    if (cmd == "stop") {
      current_state = VSX_ENGINE_STOPPED;
    } else
    if (cmd == "rewind") {
      current_state = VSX_ENGINE_REWIND;
    } else
#ifndef VSXU_NO_CLIENT
    if (cmd == "fps_d" || cmd == "fps") {
      cmd_out->add("fps_d",f2s(frame_delta_fps));
    }
    else
    if (cmd == "time_set") {
      float dd = engine_info.vtime - s2f(c->parts[1]);
      //engine_info.vtime = s2f(c->parts[1]);
      if (dd > 0) {
        engine_info.dtime = -dd;
      } else {
        engine_info.dtime = fabs(dd);
      }
    } else
#endif
#endif
