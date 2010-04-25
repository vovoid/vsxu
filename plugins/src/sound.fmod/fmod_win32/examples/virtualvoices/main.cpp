/*===============================================================================================
 Virtual Voices Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows the virtual channels of FMOD. The listener and sounce sources can be moved
 around by clicking and dragging. Sound sources will change colour depending on whether they
 are virtual or not. Red means they are real, blue means they are virtual channels.
===============================================================================================*/

#include "../../api/inc/fmod.h"
#include "../../api/inc/fmod.hpp"
#include "../../api/inc/fmod_errors.h"
#include <stdio.h>
#include <windows.h>
#include "resource.h"

#define TITLE           "Virtual Voices Example. Copyright (c), Firelight Technologies Pty, Ltd 2004-2008."
#define NUMCHANNELS     50
#define NUMREALCHANNELS 10
#define WIDTH           640
#define HEIGHT          480
#define SOURCESIZE      5
#define LISTENERSIZE    15
#define SELECTTHRESHOLD 5
#define TITLEBAROFFSET  20

class SoundSource;
class Listener;

HWND         gMainhwnd;
HINSTANCE    gHinstance;
bool         gSelected  = false;
SoundSource *gSelectedSource = 0;
SoundSource *gSoundSource[NUMCHANNELS];
Listener    *gListener;

FMOD::System    *gSystem = 0;
FMOD::Sound     *gSound = 0;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        char    errstring[1024];

        sprintf(errstring, "FMOD error! (%d) %s", result, FMOD_ErrorString(result));

        MessageBox( NULL, errstring, "FMOD error!", MB_ICONHAND|MB_OK|MB_SYSTEMMODAL);

        exit(-1);
    }
}


class SoundSource
{
  private:
    FMOD::Channel *mChannel;
    FMOD_VECTOR mPos, mVel;
    HBRUSH mBrushBlue;
    HBRUSH mBrushRed;
    
  public:
    SoundSource(float posx, float posy)
    {
        FMOD_RESULT result;

        mPos.x = posx;
        mPos.y = posy;
        mPos.z = 0.0f;
        mVel.x = 0.0f;
        mVel.y = 0.0f;
        mVel.z = 0.0f;

        result = gSystem->playSound(FMOD_CHANNEL_FREE, gSound, true, &mChannel);
        ERRCHECK(result);

        SetPosition(posx, posy);

        result = mChannel->setFrequency(22050.0f + (float)(rand() % 88200));
        ERRCHECK(result);

        result = mChannel->setPaused(false);
        ERRCHECK(result);

        mBrushBlue = CreateSolidBrush(RGB(0,0,255));
        mBrushRed  = CreateSolidBrush(RGB(255,0,0));
    }

    ~SoundSource()
    {
        DeleteObject(mBrushBlue);
        DeleteObject(mBrushRed);
    }

    void Draw(HDC hdc)
    {
        FMOD_RESULT result;
        bool isvirtual;

        result = mChannel->isVirtual(&isvirtual);
        ERRCHECK(result);

        if (isvirtual)
        {
            SelectObject(hdc, mBrushBlue);
        }
        else
        {
            SelectObject(hdc, mBrushRed);
        }

        Ellipse(hdc, (int)(mPos.x - SOURCESIZE),
                     (int)(mPos.y - SOURCESIZE),
                     (int)(mPos.x + SOURCESIZE),
                     (int)(mPos.y + SOURCESIZE));
    }

    void SetPosition(float posx, float posy)
    {
        FMOD_RESULT result;

        mPos.x = posx;
        mPos.y = posy;

        result = mChannel->set3DAttributes(&mPos, &mVel);
        ERRCHECK(result);
    }

    FMOD_VECTOR GetPosition()
    {
        return mPos;
    }

    bool IsSelected(float posx, float posy)
    {
        if (posx > mPos.x - SOURCESIZE - SELECTTHRESHOLD &&
            posx < mPos.x + SOURCESIZE + SELECTTHRESHOLD &&
            posy > mPos.y - SOURCESIZE - SELECTTHRESHOLD&&
            posy < mPos.y + SOURCESIZE + SELECTTHRESHOLD)
        {
            return true;
        }
        
        return false;
    }
};


class Listener
{
  private:
    FMOD_VECTOR mListenerPos;
    FMOD_VECTOR mUp, mVel, mForward;
    HBRUSH      mBrush;

  public:
    Listener(float posx, float posy)
    {
        mUp.x = 0;
        mUp.y = 0;
        mUp.z = 1;

        mForward.x =  0;
        mForward.y = -1;
        mForward.z =  0;

        mVel.x = 0;
        mVel.y = 0;
        mVel.z = 0;

        SetPosition(posx, posy);

        mBrush = CreateSolidBrush(RGB(0,0,0));
    }

    ~Listener()
    {
        DeleteObject(mBrush);
    }

    void Draw(HDC hdc)
    {
        SelectObject(hdc, mBrush);
        
        // head
        Ellipse(hdc, (int)(mListenerPos.x - LISTENERSIZE),
                     (int)(mListenerPos.y - LISTENERSIZE),
                     (int)(mListenerPos.x + LISTENERSIZE),
                     (int)(mListenerPos.y + LISTENERSIZE));

        // nose
        Ellipse(hdc, (int)(mListenerPos.x - (LISTENERSIZE / 3)),
                     (int)(mListenerPos.y - (LISTENERSIZE / 3) - LISTENERSIZE),
                     (int)(mListenerPos.x + (LISTENERSIZE / 3)),
                     (int)(mListenerPos.y + (LISTENERSIZE / 3) - LISTENERSIZE));

        // left ear
        Ellipse(hdc, (int)(mListenerPos.x - (LISTENERSIZE / 3) - LISTENERSIZE),
                     (int)(mListenerPos.y - (LISTENERSIZE / 3)),
                     (int)(mListenerPos.x + (LISTENERSIZE / 3) - LISTENERSIZE),
                     (int)(mListenerPos.y + (LISTENERSIZE / 3)));

        // right ear
        Ellipse(hdc, (int)(mListenerPos.x - (LISTENERSIZE / 3) + LISTENERSIZE),
                     (int)(mListenerPos.y - (LISTENERSIZE / 3)),
                     (int)(mListenerPos.x + (LISTENERSIZE / 3) + LISTENERSIZE),
                     (int)(mListenerPos.y + (LISTENERSIZE / 3)));
    }

    void SetPosition(float posx, float posy)
    {
        FMOD_RESULT result;

        mListenerPos.x = posx;
        mListenerPos.y = posy;
        mListenerPos.z = 0;

        result = gSystem->set3DListenerAttributes(0, &mListenerPos, &mVel, &mForward, &mUp);
        ERRCHECK(result);
    }

    FMOD_VECTOR GetPosition()
    {
        return mListenerPos;
    }

    bool IsSelected(float posx, float posy)
    {
        if (posx > mListenerPos.x - LISTENERSIZE - SELECTTHRESHOLD &&
            posx < mListenerPos.x + LISTENERSIZE + SELECTTHRESHOLD &&
            posy > mListenerPos.y - LISTENERSIZE - SELECTTHRESHOLD &&
            posy < mListenerPos.y + LISTENERSIZE + SELECTTHRESHOLD)
        {
            return true;
        }
        
        return false;
    }
};


void CloseDown() 
{
    FMOD_RESULT result;
    int         count;

    /*
        Release listener
    */
    delete gListener;

    /*
        Release sound sources
    */
    for (count = 0; count < NUMCHANNELS; count++)
    {
        if (gSoundSource[count])
        {
            delete gSoundSource[count];
        }
        gSoundSource[count] = 0;
    }

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
}


BOOL CALLBACK FMOD_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int          timerid = 0;
 
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

			return TRUE;
		}

		case WM_CLOSE:
		{

			KillTimer(hwnd, timerid);

            /*
                The timer doesnt die immediately, sleep for a bit to let it die so we dont free streams then the timer tries to access them
            */
            Sleep(200);

			CloseDown();
			EndDialog(hwnd,0);
            PostQuitMessage(0);
			return TRUE;
		}

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        {
            int count;
            POINT pt;
            RECT    rect;
        
            GetWindowRect(hwnd, &rect);
            GetCursorPos(&pt);

            gSelectedSource = 0;
            
            /*
                Check if listener was selected
            */
            if (gListener->IsSelected((float)pt.x - rect.left, (float)pt.y - rect.top - TITLEBAROFFSET))
            {
                gSelected = true;
                break;
            }
            /*
                Check if a soundsource was selected
            */
            for (count = 0; count < NUMCHANNELS; count++)
            {
                if (gSoundSource[count]->IsSelected((float)pt.x - rect.left, (float)pt.y - rect.top - TITLEBAROFFSET))
                {
                    gSelectedSource = gSoundSource[count];
                    gSelected = true;
                    break;
                }
            }

            break;
        }

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        {
            gSelected  = false;

            break;
        }

        case WM_MOUSEMOVE:
        {
            if (gSelected)
            {
                POINT pt;
                RECT    rect;
            
                GetWindowRect(hwnd, &rect);
                GetCursorPos(&pt);
                
                if (gSelectedSource)
                {
                    gSelectedSource->SetPosition((float)pt.x - rect.left, (float)pt.y - rect.top - TITLEBAROFFSET);
                }
                else
                {
                    gListener->SetPosition((float)pt.x - rect.left, (float)pt.y - rect.top - TITLEBAROFFSET);
                }
            }

            break;
        }

        case WM_PAINT :
        {
            FMOD_RESULT result;
            int         count;
            int         channels;
            char        text[512];
            HDC	        hdc = GetDC(hwnd);

            HDC     hdcbuffer    = CreateCompatibleDC(hdc);
            HBITMAP hbmbuffer    = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
            HBITMAP hbmoldbuffer = (HBITMAP)SelectObject(hdcbuffer, hbmbuffer);

            HDC     hdcmem = CreateCompatibleDC(hdc);
            HBITMAP hbmold = (HBITMAP)SelectObject(hdcmem, NULL);
            RECT    rect;

            GetClientRect(hwnd, &rect);

            FillRect(hdcbuffer, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

            /*
                Draw the sound sources
            */
            for (count = 0; count < NUMCHANNELS; count++)
            {
                gSoundSource[count]->Draw(hdcbuffer);
            }

            /*
                Draw the listener
            */
            gListener->Draw(hdcbuffer);

            /*
                Print some information
            */
            result = gSystem->getChannelsPlaying(&channels);
            ERRCHECK(result);

            sprintf(text, "Channels Playing: %d", channels);
            TextOut(hdcbuffer, 5, 5, text, strlen(text));

            sprintf(text, "Real Channels:    %d (RED)", NUMREALCHANNELS);
            TextOut(hdcbuffer, 5, 22, text, strlen(text));

            sprintf(text, "Virtual Channels: %d (BLUE)", NUMCHANNELS - NUMREALCHANNELS);
            TextOut(hdcbuffer, 5, 39, text, strlen(text));

            BitBlt(hdc, rect.left, rect.top, WIDTH, HEIGHT, hdcbuffer, rect.left, rect.top, SRCCOPY);

            SelectObject(hdcmem, hbmold);               
	        DeleteDC(hdcmem);                    
	        SelectObject(hdcbuffer, hbmoldbuffer);      
            DeleteObject(hbmbuffer);                    
            DeleteDC(hdcbuffer);

            ReleaseDC(hwnd, hdc);                       
            break;
        }

		case WM_TIMER :
		{
            RECT    rect;
            
            GetWindowRect(hwnd, &rect);

            InvalidateRect(hwnd, &rect, false);

            gSystem->update();
			
            break;
		}

		default:
        {
			return FALSE;
        }
	}

	return FALSE;
}


char SetupInterface(HINSTANCE hinst, LPSTR lpCmdLine)
{   
	gMainhwnd = CreateDialog(hinst, MAKEINTRESOURCE(IDD_DIALOG), GetDesktopWindow(), FMOD_DlgProc);

    MoveWindow(gMainhwnd, 0, 0, WIDTH, HEIGHT, TRUE);
	ShowWindow(gMainhwnd, SW_SHOW);

    SetWindowText(gMainhwnd, TITLE);

	return TRUE;
}


int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    FMOD_RESULT  result;
    MSG          msg;
    HWND         hwnd;
    int          count;
    unsigned int version;

    gHinstance = hInstance;

    /*
        Create a System object and initialize.
    */
    result = FMOD::System_Create(&gSystem);
    ERRCHECK(result);

    hwnd = FindWindow(0, TITLE);
    if (hwnd)
    {
        if (IsIconic(hwnd))
        {
            ShowWindow(hwnd, SW_RESTORE);
        }
        else
        {
            SetActiveWindow(hwnd);
        }

        return 0;
    }

    /*
        Initialise
    */
    result = gSystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        MessageBox(GetForegroundWindow(), "INCORRECT DLL VERSION!!", "FMOD ERROR", MB_OK);
    }

    result = gSystem->setSoftwareChannels(NUMREALCHANNELS);
    ERRCHECK(result);

    result = gSystem->init(NUMCHANNELS, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);

    result = gSystem->createSound("../media/drumloop.wav", FMOD_SOFTWARE | FMOD_3D | FMOD_LOOP_NORMAL, 0, &gSound);
    ERRCHECK(result);
    result = gSound->set3DMinMaxDistance(4.0f, 10000.0f);
    ERRCHECK(result);

    srand(timeGetTime());

    /*
        Create a listener in the middle of the window
    */
    gListener = new Listener(WIDTH/2, HEIGHT/2);

    /*
        Initialise all of the sound sources
    */
    for (count = 0; count < NUMCHANNELS; count++)
    {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;

        gSoundSource[count] = new SoundSource((float)x, (float)y);
    }

    /*
        Create interface and start message loop
    */
    if (!SetupInterface(hInstance, lpCmdLine)) return FALSE;

  	while (GetMessage( &msg, 0, 0, 0 ))
	{
    	TranslateMessage( &msg );
      	DispatchMessage( &msg );
  	}

	return (int)msg.wParam;
}
