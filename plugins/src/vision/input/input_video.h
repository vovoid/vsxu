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


#ifndef VIDEO_INPUT_H
#define VIDEO_INPUT_H

#include <cv.h>
#include <pthread.h>
#include "vsx_gl_global.h"
#include <math/vector/vsx_vector3.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <bitmap/vsx_bitmap.h>

#define N_BUFFERS 2

class module_video_input : public vsx_module {
  /**
   * 
   * The Abstract Base class for Video Capture class.
   * As Video capture is a CPU intensive/ blocking operation, this class implements
   * The thread management routines.
   * The Derived classes should implement the
   *    1) Worker Thread
   *    2) Module specific functions from vsx_module (eg: module_info)
   *
   * The module's thread synchronization is maintained by a "Shared Task List".
   *
   * Any thread can issue a task for any other thread.
   * Currently we have
   * 1)a worker thread which fetches video frames and puts it in the next buffer page.
   * 2)a main thread, which consumes the current buffer page and flips the current page.
   * 
   */

protected:
  vsx_bitmap m_bitm;
  vsx_module_param_bitmap* m_result;

  /**
   * The double buffer needed for smoother video capture.
   */
  IplImage* m_buffer[N_BUFFERS];
  bool m_bufferReady;

  void initializeBuffers(int w, int h, int depth, int nChannels);
  int currentPage();
  int nextPage();
  void flipPage();
  void freeBuffers();

  enum Tasks {
    INITIALIZE_CAPTURE, FETCH_FRAME, TERMINATE_CAPTURE, // Main Thread gets to add these tasks
    CONSUME_FRAME, IGNORE_FRAME, CLEANUP_CAPTURE // Worker Thread gets to add these tasks
  };
  Tasks currentTask();
  void addTask(Tasks task);

  /**
   * Reimplement this function to return if a module's state is valid/it can be allowed to initialized'.
   * For eg. this function can return false if there are no hardware cameras available for the camera module.
   */
  virtual bool isValid();

  /**
   * This is the actual function which is run in a separate thread indefinitely.
   * Reimplement this method in the derived class.
   * It is the worker's responsibility to initialize and free buffers to be used.
   */
  virtual void worker()=0;

  // Gently shutdown the worker thread
  void release_capture();

public:
  module_video_input();
  ~module_video_input();

  // Initializes the datastructures and starts the worker thread
  bool init();

  //This function is called once per every frame VSXu Renders.
  void run();

  void stop();

  //calls release_capture
  void on_delete();

private:
  //Thread details
  pthread_t m_worker;
  pthread_attr_t m_worker_attribute;

  pthread_mutex_t m_mutex;

  int m_currentPage;
  // The main shared variable
  Tasks m_currentTask;

  // Just a useless frame counter
  int nFrames;

  //Boilerplate code to start the worker thread for the real action!
  inline static void* startWorker(void *obj) {
    static_cast<module_video_input*>(obj)->worker();
    pthread_exit(0);
  }

};

#endif // VIDEO_INPUT_H
