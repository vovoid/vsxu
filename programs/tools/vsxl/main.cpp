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

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>


//FORWARD-DECLARATIONS--------------------------------------------

void initialise();
void cleanup();

void exitHandler(int signal);

//CLASSES---------------------------------------------------------

class xmlFile
{
public:
  int fd;

  char* xFGetCWD()
  {
      return getcwd(NULL, 255);
  }

  void xFOpen(const char* path)
  {
      fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  }

  int xFRead(const char** buffer, int buflen = 1024)
  {
      return read(fd, buffer, buflen);
  }

  int xFWrite(const char* buffer, int buflen = -1)
  {
    if(buflen == -1) buflen = strlen(buffer);
      return write(fd, buffer, buflen);
  }

  void xFClose()
  {
      close(fd);
  }
};

//DATA-STRUCTURES-------------------------------------------------

int running=1;

struct MAINDATA
{
  vsx_module_list_abs* modules;
  std::vector<vsx_module_specification*>* info;
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

  void exitHandler(int signal)
  {
    VSX_UNUSED(signal);
    running=0;
  }

//MAIN-FUNCTIONS--------------------------------------------------

void setExitHandler()
{
  signal(SIGHUP, exitHandler);
  signal(SIGINT, exitHandler);
  signal(SIGTERM, exitHandler);
}

void initialise(int argc,char* argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);
  std::cout<<"__Vovoid VSXu-Librarian "<< VSXU_VER;
  std::cout<<" [GNU/Linux "<<PLATFORM_BITS<<"-bit]__\n";

  /*
  for(int i=0;i<argc;i++)
  {
    //std::cout<<"argv["<<i<<"]="<<argv[i]<<"\n";
    //if(argv[i])
  }
  */

  vsx_module_list_abs* mymodules = vsx_module_list_factory_create();
  std::vector<vsx_module_specification*>* myinfo = mymodules->get_module_list();
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
