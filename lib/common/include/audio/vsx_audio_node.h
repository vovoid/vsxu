#ifndef VSX_AUDIO_NODE_H
#define VSX_AUDIO_NODE_H

// Abstract class for audio node/worker






class vsx_audio_node_parameter
{
  // to avoid threading collisions
  int valid;
  vsx_module_param_abs* param;

public:

  void set_param(vsx_module_param_abs* n_param)
  {
    param = n_param;
    valid = 1;
  }



  vsx_audio_node_parameter() :
    valid(0),
    param(0x0)
  {}
};


// [ module [ node*  ] ]   -> node* ->  [ module [ node ] ]

// The intention for this is to act as a parallell
// and simplified node structure for the audio thread to traverse
// it only supports one input and is called in a direct
// producer/consumer manner.

class vsx_audio_node
{
public:

  virtual float generate_sample_left() = 0;
  virtual float generate_sample_right() = 0;

};


class vsx_audio_node_sin_wave_generator : public vsx_audio_node
{
public:
  // pointer to the module housing the parameters
  // this will be used by the implementation to grab
  // parameters from the normal vsxu chain
  void* module;

};


#endif // VSX_AUDIO_NODE_H
