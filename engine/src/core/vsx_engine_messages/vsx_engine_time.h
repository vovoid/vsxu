#ifndef VSX_DEMO_MINI
    if (cmd == "play") {
      play();
    } else
    if (cmd == "stop") {
      current_state = VSX_ENGINE_STOPPED;
    } else
    if (cmd == "rewind") {
      current_state = VSX_ENGINE_REWIND;
    } else
#ifndef VSXU_NO_CLIENT
    if (cmd == "fps_d" || cmd == "fps") {
    	cmd_out->add("fps_d",f2s(frame_dfps));
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
