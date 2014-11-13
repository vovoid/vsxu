#define _WIN32_WINNT 0x0800
#include <windows.h>

#if defined(SM_CMONITORS) && (WINVER < 0x0500)

#undef MONITOR_DEFAULTTONULL
#undef MONITOR_DEFAULTTOPRIMARY
#undef MONITOR_DEFAULTTONEAREST
#undef MONITORINFOF_PRIMARY

#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002
#define MONITORINFOF_PRIMARY        0x00000001

#endif /* (SM_CMONITORS) && (WINVER < 0x0500) */
 
#define COMPILE_MULTIMON_STUBS
#include "multimon.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <map>
#include <container/vsx_avector.h>
#include "vsx_string.h"
using namespace std;
#include "nehegl.h"
//#define DM_POSITION             0x00000020L
#define DISPLAY_DEVICE_ACTIVE              0x00000001
#define DISPLAY_DEVICE_ATTACHED            0x00000002


Display::Display()
: m_Index(-1),
  m_XOffset(-1),
  m_YOffset(-1),
  m_PrimaryDisplay(false),
  m_DeviceName(vsx_string<>("UNKNOWN")),
  m_DeviceString(vsx_string<>("UNKNOWN"))
{
}

Display::Display(int index, int x, int y, bool primary, vsx_string<>devName, vsx_string<>devString,
                 const vector<DisplayMode>& displayModes)
: m_Index(index),
  m_XOffset(x),
  m_YOffset(y),
  m_PrimaryDisplay(primary),
  m_DeviceName(devName),
  m_DeviceString(devString),
  m_DisplayModes(displayModes)
{
}

vsx_string<>Display::GetName() const
{
    return m_DeviceString;
}

const vector<DisplayMode>& Display::GetDisplayModes() const
{
    return m_DisplayModes;
}

void Display::SelectDisplayMode(DisplayMode mode)
{
    m_SelectedDisplayMode = mode;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MONITORINFOEX monInfo;
    monInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &monInfo);

    vector<Display>& displays = *((vector<Display>*)dwData);

    int ddIndex = 0;
    bool primaryDisplay = false;
    vsx_string<>deviceName = vsx_string<>(monInfo.szDevice), deviceString;
    vector<DisplayMode> displayModes;

    DISPLAY_DEVICE displayDevice;
    displayDevice.cb = sizeof(DISPLAY_DEVICE);

    for (ddIndex = 0; EnumDisplayDevices(0, ddIndex, &displayDevice, 0); ddIndex++)
    {
        if ((vsx_string<>(displayDevice.DeviceName) == deviceName) &&
            !((bool)(DISPLAY_DEVICE_MIRRORING_DRIVER & displayDevice.StateFlags)) &&
            !((bool)(DISPLAY_DEVICE_REMOVABLE & displayDevice.StateFlags)))
        {
            deviceString = vsx_string<>(displayDevice.DeviceString);
            primaryDisplay = (bool)(DISPLAY_DEVICE_PRIMARY_DEVICE & displayDevice.StateFlags);

            DEVMODE devMode;
            devMode.dmSize = sizeof(DEVMODE);
            for (int j = 0; EnumDisplaySettingsEx(displayDevice.DeviceName, j, &devMode, 0); j++)
            {
                if (devMode.dmPelsWidth < 640 || devMode.dmPelsHeight < 480 || devMode.dmBitsPerPel < 16)
                    continue;

                POINTL position;
                position.x = monInfo.rcMonitor.left;
                position.y = monInfo.rcMonitor.top;
                devMode.dmPosition = position;

                devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT |
                    DM_DISPLAYFREQUENCY | DM_POSITION;
                LONG result = ChangeDisplaySettingsEx(displayDevice.DeviceName, &devMode, 0,
                    CDS_TEST, 0);
                if (result != DISP_CHANGE_SUCCESSFUL)
                    continue;

                displayModes.push_back(DisplayMode(devMode.dmPelsWidth, devMode.dmPelsHeight,
                devMode.dmDisplayFrequency, devMode.dmBitsPerPel));
            }

            break;
        }
    }

    displays.push_back(Display(ddIndex, monInfo.rcMonitor.left, monInfo.rcMonitor.top,
        primaryDisplay, deviceName, deviceString, displayModes));

    return TRUE;
}

vector<Display>& EnumerateDisplays()
{
    static vector<Display> s_Displays;

    if (s_Displays.empty())
    {

    
    if (!EnumDisplayMonitors(0, 0, MonitorEnumProc, (LPARAM)&s_Displays))
          printf("Could not enumerate available display devices");
//            THROW_EXCEPTION("Could not enumerate available display devices");
    }

    return s_Displays;
}

DisplayMode get_display_properties(vsx_string<>device_name) 
{
  DisplayMode ldisp;
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	DWORD dev = 0; // device index
	int id = 1; // monitor number, as used by Display Properties > Settings

	while (EnumDisplayDevices(0, dev, &dd, 0))
	{
		if (!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
		{
			// ignore virtual mirror displays
			  // get information about the monitor attached to this display adapter. dualhead cards
			  // and laptop video cards can have multiple monitors attached
    if (device_name == vsx_string<>(dd.DeviceName)) {

			  DISPLAY_DEVICE ddMon;
			  ZeroMemory(&ddMon, sizeof(ddMon));
			  ddMon.cb = sizeof(ddMon);
			  DWORD devMon = 0;

			  // please note that this enumeration may not return the correct monitor if multiple monitors
			  // are attached. this is because not all display drivers return the ACTIVE flag for the monitor
			  // that is actually active
			  while (EnumDisplayDevices(dd.DeviceName, devMon, &ddMon, 0))
			  {
				  if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE)
					  break;

				  devMon++;
			  }
        //printf("internal name: %s\n",dd.DeviceName);
			  if (!*ddMon.DeviceString)
			  {
				  EnumDisplayDevices(dd.DeviceName, 0, &ddMon, 0);
				  //if (!*ddMon.DeviceString)
					  //lstrcpy(ddMon.DeviceString, _T("Default Monitor"));
			  }

			  // get information about the display's position and the current display mode
			  DEVMODE dm;
			  ZeroMemory(&dm, sizeof(dm));
			  dm.dmSize = sizeof(dm);
			  if (EnumDisplaySettingsEx(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm, 0) == FALSE)
				  EnumDisplaySettingsEx(dd.DeviceName, ENUM_REGISTRY_SETTINGS, &dm, 0);

			  // get the monitor handle and workspace
			  HMONITOR hm = 0;
			  MONITORINFO mi;
			  ZeroMemory(&mi, sizeof(mi));
			  mi.cbSize = sizeof(mi);
			  if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
			  {
				  // display is enabled. only enabled displays have a monitor handle
				  POINT pt = { dm.dmPosition.x, dm.dmPosition.y };
				  hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
				  if (hm)
					  GetMonitorInfo(hm, &mi);
			  }

			  // format information about this monitor
			  //TCHAR buf[1000];
  			
			  // 1. MyMonitor on MyVideoCard
			  //wsprintf(buf, _T("%d. %s on %s\r\n"), id, ddMon.DeviceString, dd.DeviceString);
			  //lstrcat(msg, buf);

			  // status flags: primary, disabled, removable
			  //buf[0] = _T('\0');
			  //if (!(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
//				  lstrcat(buf, _T("disabled, "));
			  //else if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
//				  lstrcat(buf, _T("primary, "));
			  //if (dd.StateFlags & DISPLAY_DEVICE_REMOVABLE)
//				  lstrcat(buf, _T("removable, "));

			  //if (*buf)
			  //{
				  //buf[lstrlen(buf) - 2] = _T('\0');
				  //lstrcat(buf, _T("\r\n"));
				  //lstrcat(msg, buf);
			  //}

			  // width x height @ x,y - bpp - refresh rate
			  // note that refresh rate information is not available on Win9x
			  //wsprintf(buf, _T("%d x %d @ %d,%d - %d-bit - %d Hz\r\n"), dm.dmPelsWidth, dm.dmPelsHeight,
				  //dm.dmPosition.x, dm.dmPosition.y, dm.dmBitsPerPel, dm.dmDisplayFrequency);
			  //lstrcat(msg, buf);
			  ldisp.refreshRate = dm.dmDisplayFrequency;
			  ldisp.bpp = dm.dmBitsPerPel;
			  ldisp.width = dm.dmPelsWidth;
			  ldisp.height = dm.dmPelsHeight;

			  if (hm)
			  {
				  // workspace and monitor handle

				  // workspace: x,y - x,y HMONITOR: handle
        ldisp.pos_x = mi.rcMonitor.left;
        ldisp.pos_y = mi.rcMonitor.top;
        ldisp.pos_w = mi.rcMonitor.right - mi.rcMonitor.left;
        ldisp.pos_h = mi.rcMonitor.bottom - mi.rcMonitor.top;
        //printf("x: %d\n",mi.rcWork.left);
        //printf("y: %d\n",ldisp.pos_y);
        //printf("width: %d\n",ldisp.width);
        //printf("height: %d\n",ldisp.height);
				  //wsprintf(buf, _T("workspace: %d,%d - %d,%d HMONITOR: 0x%X\r\n"), 
//            mi.rcWork.left,
					  //mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom, hm);
				  //lstrcat(msg, buf);
			  }
        return ldisp;

			  // device name
			  //wsprintf(buf, _T("Device: %s\r\n\r\n"), *ddMon.DeviceName ? ddMon.DeviceName : dd.DeviceName);
			  //lstrcat(msg, buf);
			id++;
      }

		}

		dev++;
	}
  return ldisp;
}


