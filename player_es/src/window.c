/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

/* 
 * window.c
 * Platform-specific window setup.
 */

#include "window.h"

#ifdef _WIN32
/* 
 * create_window(): Set up Windows specific bits.
 *
 * uiWidth:	 Width of window to create.
 * uiHeight:	Height of window to create.
 *
 * Returns:	 Device specific window handle.
 */
HWND create_window(int uiWidth, int uiHeight) {
	WNDCLASS wc;
	RECT wRect;
	HWND sWindow;
	HINSTANCE hInstance;

	wRect.left = 0L;
	wRect.right = (long)uiWidth;
	wRect.top = 0L;
	wRect.bottom = (long)uiHeight;

	hInstance = GetModuleHandle(NULL);

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)process_window;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "OGLES";

	RegisterClass(&wc);

	AdjustWindowRectEx(&wRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

	sWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, "OGLES", "main", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, uiWidth, uiHeight, NULL, NULL, hInstance, NULL);

	ShowWindow(sWindow, SW_SHOW);
	SetForegroundWindow(sWindow);
	SetFocus(sWindow);

	return sWindow;
}

/*
 * process_window(): This function handles Windows callbacks.
 */
LRESULT CALLBACK process_window(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
	switch(uiMsg) {
		case WM_CLOSE:
				PostQuitMessage(0);
				return 0;

		case WM_ACTIVATE:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SIZE:
				return 0;
	}

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

#else

/* Waits for map notification */
Bool wait_for_map(Display *d, XEvent *e, char *win_ptr) {
	return (e->type == MapNotify && e->xmap.window == (*((Window*)win_ptr)));
}

/* Creates an X window */
Window create_window(const char *title, int width, int height, Display* display, 
	EGLDisplay sEGLDisplay, EGLConfig FBConfig, Colormap *pColormap, 
	XVisualInfo **ppVisual) 
{
    XSetWindowAttributes wa;
    XSizeHints sh;
    XEvent e;
    XVisualInfo *visual, template;

    unsigned long mask;
    long screen;

    Colormap colormap;

    int vID, n;

    Window window;

    screen = DefaultScreen(display);
    eglGetConfigAttrib(sEGLDisplay, FBConfig, EGL_NATIVE_VISUAL_ID, &vID);
    template.visualid = vID;
    visual = XGetVisualInfo(display, VisualIDMask, &template, &n);

	if (!visual) {
		printf("Couldn't get X visual info\n");
		exit(-1);
	}

    colormap = XCreateColormap(display, RootWindow(display, screen), visual->visual, AllocNone);

    wa.colormap = colormap;
    wa.background_pixel = 0xFFFFFFFF;
    wa.border_pixel = 0;
    wa.event_mask = StructureNotifyMask | ExposureMask;

    mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

    window = XCreateWindow(display, RootWindow(display, screen), 0, 0, width, height, 
		0, visual->depth, InputOutput, visual->visual, mask, &wa);

    sh.flags = USPosition;
    sh.x = 10;
    sh.y = 10;

    XSetStandardProperties(display, window, title, title, None, 0, 0, &sh);
    XMapWindow(display, window);
    XIfEvent(display, &e, wait_for_map, (char*)&window);
    XSetWMColormapWindows(display, window, &window, 1);
    XFlush(display);

    *pColormap = colormap;
    *ppVisual = visual;

    return window;
}

#endif /* _WIN32 */

