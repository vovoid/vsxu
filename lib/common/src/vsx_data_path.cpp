#include "vsx_data_path.h"
#include "vsx_version.h"

#include <vsx_platform.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <unistd.h>
#include <sys/stat.h>
#endif

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS && COMPILER == COMPILER_VISUAL_STUDIO
#include <io.h>
#endif


#include <stdio.h>
#include <vsx_argvector.h>

vsx_data_path vsx_data_path::instance;

void vsx_data_path::init()
{
  if (vsx_argvector::get_instance()->has_param_with_value("data_path"))
  {
    vsx_string<> new_data_path = vsx_argvector::get_instance()->get_param_value("data_path");
    if (new_data_path[new_data_path.size()-1] != DIRECTORY_SEPARATOR[0])
      new_data_path.push_back(DIRECTORY_SEPARATOR[0]);
    data_path = new_data_path;
    return;
  }

  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    char* home_dir = getenv ("HOME");
    data_path = vsx_string<>(home_dir)+"/.local/share/vsxu/";

    if (access(data_path.c_str(),0) != 0)
    {
      mkdir( (data_path).c_str(), 0700 );
      int r = symlink ( (data_path).c_str(), (vsx_string<>(home_dir)+"/vsxu").c_str() );
      (void)r;
    }

    data_path = data_path + VSXU_VER + "/";

    if (access(data_path.c_str(),0) != 0)
      mkdir( (data_path).c_str(),0700);

    data_path = data_path+"data/";

    if (access(data_path.c_str(),0) != 0)
    {
      mkdir( (data_path).c_str(),0700);
      mkdir( (data_path+"animations").c_str(),0700);
      mkdir( (data_path+"macros").c_str(),0700);
      mkdir( (data_path+"prods").c_str(),0700);
      mkdir( (data_path+"screenshots").c_str(),0700);
      mkdir( (data_path+"sequences").c_str(),0700);
      mkdir( (data_path+"shaders").c_str(),0700);
      mkdir( (data_path+"states").c_str(),0700);
      mkdir( (data_path+"visuals").c_str(),0700);
      mkdir( (data_path+"visuals_faders").c_str(),0700);
      mkdir( (data_path+"resources").c_str(),0700);
      // add symlinks to examples

      system((vsx_string<>("cp -Rf ") + PLATFORM_SHARED_FILES + "vsxu.conf " + data_path).c_str());
      system((vsx_string<>("cp -Rf ") + PLATFORM_SHARED_FILES + "example-macros " + data_path + "macros/examples/").c_str());
      system((vsx_string<>("cp -Rf ") + PLATFORM_SHARED_FILES + "example-states " + data_path + "states/examples/").c_str());
      system((vsx_string<>("cp -Rf ") + PLATFORM_SHARED_FILES + "example-prods " + data_path + "prods/examples/").c_str());
      system((vsx_string<>("cp -Rf ") + PLATFORM_SHARED_FILES + "example-visuals " + data_path + "visuals/examples/").c_str());
      system((vsx_string<>("cp -Rf ") + PLATFORM_SHARED_FILES + "example-resources " + data_path + "resources/examples/").c_str());
      system((vsx_string<>("cp -Rf ") + PLATFORM_SHARED_FILES + "example-faders " + data_path + "visuals_faders/examples/").c_str());
      system((vsx_string<>("cp -Rf ") + PLATFORM_SHARED_FILES + "debug-states " + data_path + "debug/").c_str());
    }
  #else // platform family = unix
    #if COMPILER == COMPILER_VISUAL_STUDIO
      char* home_dir = std::getenv("USERPROFILE");
      data_path = vsx_string<>(home_dir) + "\\vsxu\\";

      if (_access(data_path.c_str(), 0) == -1)
        CreateDirectory((data_path).c_str(), nullptr);

      data_path = data_path + VSXU_VER + "\\";

      if (_access(data_path.c_str(), 0) == -1)
        CreateDirectory((data_path).c_str(), nullptr);

      data_path = data_path + "data\\";
      if (_access(data_path.c_str(), 0) == -1)
      {
        printf("trying to create data_path: %s\n", data_path.c_str());
        CreateDirectory((data_path).c_str(), nullptr);
        CreateDirectory((data_path + "animations").c_str(), nullptr);
        CreateDirectory((data_path + "macros").c_str(), nullptr);
        CreateDirectory((data_path + "prods").c_str(), nullptr);
        CreateDirectory((data_path + "screenshots").c_str(), nullptr);
        CreateDirectory((data_path + "sequences").c_str(), nullptr);
        CreateDirectory((data_path + "shaders").c_str(), nullptr);
        CreateDirectory((data_path + "states").c_str(), nullptr);
        CreateDirectory((data_path + "visuals").c_str(), nullptr);
        CreateDirectory((data_path + "visuals_faders").c_str(), nullptr);
        CreateDirectory((data_path + "resources").c_str(), nullptr);

        #ifdef VSXU_DEBUG
          printf("xcopy command: %s\n", vsx_string<>("xcopy /E data " + data_path).c_str());
          printf("xcopy command: %s\n", vsx_string<>("xcopy /E \"" + PLATFORM_SHARED_FILES + "\\example-macros\\\" " + data_path + "").c_str());
          printf("%s", (vsx_string<>("xcopy /I /E \"") + PLATFORM_SHARED_FILES + "example-macros\" \"" + data_path + "macros\\examples\"").c_str());
        #endif

        system((vsx_string<>("xcopy \"") + PLATFORM_SHARED_FILES + "vsxu.conf\" " + data_path).c_str());
        system((vsx_string<>("xcopy /I /E \"") + PLATFORM_SHARED_FILES + "example-macros\" \"" + data_path + "macros\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"") + PLATFORM_SHARED_FILES + "example-states\" \"" + data_path + "states\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"") + PLATFORM_SHARED_FILES + "example-prods\" \"" + data_path + "prods\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"") + PLATFORM_SHARED_FILES + "example-visuals\" \"" + data_path + "visuals\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"") + PLATFORM_SHARED_FILES + "example-resources\" \"" + data_path + "resources\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"") + PLATFORM_SHARED_FILES + "example-faders\" \"" + data_path + "visuals_faders\\examples\"").c_str());
      }
    #else
      char* home_dir = getenv ("USERPROFILE");
      data_path = vsx_string<>(home_dir)+"\\vsxu\\";

      if (access(data_path.c_str(),0) != 0)
        mkdir((data_path).c_str());

      data_path = data_path+vsxu_ver+"\\";

      if (access(data_path.c_str(),0) != 0)
        mkdir( (data_path).c_str());

      data_path = data_path+"data\\";
      if (access(data_path.c_str(),0) != 0)
      {
        printf("trying to create data_path: %s\n",data_path.c_str());
        mkdir( (data_path).c_str());
        mkdir( (data_path+"animations").c_str());
        mkdir( (data_path+"macros").c_str());
        mkdir( (data_path+"prods").c_str());
        mkdir( (data_path+"screenshots").c_str());
        mkdir( (data_path+"sequences").c_str());
        mkdir( (data_path+"shaders").c_str());
        mkdir( (data_path+"states").c_str());
        mkdir( (data_path+"visuals").c_str());
        mkdir( (data_path+"visuals_faders").c_str());
        mkdir( (data_path+"resources").c_str());

        #ifdef VSXU_DEBUG
          printf("xcopy command: %s\n",vsx_string<>("xcopy /E data "+data_path).c_str());
          printf("xcopy command: %s\n",vsx_string<>("xcopy /E \""+PLATFORM_SHARED_FILES+"\\example-macros\\\" "+data_path+"").c_str());
          printf("%s",(vsx_string<>("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-macros\" \""+data_path+"macros\\examples\"").c_str());
        #endif

        system((vsx_string<>("xcopy \"")+PLATFORM_SHARED_FILES+"vsxu.conf\" "+data_path).c_str());
        system((vsx_string<>("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-macros\" \""+data_path+"macros\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-states\" \""+data_path+"states\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-prods\" \""+data_path+"prods\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-visuals\" \""+data_path+"visuals\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-resources\" \""+data_path+"resources\\examples\"").c_str());
        system((vsx_string<>("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-faders\" \""+data_path+"visuals_faders\\examples\"").c_str());
      }
    #endif
  #endif
}

