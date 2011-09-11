#include "vsx_manager.h"
#include "vsx_statelist.h"

class vsx_manager : public vsx_manager_abs
{
private:
void * int_state_manager;
public:
  // init manager with base path to where the effects (.vsx files) can be found
  // i.e. if base_path is /usr/share/vsxu/   then the engine will look in
  // /usr/share/vsxu/_visuals
  void init(const char* base_path, const char* sound_type);

  // before you render first time, you need to start
  void start();
  void render();
  // if you are going to destroy the GL Context - as is the case
  // when destroying a window - when undocking a window or going fullscreen perhaps
  // you need top stop() the engine so all visuals can unload their OpenGL handles and
  // get ready to be started again.
  void stop();

  // flipping through different visuals
  void toggle_randomizer();
  void set_randomizer(bool status);
    
  bool get_randomizer_status();
  void pick_random_visual();
  void next_visual();
  void prev_visual();
  std::string visual_loading();

  // provide metadata for information about current running effect
  std::string get_meta_visual_filename();
  std::string get_meta_visual_name();
  std::string get_meta_visual_creator();
  std::string get_meta_visual_company();

  // amplification/fx level (more = flashier, less = less busy)
  float get_fx_level();
  void inc_fx_level();
  void dec_fx_level();

  // time speed (more = faster movements, less = slow motion)
  float get_speed();
  void inc_speed();
  void dec_speed();

  void set_sound_freq(float* data);
  void set_sound_wave(float* data);


  // arbitrary engine information (statistics etc)
  // returns information about currently playing effect
  int get_engine_num_modules();

  vsx_manager();
  ~vsx_manager();
};

/****************************************************************
I M P L E M E N T A T I O N   +   F A C T O R Y
*****************************************************************/


vsx_manager_abs* manager_factory()
{
  vsx_manager_abs* mym = (vsx_manager_abs*)new vsx_manager();
  return mym;
}

vsx_manager_abs* manager_destroy(vsx_manager_abs* manager)
{
  delete (vsx_manager*)manager;
}

vsx_manager::vsx_manager()
{
  int_state_manager = (void*)new vsx_statelist();
}

vsx_manager::~vsx_manager()
{
  delete (vsx_statelist*)int_state_manager;
}

void vsx_manager::init(const char* base_path, const char* sound_type)
{
  ((vsx_statelist*)int_state_manager)->init(vsx_string(base_path),vsx_string(sound_type));
}

void vsx_manager::start()
{
((vsx_statelist*)int_state_manager)->start();
}

void vsx_manager::render()
{
  ((vsx_statelist*)int_state_manager)->render();
}

void vsx_manager::stop()
{
((vsx_statelist*)int_state_manager)->stop();
}

/****************************************************************
V I S U A L S    C H O O S E R
*****************************************************************/

void vsx_manager::toggle_randomizer()
{
  ((vsx_statelist*)int_state_manager)->toggle_randomizer();
}

void vsx_manager::set_randomizer(bool status)
{
  
}

bool vsx_manager::get_randomizer_status()
{
  return ((vsx_statelist*)int_state_manager)->get_randomizer_status();
}

void vsx_manager::pick_random_visual()
{
  ((vsx_statelist*)int_state_manager)->random_state();
}

void vsx_manager::next_visual()
{
  ((vsx_statelist*)int_state_manager)->next_state();
}

void vsx_manager::prev_visual()
{
  ((vsx_statelist*)int_state_manager)->prev_state();
}

std::string vsx_manager::visual_loading()
{
  return std::string( ((vsx_statelist*)int_state_manager)->state_loading().c_str() );
}

/****************************************************************
M E T A    I N F O R M A T I O N
*****************************************************************/

std::string vsx_manager::get_meta_visual_filename()
{
  return std::string( ((vsx_statelist*)int_state_manager)->get_meta_visual_filename().c_str() );
}

std::string vsx_manager::get_meta_visual_name()
{
//vxe->meta_fields[0]
  return std::string( ((vsx_statelist*)int_state_manager)->get_meta_visual_name().c_str() );
}

std::string vsx_manager::get_meta_visual_creator()
{
//vxe->meta_fields[1]
  return std::string( ((vsx_statelist*)int_state_manager)->get_meta_visual_creator().c_str() );
}

std::string vsx_manager::get_meta_visual_company()
{
//vxe->meta_fields[2]
  return std::string( ((vsx_statelist*)int_state_manager)->get_meta_visual_company().c_str() );
}

/****************************************************************
F X   L E V E L
*****************************************************************/

float vsx_manager::get_fx_level()
{
  return ((vsx_statelist*)int_state_manager)->get_fx_level();
}
void vsx_manager::inc_fx_level()
{
  ((vsx_statelist*)int_state_manager)->inc_amp();
}
void vsx_manager::dec_fx_level()
{
  ((vsx_statelist*)int_state_manager)->dec_amp();
}

/****************************************************************
S P E E D
*****************************************************************/

float vsx_manager::get_speed()
{
  return ((vsx_statelist*)int_state_manager)->get_speed();
}

void vsx_manager::inc_speed()
{
  ((vsx_statelist*)int_state_manager)->inc_speed();
}

void vsx_manager::dec_speed()
{
  ((vsx_statelist*)int_state_manager)->dec_speed();
}

void vsx_manager::set_sound_freq(float* data)
{
   ((vsx_statelist*)int_state_manager)->set_sound_freq(data);
}

/****************************************************************
S O U N D
*****************************************************************/

void vsx_manager::set_sound_wave(float* data)
{
  ((vsx_statelist*)int_state_manager)->set_sound_wave(data);
}


int vsx_manager::get_engine_num_modules()
{
  vsx_engine* vxe = ((vsx_statelist*)int_state_manager)->get_vxe();
  if (vxe)
  return vxe->get_num_modules();
  return 0;
}


