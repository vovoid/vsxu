VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Play Sound Example"
   ClientHeight    =   3480
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4395
   LinkTopic       =   "Form1"
   ScaleHeight     =   3480
   ScaleWidth      =   4395
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   5
      Top             =   3105
      Width           =   4395
      _ExtentX        =   7752
      _ExtentY        =   661
      Style           =   1
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
         EndProperty
      EndProperty
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3120
      Top             =   2640
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1680
      TabIndex        =   4
      Top             =   2640
      Width           =   975
   End
   Begin VB.CommandButton cmdStereoHardware 
      Caption         =   "Stereo sound using hardware mixing"
      Height          =   615
      Left            =   240
      TabIndex        =   3
      Top             =   1920
      Width           =   3855
   End
   Begin VB.CommandButton cmdMonoSoftware 
      Caption         =   "Mono sound using software mixing"
      Height          =   615
      Left            =   240
      TabIndex        =   2
      Top             =   1200
      Width           =   3855
   End
   Begin VB.CommandButton cmdMonoHardware 
      Caption         =   "Mono sound using hardware mixing"
      Height          =   615
      Left            =   240
      TabIndex        =   1
      Top             =   480
      Width           =   3855
   End
   Begin VB.Label copyright 
      Alignment       =   2  'Center
      Caption         =   "Copyright (c) Firelight Technologies 2004-2005"
      Height          =   375
      Left            =   360
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
' playsound example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to simply load and play multiple sounds.  This is about the simplest
' use of FMOD.
' This makes FMOD decode the into memory when it loads.  If the sounds are big and possibly take
' up a lot of ram, then it would be better to use the FMOD_CREATESTREAM flag so that it is
' streamed in realtime as it plays.
' ===============================================================================================
Option Explicit

Dim system As Long
Dim sound1 As Long
Dim sound2 As Long
Dim sound3 As Long
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

    result = FMOD_System_Init(system, 32, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    result = FMOD_System_CreateSound(system, "../../examples/media/drumloop.wav", FMOD_HARDWARE, sound1)
    ERRCHECK (result)

    result = FMOD_Sound_SetMode(sound1, FMOD_LOOP_OFF)
    ERRCHECK (result)

    result = FMOD_System_CreateSound(system, "../../examples/media/jaguar.wav", FMOD_SOFTWARE, sound2)
    ERRCHECK (result)

    result = FMOD_System_CreateSound(system, "../../examples/media/swish.wav", FMOD_HARDWARE, sound3)
    ERRCHECK (result)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    ' Shut down
    If sound1 Then
        result = FMOD_Sound_Release(sound1)
        ERRCHECK (result)
    End If
    If sound2 Then
        result = FMOD_Sound_Release(sound2)
        ERRCHECK (result)
    End If
    If sound3 Then
        result = FMOD_Sound_Release(sound3)
        ERRCHECK (result)
    End If
    If system Then
        result = FMOD_System_Close(system)
        ERRCHECK (result)
        
        result = FMOD_System_Release(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub cmdMonoHardware_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, 0, channel)
    ERRCHECK (result)
End Sub

Private Sub cmdMonoSoftware_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound2, 0, channel)
    ERRCHECK (result)
End Sub

Private Sub cmdStereoHardware_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound3, 0, channel)
    ERRCHECK (result)
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim ms As Long
    Dim lenms As Long
    Dim playing As Long
    Dim paused As Long
    Dim channelsplaying As Long
    
    If channel Then
        Dim currentsound As Long

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

        result = FMOD_Channel_GetCurrentSound(channel, currentsound)
        If currentsound Then
            result = FMOD_Sound_GetLength(currentsound, lenms, FMOD_TIMEUNIT_MS)
            If ((result <> FMOD_OK) And (result <> FMOD_ERR_INVALID_HANDLE) And (result <> FMOD_ERR_CHANNEL_STOLEN)) Then
                ERRCHECK (result)
            End If
        End If
    End If

    result = FMOD_System_GetChannelsPlaying(system, channelsplaying)
    ERRCHECK (result)

    StatusBar.SimpleText = "Time " & CInt(ms / 1000 / 60) & ":" & CInt(ms / 1000 Mod 60) & ":" & CInt(ms / 10 Mod 100) & _
                            "/" & CInt(lenms / 1000 / 60) & ":" & CInt(lenms / 1000 Mod 60) & ":" & CInt(lenms / 10 Mod 100) & _
                            " : " & IIf(paused, "Paused", IIf(playing, "Playing", "Stopped")) & " : Channels Playing " & channelsplaying
    
    If system Then
        result = FMOD_System_Update(system)
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
