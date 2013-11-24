#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #ifdef VSX_ENG_DLL
    #define VSX_ENGINE_GRAPHICS_DLLIMPORT __declspec (dllexport)
  #else
    #define VSX_ENGINE_GRAPHICS_DLLIMPORT __declspec (dllimport)
  #endif
#else
  #define VSX_ENGINE_GRAPHICS_DLLIMPORT
#endif

