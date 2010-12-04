// This is a bit messy, but saves loads of work when maintaining modules.
// Factory methods are created automatically by cmake, and when loading
// it's fast enough


//vsx module static factory info class

class vsxm_sf_info
{
public:
  vsx_string name;
  void* cm; //create module func
  void* dm; //delete module func
  void* nm; //number of modules
  vsxm_sf_info(vsx_string n_name, void *n_cm, void* n_dm, void* n_nm)
  {
    name = n_name;
    cm = n_cm;
    dm = n_dm;
    nm = n_nm;
  }
};

//vsx module static factory manager class

class vsxm_sf
{
  private:
  vsx_avector<vsxm_sf_info*> modules;
    
  
  public:
    // will fill out internal std::maps with pointers to factory functions
    void vsx_module_static_factory();

    void* dlopen(const char *filename);

    // fake dlsym, returns function pointers to static functions
    void* dlsym(void *handle, const char *symbol);

    // returns a list of factory names to query
    void get_factory_names(std::list<vsx_string>* mfiles);

    vsxm_sf();

};

extern vsxm_sf static_holder;

// WAH HAX
#ifdef VSXU_ENGINE_STATIC
  #define dlopen(a,b) static_holder.dlopen(a)
  #define dlsym(a,b) static_holder.dlsym(a,b)
#endif
