/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/********************
*                   *
*   NeHeGL Header   *
*                   *
**********************************************************************************
*                                                                                *
*	You Need To Provide The Following Functions:                                 *
*                                                                                *
*	BOOL Initialize (GL_Window* window, Keys* keys);                             *
*		Performs All Your Initialization                                         *
*		Returns TRUE If Initialization Was Successful, FALSE If Not              *
*		'window' Is A Parameter Used In Calls To NeHeGL                          *
*		'keys' Is A Structure Containing The Up/Down Status Of keys              *
*                                                                                *
*	void Deinitialize (void);                                                    *
*		Performs All Your DeInitialization                                       *
*                                                                                *
*	void Update (DWORD milliseconds);                                            *
*		Perform Motion Updates                                                   *
*		'milliseconds' Is The Number Of Milliseconds Passed Since The Last Call  *
*		With Whatever Accuracy GetTickCount() Provides                           *
*                                                                                *
*	void Draw (void);                                                            *
*		Perform All Your Scene Drawing                                           *
*                                                                                *
*********************************************************************************/

#ifndef GL_FRAMEWORK__INCLUDED
#define GL_FRAMEWORK__INCLUDED

#define WINVER 0x0500

#include <windows.h>								// Header File For Windows


struct DisplayMode
{
    DisplayMode() : width(-1), height(-1), refreshRate(-1), bpp(-1) {};
    DisplayMode(int w, int h, int r, int BPP) : width(w), height(h), refreshRate(r), bpp(BPP) {};

    int pos_x; // left
    int pos_y; //top
    int pos_w; //right
    int pos_h; //bottom
    int width;
    int height;
    int refreshRate;
    int bpp;
};


// Class: Display
//
// A physical display on a computer.
class Display
{
public:
    Display();
    Display(int index, int x, int y, bool primary, vsx_string devName, vsx_string devString, const std::vector<DisplayMode>& displayModes);

    //bool Defaulted() const { return m_Index == -1; }

    // Function: GetName
    //
    // Retrieves the name of the display.
    vsx_string GetName() const;

    // Function: GetDisplayModes
    //
    // Retrieves a vector of available <DisplayModes> for the display.
    const std::vector<DisplayMode>& GetDisplayModes() const;

    // Function: SelectDisplayMode
    //
    // Selects the display mode to set this monitor to when a window is
    void SelectDisplayMode(DisplayMode mode);

    int m_Index;
    int m_XOffset, m_YOffset;
    bool m_PrimaryDisplay;
    vsx_string m_DeviceName, m_DeviceString;

    std::vector<DisplayMode> m_DisplayModes;
    DisplayMode m_SelectedDisplayMode;
};

DisplayMode get_display_properties(vsx_string device_name);

// Function: EnumerateDisplays
//
// Enumerates all displays available.
//
// Returns:
// The vector of <Displays> filled with the list of displays.
std::vector<Display>& EnumerateDisplays();

#endif												// GL_FRAMEWORK__INCLUDED



