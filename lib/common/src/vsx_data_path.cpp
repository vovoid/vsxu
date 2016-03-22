#include "vsx_data_path.h"
#include "vsx_version.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <sys/stat.h>
#endif

#include <stdio.h>


vsx_data_path vsx_data_path::instance;

vsx_data_path::vsx_data_path()
{
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

      int r;
      r = symlink ( (PLATFORM_SHARED_FILES+"example-macros").c_str(), (data_path+"macros/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-states").c_str(), (data_path+"states/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-prods").c_str(), (data_path+"prods/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-visuals").c_str(), (data_path+"visuals/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-resources").c_str(), (data_path+"resources/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-faders").c_str(), (data_path+"visuals_faders/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"debug-states").c_str(), (data_path+"states/debug").c_str() );
      (void)r;
    }
  #else // platform family = unix
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
}

