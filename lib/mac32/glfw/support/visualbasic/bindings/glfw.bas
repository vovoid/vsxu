Attribute VB_Name = "GLFW"
'========================================================================
' GLFW - An OpenGL framework
' File:        glfw.bas
' Platform:    Visual Basic/Windows
' API version: 2.5
' WWW:         http://glfw.sourceforge.net
'------------------------------------------------------------------------
' Copyright (c) 2002-2005 Camilla Berglund
'
' This software is provided 'as-is', without any express or implied
' warranty. In no event will the authors be held liable for any damages
' arising from the use of this software.
'
' Permission is granted to anyone to use this software for any purpose,
' including commercial applications, and to alter it and redistribute it
' freely, subject to the following restrictions:
'
' 1. The origin of this software must not be misrepresented; you must not
'    claim that you wrote the original software. If you use this software
'    in a product, an acknowledgment in the product documentation would
'    be appreciated but is not required.
'
' 2. Altered source versions must be plainly marked as such, and must not
'    be misrepresented as being the original software.
'
' 3. This notice may not be removed or altered from any source
'    distribution.
'
'========================================================================

'========================================================================
' GLFW version
'========================================================================

Public Const GLFW_VERSION_MAJOR = 2
Public Const GLFW_VERSION_MINOR = 5
Public Const GLFW_VERSION_REVISION = 0


'========================================================================
' Input handling definitions
'========================================================================

' Key and button state/action definitions
Public Const GLFW_RELEASE = 0
Public Const GLFW_PRESS = 1

' Keyboard key definitions: 8-bit ISO-8859-1 (Latin 1) encoding is used
' for printable keys (such as A-Z, 0-9 etc), and values above 256
' represent special (non-printable) keys (e.g. F1, Page Up etc).
Public Const GLFW_KEY_UNKNOWN = -1
Public Const GLFW_KEY_SPACE = 32
Public Const GLFW_KEY_SPECIAL = 256
Public Const GLFW_KEY_ESC = (GLFW_KEY_SPECIAL + 1)
Public Const GLFW_KEY_F1 = (GLFW_KEY_SPECIAL + 2)
Public Const GLFW_KEY_F2 = (GLFW_KEY_SPECIAL + 3)
Public Const GLFW_KEY_F3 = (GLFW_KEY_SPECIAL + 4)
Public Const GLFW_KEY_F4 = (GLFW_KEY_SPECIAL + 5)
Public Const GLFW_KEY_F5 = (GLFW_KEY_SPECIAL + 6)
Public Const GLFW_KEY_F6 = (GLFW_KEY_SPECIAL + 7)
Public Const GLFW_KEY_F7 = (GLFW_KEY_SPECIAL + 8)
Public Const GLFW_KEY_F8 = (GLFW_KEY_SPECIAL + 9)
Public Const GLFW_KEY_F9 = (GLFW_KEY_SPECIAL + 10)
Public Const GLFW_KEY_F10 = (GLFW_KEY_SPECIAL + 11)
Public Const GLFW_KEY_F11 = (GLFW_KEY_SPECIAL + 12)
Public Const GLFW_KEY_F12 = (GLFW_KEY_SPECIAL + 13)
Public Const GLFW_KEY_F13 = (GLFW_KEY_SPECIAL + 14)
Public Const GLFW_KEY_F14 = (GLFW_KEY_SPECIAL + 15)
Public Const GLFW_KEY_F15 = (GLFW_KEY_SPECIAL + 16)
Public Const GLFW_KEY_F16 = (GLFW_KEY_SPECIAL + 17)
Public Const GLFW_KEY_F17 = (GLFW_KEY_SPECIAL + 18)
Public Const GLFW_KEY_F18 = (GLFW_KEY_SPECIAL + 19)
Public Const GLFW_KEY_F19 = (GLFW_KEY_SPECIAL + 20)
Public Const GLFW_KEY_F20 = (GLFW_KEY_SPECIAL + 21)
Public Const GLFW_KEY_F21 = (GLFW_KEY_SPECIAL + 22)
Public Const GLFW_KEY_F22 = (GLFW_KEY_SPECIAL + 23)
Public Const GLFW_KEY_F23 = (GLFW_KEY_SPECIAL + 24)
Public Const GLFW_KEY_F24 = (GLFW_KEY_SPECIAL + 25)
Public Const GLFW_KEY_F25 = (GLFW_KEY_SPECIAL + 26)
Public Const GLFW_KEY_UP = (GLFW_KEY_SPECIAL + 27)
Public Const GLFW_KEY_DOWN = (GLFW_KEY_SPECIAL + 28)
Public Const GLFW_KEY_LEFT = (GLFW_KEY_SPECIAL + 29)
Public Const GLFW_KEY_RIGHT = (GLFW_KEY_SPECIAL + 30)
Public Const GLFW_KEY_LSHIFT = (GLFW_KEY_SPECIAL + 31)
Public Const GLFW_KEY_RSHIFT = (GLFW_KEY_SPECIAL + 32)
Public Const GLFW_KEY_LCTRL = (GLFW_KEY_SPECIAL + 33)
Public Const GLFW_KEY_RCTRL = (GLFW_KEY_SPECIAL + 34)
Public Const GLFW_KEY_LALT = (GLFW_KEY_SPECIAL + 35)
Public Const GLFW_KEY_RALT = (GLFW_KEY_SPECIAL + 36)
Public Const GLFW_KEY_TAB = (GLFW_KEY_SPECIAL + 37)
Public Const GLFW_KEY_ENTER = (GLFW_KEY_SPECIAL + 38)
Public Const GLFW_KEY_BACKSPACE = (GLFW_KEY_SPECIAL + 39)
Public Const GLFW_KEY_INSERT = (GLFW_KEY_SPECIAL + 40)
Public Const GLFW_KEY_DEL = (GLFW_KEY_SPECIAL + 41)
Public Const GLFW_KEY_PAGEUP = (GLFW_KEY_SPECIAL + 42)
Public Const GLFW_KEY_PAGEDOWN = (GLFW_KEY_SPECIAL + 43)
Public Const GLFW_KEY_HOME = (GLFW_KEY_SPECIAL + 44)
Public Const GLFW_KEY_END = (GLFW_KEY_SPECIAL + 45)
Public Const GLFW_KEY_KP_0 = (GLFW_KEY_SPECIAL + 46)
Public Const GLFW_KEY_KP_1 = (GLFW_KEY_SPECIAL + 47)
Public Const GLFW_KEY_KP_2 = (GLFW_KEY_SPECIAL + 48)
Public Const GLFW_KEY_KP_3 = (GLFW_KEY_SPECIAL + 49)
Public Const GLFW_KEY_KP_4 = (GLFW_KEY_SPECIAL + 50)
Public Const GLFW_KEY_KP_5 = (GLFW_KEY_SPECIAL + 51)
Public Const GLFW_KEY_KP_6 = (GLFW_KEY_SPECIAL + 52)
Public Const GLFW_KEY_KP_7 = (GLFW_KEY_SPECIAL + 53)
Public Const GLFW_KEY_KP_8 = (GLFW_KEY_SPECIAL + 54)
Public Const GLFW_KEY_KP_9 = (GLFW_KEY_SPECIAL + 55)
Public Const GLFW_KEY_KP_DIVIDE = (GLFW_KEY_SPECIAL + 56)
Public Const GLFW_KEY_KP_MULTIPLY = (GLFW_KEY_SPECIAL + 57)
Public Const GLFW_KEY_KP_SUBTRACT = (GLFW_KEY_SPECIAL + 58)
Public Const GLFW_KEY_KP_ADD = (GLFW_KEY_SPECIAL + 59)
Public Const GLFW_KEY_KP_DECIMAL = (GLFW_KEY_SPECIAL + 60)
Public Const GLFW_KEY_KP_EQUAL = (GLFW_KEY_SPECIAL + 61)
Public Const GLFW_KEY_KP_ENTER = (GLFW_KEY_SPECIAL + 62)
Public Const GLFW_KEY_LAST = GLFW_KEY_KP_ENTER

' Mouse button definitions
Public Const GLFW_MOUSE_BUTTON_1 = 0
Public Const GLFW_MOUSE_BUTTON_2 = 1
Public Const GLFW_MOUSE_BUTTON_3 = 2
Public Const GLFW_MOUSE_BUTTON_4 = 3
Public Const GLFW_MOUSE_BUTTON_5 = 4
Public Const GLFW_MOUSE_BUTTON_6 = 5
Public Const GLFW_MOUSE_BUTTON_7 = 6
Public Const GLFW_MOUSE_BUTTON_8 = 7
Public Const GLFW_MOUSE_BUTTON_LAST = GLFW_MOUSE_BUTTON_8

' Mouse button aliases
Public Const GLFW_MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_1
Public Const GLFW_MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_2
Public Const GLFW_MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_3

' Joystick identifiers
Public Const GLFW_JOYSTICK_1 = 0
Public Const GLFW_JOYSTICK_2 = 1
Public Const GLFW_JOYSTICK_3 = 2
Public Const GLFW_JOYSTICK_4 = 3
Public Const GLFW_JOYSTICK_5 = 4
Public Const GLFW_JOYSTICK_6 = 5
Public Const GLFW_JOYSTICK_7 = 6
Public Const GLFW_JOYSTICK_8 = 7
Public Const GLFW_JOYSTICK_9 = 8
Public Const GLFW_JOYSTICK_10 = 9
Public Const GLFW_JOYSTICK_11 = 10
Public Const GLFW_JOYSTICK_12 = 11
Public Const GLFW_JOYSTICK_13 = 12
Public Const GLFW_JOYSTICK_14 = 13
Public Const GLFW_JOYSTICK_15 = 14
Public Const GLFW_JOYSTICK_16 = 15
Public Const GLFW_JOYSTICK_LAST = GLFW_JOYSTICK_16


'========================================================================
' Other definitions
'========================================================================

' glfwOpenWindow modes
Public Const GLFW_WINDOW = &H10001
Public Const GLFW_FULLSCREEN = &H10002

' glfwGetWindowParam tokens
Public Const GLFW_OPENED = &H20001
Public Const GLFW_ACTIVE = &H20002
Public Const GLFW_ICONIFIED = &H20003
Public Const GLFW_ACCELERATED = &H20004
Public Const GLFW_RED_BITS = &H20005
Public Const GLFW_GREEN_BITS = &H20006
Public Const GLFW_BLUE_BITS = &H20007
Public Const GLFW_ALPHA_BITS = &H20008
Public Const GLFW_DEPTH_BITS = &H20009
Public Const GLFW_STENCIL_BITS = &H2000A

' The following constants are used for both glfwGetWindowParam
' and glfwOpenWindowHint
Public Const GLFW_REFRESH_RATE = &H2000B
Public Const GLFW_ACCUM_RED_BITS = &H2000C
Public Const GLFW_ACCUM_GREEN_BITS = &H2000D
Public Const GLFW_ACCUM_BLUE_BITS = &H2000E
Public Const GLFW_ACCUM_ALPHA_BITS = &H2000F
Public Const GLFW_AUX_BUFFERS = &H20010
Public Const GLFW_STEREO = &H20011

' glfwEnable/glfwDisable tokens
Public Const GLFW_MOUSE_CURSOR = &H30001
Public Const GLFW_STICKY_KEYS = &H30002
Public Const GLFW_STICKY_MOUSE_BUTTONS = &H30003
Public Const GLFW_SYSTEM_KEYS = &H30004
Public Const GLFW_KEY_REPEAT = &H30005
Public Const GLFW_AUTO_POLL_EVENTS = &H30006

' glfwWaitThread wait modes
Public Const GLFW_WAIT = &H40001
Public Const GLFW_NOWAIT = &H40002

' glfwGetJoystickParam tokens
Public Const GLFW_PRESENT = &H50001
Public Const GLFW_AXES = &H50002
Public Const GLFW_BUTTONS = &H50003

' glfwReadImage/glfwLoadTexture2D flags
Public Const GLFW_NO_RESCALE_BIT = &H1
Public Const GLFW_ORIGIN_UL_BIT = &H2
Public Const GLFW_BUILD_MIPMAPS_BIT = &H4
Public Const GLFW_ALPHA_MAP_BIT = &H8

' Time spans longer than this (seconds) are considered to be infinity
Public Const GLFW_INFINITY = 100000#


'========================================================================
' Typedefs
'========================================================================

' The video mode structure used by glfwGetVideoModes()
Public Type GLFWvidmode
    width     As Long
    height    As Long
    redbits   As Long
    greenbits As Long
    bluebits  As Long
End Type


' Image/texture information
Public Type GLFWimage
    width         As Long
    height        As Long
    format        As Long
    BytesPerPixel As Long
    data()        As Byte
End Type



'========================================================================
' Prototypes
'========================================================================

' GLFW initialization, termination and version querying
Public Declare Function glfwInit Lib "glfw.dll" () As Long
Public Declare Sub glfwTerminate Lib "glfw.dll" ()
Public Declare Sub glfwGetVersion Lib "glfw.dll" (major As Long, minor As Long, rev As Long)

' Window handling
Public Declare Function glfwOpenWindow Lib "glfw.dll" (ByVal width As Long, ByVal height As Long, ByVal redbits As Long, ByVal greenbits As Long, ByVal bluebits As Long, ByVal alphabits As Long, ByVal depthbits As Long, ByVal stencilbits As Long, ByVal mode As Long) As Long
Public Declare Sub glfwOpenWindowHint Lib "glfw.dll" (ByVal target As Long, ByVal hint As Long)
Public Declare Sub glfwCloseWindow Lib "glfw.dll" ()
Public Declare Sub glfwSetWindowTitle Lib "glfw.dll" (ByVal title As String)
Public Declare Sub glfwGetWindowSize Lib "glfw.dll" (width As Long, height As Long)
Public Declare Sub glfwSetWindowSize Lib "glfw.dll" (ByVal width As Long, ByVal height As Long)
Public Declare Sub glfwSetWindowPos Lib "glfw.dll" (ByVal x As Long, ByVal y As Long)
Public Declare Sub glfwIconifyWindow Lib "glfw.dll" ()
Public Declare Sub glfwUnIconifyWindow Lib "glfw.dll" ()
Public Declare Sub glfwSwapBuffers Lib "glfw.dll" ()
Public Declare Sub glfwSwapInterval Lib "glfw.dll" (ByVal interval As Long)
Public Declare Function glfwGetWindowParam Lib "glfw.dll" (ByVal param As Long) As Long
Public Declare Sub glfwSetWindowSizeCallback Lib "glfw.dll" (ByVal cbfun As Long)
Public Declare Sub glfwSetWindowCloseCallback Lib "glfw.dll" (ByVal cbfun As Long)
Public Declare Sub glfwSetWindowRefreshCallback Lib "glfw.dll" (ByVal cbfun As Long)

' Video mode functions
Public Declare Function glfwGetVideoModes Lib "glfw.dll" (list() As GLFWvidmode, ByVal maxcount As Long) As Long
Public Declare Sub glfwGetDesktopMode Lib "glfw.dll" (mode As GLFWvidmode)

' Input handling
Public Declare Sub glfwPollEvents Lib "glfw.dll" ()
Public Declare Sub glfwWaitEvents Lib "glfw.dll" ()
Public Declare Function glfwGetKey Lib "glfw.dll" (ByVal key As Long) As Long
Public Declare Function glfwGetMouseButton Lib "glfw.dll" (ByVal button As Long) As Long
Public Declare Sub glfwGetMousePos Lib "glfw.dll" (xpos As Long, ypos As Long)
Public Declare Sub glfwSetMousePos Lib "glfw.dll" (ByVal xpos As Long, ByVal ypos As Long)
Public Declare Function glfwGetMouseWheel Lib "glfw.dll" () As Long
Public Declare Sub glfwSetMouseWheel Lib "glfw.dll" (ByVal pos As Long)
Public Declare Sub glfwSetKeyCallback Lib "glfw.dll" (ByVal cbfun As Long)
Public Declare Sub glfwSetCharCallback Lib "glfw.dll" (ByVal cbfun As Long)
Public Declare Sub glfwSetMouseButtonCallback Lib "glfw.dll" (ByVal cbfun As Long)
Public Declare Sub glfwSetMousePosCallback Lib "glfw.dll" (ByVal cbfun As Long)
Public Declare Sub glfwSetMouseWheelCallback Lib "glfw.dll" (ByVal cbfun As Long)

' Joystick input
Public Declare Function glfwGetJoystickParam Lib "glfw.dll" (ByVal joy As Long, ByVal param As Long) As Long
Public Declare Function glfwGetJoystickPos Lib "glfw.dll" (ByVal joy As Long, pos As Single, ByVal numaxes As Long) As Long
Public Declare Function glfwGetJoystickButtons Lib "glfw.dll" (ByVal joy As Long, buttons As Byte, ByVal numbuttons As Long) As Long

' Time
Public Declare Function glfwGetTime Lib "glfw.dll" () As Double
Public Declare Sub glfwSetTime Lib "glfw.dll" (ByVal time As Double)
Public Declare Sub glfwSleep Lib "glfw.dll" (ByVal time As Double)

' Extension support
Public Declare Function glfwExtensionSupported Lib "glfw.dll" (ByVal extension As String) As Long
Public Declare Function glfwGetProcAddress Lib "glfw.dll" (ByVal procname As String) As Long
Public Declare Sub glfwGetGLVersion Lib "glfw.dll" (major As Long, minor As Long, rev As Long)

' Threading support
Public Declare Function glfwCreateThread Lib "glfw.dll" (ByVal cbfun As Long, ByVal arg As Long) As Long
Public Declare Sub glfwDestroyThread Lib "glfw.dll" (ByVal id As Long)
Public Declare Function glfwWaitThread Lib "glfw.dll" (ByVal id As Long, ByVal waitmode As Long) As Long
Public Declare Function glfwGetThreadID Lib "glfw.dll" () As Long
Public Declare Function glfwCreateMutex Lib "glfw.dll" () As Long
Public Declare Sub glfwDestroyMutex Lib "glfw.dll" (ByVal mutex As Long)
Public Declare Sub glfwLockMutex Lib "glfw.dll" (ByVal mutex As Long)
Public Declare Sub glfwUnlockMutex Lib "glfw.dll" (ByVal mutex As Long)
Public Declare Function glfwCreateCond Lib "glfw.dll" () As Long
Public Declare Sub glfwDestroyCond Lib "glfw.dll" (ByVal cond As Long)
Public Declare Sub glfwWaitCond Lib "glfw.dll" (ByVal cond As Long, ByVal mutex As Long, ByVal timeout As Double)
Public Declare Sub glfwSignalCond Lib "glfw.dll" (ByVal cond As Long)
Public Declare Sub glfwBroadcastCond Lib "glfw.dll" (ByVal cond As Long)
Public Declare Function glfwGetNumberOfProcessors Lib "glfw.dll" () As Long

' Enable/disable functions
Public Declare Sub glfwEnable Lib "glfw.dll" (ByVal token As Long)
Public Declare Sub glfwDisable Lib "glfw.dll" (ByVal token As Long)

' Image/texture I/O support
Public Declare Function glfwReadImage Lib "glfw.dll" (ByVal name As String, img As GLFWimage, ByVal flags As Long) As Long
Public Declare Sub glfwFreeImage Lib "glfw.dll" (img As GLFWimage)
Public Declare Function glfwLoadTexture2D Lib "glfw.dll" (ByVal name As String, ByVal flags As Long) As Long
