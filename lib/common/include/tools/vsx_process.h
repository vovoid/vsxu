#pragma once

#include <vsx_platform.h>
#include <string/vsx_string_helper.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include <windows.h>
#endif
class vsx_process
{
public:

  static void launch_detached_process(vsx_string<> command, vsx_string<> working_directory)
  {
    #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
      command += " &";
      system(command.c_str());
      sleep(1);
    #endif

    #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
      PROCESS_INFORMATION pinfo;
      vsx_string<wchar_t> command_16 = vsx_string_helper::string_convert<>(command);
      vsx_string<wchar_t> working_directory_16 = vsx_string_helper::string_convert<>(working_directory);

      BOOL success = false;
      DWORD dwCreationFlags = CREATE_NO_WINDOW;

      //dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;

      STARTUPINFOW startupInfo =
      {
        sizeof( STARTUPINFO ), 0, 0, 0,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
      };
      //LPSTR

      success = CreateProcess(
        0,
        (LPSTR)command.c_str(),
        0,
        0,
        FALSE,
        dwCreationFlags,
        nullptr,
        working_directory.c_str(),
        (LPSTARTUPINFO)&startupInfo,
        &pinfo
      );

      if (success) {
          CloseHandle(pinfo.hThread);
          CloseHandle(pinfo.hProcess);
      }
    #endif
  }

};
