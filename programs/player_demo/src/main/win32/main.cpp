//#include "../vsxu_artiste/main/win32/main.cpp"


#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <map>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include "includes/nehegl.h"
#include "application.h"
#include <filesystem/vsx_filesystem.h>

bool app_ctrl;
bool app_alt;
bool app_shift;
bool disable_randomizer = false;


class OpenGLWindow;

std::map<HWND, OpenGLWindow*> window_map;
vector<Display> myscreens;

class OpenGLWindow
{
  GLsizei width_, height_;
  // Resize And Initialize The GL Window
  GLvoid resize_()
  {
    if(height_ == 0)
      height_ = 1;
    glViewport(0, 0, width_, height_);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width_ / (GLfloat)height_, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    width_ = height_ = 0;
  }

public:
  long screen_id;
  vsx_string<>classname;
  bool active;
  bool fullscreen;
  bool receive_messages;
  HDC hDC;
  HGLRC hRC;
  HWND hWnd;
  HINSTANCE hInstance;

  // Function pointer so each window can draw a different scene
  typedef bool(*DrawFunction)(int);

  GLvoid resize(GLsizei width, GLsizei height) {
    width_ = width;
    height_ = height;
  }

  OpenGLWindow()
  {
    receive_messages = true;
    screen_id = -1;
    active = true;
    fullscreen = false;
    hDC = NULL;
    hRC = NULL;
    hWnd = NULL;
    hInstance = NULL;
  }


  // All Setup For OpenGL Goes Here
  int InitGL()
  {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    return TRUE;
  }

  // Properly Kill The Window
  GLvoid Destroy()
  {
    // Are We In Fullscreen Mode?
    if(fullscreen)
    {
      // If So Switch Back To The Desktop
      ChangeDisplaySettings(NULL, 0);

      // Show Mouse Pointer
      //ShowCursor(TRUE);
    }

    // Do We Have A Rendering Context?
    if(hRC)
    {
      // Are We Able To Release The DC And RC Contexts?
      if(!wglMakeCurrent(NULL, NULL))
      {
        MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
      }

      // Are We Able To Delete The RC?
      if(!wglDeleteContext(hRC))
      {
        MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
      }

      // Set RC To NULL
      hRC = NULL;
    }

    // Are We Able To Release The DC
    if(hDC && !ReleaseDC(hWnd, hDC))
    {
      MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
      hDC = NULL;
    }

    // Are We Able To Destroy The Window?
    if (hWnd && !DestroyWindow(hWnd))
    {
      MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
      hWnd = NULL;
    }
    window_map.erase(hWnd);

    // Are We Able To Unregister Class
    if (!UnregisterClass(classname.c_str(), hInstance))
    {
      MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
      hInstance = NULL;
    }
  }

  BOOL Create(vsx_string<>className, char* title, int x, int y, int width, int height, int bits, int refreshrate, bool fullscreenflag, WNDPROC WndProc, Display my_display)
  {
    classname = className;
    //printf("classname pointer: %d\n",classname);
    GLuint		PixelFormat;			// Holds The Results After Searching For A Match
    WNDCLASS	wc;						// Windows Class Structure
    DWORD		dwExStyle;				// Window Extended Style
    DWORD		dwStyle;				// Window Style
    RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
    WindowRect.left=(long)0;			// Set Left Value To 0
    WindowRect.right=(long)width;		// Set Right Value To Requested Width
    WindowRect.top=(long)0;				// Set Top Value To 0
    WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

    fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

    hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
    wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
    wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
    wc.cbClsExtra		= 0;									// No Extra Window Data
    wc.cbWndExtra		= 0;									// No Extra Window Data
    wc.hInstance		= hInstance;							// Set The Instance
    wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
    wc.hCursor			= NULL;//LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
    wc.hbrBackground	= NULL;									// No Background Required For GL
    wc.lpszMenuName		= NULL;									// We Don't Want A Menu
    wc.lpszClassName	= className.c_str();								// Set The Class Name

    if (!RegisterClass(&wc))									// Attempt To Register The Window Class
    {
      MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;											// Return FALSE
    }

    if (fullscreen)												// Attempt Fullscreen Mode?
    {
      DEVMODE dmScreenSettings;								// Device Mode
      memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
      dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
      dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
      dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
      dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
      dmScreenSettings.dmDisplayFrequency	= refreshrate;					// Hertz
      dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;

      printf("changing display\n");
      // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
      //"Radeon X700 Series Secondary"
      if (ChangeDisplaySettingsEx(my_display.m_DeviceName.c_str(),&dmScreenSettings,NULL,CDS_FULLSCREEN,0)!=DISP_CHANGE_SUCCESSFUL)
      {
        // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
        if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","VSXu",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
        {
          fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
        }
        else
        {
          // Pop Up A Message Box Letting User Know The Program Is Closing.
          MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
          return FALSE;									// Return FALSE
        }
      }
    }

    if (fullscreen)												// Are We Still In Fullscreen Mode?
    {
      DisplayMode cur_mode = get_display_properties(my_display.m_DeviceName);
      x = cur_mode.pos_x;
      y = cur_mode.pos_y;
      printf("y window pos: %d\n",y);
      WindowRect.right = cur_mode.pos_w;
      WindowRect.bottom = cur_mode.pos_h;
      dwExStyle=WS_EX_APPWINDOW;// | WS_EX_TOPMOST;								// Window Extended Style
      dwStyle=WS_POPUP;										// Windows Style
    }
    else
    {
      dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
      dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
    }

    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

    // Create The Window
    if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
                  classname.c_str(),							// Class Name
                  title,								// Window Title
                  dwStyle |							// Defined Window Style
                  WS_CLIPSIBLINGS |					// Required Window Style
                  WS_CLIPCHILDREN,					// Required Window Style
                  x, y,								// Window Position
                  WindowRect.right-WindowRect.left,	// Calculate Window Width
                  WindowRect.bottom-WindowRect.top,	// Calculate Window Height
                  NULL,								// No Parent Window
                  NULL,								// No Menu
                  hInstance,							// Instance
                  NULL)))								// Dont Pass Anything To WM_CREATE
    {
      Destroy();								// Reset The Display
      MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }
    window_map[hWnd] = this;

    static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
    {
      sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
      1,											// Version Number
      PFD_DRAW_TO_WINDOW |						// Format Must Support Window
      PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
      PFD_DOUBLEBUFFER,							// Must Support Double Buffering
      PFD_TYPE_RGBA,								// Request An RGBA Format
      bits,										// Select Our Color Depth
      0, 0, 0, 0, 0, 0,							// Color Bits Ignored
      0,											// No Alpha Buffer
      0,											// Shift Bit Ignored
      0,											// No Accumulation Buffer
      0, 0, 0, 0,									// Accumulation Bits Ignored
      16,											// 16Bit Z-Buffer (Depth Buffer)
      0,											// No Stencil Buffer
      0,											// No Auxiliary Buffer
      PFD_MAIN_PLANE,								// Main Drawing Layer
      0,											// Reserved
      0, 0, 0										// Layer Masks Ignored
    };

    if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
    {
      Destroy();								// Reset The Display
      MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }

    if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
    {
      Destroy();								// Reset The Display
      MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }

    if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
    {
      Destroy();								// Reset The Display
      MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }

    if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
    {
      Destroy();								// Reset The Display
      MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }

    if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
    {
      Destroy();								// Reset The Display
      MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }

    ShowWindow(hWnd,SW_SHOW);						// Show The Window
    SetForegroundWindow(hWnd);						// Slightly Higher Priority
    SetFocus(hWnd);									// Sets Keyboard Focus To The Window
    height_ = height;
    width_ = width;
    resize_();					// Set Up Our Perspective GL Screen

    if (!InitGL())									// Initialize Our Newly Created GL Window
    {
      Destroy();								// Reset The Display
      MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }
    typedef BOOL (APIENTRY * wglSwapIntervalEXT_Func)(int);
    wglSwapIntervalEXT_Func wglSwapIntervalEXT = wglSwapIntervalEXT_Func(wglGetProcAddress("wglSwapIntervalEXT"));
    if (wglSwapIntervalEXT) wglSwapIntervalEXT(1);

    return TRUE;									// Success
  }

  bool Run(DrawFunction drawFunc,int id)
  {
    //wglMakeCurrent(hDC,hRC);
    if (height_ != 0)	resize_();
    // Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
    if ((active && !(*drawFunc)(id)))	// Active?  Was There A Quit Received?
    return true;
    else
    SwapBuffers(hDC);
    return false;
  }

  void activate()
  {
    printf("activate wgl  hDC: %d, hRC: %d\n",hDC, hRC);
    wglMakeCurrent(hDC,hRC);
  }
};

#define SHIFTED 0x8000
void set_modifiers() {
  app_ctrl = GetKeyState(VK_CONTROL) & SHIFTED;
  app_shift = GetKeyState(VK_SHIFT) & SHIFTED;
  app_alt = GetKeyState(VK_MENU) & SHIFTED;
}

HWND last_hwnd;
OpenGLWindow *glp;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (last_hwnd != hWnd) {
    glp = 0;
    if (window_map.find(hWnd) != window_map.end()) glp = window_map[hWnd];
  }

  if (!glp) return DefWindowProc(hWnd,uMsg,wParam,lParam);
  //printf("glp->sceen_id : %d || ",glp->screen_id);
  //printf("glp->receive_messages : %d\n",glp->receive_messages);
  last_hwnd = hWnd;
  //printf("umsg: %d\n",uMsg);
  switch (uMsg)
  {
    case WM_ERASEBKGND: return 0;
    case WM_SIZE:
    if (glp) {
      glp->resize((long)(LOWORD(lParam)),(long)(HIWORD(lParam)));		// LoWord=Width, HiWord=Height
    }
    return 0;
    //case WM_CHAR: printf("wparam: %d\n",wParam); return 0;
    case WM_CLOSE: PostQuitMessage(0); return 0;
    case WM_LBUTTONDOWN: if (glp->receive_messages) {set_modifiers(); app_mouse_down(0,LOWORD(lParam),HIWORD(lParam));} return 0;
    case WM_MBUTTONDOWN: if (glp->receive_messages) {set_modifiers(); app_mouse_down(1,LOWORD(lParam),HIWORD(lParam));} return 0;
    case WM_RBUTTONDOWN: if (glp->receive_messages) {set_modifiers(); app_mouse_down(2,LOWORD(lParam),HIWORD(lParam));} return 0;
    case WM_LBUTTONUP: if (glp->receive_messages) {set_modifiers(); app_mouse_up(0,LOWORD(lParam),HIWORD(lParam));} return 0;
    case WM_MBUTTONUP: if (glp->receive_messages) {set_modifiers(); app_mouse_up(1,LOWORD(lParam),HIWORD(lParam));} return 0;
    case WM_RBUTTONUP: if (glp->receive_messages) {set_modifiers(); app_mouse_up(2,LOWORD(lParam),HIWORD(lParam));} return 0;
    case WM_MOUSEMOVE: {
      if (glp->receive_messages) {
        set_modifiers();
        if (wParam & MK_LBUTTON || wParam & MK_MBUTTON || wParam & MK_RBUTTON)
        app_mouse_move(LOWORD(lParam),HIWORD(lParam));
        else
        app_mouse_move_passive(LOWORD(lParam),HIWORD(lParam));
      } return 0;
    }
    case WM_MOUSEWHEEL: if (glp->receive_messages) { set_modifiers();	app_mousewheel((float)((short)HIWORD(wParam))/120.0f,LOWORD(lParam),HIWORD(lParam)); } return 0;
    case WM_CHAR:
      if (glp->receive_messages) {
        set_modifiers();
        app_char((long)wParam);
      } return 0;
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
      if (glp->receive_messages) {
        set_modifiers();
        //app_ctrl = (unsigned long)GetAsyncKeyState(VK_MENU)?true:false;
        //printf("key: a=%d %d\n",'a',wParam);
        //short a = VkKeyScan(wParam);
        app_key_down((long)wParam);
      } return 0;
    case WM_SYSKEYUP:
    case WM_KEYUP: if (glp->receive_messages) {set_modifiers(); app_key_up(wParam); } return 0;
  }
  return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


/*LRESULT CALLBACK WndProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_CLOSE:
    {
      PostQuitMessage(0);
      return 0;
    }
  }
  return DefWindowProc(hWnd,uMsg,wParam,lParam);
}*/

vsx_nw_vector <OpenGLWindow*> gl;

vector<DisplayMode> pmd;
int primary_display = -1;
int num_displays = 0;

void add_window(bool full, vsx_string<>name, int screen, int display = -1) {
  OpenGLWindow* gl_ = new OpenGLWindow;
  //printf("gl_ pointer: %d\n",gl_);
  if (!full)
  gl_->Create((char*)name.c_str(), (char*)name.c_str(), pmd[screen].width/4, pmd[screen].width/4, pmd[screen].width/2, pmd[screen].height/2, pmd[screen].bpp, pmd[screen].refreshRate , full, WndProc, myscreens[screen]);
  else
  gl_->Create((char*)name.c_str(), (char*)name.c_str(), 0, 0, pmd[screen].width, pmd[screen].height, pmd[screen].bpp, pmd[screen].refreshRate , full, WndProc, myscreens[screen]);
  gl_->screen_id = screen;
//  if (gl.size())
//  printf("created gl with classname: %s\n",gl[0]->classname.c_str());
//  printf("created gl with classname: %s\n",(*gl_).classname.c_str());
  if (display == -1)
  gl.push_back(gl_);
  else
  gl[display] = gl_;
}

bool get_fullscreen(int id) { return gl[id]->fullscreen; }

void set_fullscreen(int id, bool mode) {
  if (mode != gl[id]->fullscreen) {
    //string name = gl[id].classname;
    //printf("classname: %s\n",name.c_str());
    gl[id]->Destroy();
    //glpop_back();
    add_window(mode, gl[id]->classname,gl[id]->screen_id,id);
  }
}

bool dual_monitor = false;
// implementation of app externals
int app_argc;
char** app_argv;

int main(int argc, char* argv[])
{
#ifndef VSX_MAIN_SILENT
  printf("Vovoid VSX Ultra");
#endif

  srand ( time(NULL)+rand() );

  app_argc = argc;
  app_argv = argv;

#ifdef VSXU_MULTIMONITOR
  int out_monitor = -1;
  int gui_monitor = -1;
  if (argc == 1)
   printf(" * add \"-help\" for command syntax");
  printf("\n\n");
  bool enumd = false;
  if (argc > 1)
  if (vsx_string<>(argv[1]) == "-help") {
printf("VSXu command line syntax:\n\
 -enum      shows available displays - id's from 0 and up\n\
 -main [id] main display (default: your primary display)\n\
 -out [id]  render output display (default: turned off)\n\
 -dr        disable randomizer\n\n\
\
");

  } else
    if (vsx_string<>(argv[1]) == "-dr")
  {
    disable_randomizer = true;
  } else
  if (vsx_string<>(argv[1]) == "-enum") {
    enumd = true;
  } else {
    for (int i = 1; i < argc; ++i) {
      if (vsx_string<>(argv[i]) == "-main" && i+1 < argc) gui_monitor = atoi(argv[++i]);
      else
      if (vsx_string<>(argv[i]) == "-out" && i+1 < argc) out_monitor = atoi(argv[++i]);
    }
  }


//printf("gui monitor: %d\n",gui_monitor);
//printf("out monitor: %d\n",out_monitor);
//exit(0);
#endif
  myscreens = EnumerateDisplays();
  app_set_fullscreen = &set_fullscreen;
  app_get_fullscreen = &get_fullscreen;
  vsx_string<>enumd_message;
#ifdef VSXU_MULTIMONITOR

  if (enumd) enumd_message = "-----------------------------------------------------------------\n\
[Listing displays]\n\
Please note that by selecting displays that can not render OpenGL\n\
properly, vsxu will crash.\n\
-----------------------------------------------------------------\n\
The following displays are available on this system:\n";
#endif
  for (unsigned long i = 0; i < myscreens.size(); ++i) {
#ifdef VSXU_MULTIMONITOR
    if (enumd) enumd_message += "id "+vsx_string_helper::i2s(i)+"    -     "+myscreens[i].m_DeviceName.c_str()+"\n";
#endif
    vector<DisplayMode> modes = myscreens[i].GetDisplayModes();
    pmd.push_back(get_display_properties(myscreens[i].m_DeviceName));
    if (myscreens[i].m_PrimaryDisplay) primary_display = i;
    //for (unsigned long i = 0; i < modes.size(); ++i) {
//      printf("mode: %d %d   %d bpp   %d Hz\n",modes[i].width,modes[i].height,modes[i].bpp,modes[i].refreshRate);
    //}
  }
#ifdef VSXU_MULTIMONITOR
  if (enumd) {
    enumd_message += "Use ID for the -main and -out parameters.\n\nYour primary display is id: "+vsx_string_helper::i2s(primary_display);
    MessageBox(0, enumd_message.c_str(), "Display enumeration", MB_OK);
    exit(0);
  }

  // -pri 0 -ext 1
  //exit(0);
  //printf("pmd %d %d\n",pmd[primary_display].refreshRate, pmd[primary_display].bpp);
  if (gui_monitor < 0) {
    gui_monitor = primary_display;
    if (out_monitor == gui_monitor) {
      printf("ERROR!!! Your selection of output monitor is conflicting with\nthe gui monitor...\n\
You have 2 options: 1: select another output monitor  2: specify a monitor for the gui");
      exit(0);
    }
    printf("{SYSTEM}: Gui will run on the primary monitor, id %d\n",gui_monitor);
  }
  if (gui_monitor >= (int)myscreens.size() || gui_monitor < 0) {
    printf("ID for GUI monitor is out of bounds!\n");
    exit(0);
  }
  add_window(true, "vsxu editor",gui_monitor);
  //printf("classname drawn: %s\n",gl[0]->classname.c_str());
  if (out_monitor >= 0) {
    if (out_monitor >= (int)myscreens.size()) {
      printf("ID for output monitor is out of bounds!\n");
      exit(0);
    }
    dual_monitor = true;
    add_window(true, "vsxu output",out_monitor);
    printf("{SYSTEM}: Dual-monitor active\n");
    printf("{SYSTEM}: Output will run on monitor id %d\n",out_monitor);
  }

  if (gl.size() > 1)
  gl[1]->receive_messages = false;
#else
  add_window(true, "vsxu output",primary_display);
#endif

  for (unsigned long i = 0; i < gl.size(); ++i) {
    //printf("activate %d\n",i);
    gl[i]->activate();
    app_init(i);
  }
  gl[0]->activate();
  //printf("classname drawn: %s\n",gl[0]->classname.c_str());
//  add_window(2, true, "vsxu output");
  int done = 0;
  MSG msg;
  while(!done)
  {
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(msg.message == WM_QUIT)
      {
        done = true;
      }
      else
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    else
    {
      if (gl.size() > 1)
      gl[0]->activate();

      app_pre_draw();
      unsigned long i;
      //printf("gl.size = %d\n",gl.size());
      for (i = 0; i < gl.size(); i++) {
        #ifdef VSX_DEBUG
        //printf("pre %d\n",(int)i);
        #endif
        if (i != 0) gl[i]->activate();
        done += gl[i]->Run(app_draw,i);
        #ifdef VSX_DEBUG
        //printf("done %d\n",(int)i);
        #endif
      }
      if (i > 0 && gl.size() > 1)
      {
        //printf("after activating %d",i);
        gl[0]->activate();
      }
    }
  }
  return 0;
}
