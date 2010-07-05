#include "vsx_mouse.h"
#include "GL/glfw.h"

#ifdef _WIN32
#include <windows.h>

void vsx_mouse::set_cursor(int id) {
	cursor = id;
}

void vsx_mouse::set_cursor_implement() {
	// reason for no arrow cursor is that windows sets it automatically..
	HCURSOR hCurs1; 
	switch (cursor) {
		case MOUSE_CURSOR_ARROW:
		hCurs1 = LoadCursor(NULL, IDC_ARROW);
		break;
		case MOUSE_CURSOR_HAND:
		hCurs1 = LoadCursor(NULL, IDC_HAND);
		break;
		case MOUSE_CURSOR_IBEAM:
		hCurs1 = LoadCursor(NULL, IDC_IBEAM);
		break;
		case MOUSE_CURSOR_SIZE:
		hCurs1 = LoadCursor(NULL, IDC_SIZEALL);
		break;
		case MOUSE_CURSOR_NS:
		hCurs1 = LoadCursor(NULL, IDC_SIZENS);
		break;
		case MOUSE_CURSOR_WE:
		hCurs1 = LoadCursor(NULL, IDC_SIZEWE);
		break;
		case MOUSE_CURSOR_NESW:
		hCurs1 = LoadCursor(NULL, IDC_SIZENESW);
		break;
		case MOUSE_CURSOR_NWSE:
		hCurs1 = LoadCursor(NULL, IDC_SIZENWSE);
		break;
	}
	SetCursor(hCurs1);	
}


void vsx_mouse::show_cursor()
{
  if (!visible)
  {
    ShowCursor(true);
    visible=true;
  }  
}  
void vsx_mouse::hide_cursor()
{
  if (visible)
  {
    ShowCursor(false);
    visible=false;
  }  
} 
#else
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
void vsx_mouse::set_cursor(int id) {
	cursor = id;
}

void vsx_mouse::set_cursor_implement() {
}

void vsx_mouse::show_cursor() 
{
	//glfwEnable(GLFW_MOUSE_CURSOR);
	//glfwSetMousePos((int)position.x, (int)position.y);
	/*
	if (!dpy) return;
  if ( None != pointer ) 
  {    
    XFreeCursor( dpy, pointer );    
    cursor = None;  
  }  
  XUndefineCursor( dpy, win );  
  XSync( dpy, False );
  */
}

void vsx_mouse::hide_cursor() 
{  
	//glfwDisable(GLFW_MOUSE_CURSOR);
	/*
  if ( None == pointer ) 
  {    
    char bm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };    
    Pixmap pix = XCreateBitmapFromData( dpy, win, bm, 8, 8 );    
    XColor black;    
    memset( &black, 0, sizeof( XColor ) );    
    black.flags = DoRed | DoGreen | DoBlue;    
    pointer = XCreatePixmapCursor( dpy, pix, pix, &black, &black, 0, 0 );    
    XFreePixmap( dpy, pix ); 
  }  
  XDefineCursor( dpy, win, pointer );  
  XSync( dpy, False );
  */
}
#endif //OS stuff


void vsx_mouse::set_cursor_pos(float x, float y)
{
	#ifndef VSX_NO_CLIENT
	position.set(x,y);
	//int xx = glutGet((GLenum)GLUT_WINDOW_X);
	//int yy = glutGet((GLenum)GLUT_WINDOW_Y);
	//RECT window_pos;
	//GetWindowRect(GetForegroundWindow(),&window_pos);
	//SetCursorPos((int)position.x + window_pos.left, (int)position.y + window_pos.top);
	//glutWarpPointer((int)position.x,(int)position.y);
	//printf("set mouse pos: %f, %f\n",x,y);
	glfwSetMousePos((int)(position.x), (int)(position.y));
	#endif
}

vsx_mouse::vsx_mouse()
{
	cursor = 0;
#ifdef _WIN32
	
	visible=true;
#else
#ifndef __APPLE__
	dpy = XOpenDisplay(0);
	cursor = None;
#endif
#endif
}  


//#endif //mouse
