VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Generate Tone Example"
   ClientHeight    =   8505
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4965
   LinkTopic       =   "Form1"
   ScaleHeight     =   8505
   ScaleWidth      =   4965
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.Slider panSlider 
      Height          =   375
      Left            =   240
      TabIndex        =   14
      Top             =   6960
      Width           =   4335
      _ExtentX        =   7646
      _ExtentY        =   661
      _Version        =   393216
      Min             =   -512
      Max             =   512
   End
   Begin MSComctlLib.Slider frequencySlider 
      Height          =   375
      Left            =   240
      TabIndex        =   13
      Top             =   6120
      Width           =   4335
      _ExtentX        =   7646
      _ExtentY        =   661
      _Version        =   393216
      Max             =   88200
   End
   Begin VB.Frame Pan 
      Caption         =   "Channel Pan"
      Height          =   735
      Left            =   120
      TabIndex        =   12
      Top             =   6720
      Width           =   4575
   End
   Begin VB.Frame Frequency 
      Caption         =   "Channel Frequency"
      Height          =   735
      Left            =   120
      TabIndex        =   11
      Top             =   5880
      Width           =   4575
   End
   Begin VB.Frame Volume 
      Caption         =   "Channel Volume"
      Height          =   735
      Left            =   120
      TabIndex        =   9
      Top             =   5040
      Width           =   4575
      Begin MSComctlLib.Slider volumeSlider 
         Height          =   375
         Left            =   120
         TabIndex        =   10
         Top             =   240
         Width           =   4335
         _ExtentX        =   7646
         _ExtentY        =   661
         _Version        =   393216
         Max             =   1024
      End
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3720
      Top             =   7560
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1920
      TabIndex        =   7
      Top             =   7680
      Width           =   975
   End
   Begin VB.CommandButton cmdStopChannel 
      Caption         =   "Stop Channel"
      Height          =   615
      Left            =   240
      TabIndex        =   6
      Top             =   4320
      Width           =   4455
   End
   Begin VB.CommandButton cmdWhiteNoise 
      Caption         =   "Play a white noise"
      Height          =   615
      Left            =   240
      TabIndex        =   5
      Top             =   3600
      Width           =   4455
   End
   Begin VB.CommandButton cmdSawWave 
      Caption         =   "Play a saw wave"
      Height          =   615
      Left            =   240
      TabIndex        =   4
      Top             =   2880
      Width           =   4455
   End
   Begin VB.CommandButton cmdTriangleWave 
      Caption         =   "Play a triangle wave"
      Height          =   615
      Left            =   240
      TabIndex        =   3
      Top             =   2160
      Width           =   4455
   End
   Begin VB.CommandButton cmdSquareWave 
      Caption         =   "Play a square wave"
      Height          =   615
      Left            =   240
      TabIndex        =   2
      Top             =   1440
      Width           =   4455
   End
   Begin VB.CommandButton cmdSineWave 
      Caption         =   "Play a sine wave"
      Height          =   615
      Left            =   240
      TabIndex        =   1
      Top             =   720
      Width           =   4455
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   8
      Top             =   8130
      Width           =   4965
      _ExtentX        =   8758
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
      Top             =   240
      Width           =   4455
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ===============================================================================================
' GenerateTone example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how simply play generated tones using FMOD_System_PlayDSP instead of
' manually connecting and disconnecting DSP units.
' ===============================================================================================
Option Explicit

Dim system As Long
Dim channel As Long
Dim dsp As Long
Dim channelpan As Single

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

    'Create an oscillator DSP unit for the tone.
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_OSCILLATOR, dsp)
    ERRCHECK (result)
    result = FMOD_DSP_SetParameter(dsp, FMOD_DSP_OSCILLATOR_RATE, 440#)         ' musical note 'A'
    ERRCHECK (result)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    'Shut down
    If dsp Then
        result = FMOD_DSP_Release(dsp)
        ERRCHECK (result)
    End If
   
    If system Then
        result = FMOD_System_Close(system)
        ERRCHECK (result)
        
        result = FMOD_System_Release(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub cmdSineWave_Click()
    Dim result As FMOD_RESULT
    Dim Frequency, Pan As Single
    
    result = FMOD_System_PlayDSP(system, FMOD_CHANNEL_REUSE, dsp, 1, channel)
    ERRCHECK (result)
    result = FMOD_Channel_SetVolume(channel, 0.5)
    ERRCHECK (result)
    result = FMOD_DSP_SetParameter(dsp, FMOD_DSP_OSCILLATOR_TYPE, 0)
    ERRCHECK (result)
    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
    
    result = FMOD_Channel_GetFrequency(channel, Frequency)
    ERRCHECK (result)
    result = FMOD_Channel_GetPan(channel, Pan)
    ERRCHECK (result)
    
    volumeSlider.value = 512
    frequencySlider.value = Frequency
    panSlider.value = Pan * 512
End Sub

Private Sub cmdSquareWave_Click()
    Dim result As FMOD_RESULT
    Dim Frequency, Pan As Single

    result = FMOD_System_PlayDSP(system, FMOD_CHANNEL_REUSE, dsp, 1, channel)
    ERRCHECK (result)
    result = FMOD_Channel_SetVolume(channel, 0.125)
    ERRCHECK (result)
    result = FMOD_DSP_SetParameter(dsp, FMOD_DSP_OSCILLATOR_TYPE, 1)
    ERRCHECK (result)
    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
    
    result = FMOD_Channel_GetFrequency(channel, Frequency)
    ERRCHECK (result)
    result = FMOD_Channel_GetPan(channel, Pan)
    ERRCHECK (result)
    
    volumeSlider.value = 128
    frequencySlider.value = Frequency
    panSlider.value = Pan * 512
End Sub

Private Sub cmdTriangleWave_Click()
    Dim result As FMOD_RESULT
    Dim Frequency, Pan As Single
    
    result = FMOD_System_PlayDSP(system, FMOD_CHANNEL_REUSE, dsp, 1, channel)
    ERRCHECK (result)
    result = FMOD_Channel_SetVolume(channel, 0.5)
    ERRCHECK (result)
    result = FMOD_DSP_SetParameter(dsp, FMOD_DSP_OSCILLATOR_TYPE, 2)
    ERRCHECK (result)
    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
    
    result = FMOD_Channel_GetFrequency(channel, Frequency)
    ERRCHECK (result)
    result = FMOD_Channel_GetPan(channel, Pan)
    ERRCHECK (result)
    
    volumeSlider.value = 512
    frequencySlider.value = Frequency
    panSlider.value = Pan * 512
End Sub

Private Sub cmdSawWave_Click()
    Dim result As FMOD_RESULT
    Dim Frequency, Pan As Single
    
    result = FMOD_System_PlayDSP(system, FMOD_CHANNEL_REUSE, dsp, 1, channel)
    ERRCHECK (result)
    result = FMOD_Channel_SetVolume(channel, 0.125)
    ERRCHECK (result)
    result = FMOD_DSP_SetParameter(dsp, FMOD_DSP_OSCILLATOR_TYPE, 4)
    ERRCHECK (result)
    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
    
    result = FMOD_Channel_GetFrequency(channel, Frequency)
    ERRCHECK (result)
    result = FMOD_Channel_GetPan(channel, Pan)
    ERRCHECK (result)
    
    volumeSlider.value = 128
    frequencySlider.value = Frequency
    panSlider.value = Pan * 512
End Sub

Private Sub cmdWhiteNoise_Click()
    Dim result As FMOD_RESULT
    Dim Frequency, Pan As Single
    
    result = FMOD_System_PlayDSP(system, FMOD_CHANNEL_REUSE, dsp, 1, channel)
    ERRCHECK (result)
    result = FMOD_Channel_SetVolume(channel, 0.25)
    ERRCHECK (result)
    result = FMOD_DSP_SetParameter(dsp, FMOD_DSP_OSCILLATOR_TYPE, 5)
    ERRCHECK (result)
    result = FMOD_Channel_SetPaused(channel, 0)
    ERRCHECK (result)
    
    result = FMOD_Channel_GetFrequency(channel, Frequency)
    ERRCHECK (result)
    result = FMOD_Channel_GetPan(channel, Pan)
    ERRCHECK (result)
    
    volumeSlider.value = 256
    frequencySlider.value = Frequency
    panSlider.value = Pan * 512
End Sub

Private Sub cmdStopChannel_Click()
    Dim result As FMOD_RESULT
    
    If channel Then
        result = FMOD_Channel_Stop(channel)
    End If
End Sub

Private Sub frequencySlider_Click()
    Dim result As FMOD_RESULT
    
    If channel Then
        Dim Frequency As Single
        
        Frequency = frequencySlider.value
        
        result = FMOD_Channel_SetFrequency(channel, Frequency)
        ERRCHECK (result)
    End If
End Sub

Private Sub volumeSlider_Click()
    Dim result As FMOD_RESULT
    
    If channel Then
        Dim Volume As Single
        
        Volume = volumeSlider.value / 1024
        
        result = FMOD_Channel_SetVolume(channel, Volume)
        ERRCHECK (result)
    End If
End Sub

Private Sub panSlider_Click()
    Dim result As FMOD_RESULT
    
    If channel Then
        Dim Pan As Single
        
        Pan = panSlider.value / 512
        
        result = FMOD_Channel_SetPan(channel, Pan)
        ERRCHECK (result)
    End If
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim Frequency As Single
    Dim Volume As Single
    Dim Pan As Single
    Dim playing As Long

    If channel Then
        result = FMOD_Channel_GetPan(channel, channelpan)
        result = FMOD_Channel_GetFrequency(channel, Frequency)
        result = FMOD_Channel_GetVolume(channel, Volume)
        result = FMOD_Channel_IsPlaying(channel, playing)
    End If

    StatusBar.SimpleText = "Channel " & IIf(playing, "playing", "stopped") & " : Frequency " & _
                            Frequency & " Volume " & Volume & " Pan " & channelpan
    
    If system Then
        result = FMOD_System_Update(system)
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
