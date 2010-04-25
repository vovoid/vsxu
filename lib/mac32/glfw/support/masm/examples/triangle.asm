;=========================================================================
; This is a small test application for GLFW.
; The program opens a window (640x480), and renders a spinning colored
; triangle (it is controlled with both the GLFW timer and the mouse). It
; also calculates the rendering speed (FPS), which is displayed in the
; window title bar.
;
; This program was converted from C to x86 assembler by Toni Jovanoski.
;=========================================================================

.686
.MODEL FLAT, STDCALL
OPTION CASEMAP:NONE


;#########################################################################
;# INCLUDES                                                              #
;#########################################################################

; Standard Windows includes
INCLUDE windows.inc
INCLUDE kernel32.inc
INCLUDE masm32.inc

; OpenGL includes
INCLUDE opengl32.inc
INCLUDE glu32.inc
INCLUDE glfw.inc

; Macros
INCLUDE fpc.mac                       ; floating point constant macro


;#########################################################################
;# INCLUDE LIBS                                                          #
;#########################################################################

; Standard Windows libs
INCLUDELIB kernel32.lib
INCLUDELIB masm32.lib

; OpenGL libs
INCLUDELIB opengl32.lib
INCLUDELIB glu32.lib
INCLUDELIB glfwdll.lib


;#########################################################################
;# INIT DATA                                                             #
;#########################################################################

.DATA


;#########################################################################
;# UNINIT DATA                                                           #
;#########################################################################

.DATA?

ALIGN 8

t               REAL8       ?
t0              REAL8       ?
fps             REAL8       ?
ratio           REAL8       ?

temp            REAL4       ?

windowwidth     DWORD       ?
windowheight    DWORD       ?
cursorx         DWORD       ?
cursory         DWORD       ?
frames          DWORD       ?

windowtitle     BYTE        64 DUP (?)


;#########################################################################
;# CODE                                                                  #
;#########################################################################

.CODE

start:

    push    ebx

    ;We init GLFW
    INVOKE  glfwInit

    ;We open window
    INVOKE  glfwOpenWindow, 640, 480, 0, 0, 0, 0, 0, 0, GLFW_WINDOW

    ;If there is a problem with opening window, get out
    .IF (!eax)
        INVOKE glfwTerminate
        INVOKE ExitProcess, NULL
    .ENDIF

    ;Enabling sticky keys
    INVOKE  glfwEnable, GLFW_STICKY_KEYS

    ;we don't want vsync on
    INVOKE  glfwSwapInterval, 0

    ;in frames we keep how much frames are rendered
    and     frames, 0

    ;get a  first "reference" time
    INVOKE  glfwGetTime
    fstp    t0

    ;repeat until ESC is pressed or window is closed
    .REPEAT


        INVOKE  glfwGetTime
        fst     t                   ;don't pop we gonna use him again

        ;we find how much time had been elapsed
        fsub    t0                  ;st(0)=t-t0
        fld1                        ;1, t-t0

        ;we test if elapsed time is above or below 1 second
        fcomip  st(0), st(1)        ;compare 1 with t-t0 (PPro upward instruction!!!)

        ja  @F

            ;if elapsed time is above 1 second calculate fps
            fild    frames          ;frames, t-t0
            fdivrp  st(1), st(0)    ;frames/t-t0
            fstp    fps             ;calculate fps

            ;convert to string
            INVOKE  FloatToStr, fps, ADDR windowtitle   ;float to string
            INVOKE  glfwSetWindowTitle, ADDR windowtitle

            ;update t0, or t is now new "reference" time and reset frames
            mov     eax, DWORD PTR [t]      ;using integer move instead FPU move
            mov     ecx, DWORD PTR [t+4]
            mov     DWORD PTR [t0], eax
            mov     DWORD PTR [t0+4], ecx
            and     frames, 0               ;reset frames

        @@:

        fstp    st(0)           ;free st(0) because st(0)=t-t0
        inc     frames

        ;we need mouse position for rotating
        INVOKE  glfwGetMousePos, ADDR cursorx, ADDR cursory

        ;we need size of window to setup the viewport and perspective
        INVOKE  glfwGetWindowSize, ADDR windowwidth, ADDR windowheight

        .IF (windowheight==0)
            mov windowheight, 1         ;avoid divide with zero
        .ENDIF

        ;setting viewport
        INVOKE  glViewport, 0, 0, windowwidth, windowheight

        ;clearing color for framebuffer, pure black
        INVOKE  glClearColor, fpc(0), fpc(0), fpc(0), fpc(0)

        ;clear framebuffer
        INVOKE  glClear, GL_COLOR_BUFFER_BIT

        ;setting up perspective, first we "reset" projection matrix and
        ;then we set the perspective
        INVOKE  glMatrixMode, GL_PROJECTION
        INVOKE  glLoadIdentity

        ;calculate ratio (ratio = windowwidth / windowheight)
        fild    windowwidth
        fild    windowheight
        fdivp   st(1), st(0)
        fstp    ratio

        ;FOV=65, znear=1, zfar=100
        INVOKE  gluPerspective, fpc(REAL8 65), ratio, fpc(REAL8 1), fpc(REAL8 100)

        ;we "reset" the modelview matrix and we place the camera
        INVOKE  glMatrixMode, GL_MODELVIEW
        INVOKE  glLoadIdentity

        INVOKE  gluLookAt, fpc(REAL8 0), fpc(REAL8 1), fpc(REAL8 0),\       ;set camera
                           fpc(REAL8 0), fpc(REAL8 20), fpc(REAL8 0),\
                           fpc(REAL8 0), fpc(REAL8 0), fpc(REAL8 1)

        INVOKE  glTranslatef, fpc(0), fpc(14), fpc(0)

        ;calculate the angle of rotation
        fild    cursorx
        fmul    fpc(0.3)        ;0.3*x
        fld     t               ;t, 0.3*x
        fmul    fpc(100)        ;100*t, 0.3*x
        faddp   st(1), st(0)    ;100*t+0.3*x
        fstp    temp

        INVOKE  glRotatef, temp, fpc(0), fpc(0), fpc(1)

        ;draw a triagle
        INVOKE  glBegin, GL_TRIANGLES
            INVOKE  glColor3f, fpc(1), fpc(0), fpc(0)
            INVOKE  glVertex3f, fpc(-5), fpc(0), fpc(-4)
            INVOKE  glColor3f, fpc(0), fpc(1), fpc(0)
            INVOKE  glVertex3f, fpc(5), fpc(0), fpc(-4)
            INVOKE  glColor3f, fpc(0), fpc(0), fpc(1)
            INVOKE  glVertex3f, fpc(0), fpc(0), fpc(6)
        INVOKE  glEnd

        ;swap buffers
        INVOKE  glfwSwapBuffers

        ;get state of ESC key
        INVOKE  glfwGetKey, GLFW_KEY_ESC
        mov     ebx, eax

        ;get state of window (opened or closed)
        INVOKE  glfwGetWindowParam, GLFW_OPENED

    .UNTIL (ebx) || (!eax)      ;if is pressed ESC or window is closed get out

    INVOKE  glfwTerminate

    pop     ebx

    INVOKE  ExitProcess, NULL

END start
