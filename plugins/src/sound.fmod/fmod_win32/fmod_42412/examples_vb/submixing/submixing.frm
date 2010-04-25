VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Sub-mixing Example"
   ClientHeight    =   5355
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   8580
   LinkTopic       =   "Form1"
   ScaleHeight     =   5355
   ScaleWidth      =   8580
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdLowPass 
      Caption         =   "Lowpass on master group (everything)"
      Height          =   735
      Left            =   360
      TabIndex        =   7
      Top             =   3960
      Width           =   1815
   End
   Begin VB.CommandButton cmdFlange 
      Caption         =   "Flange on group B"
      Height          =   495
      Left            =   2400
      TabIndex        =   6
      Top             =   3360
      Width           =   1815
   End
   Begin VB.CommandButton cmdReverb 
      Caption         =   "Reverb on group A"
      Height          =   495
      Left            =   360
      TabIndex        =   5
      Top             =   3360
      Width           =   1815
   End
   Begin VB.CommandButton cmdMuteB 
      Caption         =   "Mute/UnMute group B"
      Height          =   495
      Left            =   2400
      TabIndex        =   4
      Top             =   2760
      Width           =   1815
   End
   Begin VB.CommandButton cmdMuteA 
      Caption         =   "Mute/UnMute group A"
      Height          =   495
      Left            =   360
      TabIndex        =   3
      Top             =   2760
      Width           =   1815
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   3840
      TabIndex        =   1
      Top             =   4440
      Width           =   975
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   8040
      Top             =   4560
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   2
      Top             =   4980
      Width           =   8580
      _ExtentX        =   15134
      _ExtentY        =   661
      Style           =   1
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
         EndProperty
      EndProperty
   End
   Begin VB.Line Line11 
      X1              =   6240
      X2              =   6840
      Y1              =   2760
      Y2              =   3240
   End
   Begin VB.Line Line10 
      X1              =   6240
      X2              =   6840
      Y1              =   2640
      Y2              =   2640
   End
   Begin VB.Line Line9 
      X1              =   6240
      X2              =   6840
      Y1              =   2520
      Y2              =   2160
   End
   Begin VB.Line Line8 
      X1              =   6360
      X2              =   6840
      Y1              =   1320
      Y2              =   1440
   End
   Begin VB.Line Line7 
      X1              =   6360
      X2              =   6960
      Y1              =   1200
      Y2              =   840
   End
   Begin VB.Line Line6 
      X1              =   5040
      X2              =   5520
      Y1              =   2400
      Y2              =   2640
   End
   Begin VB.Line Line5 
      X1              =   5160
      X2              =   5640
      Y1              =   1560
      Y2              =   1320
   End
   Begin VB.Line Line4 
      X1              =   4080
      X2              =   4560
      Y1              =   2040
      Y2              =   2280
   End
   Begin VB.Line Line3 
      X1              =   4080
      X2              =   4560
      Y1              =   1800
      Y2              =   1680
   End
   Begin VB.Line Line2 
      X1              =   2400
      X2              =   3000
      Y1              =   1920
      Y2              =   1920
   End
   Begin VB.Line Line1 
      X1              =   1200
      X2              =   1560
      Y1              =   1920
      Y2              =   1920
   End
   Begin VB.Label Label12 
      Caption         =   "mastergroup"
      Height          =   255
      Left            =   3120
      TabIndex        =   19
      Top             =   1800
      Width           =   975
   End
   Begin VB.Label Label11 
      Caption         =   "flange"
      Height          =   255
      Left            =   4560
      TabIndex        =   18
      Top             =   2280
      Width           =   975
   End
   Begin VB.Label Label10 
      Caption         =   "reverb"
      Height          =   255
      Left            =   4560
      TabIndex        =   17
      Top             =   1440
      Width           =   975
   End
   Begin VB.Label Label9 
      Caption         =   "group A"
      Height          =   255
      Left            =   5640
      TabIndex        =   16
      Top             =   1080
      Width           =   975
   End
   Begin VB.Label Label8 
      Caption         =   "group B"
      Height          =   255
      Left            =   5640
      TabIndex        =   15
      Top             =   2520
      Width           =   975
   End
   Begin VB.Label Label7 
      Caption         =   "drumloop.wav"
      Height          =   255
      Left            =   6960
      TabIndex        =   14
      Top             =   600
      Width           =   1095
   End
   Begin VB.Label Label6 
      Caption         =   "jaguar.wav"
      Height          =   255
      Left            =   6960
      TabIndex        =   13
      Top             =   1320
      Width           =   975
   End
   Begin VB.Label Label5 
      Caption         =   "c.ogg"
      Height          =   255
      Left            =   6960
      TabIndex        =   12
      Top             =   2040
      Width           =   975
   End
   Begin VB.Label Label4 
      Caption         =   "d.ogg"
      Height          =   255
      Left            =   6960
      TabIndex        =   11
      Top             =   2520
      Width           =   975
   End
   Begin VB.Label Label3 
      Caption         =   "e.ogg"
      Height          =   255
      Left            =   6960
      TabIndex        =   10
      Top             =   3120
      Width           =   975
   End
   Begin VB.Label Label2 
      Caption         =   "lowpass"
      Height          =   255
      Left            =   1680
      TabIndex        =   9
      Top             =   1800
      Width           =   975
   End
   Begin VB.Label Label1 
      Caption         =   "soundcard"
      Height          =   255
      Left            =   240
      TabIndex        =   8
      Top             =   1800
      Width           =   975
   End
   Begin VB.Label copyright 
      Alignment       =   2  'Center
      Caption         =   "Copyright (c) Firelight Technologies 2004-2005"
      Height          =   375
      Left            =   2040
      TabIndex        =   0
      Top             =   120
      Width           =   4455
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'===============================================================================================
' submixing example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to put channels into channel groups, so that you can affect a group
' of channels at a time instead of just one channel at a time.
'===============================================================================================
Option Explicit

Dim system As Long
Dim Sound(0 To 4) As Long
Dim channel(0 To 4) As Long
Dim groupA As Long
Dim groupB As Long
Dim mastergroup As Long
Dim dspreverb As Long
Dim dspflange As Long
Dim dsplowpass As Long
Dim mutea As Long
Dim muteb As Long
Dim reverbon As Long
Dim flangeon As Long
Dim lowpasson As Long


Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim count As Long
    
    mutea = 1
    muteb = 1
    reverbon = 1
    flangeon = 1
    lowpasson = 1
    
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

    result = FMOD_System_CreateSound(system, "../../examples/media/drumloop.wav", FMOD_SOFTWARE Or FMOD_LOOP_NORMAL, Sound(0))
    ERRCHECK (result)
    result = FMOD_System_CreateSound(system, "../../examples/media/jaguar.wav", FMOD_SOFTWARE Or FMOD_LOOP_NORMAL, Sound(1))
    ERRCHECK (result)
    result = FMOD_System_CreateSound(system, "../../examples/media/c.ogg", FMOD_SOFTWARE Or FMOD_LOOP_NORMAL, Sound(2))
    ERRCHECK (result)
    result = FMOD_System_CreateSound(system, "../../examples/media/d.ogg", FMOD_SOFTWARE Or FMOD_LOOP_NORMAL, Sound(3))
    ERRCHECK (result)
    result = FMOD_System_CreateSound(system, "../../examples/media/e.ogg", FMOD_SOFTWARE Or FMOD_LOOP_NORMAL, Sound(4))
    ERRCHECK (result)

    result = FMOD_System_CreateChannelGroup(system, "Group A", groupA)
    ERRCHECK (result)

    result = FMOD_System_CreateChannelGroup(system, "Group B", groupB)
    ERRCHECK (result)
    
    result = FMOD_System_GetMasterChannelGroup(system, mastergroup)
    ERRCHECK (result)

    result = FMOD_ChannelGroup_AddGroup(mastergroup, groupA)
    ERRCHECK (result)

    result = FMOD_ChannelGroup_AddGroup(mastergroup, groupB)
    ERRCHECK (result)

    '
    '   Start all the sounds!
    '
    For count = 0 To 4
        result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, Sound(count), 1, channel(count))
        ERRCHECK (result)
        If count < 2 Then
            result = FMOD_Channel_SetChannelGroup(channel(count), groupA)
        Else
            result = FMOD_Channel_SetChannelGroup(channel(count), groupB)
        End If
        ERRCHECK (result)
        result = FMOD_Channel_SetPaused(channel(count), 0)
        ERRCHECK (result)
    Next count

    '
    '   Create the DSP effects we want to apply to our submixes.
    '
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_REVERB, dspreverb)
    ERRCHECK (result)

    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_FLANGE, dspflange)
    ERRCHECK (result)
    result = FMOD_DSP_SetParameter(dspflange, FMOD_DSP_FLANGE_RATE, 1#)
    ERRCHECK (result)

    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_LOWPASS, dsplowpass)
    ERRCHECK (result)
    result = FMOD_DSP_SetParameter(dsplowpass, FMOD_DSP_LOWPASS_CUTOFF, 500#)
    ERRCHECK (result)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    Dim count As Long
    
    For count = 0 To 4
        If Sound(count) Then
            result = FMOD_Sound_Release(Sound(count))
            ERRCHECK (result)
        End If
    Next count
    
    If dspreverb Then
        result = FMOD_DSP_Release(dspreverb)
        ERRCHECK (result)
    End If
    
    If dspflange Then
        result = FMOD_DSP_Release(dspflange)
        ERRCHECK (result)
    End If
    
    If dsplowpass Then
        result = FMOD_DSP_Release(dsplowpass)
        ERRCHECK (result)
    End If
    
    If groupA Then
        result = FMOD_ChannelGroup_Release(groupA)
        ERRCHECK (result)
    End If
    
    If groupB Then
        result = FMOD_ChannelGroup_Release(groupB)
        ERRCHECK (result)
    End If
    
    If system Then
        result = FMOD_System_Close(system)
        ERRCHECK (result)
        result = FMOD_System_Release(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub cmdFlange_Click()
    Dim result As FMOD_RESULT
    
    If flangeon Then
        result = FMOD_ChannelGroup_AddDSP(groupB, dspflange)
    Else
        result = FMOD_DSP_Remove(dspflange)
    End If
    
    If flangeon Then
        flangeon = 0
    Else
        flangeon = 1
    End If
End Sub

Private Sub cmdLowPass_Click()
    Dim result As FMOD_RESULT
    
    If lowpasson Then
        result = FMOD_ChannelGroup_AddDSP(mastergroup, dsplowpass)
    Else
        result = FMOD_DSP_Remove(dsplowpass)
    End If
    
    If lowpasson Then
        lowpasson = 0
    Else
        lowpasson = 1
    End If
End Sub

Private Sub cmdReverb_Click()
    Dim result As FMOD_RESULT
    
    If reverbon Then
        result = FMOD_ChannelGroup_AddDSP(groupA, dspreverb)
    Else
        result = FMOD_DSP_Remove(dspreverb)
    End If
    
    If reverbon Then
        reverbon = 0
    Else
        reverbon = 1
    End If
End Sub

Private Sub cmdMuteA_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_ChannelGroup_SetMute(groupA, mutea)
    
    If mutea Then
        mutea = 0
    Else
        mutea = 1
    End If
End Sub

Private Sub cmdMuteB_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_ChannelGroup_SetMute(groupB, muteb)
    
    If muteb Then
        muteb = 0
    Else
        muteb = 1
    End If
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim channelsplaying As Long
    
    If system Then
        result = FMOD_System_GetChannelsPlaying(system, channelsplaying)
        result = FMOD_System_Update(system)
    End If

    StatusBar.SimpleText = "Channels Playing " & channelsplaying
    
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
