// This is a bit messy, but saves loads of work when maintaining modules.
// Factory methods are created automatically by cmake, and when loading
// it's fast enough


//vsx module static factory info class

class vsxm_sf
{
  private:
    std::list<vsx_string> module_names;
  
  public:
    // will fill out internal std::maps with pointers to factory functions
    void vsx_module_static_factory();

    void* dlopen(const char *filename);

    // fake dlsym, returns function pointers to static functions
    void* dlsym(void *handle, const char *symbol);

    // returns a list of factory names to query
    std::list<vsx_string> get_factory_names();

}