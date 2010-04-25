VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Multiple Speaker Output Example"
   ClientHeight    =   5985
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4275
   LinkTopic       =   "Form1"
   ScaleHeight     =   5985
   ScaleWidth      =   4275
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdRightStereo 
      Caption         =   "Play right part of a stereo sound on the CENTER speaker"
      Height          =   495
      Left            =   240
      TabIndex        =   12
      Top             =   4560
      Width           =   3855
   End
   Begin VB.CommandButton cmdStereoSwapped 
      Caption         =   "Play stereo sound on front speakers but with channels swapped"
      Height          =   495
      Left            =   240
      TabIndex        =   11
      Top             =   3960
      Width           =   3855
   End
   Begin VB.CommandButton cmdStereoFront 
      Caption         =   "Play stereo sound on front speakers"
      Height          =   495
      Left            =   240
      TabIndex        =   10
      Top             =   3360
      Width           =   3855
   End
   Begin VB.CommandButton cmdRearRight 
      Caption         =   "REAR RIGHT"
      Height          =   855
      Left            =   2880
      TabIndex        =   9
      Top             =   2400
      Width           =   1215
   End
   Begin VB.CommandButton cmdRearLeft 
      Caption         =   "REAR LEFT"
      Height          =   855
      Left            =   240
      TabIndex        =   8
      Top             =   2400
      Width           =   1215
   End
   Begin VB.CommandButton cmdSideRight 
      Caption         =   "SIDE RIGHT"
      Height          =   855
      Left            =   2880
      TabIndex        =   7
      Top             =   1440
      Width           =   1215
   End
   Begin VB.CommandButton cmdSideLeft 
      Caption         =   "SIDE LEFT"
      Height          =   855
      Left            =   240
      TabIndex        =   6
      Top             =   1440
      Width           =   1215
   End
   Begin VB.CommandButton cmdFrontRight 
      Caption         =   "FRONT RIGHT"
      Height          =   855
      Left            =   2880
      TabIndex        =   5
      Top             =   480
      Width           =   1215
   End
   Begin VB.CommandButton cmdCenter 
      Caption         =   "CENTER"
      Height          =   855
      Left            =   1560
      TabIndex        =   4
      Top             =   480
      Width           =   1215
   End
   Begin VB.CommandButton cmdFrontLeft 
      Caption         =   "FRONT LEFT"
      Height          =   855
      Left            =   240
      TabIndex        =   3
      Top             =   480
      Width           =   1215
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1680
      TabIndex        =   2
      Top             =   5160
      Width           =   975
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3120
      Top             =   5160
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   1
      Top             =   5610
      Width           =   4275
      _ExtentX        =   7541
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
' Multi Speaker Output Example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to play sounds in multiple speakers, and also how to even assign
' sound subchannels, such as those in a stereo sound to different individual speakers.
' ===============================================================================================
Option Explicit

Dim system As Long
Dim sound1 As Long
Dim sound2 As Long
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

    
    ' Choose the highest mode possible.
    ' If the user doesn't have 7.1 speakers, then only the speakers they have will be audible.
    result = FMOD_System_SetSpeakerMode(system, FMOD_SPEAKERMODE_7POINT1)
    ERRCHECK (result)
    
    result = FMOD_System_Init(system, 32, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    result = FMOD_System_CreateSound(system, "../../examples/media/drumloop.wav", (FMOD_SOFTWARE Or FMOD_2D), sound1)
    ERRCHECK (result)
    result = FMOD_Sound_SetMode(sound1, FMOD_LOOP_OFF)
    ERRCHECK (result)

    result = FMOD_System_CreateSound(system, "../../examples/media/stereo.ogg", (FMOD_SOFTWARE Or FMOD_2D), sound2)
    ERRCHECK (result)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    ' Shutdown
    If sound1 Then
        result = FMOD_Sound_Release(sound1)
        ERRCHECK (result)
    End If
    If sound2 Then
        result = FMOD_Sound_Release(sound2)
        ERRCHECK (result)
    End If
    If system Then
        result = FMOD_System_Close(system)
        ERRCHECK (result)
        
        result = FMOD_System_Release(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub cmdFrontLeft_Click()
    Dim result As FMOD_RESULT

    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, 1, channel)
    ERRCHECK (result)

    result = FMOD_Channel_SetSpeakerMix(channel, 1, 0, 0, 0, 0, 0, 0, 0)
    ERRCHECK (result)

    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub cmdFrontRight_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, 1, channel)
    ERRCHECK (result)

    result = FMOD_Channel_SetSpeakerMix(channel, 0, 1, 0, 0, 0, 0, 0, 0)
    ERRCHECK (result)

    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub cmdCenter_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, 1, channel)
    ERRCHECK (result)

    result = FMOD_Channel_SetSpeakerMix(channel, 0, 0, 1, 0, 0, 0, 0, 0)
    ERRCHECK (result)

    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub cmdRearLeft_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, 1, channel)
    ERRCHECK (result)

    result = FMOD_Channel_SetSpeakerMix(channel, 0, 0, 0, 0, 1, 0, 0, 0)
    ERRCHECK (result)

    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub cmdRearRight_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, 1, channel)
    ERRCHECK (result)

    result = FMOD_Channel_SetSpeakerMix(channel, 0, 0, 0, 0, 0, 1, 0, 0)
    ERRCHECK (result)

    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub cmdSideLeft_Click()
    Dim result As FMOD_RESULT

    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, 1, channel)
    ERRCHECK (result)

    result = FMOD_Channel_SetSpeakerMix(channel, 0, 0, 0, 0, 0, 0, 1, 0)
    ERRCHECK (result)

    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub cmdSideRight_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, 1, channel)
    ERRCHECK (result)

    result = FMOD_Channel_SetSpeakerMix(channel, 0, 0, 0, 0, 0, 0, 0, 1)
    ERRCHECK (result)

    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub cmdStereoFront_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound2, 1, channel)
    ERRCHECK (result)

    
    ' By default a stereo sound would play in all right and all left speakers, so this forces it to just the front.
    result = FMOD_Channel_SetSpeakerMix(channel, 1, 1, 0, 0, 0, 0, 0, 0)
    ERRCHECK (result)

    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub cmdStereoSwapped_Click()
    Dim result As FMOD_RESULT
    Dim levels(0 To 1) As Single
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound2, 1, channel)
    ERRCHECK (result)

    ' Clear out all speakers first.
    result = FMOD_Channel_SetSpeakerMix(channel, 0, 0, 0, 0, 0, 0, 0, 0)
    ERRCHECK (result)

    
    ' Put the left channel of the sound in the right speaker.
    levels(0) = 0    ' This array represents the source stereo sound.  l/r
    levels(1) = 1

    result = FMOD_Channel_SetSpeakerLevels(channel, FMOD_SPEAKER_FRONT_LEFT, levels(0), 2)
    ERRCHECK (result)
    
    ' Put the right channel of the sound in the left speaker.
    levels(0) = 1024 ' This array represents the source stereo sound. l/r
    levels(1) = 0

    result = FMOD_Channel_SetSpeakerLevels(channel, FMOD_SPEAKER_FRONT_RIGHT, levels(0), 2)
    ERRCHECK (result)

    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub cmdRightStereo_Click()
    Dim result As FMOD_RESULT
    Dim levels(0 To 1) As Single
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound2, 1, channel)
    ERRCHECK (result)
    
    ' Clear out all speakers first.
    result = FMOD_Channel_SetSpeakerMix(channel, 0, 0, 0, 0, 0, 0, 0, 0)
    ERRCHECK (result)

    ' Put the right channel of the sound in the center speaker.
    levels(0) = 0    ' This array represents the source stereo sound.  l/r
    levels(1) = 1
    
    result = FMOD_Channel_SetSpeakerLevels(channel, FMOD_SPEAKER_FRONT_CENTER, levels(0), 2)
    ERRCHECK (result)
        
    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim ms As Long
    Dim lenms As Long
    Dim playing As Long
    Dim paused As Long
    Dim channelsplaying As Long
    Dim currentsound As Long
    
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
       
        result = FMOD_Channel_GetCurrentSound(channel, currentsound)
        If currentsound Then
            result = FMOD_Sound_GetLength(currentsound, lenms, FMOD_TIMEUNIT_MS)
            If ((result <> FMOD_OK) And (result <> FMOD_ERR_INVALID_HANDLE) And (result <> FMOD_ERR_CHANNEL_STOLEN)) Then
                ERRCHECK (result)
            End If
        End If
        
        result = FMOD_System_GetChannelsPlaying(system, channelsplaying)
        ERRCHECK (result)
    End If
    
    StatusBar.SimpleText = "Time " & CInt(ms / 1000 / 60) & ":" & CInt(ms / 1000 Mod 60) & ":" & (ms / 10 Mod 100) & _
                           "/" & CInt(lenms / 1000 / 60) & ":" & CInt(lenms / 1000 Mod 60) & ":" & CInt(lenms / 10 Mod 100) & _
                           IIf(paused, " Paused ", IIf(playing, " Playing ", " Stopped ")) & " Channels Playing " & channelsplaying
    
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
