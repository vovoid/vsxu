VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Netstream Example"
   ClientHeight    =   2595
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4440
   LinkTopic       =   "Form1"
   ScaleHeight     =   2595
   ScaleWidth      =   4440
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdPauseResume 
      Caption         =   "Pause / Resume"
      Height          =   495
      Left            =   2280
      TabIndex        =   5
      Top             =   1200
      Width           =   1815
   End
   Begin VB.CommandButton cmdStart 
      Caption         =   "Start"
      Height          =   495
      Left            =   360
      TabIndex        =   4
      Top             =   1200
      Width           =   1815
   End
   Begin VB.TextBox urlText 
      Height          =   285
      Left            =   240
      TabIndex        =   3
      Top             =   720
      Width           =   3975
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1680
      TabIndex        =   2
      Top             =   1800
      Width           =   975
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3240
      Top             =   1800
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   1
      Top             =   2220
      Width           =   4440
      _ExtentX        =   7832
      _ExtentY        =   661
      Style           =   1
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
         EndProperty
      EndProperty
   End
   Begin VB.Label copyright 
      Alignment       =   2  'Center
      Caption         =   "Copyright (c) Firelight Technologies 2004-2005"
      Height          =   495
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   3855
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'===============================================================================================
' netstream example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to play streaming audio from the internet
'===============================================================================================
Option Explicit

Dim system As Long
Dim sound As Long
Dim channel As Long
Dim soundcreated As Boolean
Dim percent As Long

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    
    soundcreated = False
    
    ' Create a System object and initialize.
    result = FMOD_System_Create(system)
    ERRCHECK (result)
    
    result = FMOD_System_GetVersion(system, version)
    ERRCHECK (result)

    If version <> FMOD_VERSION Then
        MsgBox "Error!  You are using an old version of FMOD " & Hex$(version) & ". " & _
               "This program requires " & Hex$(FMOD_VERSION)
    End If
    
    result = FMOD_System_Init(system, 1, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    ' Bump up the file buffer size a little bit for netstreams (to account for lag).  Decode buffer is left at default.
    result = FMOD_System_SetStreamBufferSize(system, 65536, FMOD_TIMEUNIT_RAWBYTES)
    ERRCHECK (result)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
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

Private Sub cmdStart_Click()
    Dim result As FMOD_RESULT
    
    If Not soundcreated Then
        StatusBar.SimpleText = "Buffering..."
        
        result = FMOD_System_CreateSound(system, urlText.Text, (FMOD_HARDWARE Or FMOD_2D Or FMOD_CREATESTREAM Or FMOD_NONBLOCKING), sound)
        ERRCHECK (result)
        
        soundcreated = True
    End If
End Sub

Private Sub cmdPauseResume_Click()
    Dim result As FMOD_RESULT
    Dim paused As Long
    
    If channel Then
        result = FMOD_Channel_GetPaused(channel, paused)
        ERRCHECK (result)

        If paused Then
            result = FMOD_Channel_SetPaused(channel, 0)
        Else
            result = FMOD_Channel_SetPaused(channel, 1)
        End If
        
        ERRCHECK (result)
    End If
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim starving As Long
    
    If sound Then
        Dim openstate As FMOD_OPENSTATE
        
        result = FMOD_Sound_GetOpenState(sound, openstate, percent, starving)
        ERRCHECK (result)
        
        If (openstate = FMOD_OPENSTATE_READY) And channel = 0 Then
            result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound, 0, channel)
            ERRCHECK (result)
        End If
    End If
    
    If channel Then
        Dim paused As Long
        Dim playing As Long
        Dim ms As Long
        Dim nullstring As String

break:
        result = FMOD_Channel_GetPaused(channel, paused)
        ERRCHECK (result)
        result = FMOD_Channel_IsPlaying(channel, playing)
        ERRCHECK (result)
        result = FMOD_Channel_GetPosition(channel, ms, FMOD_TIMEUNIT_MS)
        ERRCHECK (result)

        StatusBar.SimpleText = "Time " & CInt(ms / 1000 / 60) & ":" & CInt(ms / 1000 Mod 60) & ":" & CInt(ms / 10 Mod 100) & " : " & IIf(paused, "Paused", IIf(playing, "Playing", "Stopped")) & " (" & percent & "%)" & IIf(starving, "STARVING", "    ")
    End If
    
    If system Then
        Call FMOD_System_Update(system)
    End If
End Sub

Private Sub cmdExit_Click()
    Unload Me
    End
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
