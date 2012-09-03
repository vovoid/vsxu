/**
* @author Dinesh Manajipet, Vovoid Media Technologies - Copyright (C) 2012-2020
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#ifndef MODULE_VIDEO_INPUT_H
#define MODULE_VIDEO_INPUT_H

#include <pthread.h>
#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"

#include <highgui.h>
#include <unistd.h>

#define N_BUFFERS 2

class module_video_input : public vsx_module {
  /**
   * The Abstract Base class for Video Capture class.
   * As Video capture is a CPU intensive/ blocking operation, this class implements
   * The thread management routines.
   * The Derived classes should implement the
   *    1) Worker Thread
   *    2) Module specific functions from vsx_module (eg: module_info)
   *
   * TODO: Implement a state machine representing the Worker's current state
   */

  //Thread details
  pthread_t m_worker;
  pthread_attr_t m_worker_attribute;

protected:
  vsx_bitmap m_bitm;
  vsx_module_param_bitmap* result;

  /**
   * The double buffer needed for smoother video capture.
   */
  IplImage* m_buffer[N_BUFFERS];
  bool m_buffersReady;
  int m_currentBuffer;

  bool m_isValid;
  bool m_isRunning;

  int width, height;
  int previous_frame;

  /**
   * A nice mutex for all the synchronization needs of the worker thread.
   */
  pthread_mutex_t m_mutex;

  void initializeBuffers(int w, int h, int depth, int nChannels);
  void freeBuffers();

  /**
   * This is the actual function which is run in a separate thread indefinitely.
   * Reimplement this method in the derived class.
   * It is the worker's responsibility to initialize and free buffers to be used.
   */
  virtual void *worker()=0;

  /**
   * Boilerplate code to start the worker thread for the real action!
   */
  static void* startWorker(void *obj) {
    static_cast<module_video_input*>(obj)->worker();
    pthread_exit(0);
  }

  /**
   * Gently shutdown the worker thread
   */
  void release_capture();

public:
  module_video_input();
  /**
   * Initialize the module
   */
  bool init();

  /**
   * This function is called once per every frame VSXu Renders.
   */
  void run();

  void stop();

  void on_delete();
};

#endif // MODULE_VIDEO_INPUT_H
