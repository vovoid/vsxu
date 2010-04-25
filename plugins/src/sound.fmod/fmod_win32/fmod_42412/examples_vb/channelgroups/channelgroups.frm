VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "ChannelGroups Example"
   ClientHeight    =   3435
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   3600
   LinkTopic       =   "Form1"
   ScaleHeight     =   3435
   ScaleWidth      =   3600
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdMuteMaster 
      Caption         =   "Mute/UnMute group A and B (master group)"
      Height          =   615
      Left            =   600
      TabIndex        =   5
      Top             =   1800
      Width           =   2415
   End
   Begin VB.CommandButton cmdMuteB 
      Caption         =   "Mute/UnMute group B"
      Height          =   495
      Left            =   600
      TabIndex        =   4
      Top             =   1200
      Width           =   2415
   End
   Begin VB.CommandButton cmdMuteA 
      Caption         =   "Mute/UnMute group A"
      Height          =   495
      Left            =   600
      TabIndex        =   3
      Top             =   600
      Width           =   2415
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1320
      TabIndex        =   2
      Top             =   2520
      Width           =   975
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   2640
      Top             =   2520
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   1
      Top             =   3060
      Width           =   3600
      _ExtentX        =   6350
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
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3375
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'===============================================================================================
' channelgroups example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to put channels into channel groups, so that you can affect a group
' of channels at a time instead of just one channel at a time.
'===============================================================================================
Option Explicit

Dim system As Long
Dim Sound(0 To 5) As Long
Dim channel(0 To 5) As Long
Dim groupA As Long
Dim groupB As Long
Dim masterGroup As Long
Dim muteA As Long
Dim muteB As Long
Dim mutemaster As Long

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim count As Long
    
    muteA = 1
    muteB = 1
    mutemaster = 1
    
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
    
    result = FMOD_System_CreateSound(system, "../../examples/media/drumloop.wav", FMOD_LOOP_NORMAL, Sound(0))
    ERRCHECK (result)
    result = FMOD_System_CreateSound(system, "../../examples/media/jaguar.wav", FMOD_LOOP_NORMAL, Sound(1))
    ERRCHECK (result)
    result = FMOD_System_CreateSound(system, "../../examples/media/swish.wav", FMOD_LOOP_NORMAL, Sound(2))
    ERRCHECK (result)
    result = FMOD_System_CreateSound(system, "../../examples/media/c.ogg", FMOD_LOOP_NORMAL, Sound(3))
    ERRCHECK (result)
    result = FMOD_System_CreateSound(system, "../../examples/media/d.ogg", FMOD_LOOP_NORMAL, Sound(4))
    ERRCHECK (result)
    result = FMOD_System_CreateSound(system, "../../examples/media/e.ogg", FMOD_LOOP_NORMAL, Sound(5))
    ERRCHECK (result)

    result = FMOD_System_CreateChannelGroup(system, "Group A", groupA)
    ERRCHECK (result)

    result = FMOD_System_CreateChannelGroup(system, "Group B", groupB)
    ERRCHECK (result)
    
    result = FMOD_System_GetMasterChannelGroup(system, masterGroup)
    ERRCHECK (result)

    '
    '    Instead of being independent, set the group A and B to be children of the master group.
    '
    result = FMOD_ChannelGroup_AddGroup(masterGroup, groupA)
    ERRCHECK (result)

    result = FMOD_ChannelGroup_AddGroup(masterGroup, groupB)
    ERRCHECK (result)

    '
    '   Start all the sounds!
    '
    For count = 0 To 5
        result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, Sound(count), 1, channel(count))
        ERRCHECK (result)
        If count < 3 Then
            result = FMOD_Channel_SetChannelGroup(channel(count), groupA)
        Else
            result = FMOD_Channel_SetChannelGroup(channel(count), groupB)
        End If
        ERRCHECK (result)
        result = FMOD_Channel_SetPaused(channel(count), 0)
        ERRCHECK (result)
    Next count

    '
    '   Change the volume of each group, just because we can!  (And makes it less of a loud noise).
    '
    result = FMOD_ChannelGroup_SetVolume(groupA, 0.5)
    ERRCHECK (result)
    result = FMOD_ChannelGroup_SetVolume(groupB, 0.5)
    ERRCHECK (result)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    Dim count As Long
    
    For count = 0 To 5
        If Sound(count) Then
            result = FMOD_Sound_Release(Sound(count))
            ERRCHECK (result)
        End If
    Next count

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

Private Sub cmdMuteA_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_ChannelGroup_SetMute(groupA, muteA)
    
    If muteA Then
        muteA = 0
    Else
        muteA = 1
    End If
End Sub

Private Sub cmdMuteB_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_ChannelGroup_SetMute(groupB, muteB)
    
    If muteB Then
        muteB = 0
    Else
        muteB = 1
    End If
End Sub

Private Sub cmdMuteMaster_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_ChannelGroup_SetMute(masterGroup, mutemaster)
    
    If mutemaster Then
        mutemaster = 0
    Else
        mutemaster = 1
    End If
End Sub

Private Sub cmdExit_Click()
    Unload Me
    End
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

Private Sub ERRCHECK(result As FMOD_RESULT)
    Dim msgResult As VbMsgBoxResult
    
    If result <> FMOD_OK Then
        msgResult = MsgBox("FMOD error! (" & result & ") " & FMOD_ErrorString(result))
    End If
    
    If msgResult Then
        End
    End If
End Sub
