VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Use Plugins Example"
   ClientHeight    =   8040
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4575
   ScaleHeight     =   8040
   ScaleWidth      =   4575
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdPlay 
      Caption         =   "Play"
      Height          =   495
      Left            =   3120
      TabIndex        =   9
      Top             =   6720
      Width           =   1335
   End
   Begin VB.Frame FrameOutput 
      Caption         =   "Output plugins"
      Height          =   1575
      Left            =   120
      TabIndex        =   5
      Top             =   5040
      Width           =   4335
      Begin VB.ListBox ListOutput 
         Height          =   1230
         Left            =   120
         TabIndex        =   8
         Top             =   240
         Width           =   4095
      End
   End
   Begin VB.Frame FrameDSP 
      Caption         =   "DSP plugins"
      Height          =   2055
      Left            =   120
      TabIndex        =   4
      Top             =   2880
      Width           =   4335
      Begin VB.ListBox ListDSP 
         Height          =   1620
         Left            =   120
         TabIndex        =   7
         Top             =   240
         Width           =   4095
      End
   End
   Begin VB.Frame FrameCodec 
      Caption         =   "Codec plugins"
      Height          =   2175
      Left            =   120
      TabIndex        =   3
      Top             =   600
      Width           =   4335
      Begin VB.ListBox ListCodec 
         Height          =   1815
         Left            =   120
         TabIndex        =   6
         Top             =   240
         Width           =   4095
      End
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3480
      Top             =   7200
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1800
      TabIndex        =   2
      Top             =   7200
      Width           =   975
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   0
      Top             =   7665
      Width           =   4575
      _ExtentX        =   8070
      _ExtentY        =   661
      Style           =   1
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
         EndProperty
      EndProperty
   End
   Begin VB.Label Label1 
      Caption         =   "Select an output plugin and press 'Play'"
      Height          =   255
      Left            =   120
      TabIndex        =   10
      Top             =   6840
      Width           =   2775
   End
   Begin VB.Label copyright 
      Alignment       =   2  'Center
      Caption         =   "Copyright (c) Firelight Technologies 2004-2005"
      Height          =   375
      Left            =   600
      TabIndex        =   1
      Top             =   120
      Width           =   3375
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ===============================================================================================
' Use Plugins Example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to use FMODEXP.DLL and its associated plugins.
' The example lists the available plugins, and loads a new plugin that isnt normally included
' with FMOD Ex, which is output_mp3.dll.  When this is loaded, it can be chosen as an output
' mode, for realtime encoding of audio output into the mp3 format.
' ===============================================================================================
Option Explicit

Dim system As Long
Dim sound As Long
Dim channel As Long
Dim initialised As Boolean

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim count As Long
    Dim numplugins As Long
    Dim pluginname(0 To 255) As Byte
    
    Timer.Enabled = False
    
    ' Create a System object and initialize.
    result = FMOD_System_Create(system)
    ERRCHECK (result)
    
    result = FMOD_System_GetVersion(system, version)
    ERRCHECK (result)

    If version <> FMOD_VERSION Then
        MsgBox "Error!  You are using an old version of FMOD " & Hex$(version) & ". " & _
               "This program requires " & Hex$(FMOD_VERSION)
    End If
    
    ' Set the source directory for all of the FMOD plugins.
    result = FMOD_System_SetPluginPath(system, "../../api/plugins")
    ERRCHECK (result)

    
    ' Load up an extra plugin that is not normally used by FMOD.
    result = FMOD_System_LoadPlugin(system, "output_mp3.dll", 0, 0)
    If result = FMOD_ERR_FILE_NOTFOUND Then
        ' If it isn't in the same directory, try for the plugin directory.

        result = FMOD_System_LoadPlugin(system, "../../examples/bin/output_mp3.dll", 0, 0)
        ERRCHECK (result)
    End If
    
    ' Display plugins
    result = FMOD_System_GetNumPlugins(system, FMOD_PLUGINTYPE_CODEC, numplugins)
    ERRCHECK (result)
    
    For count = 0 To numplugins - 1
        result = FMOD_System_GetPluginInfo(system, FMOD_PLUGINTYPE_CODEC, count, pluginname(0), 256, 0)
        ERRCHECK (result)
        
        ListCodec.AddItem StrConv(pluginname, vbUnicode)
    Next
    
    result = FMOD_System_GetNumPlugins(system, FMOD_PLUGINTYPE_DSP, numplugins)
    ERRCHECK (result)
    
    For count = 0 To numplugins - 1
        result = FMOD_System_GetPluginInfo(system, FMOD_PLUGINTYPE_DSP, count, pluginname(0), 256, 0)
        ERRCHECK (result)
        
        ListDSP.AddItem StrConv(pluginname, vbUnicode)
    Next
    
    result = FMOD_System_GetNumPlugins(system, FMOD_PLUGINTYPE_OUTPUT, numplugins)
    ERRCHECK (result)
    
    For count = 0 To numplugins - 1
        result = FMOD_System_GetPluginInfo(system, FMOD_PLUGINTYPE_OUTPUT, count, pluginname(0), 256, 0)
        ERRCHECK (result)
        
        ListOutput.AddItem StrConv(pluginname, vbUnicode)
    Next
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

Private Sub cmdPlay_Click()
    Dim result As FMOD_RESULT
    Dim selectedplugin As Long
    Dim count As Long
    
    ' Get the output plugin that was selected
    For count = 0 To ListOutput.ListCount - 1
        If ListOutput.Selected(count) Then
            selectedplugin = count
            GoTo break
        End If
    Next
    
break:

    ' Initialise system and play sound
    If Not initialised Then
        result = FMOD_System_SetOutputByPlugin(system, selectedplugin)
        ERRCHECK (result)

        result = FMOD_System_Init(system, 32, FMOD_INIT_NORMAL, 0)
        ERRCHECK (result)
        
        initialised = True
    End If

    result = FMOD_System_CreateSound(system, "../../examples/media/wave.mp3", (FMOD_SOFTWARE Or FMOD_CREATESTREAM), sound)
    ERRCHECK (result)

    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_REUSE, sound, 0, channel)
    ERRCHECK (result)
    
    ListOutput.Enabled = False
    
    Timer.Enabled = True
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


