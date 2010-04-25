VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Play Stream Example"
   ClientHeight    =   2310
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4380
   LinkTopic       =   "Form1"
   ScaleHeight     =   2310
   ScaleWidth      =   4380
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdPause 
      Caption         =   "Pause / Resume"
      Height          =   495
      Left            =   2280
      TabIndex        =   4
      Top             =   720
      Width           =   1935
   End
   Begin VB.CommandButton cmdPlay 
      Caption         =   "Play"
      Height          =   495
      Left            =   120
      TabIndex        =   3
      Top             =   720
      Width           =   1935
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3240
      Top             =   1440
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1680
      TabIndex        =   1
      Top             =   1440
      Width           =   975
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   2
      Top             =   1935
      Width           =   4380
      _ExtentX        =   7726
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
      Height          =   375
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
' ===============================================================================================
' playstream example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to simply play a stream, such as an mp3 or wav.
' The stream behaviour is achieved by specifying FMOD_CREATESTREAM in the call to
' FMOD_System_CreateSound.
' This makes FMOD decode the file in realtime as it plays, instead of loading it all at once.
' This uses far less memory, in exchange for a small runtime cpu hit.
' ===============================================================================================
Option Explicit

Dim system As Long
Dim sound As Long
Dim channel As Long

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    
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
    
    result = FMOD_System_CreateStream(system, "../../examples/media/wave.mp3", (FMOD_2D Or FMOD_HARDWARE), sound)
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

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim ms As Long
    Dim lenms As Long
    Dim playing As Long
    Dim paused As Long
    
    If channel Then
        result = FMOD_Channel_IsPlaying(channel, playing)
        If ((result <> FMOD_OK) And (result <> FMOD_ERR_INVALID_HANDLE) And (result <> FMOD_ERR_CHANNEL_STOLEN)) Then
            ERRCHECK (result)
        End If
    
        result = FMOD_Channel_GetPaused(channel, paused)
        If ((result <> FMOD_OK) And (result <> FMOD_ERR_INVALID_HANDLE) And (result <> FMOD_ERR_CHANNEL_STOLEN)) Then
            ERRCHECK (result)
        End If
    
        result = FMOD_Channel_GetPosition(channel, ms, FMOD_TIMEUNIT_MS)
        If ((result <> FMOD_OK) And (result <> FMOD_ERR_INVALID_HANDLE) And (result <> FMOD_ERR_CHANNEL_STOLEN)) Then
            ERRCHECK (result)
        End If
    
        result = FMOD_Sound_GetLength(sound, lenms, FMOD_TIMEUNIT_MS)
        If ((result <> FMOD_OK) And (result <> FMOD_ERR_INVALID_HANDLE) And (result <> FMOD_ERR_CHANNEL_STOLEN)) Then
            ERRCHECK (result)
        End If
    End If
    
    StatusBar.SimpleText = "Time " & CInt(ms / 1000 / 60) & ":" & CInt(ms / 1000 Mod 60) & ":" & CInt(ms / 10 Mod 100) & _
                               "/" & CInt(lenms / 1000 / 60) & ":" & CInt(lenms / 1000 Mod 60) & ":" & CInt(lenms / 10 Mod 100)
    
    If system Then
        result = FMOD_System_Update(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub cmdPlay_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound, 0, channel)
    ERRCHECK (result)
End Sub

Private Sub cmdPause_Click()
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
