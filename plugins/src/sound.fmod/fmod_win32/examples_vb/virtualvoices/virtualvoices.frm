VERSION 5.00
Begin VB.Form Form1 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Virtual Voices Example. Copyright (c), Firelight Technologies Pty, Ltd 2004-2005."
   ClientHeight    =   5925
   ClientLeft      =   45
   ClientTop       =   360
   ClientWidth     =   8220
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5925
   ScaleWidth      =   8220
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   6240
      Top             =   4800
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ===============================================================================================
' Virtual Voices Example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows the virtual channels of FMOD. The listener and sounce sources can be moved
' around by clicking and dragging. Sound sources will change colour depending on whether they
' are virtual or not. Red means they are real, blue means they are virtual channels.
' ===============================================================================================
Option Explicit

Private Const NUMCHANNELS As Integer = 50
Private Const NUMREALCHANNELS As Integer = 10
Private Const SOURCESIZE As Integer = 5
Private Const LISTENERSIZE As Integer = 15
Private Const SELECTTHRESHOLD As Integer = 5
Private Const WIDTH_ As Integer = 640
Private Const HEIGHT_ As Integer = 480

Private Type SoundSource
    pos As FMOD_VECTOR
    vel As FMOD_VECTOR
    channel As Long
End Type

Private Type Listener
    listenerpos As FMOD_VECTOR
    up As FMOD_VECTOR
    vel As FMOD_VECTOR
    forward As FMOD_VECTOR
End Type

Dim system As Long
Dim sound As Long

Dim brushBlue As Long
Dim brushRed As Long
Dim brushBlack As Long
Dim brushWhite As Long

Dim selected As Boolean

Dim selectedsource As Long
Dim Source(0 To NUMCHANNELS - 1) As SoundSource
Dim listen As Listener

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim count As Long
    
    selectedsource = -1
    
    ' Set the size of the form
    Me.Width = WIDTH_ * Screen.TwipsPerPixelX
    Me.Height = HEIGHT_ * Screen.TwipsPerPixelY
    
    ' Create the brushes we will be using
    brushBlue = CreateSolidBrush(RGB(0, 0, 255))
    brushRed = CreateSolidBrush(RGB(255, 0, 0))
    brushBlack = CreateSolidBrush(RGB(0, 0, 0))
    brushWhite = CreateSolidBrush(RGB(255, 255, 255))
    
    ' Create a System object and initialize.
    result = FMOD_System_Create(system)
    ERRCHECK (result)
    
    result = FMOD_System_GetVersion(system, version)
    ERRCHECK (result)

    If version <> FMOD_VERSION Then
        MsgBox "Error!  You are using an old version of FMOD " & Hex$(version) & ". " & _
               "This program requires " & Hex$(FMOD_VERSION)
    End If

    result = FMOD_System_SetSoftwareChannels(system, NUMREALCHANNELS)
    ERRCHECK (result)

    result = FMOD_System_Init(system, NUMCHANNELS, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    result = FMOD_System_CreateSound(system, "../../examples/media/drumloop.wav", FMOD_SOFTWARE Or FMOD_3D Or FMOD_LOOP_NORMAL, sound)
    ERRCHECK (result)
    result = FMOD_Sound_Set3DMinMaxDistance(sound, 4, 10000)
    ERRCHECK (result)
    
    Randomize
    
    ' Initialise listener to middle of window
    ListenerInit WIDTH_ / 2, HEIGHT_ / 2
    
    ' Initialise the sound sources
    For count = 0 To NUMCHANNELS - 1
        SoundSourceInit Source(count), Rand(30, WIDTH_ - 30), Rand(30, HEIGHT_ - 50)
    Next
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    ' Delete brushes
    DeleteObject brushRed
    DeleteObject brushBlue
    DeleteObject brushBlack
    
    ' Shutdown
    If sound Then
        result = FMOD_Sound_Release(sound)
        ERRCHECK (result)
    End If
    If system Then
        result = FMOD_System_Close(system)
        ERRCHECK (result)
        result = FMOD_System_Release(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub SoundSourceInit(src As SoundSource, posx As Single, posy As Single)
    Dim result As FMOD_RESULT
    
    src.pos.x = posx
    src.pos.y = posy
    src.pos.z = 0
    src.vel.x = 0
    src.vel.y = 0
    src.vel.z = 0
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound, 1, src.channel)
    ERRCHECK (result)
    
    SoundSourceSetPosition src, posx, posy
    
    result = FMOD_Channel_SetFrequency(src.channel, Rand(11025, 44100))
    ERRCHECK (result)
    
    result = FMOD_Channel_SetPaused(src.channel, 0)
    ERRCHECK (result)
End Sub

Private Sub SoundSourceDraw(src As SoundSource, hdc As Long)
    Dim result As FMOD_RESULT
    Dim isvirtual As Long
    
    result = FMOD_Channel_IsVirtual(src.channel, isvirtual)
    ERRCHECK (result)
    
    If isvirtual Then
        SelectObject hdc, brushBlue
    Else
        SelectObject hdc, brushRed
    End If
    
    Ellipse hdc, src.pos.x - SOURCESIZE, src.pos.y - SOURCESIZE, src.pos.x + SOURCESIZE, src.pos.y + SOURCESIZE
End Sub

Private Sub SoundSourceSetPosition(src As SoundSource, posx As Single, posy As Single)
    Dim result As FMOD_RESULT
    
    src.pos.x = posx
    src.pos.y = posy
    src.pos.z = 0
    
    result = FMOD_Channel_Set3DAttributes(src.channel, src.pos, src.vel)
    ERRCHECK (result)
End Sub

Private Function SoundSourceIsSelected(src As SoundSource, posx As Single, posy As Single) As Boolean
    If posx > src.pos.x - SOURCESIZE - SELECTTHRESHOLD And _
       posx < src.pos.x + SOURCESIZE + SELECTTHRESHOLD And _
       posy > src.pos.y - SOURCESIZE - SELECTTHRESHOLD And _
       posy < src.pos.y + SOURCESIZE + SELECTTHRESHOLD Then
        
        SoundSourceIsSelected = True
    Else
        SoundSourceIsSelected = False
    End If
End Function

Private Sub ListenerInit(posx As Single, posy As Single)
    listen.up.x = 0
    listen.up.y = 0
    listen.up.z = 1
    
    listen.forward.x = 1
    listen.forward.y = 0
    listen.forward.z = 0
    
    listen.vel.x = 0
    listen.vel.y = 0
    listen.vel.z = 0
    
    ListenerSetPosition posx, posy
End Sub

Private Sub ListenerDraw(hdc As Long)
    SelectObject hdc, brushBlack
    
    ' Head
    Ellipse hdc, listen.listenerpos.x - LISTENERSIZE, _
                listen.listenerpos.y - LISTENERSIZE, _
                listen.listenerpos.x + LISTENERSIZE, _
                listen.listenerpos.y + LISTENERSIZE
                
    ' Nose
    Ellipse hdc, listen.listenerpos.x - (LISTENERSIZE / 3), _
                listen.listenerpos.y - (LISTENERSIZE / 3) - LISTENERSIZE, _
                listen.listenerpos.x + (LISTENERSIZE / 3), _
                listen.listenerpos.y + (LISTENERSIZE / 3) - LISTENERSIZE
   
    ' Left ear
    Ellipse hdc, listen.listenerpos.x - (LISTENERSIZE / 3) - LISTENERSIZE, _
                 listen.listenerpos.y - (LISTENERSIZE / 3), _
                 listen.listenerpos.x + (LISTENERSIZE / 3) - LISTENERSIZE, _
                 listen.listenerpos.y + (LISTENERSIZE / 3)

    ' Right ear
    Ellipse hdc, listen.listenerpos.x - (LISTENERSIZE / 3) + LISTENERSIZE, _
                 listen.listenerpos.y - (LISTENERSIZE / 3), _
                 listen.listenerpos.x + (LISTENERSIZE / 3) + LISTENERSIZE, _
                 listen.listenerpos.y + (LISTENERSIZE / 3)
End Sub

Private Sub ListenerSetPosition(posx As Single, posy As Single)
    Dim result As FMOD_RESULT
    
    listen.listenerpos.x = posx
    listen.listenerpos.y = posy
    listen.listenerpos.z = 0
    
    result = FMOD_System_Set3DListenerAttributes(system, 0, listen.listenerpos, listen.vel, listen.forward, listen.up)
    ERRCHECK (result)
End Sub

Private Function ListenerIsSelected(posx As Single, posy As Single) As Boolean
    
    If posx > listen.listenerpos.x - LISTENERSIZE - SELECTTHRESHOLD And _
       posx < listen.listenerpos.x + LISTENERSIZE + SELECTTHRESHOLD And _
       posy > listen.listenerpos.y - LISTENERSIZE - SELECTTHRESHOLD And _
       posy < listen.listenerpos.y + LISTENERSIZE + SELECTTHRESHOLD Then
       
        ListenerIsSelected = True
    Else
        ListenerIsSelected = False
    End If
End Function

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim hdc As Long
    Dim hdcbuffer As Long
    Dim hdcmem As Long
    Dim hbmold As Long
    Dim hbmbuffer As Long
    Dim hbmoldbuffer As Long
    Dim brush As Long
    Dim count As Long
    Dim channels As Long
    Dim text As String
    Dim rectangle As RECT

    hdc = GetDC(Me.hwnd)
    hdcbuffer = CreateCompatibleDC(hdc)
    hbmbuffer = CreateCompatibleBitmap(hdc, WIDTH_, HEIGHT_)
    hbmoldbuffer = SelectObject(hdcbuffer, hbmbuffer)
    hdcmem = CreateCompatibleDC(hdc)
    hbmold = SelectObject(hdcmem, 0)
    
    GetClientRect Me.hwnd, rectangle
    
    FillRect hdcbuffer, rectangle, brushWhite

    ' Draw sound sources
    For count = 0 To NUMCHANNELS - 1
        SoundSourceDraw Source(count), hdcbuffer
    Next
    
    ' Draw the listener
    ListenerDraw hdcbuffer
    
    ' Print some information
    result = FMOD_System_GetChannelsPlaying(system, channels)
    ERRCHECK (result)
    
    text = "Channels Playing: " & channels
    TextOut hdcbuffer, 5, 5, text, Len(text)
    
    text = "Real Channels:    " & NUMREALCHANNELS & " (RED)"
    TextOut hdcbuffer, 5, 22, text, Len(text)
    
    text = "Virtual Channels: " & (NUMCHANNELS - NUMREALCHANNELS) & " (BLUE)"
    TextOut hdcbuffer, 5, 39, text, Len(text)
    
    BitBlt hdc, rectangle.Left, rectangle.Top, WIDTH_, HEIGHT_, hdcbuffer, rectangle.Left, rectangle.Top, vbSrcCopy

    SelectObject hdcmem, hbmold
    DeleteDC hdcmem
    SelectObject hdcbuffer, hbmoldbuffer
    DeleteObject hbmbuffer
    DeleteDC hdcbuffer
    ReleaseDC Me.hwnd, hdc
    
    
    If system Then
        result = FMOD_System_Update(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub Form_MouseDown(Button As Integer, Shift As Integer, x As Single, y As Single)
    Dim count As Long
    
    selectedsource = -1
    
    ' Check if listener was selected
    selected = ListenerIsSelected(x / Screen.TwipsPerPixelX, y / Screen.TwipsPerPixelY)

    ' Check if a sound source was selected if listener wasn't selected
    If Not selected Then
        For count = 0 To NUMCHANNELS - 1
            selected = SoundSourceIsSelected(Source(count), x / Screen.TwipsPerPixelX, y / Screen.TwipsPerPixelY)
            If selected Then
                selectedsource = count
                GoTo break
            End If
        Next
    End If
break:
End Sub

Private Sub Form_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single)
    selected = False
End Sub

Private Sub Form_MouseMove(Button As Integer, Shift As Integer, x As Single, y As Single)
    Dim count As Long
    
    If selected Then
        If selectedsource < 0 Then
            ListenerSetPosition x / Screen.TwipsPerPixelX, y / Screen.TwipsPerPixelY
        Else
            SoundSourceSetPosition Source(selectedsource), x / Screen.TwipsPerPixelX, y / Screen.TwipsPerPixelY
        End If
    End If
End Sub

Private Sub ERRCHECK(result As FMOD_RESULT)
    Dim msgResult As VbMsgBoxResult
    
    If result <> FMOD_OK Then
        msgResult = MsgBox("FMOD error! (" & result & ") " & FMOD_ErrorString(result))
    End If
    
    If msgResult Then
        End
    End If
End Sub

Private Function Rand(ByVal Low As Long, ByVal High As Long) As Long
    Rand = Int((High - Low + 1) * Rnd) + Low
End Function
