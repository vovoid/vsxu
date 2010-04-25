/*===============================================================================================
 DSP Plugin Viewer Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 This example ....
===============================================================================================*/

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h> 
#include <stdio.h>
#include <math.h>

#include "resource.h"

#include "../../api/inc/fmod.h"
#include "../../api/inc/fmod.hpp"
#include "../../api/inc/fmod_errors.h"

#define TITLE  "DSP Plugin Viewer Example. Copyright (c), Firelight Technologies Pty, Ltd 2004-2009."
#define GRAPHICWINDOW_WIDTH     256
#define GRAPHICWINDOW_HEIGHT    116
#define IDC_ACTIVEBUTTON        0x8801
#define IDC_REMOVEBUTTON        0x8802
#define IDC_CONFIGBUTTON        0x8803

/*
    Structures
*/
struct Plugin;
struct PluginParam
{
    Plugin     *plugin;
    int         index;
    HWND        sliderhwnd;
    HWND        namehwnd;
    HWND        valuehwnd;
    WNDPROC     oldtrackbarproc;
    int         dragging;
    float       min, max;
};

struct Plugin
{
    Plugin     *next;           // linked list node.
    Plugin     *prev;           // linked list node.
    FMOD::DSP  *dsp;
    HWND        hwnd;
    HWND        activehwnd;
    HWND        removehwnd;
    HWND        confighwnd;     // button
    HWND        configguihwnd;
    WNDPROC     oldpluginproc;
    int         numparams;
    int         windowwidth;
    int         windowheight;
    int         configwidth;
    int         configheight;
    bool        configactive;
    PluginParam param[64];      // hopefully there are no more than 64 parameters in a plugin!
};

#define SLIDERHEIGHT 20

/*
    Globals
*/
HWND            gMainHwnd           = 0;
HWND            gPluginHwnd         = 0;
HINSTANCE       gHinstance          = 0;
FMOD::System   *gSystem             = 0;
FMOD::Sound    *gSound              = 0;
FMOD::Channel  *gChannel            = 0;
FMOD::Sound    *gSoundRecord        = 0;
FMOD::Channel  *gChannelRecord      = 0;
int             gPluginWindowHeight = 5;
int             gPluginWindowWidth  = 0;
WNDPROC         gOldProgressProc    = 0;
Plugin          gPluginHead;
BITMAPINFO      gGraphicWindowBitmap;
RGBQUAD         gGraphicWindowBitmapData[GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT];


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        char    errstring[1024];

        sprintf(errstring, "FMOD error! (%d) %s", result, FMOD_ErrorString(result));

        MessageBox( NULL, errstring, "FMOD error!", MB_ICONHAND|MB_OK|MB_SYSTEMMODAL);
    }
}

LRESULT CALLBACK FMOD_PluginInstanceProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FMOD_TrackbarProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FMOD_ProgressWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FMOD_PluginWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void PlotSpectrum(HDC hdc, int winx, int winy, bool uselog)
{
    FMOD_RESULT result;
    int count, count2, numchannels;
    float spectrum[8][512];
    float max;
    int x, y;
    HRESULT hr;

    #define calcoffset(_x, _y) (GRAPHICWINDOW_WIDTH * (_y)) + (_x)

    result = gSystem->getSoftwareFormat(0, 0, &numchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return;
    }
    
    /*
        Draw a black square with grey lines through it.
    */
    memset(gGraphicWindowBitmapData, 0, GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT * sizeof(RGBQUAD));

    for (x=0; x < GRAPHICWINDOW_WIDTH; x++)
    {
        RGBQUAD *pixel;
        
        for (y = 0; y < 30; y++)
        {
            pixel = &gGraphicWindowBitmapData[calcoffset(x, GRAPHICWINDOW_HEIGHT * y / 30)];
            pixel->rgbRed   = 0x40;
            pixel->rgbGreen = 0x40;
            pixel->rgbBlue  = 0x40;
        }
    }
    for (x=0; x < GRAPHICWINDOW_WIDTH; x+=64)
    {
        RGBQUAD *pixel;
        
        for (y = 0; y < GRAPHICWINDOW_HEIGHT; y++)
        {
            pixel = &gGraphicWindowBitmapData[calcoffset(x, y)];
            pixel->rgbRed   = 0x40;
            pixel->rgbGreen = 0x40;
            pixel->rgbBlue  = 0x40;
        }
    }

    max = 0;
    count = 0;

    for (count = 0; count < numchannels; count++)
    {
        result = gSystem->getSpectrum(spectrum[count], 512, count, FMOD_DSP_FFT_WINDOW_TRIANGLE);    /* returns an array of 512 floats */
        if (result != FMOD_OK)
        {
            return;
        }

        {    
            for (count2=0; count2 < 512; count2++)
            {
                if (spectrum[count][count2] > max)
                {
                    max = spectrum[count][count2];
                }
            }        
        }

        if (max > 0.0001f)
        {
            /*
                Spectrum graphic is 256 entries wide, and the spectrum is 512 entries.
                The upper band of frequencies at 44khz is pretty boring (ie 11-22khz), so we are only
                going to display the first 256 frequencies, or (0-11khz)
            */
            for (x = 0; x < 512; x++)
            {
                RGBQUAD *pixel;
                float val, db;

                val = spectrum[count][x];

                if (uselog)
                {
                    /*
                        1.0   = 0db
                        0.5   = -6db
                        0.25  = -12db
                        0.125 = -24db

                    */
                    db = 10.0f * (float)log10(val) * 2.0f;

                    val = db;
                    if (val < -150)
                    {
                        val = -150;
                    }

                    val /= -150.0f;
                    val = 1.0f - val;

                    y = (int)(val * (float)GRAPHICWINDOW_HEIGHT);
                }
                else
                {
                    y = (int)(val / max * (float)GRAPHICWINDOW_HEIGHT);
                }
        
                if (y >= GRAPHICWINDOW_HEIGHT)
                {
                    y = GRAPHICWINDOW_HEIGHT - 1;
                }

                for (count2 = 0; count2 < y; count2++)
                {
                    int r,g,b;

                    pixel = &gGraphicWindowBitmapData[calcoffset(x * GRAPHICWINDOW_WIDTH / 512, count2)];

                    r = (count2 << 1);
                    g = 0xFF - (count2 << 1);
                    b = 0x1F;

                    pixel->rgbRed   = r > 255 ? 255 : r;
                    pixel->rgbGreen = g > 255 ? 255 : g;
                    pixel->rgbBlue  = b > 255 ? 255 : b;

                }
            }
        }
    }

    hr = SetDIBitsToDevice(
        hdc,                                    // Target device HDC
        winx,
        winy,
        GRAPHICWINDOW_WIDTH,                    // Destination width
        GRAPHICWINDOW_HEIGHT,                   // Destination height
        0,                                      // X source position
        0,                                      // Adjusted Y source position
        (UINT)0,                                // Start scan line
        gGraphicWindowBitmap.bmiHeader.biHeight, // Scan lines present
        gGraphicWindowBitmapData,                // Image data
        &gGraphicWindowBitmap,                   // DIB header
        DIB_RGB_COLORS);                        // Type of palette
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void PlotOscilliscope(HDC hdc, int winx, int winy)
{
    int channel, count, count2, numchannels;
    float xoff, step;
    float *src;
    static float OscBuffer[GRAPHICWINDOW_WIDTH];
    
    memset(gGraphicWindowBitmapData, 0, GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT * sizeof(RGBQUAD));

    gSystem->getSoftwareFormat(0, 0, &numchannels, 0, 0, 0);

    for (channel = 0; channel < numchannels; channel++)
    {
        gSystem->getWaveData(OscBuffer, GRAPHICWINDOW_WIDTH, channel);

        /*
            xoff is the x position that is scaled lookup of the dsp block according to the graphical
            window size.
        */
        xoff = 0;
        step = 1;

        src = OscBuffer;
        for (count=0; count < GRAPHICWINDOW_WIDTH; count++)
        {
            RGBQUAD *pixel;
            int x, y, y2;
    
            #define calcoffset2(_x, _y) (GRAPHICWINDOW_WIDTH * _y) + _x

            x  = (int)xoff;
            y  = (int)((src[x]           + 1.0f) / 2.0f * (float)GRAPHICWINDOW_HEIGHT);
            y2 = (int)((src[x+(int)step] + 1.0f) / 2.0f * (float)GRAPHICWINDOW_HEIGHT);

            y  = y  < 0 ? 0 : y  >= GRAPHICWINDOW_HEIGHT ? GRAPHICWINDOW_HEIGHT-1 : y;
            y2 = y2 < 0 ? 0 : y2 >= GRAPHICWINDOW_HEIGHT ? GRAPHICWINDOW_HEIGHT-1 : y2;

            if (y > y2)
            {
                int tmp = y;
                y = y2;
                y2 = tmp;
            }

            for (count2=y; count2<= y2; count2++)
            {
                pixel = &gGraphicWindowBitmapData[calcoffset2(count, count2)];
                pixel->rgbRed   = 0xff;
                pixel->rgbGreen = 0xff;
                pixel->rgbBlue  = 0xaf;
            }

            xoff += step;
        }
    }

    SetDIBitsToDevice(
        hdc,                                    // Target device HDC
        winx,
        winy,
        GRAPHICWINDOW_WIDTH,                    // Destination width
        GRAPHICWINDOW_HEIGHT,                   // Destination height
        0,                                      // X source position
        0,                                      // Adjusted Y source position
        (UINT)0,                                // Start scan line
        gGraphicWindowBitmap.bmiHeader.biHeight, // Scan lines present
        gGraphicWindowBitmapData,                // Image data
        &gGraphicWindowBitmap,                   // DIB header
        DIB_RGB_COLORS);                        // Type of palette
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT RefreshPluginList()
{
    RECT        rect;
    SCROLLINFO  scrollinfo;
    Plugin     *plugin;

    gPluginWindowWidth = 0;
    gPluginWindowHeight = 5;

    GetWindowRect(gPluginHwnd, &rect);
    InvalidateRect(gPluginHwnd, &rect, TRUE);

    /*
        Now re-add using new list.
    */
    plugin = gPluginHead.next;
    while (plugin != &gPluginHead)
    {
        MoveWindow(plugin->hwnd, 5, 
                                 gPluginWindowHeight, 
                                 plugin->configwidth > plugin->windowwidth ? plugin->configwidth : plugin->windowwidth, 
                                 plugin->windowheight + (plugin->configactive ? plugin->configheight : 0), 
                                 TRUE);

        gPluginWindowHeight += plugin->windowheight;
        if (plugin->windowwidth > gPluginWindowWidth)
        {
            gPluginWindowWidth = plugin->windowwidth;
        }
        
        if (plugin->configactive)
        {
            gPluginWindowHeight += 5;                           /* gap below sliders and gui */
            gPluginWindowHeight += plugin->configheight;
            if (plugin->configwidth + 5 > gPluginWindowWidth)
            {
                gPluginWindowWidth = plugin->configwidth;
            }
        }

        gPluginWindowHeight += 5;   /* gap below plugin */

        plugin = plugin->next;
    }

    memset(&scrollinfo, 0, sizeof(SCROLLINFO));
    scrollinfo.cbSize = sizeof(SCROLLINFO);
    scrollinfo.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
    scrollinfo.nMin   = 0;
    scrollinfo.nMax   = gPluginWindowHeight;
    scrollinfo.nPage  = rect.bottom / 2;
    scrollinfo.nPos   = 0;
    SetScrollInfo(gPluginHwnd, SB_VERT, &scrollinfo, TRUE);

    memset(&scrollinfo, 0, sizeof(SCROLLINFO));
    scrollinfo.cbSize = sizeof(SCROLLINFO);
    scrollinfo.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
    scrollinfo.nMin   = 0;
    scrollinfo.nMax   = gPluginWindowWidth;
    scrollinfo.nPage  = (rect.right - rect.left) - 35;
    scrollinfo.nPos   = 0;
    SetScrollInfo(gPluginHwnd, SB_HORZ, &scrollinfo, TRUE);

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT RemovePlugin(Plugin *plugin)
{
    FMOD_RESULT result;

    if (plugin->hwnd)
    {
        DestroyWindow(plugin->hwnd);
    }
    
    plugin->next->prev = plugin->prev;
    plugin->prev->next = plugin->next;
    
    result = plugin->dsp->release();
    ERRCHECK(result);

    RefreshPluginList();

    delete plugin;   

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT AddPlugin(char *name, char *displayname)
{
    FMOD_RESULT result;
    FMOD_PLUGINTYPE plugintype;
    Plugin *plugin;
    unsigned int handle;
    
    result = gSystem->loadPlugin(name, &handle);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = gSystem->getPluginInfo(handle, &plugintype, 0,0,0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (plugintype != FMOD_PLUGINTYPE_DSP)
    {
        gSystem->unloadPlugin(handle);
        return FMOD_ERR_PLUGIN;
    }

    plugin = new Plugin;
    if (!plugin)
    {
        return FMOD_ERR_MEMORY;
    }

    memset(plugin, 0, sizeof(Plugin));
    
    result = gSystem->createDSPByPlugin(handle, &plugin->dsp);
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Add to end of circular linked list.
    */
    plugin->prev = gPluginHead.prev;
    gPluginHead.prev->next = plugin;
    gPluginHead.prev = plugin;
    plugin->next = &gPluginHead;

    /*
        Add the plugin dialogue to the plugin window
    */
    {
        RECT          rect;
        SCROLLINFO    scrollinfo;
        char          name[256], s[256];
        unsigned int  version = 0;
        int           count2;

        GetWindowRect(gPluginHwnd, &rect);

        plugin->dsp->getInfo(name, &version, 0, &plugin->configwidth, &plugin->configheight);
        plugin->dsp->getNumParameters(&plugin->numparams);

        sprintf(s, "%s %1x.%2x", name, version >> 16, version & 0xFFFF);

        plugin->windowheight = ((plugin->numparams + 1) * (SLIDERHEIGHT + 5)) + 10;
        plugin->windowwidth  = (rect.right - rect.left) - 35;
        plugin->hwnd         = CreateWindow("STATIC",   s,      WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 5,                         gPluginWindowHeight, plugin->windowwidth, plugin->windowheight, gPluginHwnd,  NULL,     0, 0);
        plugin->removehwnd   = CreateWindow("BUTTON", "Remove", WS_CHILD | WS_VISIBLE,                               plugin->windowwidth - 105, 5,                   100,                 20,                   plugin->hwnd, (HMENU)IDC_REMOVEBUTTON, 0, 0);
        plugin->confighwnd   = CreateWindow("BUTTON", "Config", WS_CHILD | WS_VISIBLE,                               plugin->windowwidth - 210, 5,                   100,                 20,                   plugin->hwnd, (HMENU)IDC_CONFIGBUTTON, 0, 0);
        plugin->activehwnd   = CreateWindow("BUTTON", "Active", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,             plugin->windowwidth - 315, 5,                   100,                 20,                   plugin->hwnd, (HMENU)IDC_ACTIVEBUTTON, 0, 0);
        plugin->configactive = false;

        SetWindowLong(plugin->hwnd, GWL_USERDATA, (LONG)plugin);
        plugin->oldpluginproc = (WNDPROC)GetWindowLong(plugin->hwnd, GWL_WNDPROC);
        SetWindowLong(plugin->hwnd, GWL_WNDPROC,  (LONG)FMOD_PluginInstanceProc);

        memset(plugin->param, 0, sizeof(PluginParam) * 32);

        /*
            Now add sliders for each plugin
        */
        for (count2 = 0; count2 < plugin->numparams; count2++)
        {
            PluginParam *param = &plugin->param[count2];
            char paramname[32], label[32], valuestr[32];
            float value, scale;
            int x = 0, sliderwidth;

            plugin->dsp->getParameterInfo(count2, paramname, label, 0, 0, &param->min, &param->max);
            plugin->dsp->getParameter(count2, &value, valuestr, 32);

            sliderwidth = (rect.right - rect.left) - 350;

            param->plugin = plugin;
            param->index  = count2;
            scale = sliderwidth / fabsf(param->max - param->min); 

            sprintf(s, "%s %s", valuestr, label);
            x = 5;
            param->namehwnd   = CreateWindow("STATIC",       paramname, WS_CHILD | WS_VISIBLE, x, 30 + (count2 * (SLIDERHEIGHT + 5)), 150-5,       SLIDERHEIGHT, plugin->hwnd, NULL, 0, 0);
            x += 145;
            param->sliderhwnd = CreateWindow(TRACKBAR_CLASS, "",        WS_CHILD | WS_VISIBLE, x, 30 + (count2 * (SLIDERHEIGHT + 5)), sliderwidth, SLIDERHEIGHT, plugin->hwnd, NULL, 0, 0);
            x += sliderwidth;
            param->valuehwnd  = CreateWindow("STATIC",       s,         WS_CHILD | WS_VISIBLE, x, 30 + (count2 * (SLIDERHEIGHT + 5)), 150-5,       SLIDERHEIGHT, plugin->hwnd, NULL, 0, 0);
            /*
	            Subclass the position slider so we can get the mouse messages and process them there
            */
            SetWindowLong(param->sliderhwnd, GWL_USERDATA, (LONG)param);
            param->oldtrackbarproc = (WNDPROC)GetWindowLong(param->sliderhwnd, GWL_WNDPROC);
            SetWindowLong(param->sliderhwnd, GWL_WNDPROC,  (LONG)FMOD_TrackbarProc);

            SendMessage(param->sliderhwnd, TBM_SETRANGE,    (WPARAM)TRUE,   (LPARAM)MAKELONG(0, sliderwidth));
            SendMessage(param->sliderhwnd, TBM_SETPAGESIZE, (WPARAM)0,      (LPARAM)scale);
            SendMessage(param->sliderhwnd, TBM_SETPOS,      (WPARAM)TRUE,   (LPARAM)( (value - param->min) / (param->max - param->min) * sliderwidth ));  

        }

        gPluginWindowHeight += (plugin->windowheight + 5);


        memset(&scrollinfo, 0, sizeof(SCROLLINFO));
        scrollinfo.cbSize = sizeof(SCROLLINFO);
        scrollinfo.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
        scrollinfo.nMin   = 0;
        scrollinfo.nMax   = gPluginWindowHeight- 30;
        scrollinfo.nPage  = rect.bottom / 2;
        SetScrollInfo(gPluginHwnd, SB_VERT, &scrollinfo, TRUE);

        memset(&scrollinfo, 0, sizeof(SCROLLINFO));
        scrollinfo.cbSize = sizeof(SCROLLINFO);
        scrollinfo.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
        scrollinfo.nMin   = 0;
        scrollinfo.nMax   = gPluginWindowWidth;
        scrollinfo.nPage  = (rect.right - rect.left) - 35;
        SetScrollInfo(gPluginHwnd, SB_HORZ, &scrollinfo, TRUE);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
LRESULT CALLBACK FMOD_ProgressWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
        case WM_LBUTTONDOWN:
        case WM_MOUSEMOVE:
        {
            FMOD_RESULT  result;
            RECT         r;
            int          width, xpos = LOWORD(lParam);  // horizontal position of cursor 
            unsigned int length;

            if (message == WM_MOUSEMOVE && wParam != MK_LBUTTON)
            {
                break;
            }
        
            GetWindowRect(hwnd, &r);
            width = r.right - r.left;
            
            result = gSound->getLength(&length, FMOD_TIMEUNIT_MODORDER);
            if (result != FMOD_OK)
            {
                gSound->getLength(&length, FMOD_TIMEUNIT_MS);
            }

            result = gChannel->setPosition((unsigned int)((float)length / (float)width * (float)xpos), FMOD_TIMEUNIT_MODORDER);
            if (result != FMOD_OK)
            {
                gChannel->setPosition((unsigned int)((float)length / (float)width * (float)xpos), FMOD_TIMEUNIT_MS);
            }
            break;
        }
	};
    
	return (long)CallWindowProc(gOldProgressProc, hwnd, message, wParam, lParam);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
LRESULT CALLBACK FMOD_PluginInstanceProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC oldpluginproc;

    Plugin *plugin = (Plugin *)GetWindowLong(hwnd, GWL_USERDATA);

    oldpluginproc = plugin->oldpluginproc;

	switch (msg)
	{
        case WM_COMMAND:
        {
			switch (LOWORD(wParam))
			{
                case IDC_ACTIVEBUTTON:
                {
                    if (Button_GetCheck(plugin->activehwnd))
                    {
                        gSystem->addDSP(plugin->dsp, 0);
                    }
                    else if (!Button_GetCheck(plugin->activehwnd))
                    {
                        plugin->dsp->remove();
                    }
                    break;
                }
                case IDC_REMOVEBUTTON:
                {
                    RemovePlugin(plugin);
                    break;
                }
                case IDC_CONFIGBUTTON:
                {
                    if (plugin->dsp)
                    {
                        if (plugin->configactive)
                        {
                            plugin->dsp->showConfigDialog(hwnd, false);
                            DestroyWindow(plugin->configguihwnd);

                            plugin->configactive = false;
                            RefreshPluginList();                       

                            plugin->configguihwnd = 0;
                        }
                        else
                        {
                            plugin->configactive = true;
                            RefreshPluginList();
                            plugin->configguihwnd = CreateWindow("STATIC", 
                                                                 "", 
                                                                 WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 
                                                                 5,                         
                                                                 plugin->windowheight, 
                                                                 plugin->configwidth, 
                                                                 plugin->configheight, 
                                                                 plugin->hwnd,  
                                                                 NULL,     
                                                                 0, 
                                                                 0);
                            plugin->dsp->showConfigDialog(plugin->configguihwnd, true);
                        }
                    }
                    break;
                }
            }
            break;
        }
    };

    return CallWindowProc(oldpluginproc, hwnd, msg, wParam, lParam);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
LRESULT CALLBACK FMOD_TrackbarProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PluginParam *param = (PluginParam *)GetWindowLong(hwnd, GWL_USERDATA);
    bool update = false;

	switch (msg)
	{
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            update = true;
            break;
        }
        case WM_LBUTTONDOWN:
        {
            param->dragging = TRUE;
            update = true;
            break;
        }
        case WM_LBUTTONUP:
        {
            param->dragging = FALSE;
            break;
        }
        case WM_MOUSEMOVE:
        {
            if (param->dragging)
            {
                update = true;
            }
            break;
        }
    };

    if (update)
    {
        RECT rect;
        int pos = (int)SendMessage(hwnd, TBM_GETPOS, 0, 0);
        float value;
        char valuestr[32];
        char label[32];
        char s[256];
        int sliderwidth;

        GetWindowRect(hwnd, &rect);

        sliderwidth = rect.right - rect.left;

        value = (float)pos + 0.5f;
        value /= sliderwidth;
        value *= (param->max - param->min);
        value += param->min;

        param->plugin->dsp->getParameterInfo(param->index, 0, label, 0, 0, 0, 0);
        param->plugin->dsp->setParameter(param->index, value);
        param->plugin->dsp->getParameter(param->index, 0, valuestr, 32);

        sprintf(s, "%s %s", valuestr, label);

        SendMessage(param->valuehwnd, WM_SETTEXT, 0, (LPARAM)s);
    }

    return CallWindowProc(param->oldtrackbarproc, hwnd, msg, wParam, lParam);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
LRESULT CALLBACK FMOD_PluginWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
        case WM_HSCROLL:        
        {
            int lineWidth = 1;

            switch (LOWORD(wParam))
            {         
                int OldPos, NewPos;
                
                case SB_PAGEDOWN:         
                case SB_LINEDOWN:           
                    OldPos = GetScrollPos (hwnd, SB_HORZ);           
                    SetScrollPos (hwnd, SB_HORZ, (OldPos+1), TRUE);           
                    NewPos = GetScrollPos (hwnd, SB_HORZ);           
                    ScrollWindow (hwnd, (OldPos-NewPos)*lineWidth, 0, NULL, NULL);         
                    break;          
                case SB_PAGEUP:         
                case SB_LINEUP:           
                    OldPos = GetScrollPos (hwnd, SB_HORZ);           
                    SetScrollPos (hwnd, SB_HORZ, (OldPos-1), TRUE);           
                    NewPos = GetScrollPos (hwnd, SB_HORZ);           
                    ScrollWindow (hwnd, (OldPos-NewPos)*lineWidth, 0, NULL, NULL);         
                    break;          
                case SB_THUMBPOSITION:           
                case SB_THUMBTRACK: 
                    OldPos = GetScrollPos (hwnd, SB_HORZ);           
                    NewPos = HIWORD (wParam);           
                    SetScrollPos (hwnd, SB_HORZ, NewPos, TRUE);           
                    ScrollWindow (hwnd, (OldPos-NewPos)*lineWidth, 0, NULL, NULL);         
                    break;        
            } ;
            break;
        }
        case WM_VSCROLL:        
        {
            int lineHeight = 1;

            switch (LOWORD(wParam))
            {         
                int OldPos, NewPos;
                
                case SB_PAGEDOWN:         
                case SB_LINEDOWN:           
                    OldPos = GetScrollPos (hwnd, SB_VERT);           
                    SetScrollPos (hwnd, SB_VERT, (OldPos+1), TRUE);           
                    NewPos = GetScrollPos (hwnd, SB_VERT);           
                    ScrollWindow (hwnd, 0,(OldPos-NewPos)*lineHeight, NULL, NULL);         
                    break;          
                case SB_PAGEUP:         
                case SB_LINEUP:           
                    OldPos = GetScrollPos (hwnd, SB_VERT);           
                    SetScrollPos (hwnd, SB_VERT, (OldPos-1), TRUE);           
                    NewPos = GetScrollPos (hwnd, SB_VERT);           
                    ScrollWindow (hwnd, 0,(OldPos-NewPos)*lineHeight, NULL, NULL);         
                    break;          
                case SB_THUMBPOSITION:           
                case SB_THUMBTRACK: 
                    OldPos = GetScrollPos (hwnd, SB_VERT);           
                    NewPos = HIWORD (wParam);           
                    SetScrollPos (hwnd, SB_VERT, NewPos, TRUE);           
                    ScrollWindow (hwnd, 0, (OldPos-NewPos)*lineHeight, NULL, NULL);         
                    break;        
            } ;
            break;
        }
	}

    return DefWindowProc (hwnd, msg, wParam, lParam) ;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
BOOL CALLBACK FMOD_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int timerid = 0;
 
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			/*
			    SET UP A PAINT TIMER FOR INTERFACE
			*/
			timerid = SetTimer(hwnd, 0, 10, (TIMERPROC)0); 
			if (!timerid)
			{
    			MessageBox( NULL, "Too many timers in use", "Error", MB_ICONHAND|MB_OK|MB_SYSTEMMODAL);
    			return FALSE;
			}

            /*
                Set up the child plugin window
            */
            {
                RECT        recordrect;
                RECT        rect;
                WNDCLASS    wc;
                char szWindowClass[] = { TEXT("Plugin Window") };

                memset(&wc, 0, sizeof(WNDCLASS));
                wc.style			= CS_HREDRAW | CS_VREDRAW;
                wc.lpfnWndProc		= (WNDPROC)FMOD_PluginWindowProc;
                wc.hInstance		= gHinstance;
                wc.hIcon			= 0;
                wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
                wc.hbrBackground	= (HBRUSH)GetStockObject(LTGRAY_BRUSH);
                wc.lpszMenuName		= 0;
                wc.lpszClassName	= szWindowClass;

                RegisterClass(&wc);

                GetWindowRect(hwnd, &rect);
                GetWindowRect(GetDlgItem(hwnd, IDC_STATIC_RECORD), &recordrect);

                gPluginHwnd = CreateWindowEx(WS_EX_CLIENTEDGE, szWindowClass, "", WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPSIBLINGS | WS_VISIBLE, 5, recordrect.bottom, rect.right - rect.left - 25, (rect.bottom - rect.top) - recordrect.bottom - 65, hwnd, 0, 0, 0);
            }

            gPluginHead.next = &gPluginHead;
            gPluginHead.prev = &gPluginHead;

	        gOldProgressProc = (WNDPROC)GetWindowLong(GetDlgItem(hwnd, IDC_PROGRESS1), GWL_WNDPROC);
            SetWindowLong(GetDlgItem(hwnd, IDC_PROGRESS1), GWL_WNDPROC, (LONG)&FMOD_ProgressWindowProc);

			return TRUE;
		}
        case WM_COMMAND:
        {
			switch (LOWORD(wParam))
			{
                case IDC_OPENBUTTON:
                {
	                OPENFILENAME	ofn;					/* common dialog box structure   */
	                char			szDirName[MAX_PATH];    /* directory string              */
	                char			szFile[20481];			/* filename string               */
	                char			szFileTitle[4096];      /* filename string               */

	                /*
                        Obtain the system directory name and store it in szDirName. 
                    */
	                GetSystemDirectory(szDirName, sizeof(szDirName)); 

	                /*
                        Place the terminating null character in the szFile.
                    */
	                szFile[0] = '\0';
                    szFileTitle[0] = '\0';

	                /*
                        Set the members of the OPENFILENAME structure.
                    */
	                memset(&ofn, 0, sizeof(ofn));
	                ofn.lStructSize       = sizeof(OPENFILENAME); 
	                ofn.hwndOwner         = hwnd; 
	                ofn.lpstrTitle        = "Open\0";
	                ofn.lpstrFilter       = "All audio files (*.*)\0*.*\0\0";
	                ofn.lpstrCustomFilter = NULL;
	                ofn.nFilterIndex      = 1; 
	                ofn.lpstrFile         = szFile; 
	                ofn.nMaxFile          = 20480;     /* Huge value allows many files to be multi-selected */
	                ofn.lpstrFileTitle    = szFileTitle;
	                ofn.nMaxFileTitle     = 2048;
	                ofn.lpstrInitialDir   = ".\0";
	                ofn.Flags             = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; 

	                /*
                        Display the Open dialog box.
                    */
	                if (GetOpenFileName(&ofn)) 
	                {
                        FMOD_RESULT result;

                        if (gSound)
                        {
                            if (gChannel)
                            {
                                gChannel->stop();
                                gChannel = 0;
                                SendMessage(GetDlgItem(hwnd, IDC_PLAY), WM_SETTEXT, 0, (LPARAM)"Play");
                            }
                            gSound->release();
                        }
                        result = gSystem->createStream(ofn.lpstrFile, FMOD_2D | FMOD_SOFTWARE, 0, &gSound);
                        ERRCHECK(result);

                        SendMessage(GetDlgItem(hwnd, IDC_EDIT1), WM_SETTEXT, 0, (LPARAM)ofn.lpstrFile);
                    }
                    break;
                }
                case IDC_PLAY:
                {
                    bool isplaying = false;;
    
                    if (gChannel)
                    {
                        gChannel->isPlaying(&isplaying);
                    }

                    if (isplaying || !gSound)
                    {
                        if (gChannel)
                        {
                            gChannel->stop();
                            gChannel = 0;
                        }
                        SendMessage(GetDlgItem(hwnd, IDC_PLAY), WM_SETTEXT, 0, (LPARAM)"Play");
                    }
                    else
                    {
                        gSystem->playSound(FMOD_CHANNEL_FREE, gSound, false, &gChannel);
                        SendMessage(GetDlgItem(hwnd, IDC_PLAY), WM_SETTEXT, 0, (LPARAM)"Stop");
                    }
                    break;
                }
                case IDC_PAUSE:
                {
                    if (gChannel)
                    {
                        bool paused;

                        gChannel->getPaused(&paused);
                        if (paused)
                        {
                            gChannel->setPaused(false);
                            SendMessage(GetDlgItem(hwnd, IDC_PAUSE), WM_SETTEXT, 0, (LPARAM)"Pause");
                        }
                        else
                        {
                            gChannel->setPaused(true);
                            SendMessage(GetDlgItem(hwnd, IDC_PAUSE), WM_SETTEXT, 0, (LPARAM)"UnPause");
                        }
                    }
                    break;
                }
                case IDC_RECORD:
                {
                    if (!gSoundRecord)
                    {
                        FMOD_RESULT result;
                        FMOD_OUTPUTTYPE output;
                        FMOD_CREATESOUNDEXINFO exinfo;

                        FMOD_MODE mode = FMOD_2D | FMOD_OPENUSER;

                        mode |= FMOD_LOOP_NORMAL;
                        mode |= FMOD_SOFTWARE;
  
                        #define USERLENGTH (44100 * 5)
              
                        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
                        exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
                        exinfo.length           = USERLENGTH * 2 * sizeof(short);
                        exinfo.numchannels      = 2;
                        exinfo.defaultfrequency = 44100;
                        exinfo.format           = FMOD_SOUND_FORMAT_PCM16;

                        result = gSystem->createSound(0, mode, &exinfo, &gSoundRecord);
                        ERRCHECK(result);

                        result = gSystem->recordStart(0, gSoundRecord, true);
                        ERRCHECK(result);

                        gSystem->getOutput(&output);
                        if (output != FMOD_OUTPUTTYPE_ASIO)
                        {
                            Sleep(100);
                        }

                        result = gSystem->playSound(FMOD_CHANNEL_FREE, gSoundRecord, false, &gChannelRecord);
                        ERRCHECK(result);

                        SendMessage(GetDlgItem(hwnd, IDC_RECORD), WM_SETTEXT, 0, (LPARAM)"Stop Recording");
                    }
                    else
                    {
                        gSystem->recordStop(0);

                        gChannelRecord->stop();
                        gChannelRecord = 0;
                        gSoundRecord->release();
                        gSoundRecord = 0;

                        SendMessage(GetDlgItem(hwnd, IDC_RECORD), WM_SETTEXT, 0, (LPARAM)"Record");
                    }
                    break;
                }
                case IDC_ADDPLUGIN:
                {
	                OPENFILENAME	ofn;					/* common dialog box structure   */
	                char			szDirName[MAX_PATH];    /* directory string              */
	                char			szFile[20481];			/* filename string               */
	                char			szFileTitle[4096];      /* filename string               */

	                /*
                        Obtain the system directory name and store it in szDirName. 
                    */
	                GetSystemDirectory(szDirName, sizeof(szDirName)); 

	                /*
                        Place the terminating null character in the szFile.
                    */
	                szFile[0] = '\0';
                    szFileTitle[0] = '\0';

	                /*
                        Set the members of the OPENFILENAME structure.
                    */
	                memset(&ofn, 0, sizeof(ofn));
	                ofn.lStructSize       = sizeof(OPENFILENAME); 
	                ofn.hwndOwner         = hwnd; 
	                ofn.lpstrTitle        = "Open\0";
                    ofn.lpstrFilter       = "FMOD Plugins (dsp*.dll)\0dsp*.dll\0VST Plugins (*.dll)\0*.dll\0All files (*.*)\0*.*\0\0";
	                ofn.lpstrCustomFilter = NULL;
	                ofn.nFilterIndex      = 1; 
	                ofn.lpstrFile         = szFile; 
	                ofn.nMaxFile          = 20480;     /* Huge value allows many files to be multi-selected */
	                ofn.lpstrFileTitle    = szFileTitle;
	                ofn.nMaxFileTitle     = 2048;
	                ofn.lpstrInitialDir   = "..\\..\\api\\plugins\0";
	                ofn.Flags             = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT; 

	                /*
                        Display the Open dialog box.
                    */
	                if (GetOpenFileName(&ofn)) 
	                {
		                char *path = ofn.lpstrFile;
		                char  *fname = ofn.lpstrFile;
		                char  name[256];

		                /*
                            Single file
                        */
		                if (ofn.nFileOffset)
		                {
			                /*
                                Skip to the first filename
                            */
			                fname +=ofn.nFileOffset;
			                *(fname-1) = 0;		/* clear the backslash before the filename so the loop below handles it */
		                }
		                /*
                            Multiple files
                        */
		                else
		                {
			                while (*fname++);	/* search to the first filename */
		                }

		                do 
		                {
                            FMOD_RESULT result;

			                /*
                                Put path in name
                            */
                            sprintf(name, "%s%s%s", path, "\\", fname);

			                /*
                                Open the file.
                            */
                            result = AddPlugin(name, fname);
                            ERRCHECK(result);

			                while (*fname++);

                            Sleep(1);

		                } while (*fname);
                    }
                    break;
                }
            }
            break;
        }
		case WM_CLOSE:
		{
            FMOD_RESULT result;

			KillTimer(hwnd, timerid);

            /*
                The timer doesnt die immediately, sleep for a bit to let it die so we dont free streams then the timer tries to access them
            */
            Sleep(200);

            /*
                Shutdown FMOD
            */
            if (gSound)
            {
                result = gSound->release();
                ERRCHECK(result);
            }
            if (gSystem)
            {
                result = gSystem->close();
                ERRCHECK(result);

                result = gSystem->release();
                ERRCHECK(result);
            }

			EndDialog(hwnd,0);
            PostQuitMessage(0);
			return TRUE;
		}
        
		case WM_TIMER :
		{
			HDC hdc = GetDC(hwnd);
            RECT        rect;
            
            GetWindowRect(GetDlgItem(gMainHwnd,IDC_STATIC_ANALYZER), &rect);

            gSystem->update();		

            PlotSpectrum(hdc,     (((rect.right  - rect.left) / 4) -      (GRAPHICWINDOW_WIDTH  / 2)),
                                  (((rect.bottom - rect.top)  / 2) -      (GRAPHICWINDOW_HEIGHT / 2)), true);
            PlotOscilliscope(hdc, ((((rect.right - rect.left) / 4) * 3) - (GRAPHICWINDOW_WIDTH  / 2)),
                                  (((rect.bottom - rect.top)  / 2) -      (GRAPHICWINDOW_HEIGHT / 2)));
            
            if (gChannel)
            {
                FMOD_RESULT result;
                unsigned int currtime, length;
                bool playing;

                result = gChannel->getPosition(&currtime, FMOD_TIMEUNIT_MODORDER);
                if (result != FMOD_OK)
                {
                    gChannel->getPosition(&currtime, FMOD_TIMEUNIT_MS);
                }

                result = gSound->getLength(&length, FMOD_TIMEUNIT_MODORDER);
                if (result != FMOD_OK)
                {
                    gSound->getLength(&length, FMOD_TIMEUNIT_MS);
                }

                SendMessage(GetDlgItem(gMainHwnd,IDC_PROGRESS1), PBM_SETRANGE, 0, MAKELPARAM(0, 1000) );
 			    SendMessage(GetDlgItem(gMainHwnd,IDC_PROGRESS1), PBM_SETPOS, (WPARAM)((float)currtime / (float)length * 1000.0f), 0);

                gChannel->isPlaying(&playing);
                if (!playing)
                {
                    gChannel = 0;
                    SendMessage(GetDlgItem(hwnd, IDC_PLAY), WM_SETTEXT, 0, (LPARAM)"Play");
                }
            }

			ReleaseDC(hwnd, hdc);
            break;
		}
	}

	return FALSE;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
BOOL CALLBACK FMOD_OutputSelectProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int selected = 0;
 
	switch (msg)
	{
		case WM_INITDIALOG:
		{
            Button_SetCheck(GetDlgItem(hwnd,IDC_DSOUND), TRUE);
			return TRUE;
		}
        case WM_COMMAND:
        {
			switch (LOWORD(wParam))
			{
                case IDOK:
                {
                    if (Button_GetCheck(GetDlgItem(hwnd,IDC_DSOUND)))
                    {
                        selected = 0;
                    }
                    if (Button_GetCheck(GetDlgItem(hwnd,IDC_WINMM)))
                    {
                        selected = 1;
                    }
                    if (Button_GetCheck(GetDlgItem(hwnd,IDC_ASIO)))
                    {
                        selected = 2;
                    }

			        EndDialog(hwnd, selected);
                    break;
                }
                case IDCANCEL:
                {
	        		EndDialog(hwnd, -1);
                    PostQuitMessage(0);
                    break;
                }
            }
            return TRUE;
        }
		case WM_CLOSE:
		{
			EndDialog(hwnd, -1);
            PostQuitMessage(0);
			return TRUE;
		}
    }

	return FALSE;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
int Initialize(HINSTANCE hinst, LPSTR lpCmdLine)
{   
    FMOD_RESULT result;
    unsigned int version;
    int retval;

    /*
        Initialise FMOD
    */

    result = FMOD::Memory_Initialize(malloc(10 * 1024*1024), 10 * 1024*1024, 0, 0, 0);
    ERRCHECK(result);

    result = FMOD::System_Create(&gSystem);
    ERRCHECK(result);

    result = gSystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        MessageBox(GetForegroundWindow(), "INCORRECT DLL VERSION!!", "FMOD ERROR", MB_OK);
    }

    result = gSystem->setPluginPath("../../api/plugins");
    ERRCHECK(result);
    
    /*
        Display selection dialog
    */
	retval = DialogBox(hinst, MAKEINTRESOURCE(IDD_OUTPUTSELECT), GetDesktopWindow(), FMOD_OutputSelectProc);
    if (retval < 0)
    {
        return FALSE;
    }

    switch (retval)
    {
        case 0: 
        {
            gSystem->setOutput(FMOD_OUTPUTTYPE_DSOUND); 
            break;
        }
        case 1: 
        {
            gSystem->setOutput(FMOD_OUTPUTTYPE_WINMM); 
            break;
        }
        case 2: 
        {
            gSystem->setOutput(FMOD_OUTPUTTYPE_ASIO); 
            break;
        }
    }

    /*
        Set the output rate to 44100 instead of the default 48000, because some winamp plugins wont work unless they are 44100.
    */
    result = gSystem->setSoftwareFormat(44100, FMOD_SOUND_FORMAT_PCM16, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
    ERRCHECK(result);

    /*
        Initialize FMOD.
    */
    result = gSystem->init(32, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);
    if (result != FMOD_OK)
    {
        return FALSE;
    }

    /*
        Create window and graphics 
    */
	gMainHwnd = CreateDialog(hinst, MAKEINTRESOURCE(IDD_DIALOG), GetDesktopWindow(), FMOD_DlgProc);

	ShowWindow(gMainHwnd, SW_SHOW);

    SetWindowText(gMainHwnd, TITLE);

    // make the header
    ZeroMemory( &gGraphicWindowBitmap, sizeof( BITMAPINFO ) );
    gGraphicWindowBitmap.bmiHeader.biSize = sizeof( gGraphicWindowBitmap.bmiHeader );
    gGraphicWindowBitmap.bmiHeader.biWidth  = GRAPHICWINDOW_WIDTH;
    gGraphicWindowBitmap.bmiHeader.biHeight = GRAPHICWINDOW_HEIGHT;
    gGraphicWindowBitmap.bmiHeader.biPlanes = 1;
    gGraphicWindowBitmap.bmiHeader.biBitCount = 32;
    gGraphicWindowBitmap.bmiHeader.biCompression = BI_RGB;
    gGraphicWindowBitmap.bmiHeader.biSizeImage = 0;
    gGraphicWindowBitmap.bmiHeader.biXPelsPerMeter = 0;
    gGraphicWindowBitmap.bmiHeader.biYPelsPerMeter = 0;
    gGraphicWindowBitmap.bmiHeader.biClrUsed = 0;
    gGraphicWindowBitmap.bmiHeader.biClrImportant = 0;

	return TRUE;
}


int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    MSG          msg;

    gHinstance = hInstance;

    /*
        Create interface and start message loop
    */
    if (!Initialize(hInstance, lpCmdLine)) return FALSE;

  	while (GetMessage( &msg, 0, 0, 0 ))
	{
    	TranslateMessage( &msg );
      	DispatchMessage( &msg );
  	}

	return (int)msg.wParam;
}
