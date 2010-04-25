;========================================================================
; GLFW - An OpenGL framework
; File:        glfw.inc
; Platform:    MASM/Windows
; API version: 2.5
; WWW:         http://glfw.sourceforge.net
;------------------------------------------------------------------------
; Copyright (c) 2002-2005 Camilla Berglund
;
; This software is provided 'as-is', without any express or implied
; warranty. In no event will the authors be held liable for any damages
; arising from the use of this software.
;
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
;
; 1. The origin of this software must not be misrepresented; you must not
;    claim that you wrote the original software. If you use this software
;    in a product, an acknowledgment in the product documentation would
;    be appreciated but is not required.
;
; 2. Altered source versions must be plainly marked as such, and must not
;    be misrepresented as being the original software.
;
; 3. This notice may not be removed or altered from any source
;    distribution.
;
;========================================================================

;#########################################################################
;# GLFW VERSION                                                          #
;#########################################################################

GLFW_VERSION_MAJOR    equ 2
GLFW_VERSION_MINOR    equ 5
GLFW_VERSION_REVISION equ 0


;#########################################################################
;# INPUT HANDLING EQUATES                                                #
;#########################################################################

; Key and button state/action definitions
GLFW_RELEASE          equ     0
GLFW_PRESS            equ     1

; Keyboard key definitions: 8-bit ISO-8859-1 (Latin 1) encoding is used
; for printable keys (such as A-Z, 0-9 etc), and values above 256
; represent special (non-printable) keys (e.g. F1, Page Up etc).
GLFW_KEY_UNKNOWN      equ   -1
GLFW_KEY_SPACE        equ   32
GLFW_KEY_SPECIAL      equ   256
GLFW_KEY_ESC          equ   (GLFW_KEY_SPECIAL+1)
GLFW_KEY_F1           equ   (GLFW_KEY_SPECIAL+2)
GLFW_KEY_F2           equ   (GLFW_KEY_SPECIAL+3)
GLFW_KEY_F3           equ   (GLFW_KEY_SPECIAL+4)
GLFW_KEY_F4           equ   (GLFW_KEY_SPECIAL+5)
GLFW_KEY_F5           equ   (GLFW_KEY_SPECIAL+6)
GLFW_KEY_F6           equ   (GLFW_KEY_SPECIAL+7)
GLFW_KEY_F7           equ   (GLFW_KEY_SPECIAL+8)
GLFW_KEY_F8           equ   (GLFW_KEY_SPECIAL+9)
GLFW_KEY_F9           equ   (GLFW_KEY_SPECIAL+10)
GLFW_KEY_F10          equ   (GLFW_KEY_SPECIAL+11)
GLFW_KEY_F11          equ   (GLFW_KEY_SPECIAL+12)
GLFW_KEY_F12          equ   (GLFW_KEY_SPECIAL+13)
GLFW_KEY_F13          equ   (GLFW_KEY_SPECIAL+14)
GLFW_KEY_F14          equ   (GLFW_KEY_SPECIAL+15)
GLFW_KEY_F15          equ   (GLFW_KEY_SPECIAL+16)
GLFW_KEY_F16          equ   (GLFW_KEY_SPECIAL+17)
GLFW_KEY_F17          equ   (GLFW_KEY_SPECIAL+18)
GLFW_KEY_F18          equ   (GLFW_KEY_SPECIAL+19)
GLFW_KEY_F19          equ   (GLFW_KEY_SPECIAL+20)
GLFW_KEY_F20          equ   (GLFW_KEY_SPECIAL+21)
GLFW_KEY_F21          equ   (GLFW_KEY_SPECIAL+22)
GLFW_KEY_F22          equ   (GLFW_KEY_SPECIAL+23)
GLFW_KEY_F23          equ   (GLFW_KEY_SPECIAL+24)
GLFW_KEY_F24          equ   (GLFW_KEY_SPECIAL+25)
GLFW_KEY_F25          equ   (GLFW_KEY_SPECIAL+26)
GLFW_KEY_UP           equ   (GLFW_KEY_SPECIAL+27)
GLFW_KEY_DOWN         equ   (GLFW_KEY_SPECIAL+28)
GLFW_KEY_LEFT         equ   (GLFW_KEY_SPECIAL+29)
GLFW_KEY_RIGHT        equ   (GLFW_KEY_SPECIAL+30)
GLFW_KEY_LSHIFT       equ   (GLFW_KEY_SPECIAL+31)
GLFW_KEY_RSHIFT       equ   (GLFW_KEY_SPECIAL+32)
GLFW_KEY_LCTRL        equ   (GLFW_KEY_SPECIAL+33)
GLFW_KEY_RCTRL        equ   (GLFW_KEY_SPECIAL+34)
GLFW_KEY_LALT         equ   (GLFW_KEY_SPECIAL+35)
GLFW_KEY_RALT         equ   (GLFW_KEY_SPECIAL+36)
GLFW_KEY_TAB          equ   (GLFW_KEY_SPECIAL+37)
GLFW_KEY_ENTER        equ   (GLFW_KEY_SPECIAL+38)
GLFW_KEY_BACKSPACE    equ   (GLFW_KEY_SPECIAL+39)
GLFW_KEY_INSERT       equ   (GLFW_KEY_SPECIAL+40)
GLFW_KEY_DEL          equ   (GLFW_KEY_SPECIAL+41)
GLFW_KEY_PAGEUP       equ   (GLFW_KEY_SPECIAL+42)
GLFW_KEY_PAGEDOWN     equ   (GLFW_KEY_SPECIAL+43)
GLFW_KEY_HOME         equ   (GLFW_KEY_SPECIAL+44)
GLFW_KEY_END          equ   (GLFW_KEY_SPECIAL+45)
GLFW_KEY_KP_0         equ   (GLFW_KEY_SPECIAL+46)
GLFW_KEY_KP_1         equ   (GLFW_KEY_SPECIAL+47)
GLFW_KEY_KP_2         equ   (GLFW_KEY_SPECIAL+48)
GLFW_KEY_KP_3         equ   (GLFW_KEY_SPECIAL+49)
GLFW_KEY_KP_4         equ   (GLFW_KEY_SPECIAL+50)
GLFW_KEY_KP_5         equ   (GLFW_KEY_SPECIAL+51)
GLFW_KEY_KP_6         equ   (GLFW_KEY_SPECIAL+52)
GLFW_KEY_KP_7         equ   (GLFW_KEY_SPECIAL+53)
GLFW_KEY_KP_8         equ   (GLFW_KEY_SPECIAL+54)
GLFW_KEY_KP_9         equ   (GLFW_KEY_SPECIAL+55)
GLFW_KEY_KP_DIVIDE    equ   (GLFW_KEY_SPECIAL+56)
GLFW_KEY_KP_MULTIPLY  equ   (GLFW_KEY_SPECIAL+57)
GLFW_KEY_KP_SUBTRACT  equ   (GLFW_KEY_SPECIAL+58)
GLFW_KEY_KP_ADD       equ   (GLFW_KEY_SPECIAL+59)
GLFW_KEY_KP_DECIMAL   equ   (GLFW_KEY_SPECIAL+60)
GLFW_KEY_KP_EQUAL     equ   (GLFW_KEY_SPECIAL+61)
GLFW_KEY_KP_ENTER     equ   (GLFW_KEY_SPECIAL+62)
GLFW_KEY_LAST         equ   GLFW_KEY_KP_ENTER

; Mouse button definitions
GLFW_MOUSE_BUTTON_1      equ  0
GLFW_MOUSE_BUTTON_2      equ  1
GLFW_MOUSE_BUTTON_3      equ  2
GLFW_MOUSE_BUTTON_4      equ  3
GLFW_MOUSE_BUTTON_5      equ  4
GLFW_MOUSE_BUTTON_6      equ  5
GLFW_MOUSE_BUTTON_7      equ  6
GLFW_MOUSE_BUTTON_8      equ  7
GLFW_MOUSE_BUTTON_LAST   equ  GLFW_MOUSE_BUTTON_8

; Mouse button aliases
GLFW_MOUSE_BUTTON_LEFT   equ  GLFW_MOUSE_BUTTON_1
GLFW_MOUSE_BUTTON_RIGHT  equ  GLFW_MOUSE_BUTTON_2
GLFW_MOUSE_BUTTON_MIDDLE equ  GLFW_MOUSE_BUTTON_3

; Joystick identifiers
GLFW_JOYSTICK_1           equ   0
GLFW_JOYSTICK_2           equ   1
GLFW_JOYSTICK_3           equ   2
GLFW_JOYSTICK_4           equ   3
GLFW_JOYSTICK_5           equ   4
GLFW_JOYSTICK_6           equ   5
GLFW_JOYSTICK_7           equ   6
GLFW_JOYSTICK_8           equ   7
GLFW_JOYSTICK_9           equ   8
GLFW_JOYSTICK_10          equ   9
GLFW_JOYSTICK_11          equ   10
GLFW_JOYSTICK_12          equ   11
GLFW_JOYSTICK_13          equ   12
GLFW_JOYSTICK_14          equ   13
GLFW_JOYSTICK_15          equ   14
GLFW_JOYSTICK_16          equ   15
GLFW_JOYSTICK_LAST        equ   GLFW_JOYSTICK_16


;#########################################################################
;# OTHER EQUATES                                                         #
;#########################################################################

; glfwOpenWindow modes
GLFW_WINDOW               equ   00010001h
GLFW_FULLSCREEN           equ   00010002h

; glfwGetWindowParam tokens
GLFW_OPENED               equ   00020001h
GLFW_ACTIVE               equ   00020002h
GLFW_ICONIFIED            equ   00020003h
GLFW_ACCELERATED          equ   00020004h
GLFW_RED_BITS             equ   00020005h
GLFW_GREEN_BITS           equ   00020006h
GLFW_BLUE_BITS            equ   00020007h
GLFW_ALPHA_BITS           equ   00020008h
GLFW_DEPTH_BITS           equ   00020009h
GLFW_STENCIL_BITS         equ   0002000Ah

; The following constants are used for both glfwGetWindowParam
; and glfwOpenWindowHint
GLFW_REFRESH_RATE         equ   0002000Bh
GLFW_ACCUM_RED_BITS       equ   0002000Ch
GLFW_ACCUM_GREEN_BITS     equ   0002000Dh
GLFW_ACCUM_BLUE_BITS      equ   0002000Eh
GLFW_ACCUM_ALPHA_BITS     equ   0002000Fh
GLFW_AUX_BUFFERS          equ   00020010h
GLFW_STEREO               equ   00020011h

; glfwEnable/glfwDisable tokens
GLFW_MOUSE_CURSOR         equ   00030001h
GLFW_STICKY_KEYS          equ   00030002h
GLFW_STICKY_MOUSE_BUTTONS equ   00030003h
GLFW_SYSTEM_KEYS          equ   00030004h
GLFW_KEY_REPEAT           equ   00030005h
GLFW_AUTO_POLL_EVENTS     equ   00030006h

; glfwWaitThread wait modes
GLFW_WAIT                 equ   00040001h
GLFW_NOWAIT               equ   00040002h

; glfwGetJoystickParam tokens
GLFW_PRESENT              equ   00050001h
GLFW_AXES                 equ   00050002h
GLFW_BUTTONS              equ   00050003h

; glfwReadImage/glfwLoadTexture2D flags
GLFW_NO_RESCALE_BIT       equ   00000001h ; Only for glfwReadImage
GLFW_ORIGIN_UL_BIT        equ   00000002h
GLFW_BUILD_MIPMAPS_BIT    equ   00000004h ; Only for glfwLoadTexture2D
GLFW_ALPHA_MAP_BIT        equ   00000008h

; Time spans longer than this (seconds) are considered to be infinity
GLFW_INFINITY             equ   100000.0


;#########################################################################
;# TYPEDEFS                                                              #
;#########################################################################

; The video mode structure used by glfwGetVideoModes()
GLFWvidmode STRUCT
    _Width    DWORD   ?
    Height    DWORD   ?
    RedBits   DWORD   ?
    GreenBits DWORD   ?
    BlueBits  DWORD   ?
GLFWvidmode ENDS

; Image/texture information
GLFWimage STRUCT
    _Width        DWORD   ?
    Height        DWORD   ?
    Format        DWORD   ?
    BytesPerPixel DWORD   ?
    Data          DWORD   ?
GLFWimage ENDS

; Thread ID
GLFWThread TYPEDEF DWORD

; Mutex object
GLFWMutex TYPEDEF DWORD

; Condition variable object
GLFWCond TYPEDEF DWORD


;#########################################################################
;# PROTOTYPES                                                            #
;#########################################################################

; GLFW initialization, termination and version querying
glfwInit                     PROTO   STDCALL
glfwTerminate                PROTO   STDCALL
glfwGetVersion               PROTO   STDCALL :DWORD, :DWORD, :DWORD

; Window handling
glfwOpenWindow               PROTO   STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
glfwOpenWindowHint           PROTO   STDCALL :DWORD, :DWORD
glfwCloseWindow              PROTO   STDCALL
glfwSetWindowTitle           PROTO   STDCALL :DWORD
glfwGetWindowSize            PROTO   STDCALL :DWORD, :DWORD
glfwSetWindowSize            PROTO   STDCALL :DWORD, :DWORD
glfwSetWindowPos             PROTO   STDCALL :DWORD, :DWORD
glfwIconifyWindow            PROTO   STDCALL
glfwRestoreWindow            PROTO   STDCALL
glfwSwapBuffers              PROTO   STDCALL
glfwSwapInterval             PROTO   STDCALL :DWORD
glfwGetWindowParam           PROTO   STDCALL :DWORD
glfwSetWindowSizeCallback    PROTO   STDCALL :DWORD
glfwSetWindowCloseCallback   PROTO   STDCALL :DWORD
glfwSetWindowRefreshCallback PROTO   STDCALL :DWORD

; Video mode functions
glfwGetVideoModes            PROTO   STDCALL :DWORD, :DWORD
glfwGetDesktopMode           PROTO   STDCALL :DWORD

; Input handling
glfwPollEvents               PROTO   STDCALL
glfwWaitEvents               PROTO   STDCALL
glfwGetKey                   PROTO   STDCALL :DWORD
glfwGetMouseButton           PROTO   STDCALL :DWORD
glfwGetMousePos              PROTO   STDCALL :DWORD, :DWORD
glfwSetMousePos              PROTO   STDCALL :DWORD, :DWORD
glfwGetMouseWheel            PROTO   STDCALL
glfwSetMouseWheel            PROTO   STDCALL :DWORD
glfwSetKeyCallback           PROTO   STDCALL :DWORD
glfwSetCharCallback          PROTO   STDCALL :DWORD
glfwSetMouseButtonCallback   PROTO   STDCALL :DWORD
glfwSetMousePosCallback      PROTO   STDCALL :DWORD
glfwSetMouseWheelCallback    PROTO   STDCALL :DWORD

; Joystick input
glfwGetJoystickParam         PROTO  STDCALL :DWORD, :DWORD
glfwGetJoystickPos           PROTO  STDCALL :DWORD, :DWORD, :DWORD
glfwGetJoystickButtons       PROTO  STDCALL :DWORD, :DWORD, :DWORD

; Time
glfwGetTime                  PROTO  STDCALL
glfwSetTime                  PROTO  STDCALL :QWORD
glfwSleep                    PROTO  STDCALL :QWORD

; Extension support
glfwExtensionSupported       PROTO  STDCALL :DWORD
glfwGetProcAddress           PROTO  STDCALL :DWORD
glfwGetGLVersion             PROTO  STDCALL :DWORD, :DWORD, :DWORD

; Threading support
glfwCreateThread             PROTO  STDCALL :DWORD, :DWORD
glfwDestroyThread            PROTO  STDCALL :DWORD
glfwWaitThread               PROTO  STDCALL :DWORD, :DWORD
glfwGetThreadID              PROTO  STDCALL
glfwCreateMutex              PROTO  STDCALL
glfwDestroyMutex             PROTO  STDCALL :DWORD
glfwLockMutex                PROTO  STDCALL :DWORD
glfwUnlockMutex              PROTO  STDCALL :DWORD
glfwCreateCond               PROTO  STDCALL
glfwDestroyCond              PROTO  STDCALL :DWORD
glfwWaitCond                 PROTO  STDCALL :DWORD, :DWORD, :QWORD
glfwSignalCond               PROTO  STDCALL :DWORD
glfwBroadcastCond            PROTO  STDCALL :DWORD
glfwGetNumberOfProcessors    PROTO  STDCALL

; Enable/disable functions
glfwEnable                   PROTO  STDCALL :DWORD
glfwDisable                  PROTO  STDCALL :DWORD

; Image/texture I/O support
glfwReadImage                PROTO  STDCALL :DWORD, :DWORD, :DWORD
glfwFreeImage                PROTO  STDCALL :DWORD
glfwLoadTexture2D            PROTO  STDCALL :DWORD, :DWORD
