VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Multiple Sound Cards Example"
   ClientHeight    =   2550
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4440
   LinkTopic       =   "Form1"
   ScaleHeight     =   2550
   ScaleWidth      =   4440
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdPlayB 
      Caption         =   "Play"
      Height          =   375
      Left            =   3240
      TabIndex        =   6
      Top             =   1200
      Width           =   855
   End
   Begin VB.CommandButton cmdPlayA 
      Caption         =   "Play"
      Height          =   375
      Left            =   3240
      TabIndex        =   5
      Top             =   720
      Width           =   855
   End
   Begin VB.ComboBox ComboB 
      Height          =   315
      Left            =   360
      TabIndex        =   4
      Text            =   "Select Sound Card"
      Top             =   1200
      Width           =   2655
   End
   Begin VB.ComboBox ComboA 
      Height          =   315
      Left            =   360
      TabIndex        =   3
      Text            =   "Select Sound Card"
      Top             =   720
      Width           =   2655
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3360
      Top             =   1680
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1800
      TabIndex        =   1
      Top             =   1680
      Width           =   975
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   2
      Top             =   2175
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
' ===============================================================================================
' multiplesoundcard example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to simply load and play multiple sounds.  This is about the simplest
' use of FMOD.
' This makes FMOD decode the into memory when it loads.  If the sounds are big and possibly take
' up a lot of ram, then it would be better to use the FMOD_CREATESTREAM flag so that it is
' streamed in realtime as it plays.
' ===============================================================================================
Option Explicit

Dim systemA As Long
Dim systemB As Long
Dim soundA As Long
Dim soundB As Long
Dim channelA As Long
Dim channelB As Long

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim Numdrivers As Long
    Dim count As Long
    Dim drivernamearray(0 To 255) As Byte
    Dim drivername As String
    Dim guid As FMOD_GUID
    
    cmdPlayA.Enabled = False
    cmdPlayB.Enabled = False
    
    ' Create Sound Card A
    result = FMOD_System_Create(systemA)
    ERRCHECK (result)
    
    result = FMOD_System_GetVersion(systemA, version)
    ERRCHECK (result)

    If version <> FMOD_VERSION Then
        MsgBox "Error!  You are using an old version of FMOD " & Hex$(version) & ". " & _
               "This program requires " & Hex$(FMOD_VERSION)
    End If
    
    result = FMOD_System_GetNumDrivers(systemA, Numdrivers)
    ERRCHECK (result)
    
    For count = 0 To Numdrivers - 1
        result = FMOD_System_GetDriverInfo(systemA, count, drivernamearray(0), 256, guid)
        ERRCHECK (result)
     
        drivername = StrConv(drivernamearray, vbUnicode)
        
        ComboA.AddItem drivername
    Next
    
    ' Create Sound Card B
    result = FMOD_System_Create(systemB)
    ERRCHECK (result)
    
    result = FMOD_System_GetVersion(systemB, version)
    ERRCHECK (result)

    If version <> FMOD_VERSION Then
        MsgBox "Error!  You are using an old version of FMOD " & version & ". " & _
               "This program requires " & FMOD_VERSION
    End If
    
    result = FMOD_System_GetNumDrivers(systemB, Numdrivers)
    ERRCHECK (result)
    
    For count = 0 To Numdrivers - 1
        result = FMOD_System_GetDriverInfo(systemB, count, drivernamearray(0), 256, guid)
        ERRCHECK (result)
     
        drivername = StrConv(drivernamearray, vbUnicode)
        
        ComboB.AddItem drivername
    Next
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    ' Shutdown
    If soundA Then
        result = FMOD_Sound_Release(soundA)
        ERRCHECK (result)
    End If
    If soundB Then
        result = FMOD_Sound_Release(soundB)
        ERRCHECK (result)
    End If
    
    If systemA Then
        result = FMOD_System_Close(systemA)
        ERRCHECK (result)
        
        result = FMOD_System_Release(systemA)
        ERRCHECK (result)
    End If
    If systemB Then
        result = FMOD_System_Close(systemB)
        ERRCHECK (result)
        
        result = FMOD_System_Release(systemB)
        ERRCHECK (result)
    End If
End Sub

Private Sub ComboA_Click()
    Dim result As FMOD_RESULT
        
    result = FMOD_System_SetDriver(systemA, CLng(ComboA.ListIndex))
    ERRCHECK (result)
    
    result = FMOD_System_Init(systemA, 32, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    ComboA.Enabled = False
    cmdPlayA.Enabled = True
    
    ' Load a sample into sound card
    result = FMOD_System_CreateSound(systemA, "../../examples/media/drumloop.wav", FMOD_HARDWARE, soundA)
    ERRCHECK (result)
    result = FMOD_Sound_SetMode(soundA, FMOD_LOOP_OFF)
    ERRCHECK (result)
End Sub

Private Sub ComboB_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_SetDriver(systemB, CLng(ComboB.ListIndex))
    ERRCHECK (result)
    
    result = FMOD_System_Init(systemB, 32, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    ComboB.Enabled = False
    cmdPlayB.Enabled = True
    
    ' Load a sample into sound card
    result = FMOD_System_CreateSound(systemB, "../../examples/media/jaguar.wav", FMOD_HARDWARE, soundB)
    ERRCHECK (result)
End Sub

Private Sub cmdPlayA_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(systemA, FMOD_CHANNEL_FREE, soundA, 0, channelA)
    ERRCHECK (result)
End Sub

Private Sub cmdPlayB_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_PlaySound(systemB, FMOD_CHANNEL_FREE, soundB, 0, channelB)
    ERRCHECK (result)
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim channelsplayingA As Long
    Dim channelsplayingB As Long
    
    If channelA Then
        result = FMOD_System_GetChannelsPlaying(systemA, channelsplayingA)
        ERRCHECK (result)
    End If
    If channelB Then
        result = FMOD_System_GetChannelsPlaying(systemB, channelsplayingB)
        ERRCHECK (result)
    End If
    
    StatusBar.SimpleText = "Channels Playing A: " & channelsplayingA & " Channels Playing B: " & channelsplayingB

    If systemA Then
        Call FMOD_System_Update(systemA)
    End If
    
    If systemB Then
        Call FMOD_System_Update(systemB)
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
