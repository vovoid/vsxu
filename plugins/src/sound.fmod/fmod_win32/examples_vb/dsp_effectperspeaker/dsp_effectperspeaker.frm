VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "DSP Effect per speaker example"
   ClientHeight    =   3135
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4800
   LinkTopic       =   "Form1"
   ScaleHeight     =   3135
   ScaleWidth      =   4800
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdPanRight 
      Caption         =   "Pan Right"
      Height          =   495
      Left            =   2520
      TabIndex        =   6
      Top             =   1440
      Width           =   1935
   End
   Begin VB.CommandButton cmdPanLeft 
      Caption         =   "Pan Left"
      Height          =   495
      Left            =   240
      TabIndex        =   5
      Top             =   1440
      Width           =   2055
   End
   Begin VB.CommandButton cmdChorus 
      Caption         =   "Toggle chorus on right speaker only"
      Height          =   495
      Left            =   2520
      TabIndex        =   4
      Top             =   720
      Width           =   1935
   End
   Begin VB.CommandButton cmdReverb 
      Caption         =   "Toggle reverb on left speaker only"
      Height          =   495
      Left            =   240
      TabIndex        =   3
      Top             =   720
      Width           =   2055
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3480
      Top             =   2280
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1920
      TabIndex        =   1
      Top             =   2280
      Width           =   975
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   2
      Top             =   2760
      Width           =   4800
      _ExtentX        =   8467
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
'===============================================================================================
' DSP Effect per speaker Example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to manipulate a DSP network and as an example, creates 2 dsp effects,
' and splits a single sound into 2 audio paths, which it then filters seperately.
' To only have each audio path come out of one speaker each, DSP::setInputLevels is used just
' before the 2 branches merge back together again.
'
' For more speakers:
' 1. Use System::setSpeakerMode or System::setOutputFormat.
' 2. Create more effects, currently 2 for stereo (reverb and chorus), create one per speaker.
' 3. Under the 'Now connect the 2 effects to channeldsp head.' section, connect the extra effects
'    by duplicating the code more times.
' 4. Filter each effect to each speaker by calling DSP::setInputLevels.  Expand the existing code
'   by extending the level arrays from 2 to the number of speakers you require, and change the
'    numlevels parameter in DSP::setInputLevels from 2 to the correct number accordingly.
'===============================================================================================
Option Explicit

Dim system As Long
Dim Sound As Long
Dim channel As Long
Dim dspreverb As Long
Dim dspchorus As Long
Dim dsphead As Long
Dim dspchannelmixer As Long
Dim choruson As Long
Dim reverbon As Long
Dim dspreverbconnection As Long
Dim dspchorusconnection As Long

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim leftinputon(0 To 1) As Single
    Dim rightinputon(0 To 1) As Single
    Dim inputsoff(0 To 1) As Single
    
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
    
    result = FMOD_System_CreateSound(system, "../../examples/media/drumloop.wav", FMOD_SOFTWARE Or FMOD_LOOP_NORMAL, Sound)
    ERRCHECK (result)

    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, Sound, 0, channel)
    ERRCHECK (result)

    '
    '    Create the DSP effects.
    '
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_REVERB, dspreverb)
    ERRCHECK (result)

    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_CHORUS, dspchorus)
    ERRCHECK (result)

    '
    '    Connect up the DSP network
    '

    '
    '   When a sound is played, a subnetwork is set up in the DSP network which looks like this.
    '   Wavetable is the drumloop sound, and it feeds its data from right to left.
    '
    '   [DSPHEAD]<------------[DSPCHANNELMIXER]
    '
    result = FMOD_System_GetDSPHead(system, dsphead)
    ERRCHECK (result)

    result = FMOD_DSP_GetInput(dsphead, 0, dspchannelmixer, 0)
    ERRCHECK (result)

    '
    '   Now disconnect channeldsp head from wavetable to look like this.
    '
    '   [dsphead] [dspchannelmixer]
    '
    result = FMOD_DSP_DisconnectFrom(dsphead, dspchannelmixer)
    ERRCHECK (result)

    '
    '   Now connect the 2 effects to channeldsp head.
    '
    '             [dspreverb]
    '            /
    '   [dsphead] [dspchannelmixer]
    '            \
    '             [dspchorus]
    '
    result = FMOD_DSP_AddInput(dsphead, dspreverb, dspreverbconnection)
    ERRCHECK (result)
    result = FMOD_DSP_AddInput(dsphead, dspchorus, dspchorusconnection)
    ERRCHECK (result)
    
    '
    '   Now connect the wavetable to the 2 effects
    '
    '                 [dspreverb]
    '            /           \
    '   [dsphead] [dspchannelmixer]
    '            \           /
    '             [dspchorus]
    '
    result = FMOD_DSP_AddInput(dspreverb, dspchannelmixer, 0)
    ERRCHECK (result)
    result = FMOD_DSP_AddInput(dspchorus, dspchannelmixer, 0)
    ERRCHECK (result)

    '
    '   Now the drumloop will be twice as loud, because it is being split into 2, then recombined at the end.
    '   What we really want is to only feed the dspchannelmixer->dspreverb through the left speaker, and
    '   dspchannelmixer->dspchorus to the right speaker.
    '   We can do that simply by setting the pan, or speaker levels of the connections.
    '
    '             [dspreverb]
    '            /1,0        \
    '   [dsphead] [dspchannelmixer]
    '            \0,1        /
    '             [dspchorus]
    '
    leftinputon(0) = 1#
    leftinputon(1) = 0#
    rightinputon(0) = 0#
    rightinputon(1) = 1#
    inputsoff(0) = 0#
    inputsoff(1) = 0#
    
    result = FMOD_DSPConnection_SetLevels(dspreverbconnection, FMOD_SPEAKER_FRONT_LEFT, leftinputon(0), 2)
    ERRCHECK (result)
    result = FMOD_DSPConnection_SetLevels(dspreverbconnection, FMOD_SPEAKER_FRONT_RIGHT, inputsoff(0), 2)
    ERRCHECK (result)

    result = FMOD_DSPConnection_SetLevels(dspchorusconnection, FMOD_SPEAKER_FRONT_LEFT, inputsoff(0), 2)
    ERRCHECK (result)
    result = FMOD_DSPConnection_SetLevels(dspchorusconnection, FMOD_SPEAKER_FRONT_RIGHT, rightinputon(0), 2)
    ERRCHECK (result)
    

    result = FMOD_DSP_SetBypass(dspreverb, 1)
    result = FMOD_DSP_SetBypass(dspchorus, 1)

    result = FMOD_DSP_SetActive(dspreverb, 1)
    result = FMOD_DSP_SetActive(dspchorus, 1)
End Sub


Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    If Sound Then
        result = FMOD_Sound_Release(Sound)
        ERRCHECK (result)
        End If

    If dspreverb Then
        result = FMOD_DSP_Release(dspreverb)
        ERRCHECK (result)
    End If
        
    If dspchorus Then
        result = FMOD_DSP_Release(dspchorus)
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
    Dim channelsplaying As Long
    Dim Pan As Single

    If channel Then
        result = FMOD_Channel_GetPan(channel, Pan)
    End If
    
    If system Then
        result = FMOD_System_GetChannelsPlaying(system, channelsplaying)
        result = FMOD_System_Update(system)
    End If

    StatusBar.SimpleText = "Channels Playing " & channelsplaying & " : Pan = " & Pan
End Sub
    
Private Sub cmdChorus_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_DSP_SetBypass(dspchorus, choruson)
    ERRCHECK (result)
    
    If choruson Then
        choruson = 0
    Else
        choruson = 1
    End If
End Sub

Private Sub cmdReverb_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_DSP_SetBypass(dspreverb, reverbon)
    ERRCHECK (result)
    
    If reverbon Then
        reverbon = 0
    Else
        reverbon = 1
    End If
End Sub

Private Sub cmdPanLeft_Click()
    Dim result As FMOD_RESULT
    Dim Pan As Single
    
    result = FMOD_Channel_GetPan(channel, Pan)
    Pan = Pan - 0.1
    If Pan < -1 Then
        Pan = -1
    End If
    
    result = FMOD_Channel_SetPan(channel, Pan)
End Sub

Private Sub cmdPanRight_Click()
    Dim result As FMOD_RESULT
    Dim Pan As Single
    
    result = FMOD_Channel_GetPan(channel, Pan)
    Pan = Pan + 0.1
    If Pan < -1 Then
        Pan = -1
    End If
    
    result = FMOD_Channel_SetPan(channel, Pan)
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
