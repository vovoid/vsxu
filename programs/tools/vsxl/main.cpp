//LIBRARIES-------------------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

#include <string/vsx_string.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
#include "vsx_module_list_abs.h"
#include "vsx_module_list_factory.h"
#include "vsx_platform.h"
#include "vsx_version.h"

#if PLATFORM == PLATFORM_LINUX
  #include <unistd.h>
  #include <signal.h>
  #include <fcntl.h>
  #include <sys/stat.h>
#elif PLATFORM == PLATFORM_WINDOWS
  std::cout<<"This Platform is Currently Unsupported.\n";
#elif PLATFORM == PLATFORM_MAC
  std::cout<<"This Platform is Currently Unsupported.\n";
#elif PLATFORM == PLATFORM_SOMETHINGELSE
  std::cout<<"Error: Unknown Platform\n";
#else
  std::cout<<"Error: Could not work out Platform of host machine.\n";
#endif

//FORWARD-DECLARATIONS--------------------------------------------

void initialise();
void cleanup();

void exitHandler(int signal);

//CLASSES---------------------------------------------------------

class xmlFile
{
public:
#if PLATFORM == PLATFORM_LINUX
  int fd;
#elif PLATFORM == PLATFORM_WINDOWS
  HANDLE fd;
#elif PLATFORM == PLATFORM_MAC
  std::cout<<"This Platform is Currently Unsupported.\n";
#elif PLATFORM == PLATFORM_SOMETHINGELSE
  std::cout<<"Error: Unknown Platform\n";
#else
  std::cout<<"Error: Could not work out Platform of host machine.\n";
#endif

  char* xFGetCWD()
  {
    #if PLATFORM == PLATFORM_LINUX
      return getcwd(NULL, 255);
    #elif PLATFORM == PLATFORM_WINDOWS
      std::cout<<"This Platform is Currently Unsupported.\n";
    #elif PLATFORM == PLATFORM_MAC
      std::cout<<"This Platform is Currently Unsupported.\n";
    #elif PLATFORM == PLATFORM_SOMETHINGELSE
      std::cout<<"Error: Unknown Platform\n";
    #else
      std::cout<<"Error: Could not work out Platform of host machine.\n";
    #endif
  }

  void xFOpen(const char* path)
  {
    #if PLATFORM == PLATFORM_LINUX
      fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    #elif PLATFORM == PLATFORM_WINDOWS
      std::cout<<"This Platform is Currently Unsupported.\n";
    #elif PLATFORM == PLATFORM_MAC
      std::cout<<"This Platform is Currently Unsupported.\n";
    #elif PLATFORM == PLATFORM_SOMETHINGELSE
      std::cout<<"Error: Unknown Platform\n";
    #else
      std::cout<<"Error: Could not work out Platform of host machine.\n";
    #endif
  }

  int xFRead(const char** buffer, int buflen = 1024)
  {
    #if PLATFORM == PLATFORM_LINUX
      return read(fd, buffer, buflen);
    #elif PLATFORM == PLATFORM_WINDOWS
      std::cout<<"This Platform is Currently Unsupported.\n";
      return -1;
    #elif PLATFORM == PLATFORM_MAC
      std::cout<<"This Platform is Currently Unsupported.\n";
      return -1;
    #elif PLATFORM == PLATFORM_SOMETHINGELSE
      std::cout<<"Error: Unknown Platform\n";
      return -1;
    #else
      std::cout<<"Error: Could not work out Platform of host machine.\n";
      return -1;
    #endif
  }

  int xFWrite(const char* buffer, int buflen = -1)
  {
    if(buflen == -1) buflen = strlen(buffer);
    #if PLATFORM == PLATFORM_LINUX
      return write(fd, buffer, buflen);
    #elif PLATFORM == PLATFORM_WINDOWS
      std::cout<<"This Platform is Currently Unsupported.\n";
      return -1;
    #elif PLATFORM == PLATFORM_MAC
      std::cout<<"This Platform is Currently Unsupported.\n";
      return -1;
    #elif PLATFORM == PLATFORM_SOMETHINGELSE
      std::cout<<"Error: Unknown Platform\n";
      return -1;
    #else
      std::cout<<"Error: Could not work out Platform of host machine.\n";
      return -1;
    #endif
  }

  void xFClose()
  {
    #if PLATFORM == PLATFORM_LINUX
      close(fd);
    #elif PLATFORM == PLATFORM_WINDOWS
      std::cout<<"This Platform is Currently Unsupported.\n";
    #elif PLATFORM == PLATFORM_MAC
      std::cout<<"This Platform is Currently Unsupported.\n";
    #elif PLATFORM == PLATFORM_SOMETHINGELSE
      std::cout<<"Error: Unknown Platform\n";
    #else
      std::cout<<"Error: Could not work out Platform of host machine.\n";
    #endif
  }
};

//DATA-STRUCTURES-------------------------------------------------

int running=1;

struct MAINDATA
{
  vsx_module_list_abs* modules;
  std::vector<vsx_module_info*>* info;
  xmlFile* file;
};
MAINDATA g;

//CORE-FUNCTIONS--------------------------------------------------

std::string xmlFormat(std::string str)
{
  for(size_t i=0;i<str.length();++i)
  {
    if(str[i] == (char)34) // "
    {
      str.replace(i, 1,"&quot;");
      i=i+6;
    }
    if(str[i] == (char)38) // &
    {
      str.replace(i, 1,"&amp;");
      i=i+5;
    }
    if(str[i] == (char)39) // '
    {
      str.replace(i, 1,"&apos;");
      i=i+6;
    }
    if(str[i] == (char)60) // <
    {
      str.replace(i, 1,"&lt;");
      i=i+4;
    }
    if(str[i] == (char)62) // >
    {
      str.replace(i, 1,"&gt;");
      i=i+4;
    }
  }
  return str;
}

//SIGNAL-HANDLERS-------------------------------------------------

#if PLATFORM == PLATFORM_LINUX
  void exitHandler(int signal)
  {
    VSX_UNUSED(signal);
    running=0;
  }
#elif PLATFORM == PLATFORM_WINDOWS
  std::cout<<"This Platform is Currently Unsupported.\n";
#elif PLATFORM == PLATFORM_MAC
  std::cout<<"This Platform is Currently Unsupported.\n";
#elif PLATFORM == PLATFORM_SOMETHINGELSE
  std::cout<<"Error: Unknown Platform\n";
#else
  std::cout<<"Error: Could not work out Platform of host machine.\n";
#endif

//MAIN-FUNCTIONS--------------------------------------------------

void setExitHandler()
{
#if PLATFORM == PLATFORM_LINUX
  signal(SIGHUP, exitHandler);
  signal(SIGINT, exitHandler);
  signal(SIGTERM, exitHandler);
#elif PLATFORM == PLATFORM_WINDOWS
  std::cout<<"This Platform is Currently Unsupported.\n";
#elif PLATFORM == PLATFORM_MAC
  std::cout<<"This Platform is Currently Unsupported.\n";
#elif PLATFORM == PLATFORM_SOMETHINGELSE
  std::cout<<"Error: Unknown Platform\n";
#else
  std::cout<<"Error: Could not work out Platform of host machine.\n";
#endif
}

void initialise(int argc,char* argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);
  std::cout<<"__Vovoid VSXu-Librarian "<< VSXU_VER;
#if PLATFORM == PLATFORM_LINUX
  std::cout<<" [GNU/Linux "<<PLATFORM_BITS<<"-bit]__\n";
#elif PLATFORM == PLATFORM_WINDOWS
  std::cout<<" [Windows "<<PLATFORM_BITS<<"-bit]__\n";
  std::cout<<"This Platform is Currently Unsupported.\n";
#elif PLATFORM == PLATFORM_MAC
  std::cout<<" [Macintosh "<<PLATFORM_BITS<<"-bit]__\n";
  std::cout<<"This Platform is Currently Unsupported.\n";
#elif PLATFORM == PLATFORM_SOMETHINGELSE
  std::cout<<"__\n";
  std::cout<<"Error: Unknown Platform\n";
#else
  std::cout<<"__\n";
  std::cout<<"Error: Could not work out Platform of host machine.\n";
#endif
  

  /*
  for(int i=0;i<argc;i++)
  {
    //std::cout<<"argv["<<i<<"]="<<argv[i]<<"\n";
    //if(argv[i])
  }
  */

  vsx_module_list_abs* mymodules = vsx_module_list_factory_create();
  std::vector<vsx_module_info*>* myinfo = mymodules->get_module_list();
  std::cout<<"Modules Detected: "<<myinfo->size()<<std::endl;

  std::stringstream ss;

  for(int i = 0; i < (int)myinfo->size(); ++i)
  {
    ss << "  <module_" << i + 1 << ">\n"
       << "    <identifier>"      << (*myinfo)[i]->identifier.c_str()                << "</identifier>\n"
       << "    <component_class>" << (*myinfo)[i]->component_class.c_str()           << "</component_class>\n"
       << "    <description>"     << xmlFormat((*myinfo)[i]->description.c_str())    << "</description>\n"
       << "    <in_param_spec>"   << xmlFormat((*myinfo)[i]->in_param_spec.c_str())  << "</in_param_spec>\n"
       << "    <out_param_spec>"  << xmlFormat((*myinfo)[i]->out_param_spec.c_str()) << "</out_param_spec>\n"
       << "    <tunnel>"          << (((*myinfo)[i]->tunnel) ? "true" : "false")     << "</tunnel>\n"
       << "    <output>"          << (*myinfo)[i]->output                            << "</output>\n"
       << "  </module_" << i + 1 << ">\n\n";
  }

  std::stringstream xmlss;
  xmlss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" << "<xml>" << ss.str() << "</xml>";

  std::stringstream ss2;
  g.file = new xmlFile;
  ss2<<g.file->xFGetCWD()<<"/modules.xml";
  //Do error checking here...
  g.file->xFOpen(ss2.str().c_str());
  g.file->xFWrite(xmlss.str().c_str(), xmlss.str().length());
  
  std::cout<<"Outputting xml file to: "<<ss2.str().c_str()<<std::endl;
}

void cleanup()
{
  std::cout<<"Done!\n";
  g.file->xFClose();
  delete g.info;
  delete g.file;
}

//MAIN-PROGRAM----------------------------------------------------

int main(int argc,char* argv[])
{
  setExitHandler();
  initialise(argc,argv);
  cleanup();
  return 0;
}
