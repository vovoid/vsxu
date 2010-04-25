VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "UserCreatedSound Example"
   ClientHeight    =   3885
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4350
   LinkTopic       =   "Form1"
   ScaleHeight     =   3885
   ScaleWidth      =   4350
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3240
      Top             =   3000
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1680
      TabIndex        =   4
      Top             =   3000
      Width           =   975
   End
   Begin VB.CommandButton cmdPauseResume 
      Caption         =   "Pause / Resume"
      Height          =   615
      Left            =   240
      TabIndex        =   3
      Top             =   2280
      Width           =   3855
   End
   Begin VB.CommandButton cmdStaticSample 
      Caption         =   "Play as a static in memory sample"
      Height          =   615
      Left            =   240
      TabIndex        =   2
      Top             =   1560
      Width           =   3855
   End
   Begin VB.CommandButton cmdRuntimeStream 
      Caption         =   "Play as a runtime decoded stream"
      Height          =   615
      Left            =   240
      TabIndex        =   1
      Top             =   840
      Width           =   3855
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   5
      Top             =   3510
      Width           =   4350
      _ExtentX        =   7673
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
      Top             =   240
      Width           =   3855
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ===============================================================================================
' User Created Sound Example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.

' This example shows how create a sound with data filled by the user.
' It shows a user created static sample, followed by a user created stream.
' The former allocates all memory needed for the sound and is played back as a static sample,
' while the latter streams the data in chunks as it plays, using far less memory.
' ===============================================================================================*/

Dim system As Long
Dim Sound As Long
Dim channel As Long
Dim createsoundexinfo As FMOD_CREATESOUNDEXINFO
Dim Mode As FMOD_MODE
Dim soundcreated As Boolean

Private Function GetFunctionPointer(fptr As Long) As Long
    GetFunctionPointer = fptr
End Function

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim Channels, Frequency As Long
    
    Channels = 2
    Frequency = 44100
    soundcreated = False
    
    Mode = FMOD_2D Or FMOD_DEFAULT Or FMOD_OPENUSER Or FMOD_LOOP_NORMAL Or FMOD_HARDWARE
    
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
    
    createsoundexinfo.cbsize = LenB(createsoundexinfo)
    createsoundexinfo.fileoffset = 0
    createsoundexinfo.Length = Frequency * Channels * 2 * 2
    createsoundexinfo.Numchannels = Channels
    createsoundexinfo.defaultfrequency = Frequency
    createsoundexinfo.Format = FMOD_SOUND_FORMAT_PCM16
    createsoundexinfo.pcmreadcallback = GetFunctionPointer(AddressOf pcmreadcallback)
    createsoundexinfo.pcmsetposcallback = GetFunctionPointer(AddressOf pcmsetposcallback)
    createsoundexinfo.dlsname = 0
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    ' Shutdown
    If system Then
        result = FMOD_System_Close(system)
        ERRCHECK (result)
        result = FMOD_System_Release(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub cmdRuntimeStream_Click()
    Dim result As FMOD_RESULT
    
    If Not soundcreated Then
        result = FMOD_System_CreateSoundEx(system, 0, Mode Or FMOD_CREATESTREAM, createsoundexinfo, Sound)
        ERRCHECK (result)
        
        soundcreated = True
    End If
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, Sound, 0, channel)
End Sub

Private Sub cmdStaticSample_Click()
    Dim result As FMOD_RESULT
    
    If Not soundcreated Then
        result = FMOD_System_CreateSoundEx(system, 0, Mode, createsoundexinfo, Sound)
        ERRCHECK (result)
        
        soundcreated = True
    End If
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, Sound, 0, channel)
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
