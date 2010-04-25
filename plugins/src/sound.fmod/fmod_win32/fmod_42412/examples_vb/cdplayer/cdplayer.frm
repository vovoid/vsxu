VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "CD Player Example"
   ClientHeight    =   2415
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   3540
   LinkTopic       =   "Form1"
   ScaleHeight     =   2415
   ScaleWidth      =   3540
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdNext 
      Caption         =   "Next Track"
      Height          =   495
      Left            =   1800
      TabIndex        =   5
      Top             =   960
      Width           =   1575
   End
   Begin VB.CommandButton cmdPlay 
      Caption         =   "Play"
      Height          =   495
      Left            =   120
      TabIndex        =   4
      Top             =   960
      Width           =   1575
   End
   Begin VB.ComboBox ComboDriveSelect 
      Height          =   315
      Left            =   720
      TabIndex        =   3
      Text            =   "Select CDROM Drive"
      Top             =   480
      Width           =   2295
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1200
      TabIndex        =   2
      Top             =   1560
      Width           =   975
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   2760
      Top             =   1560
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   1
      Top             =   2040
      Width           =   3540
      _ExtentX        =   6244
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
      Left            =   0
      TabIndex        =   0
      Top             =   120
      Width           =   3615
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ===============================================================================================
' cdda example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to play CD tracks digitally and generate a CDDB query
' ===============================================================================================
Option Explicit

Dim system As Long
Dim sound As Long
Dim cdsound As Long
Dim channel As Long
Dim numtracks As Long
Dim currenttrack As Long
Dim playstarted As Boolean

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim alldrives As String
    Dim drive As String

    cmdNext.Enabled = False
    cmdPlay.Enabled = False
    
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
    
    
    ' Bump up the file buffer size a bit from the 16k default for CDDA, because it is a slower medium.
    result = FMOD_System_SetStreamBufferSize(system, 65536, FMOD_TIMEUNIT_RAWBYTES)
    ERRCHECK (result)

    ' Get all of the CD/DVD ROM Drives
    alldrives = Space((26 * 4) + 1)
    
    If GetLogicalDriveStrings(Len(alldrives), alldrives) Then
        alldrives = Trim(alldrives)
    End If
    
    Do Until alldrives = Chr(0)
        drive = NextString(alldrives)
        
        ' Only put CD/DVD ROM drives into the list
        If GetDriveType(drive) = 5 Then
            drive = Mid$(drive, 1, 2)
            ComboDriveSelect.AddItem drive
        End If
    Loop
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    ' Shutdown
    If cdsound Then
        result = FMOD_Sound_Release(cdsound)
        ERRCHECK (result)
    End If
    If system Then
        result = FMOD_System_Close(system)
        ERRCHECK (result)
        
        result = FMOD_System_Release(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub ComboDriveSelect_Click()
    Dim result As FMOD_RESULT
    Dim selected As String
    
    selected = ComboDriveSelect
    
    result = FMOD_System_CreateSound(system, selected, (FMOD_HARDWARE Or FMOD_2D Or FMOD_CREATESTREAM Or FMOD_OPENONLY), cdsound)
    ERRCHECK (result)
    result = FMOD_Sound_GetNumSubSounds(cdsound, numtracks)
    ERRCHECK (result)
    result = FMOD_Sound_GetSubSound(cdsound, currenttrack, sound)
    ERRCHECK (result)

    ComboDriveSelect.Enabled = False
    cmdNext.Enabled = True
    cmdPlay.Enabled = True
End Sub

Private Sub cmdPlay_Click()
    Dim result As FMOD_RESULT
    Dim paused As Long
    
    If playstarted Then
        result = FMOD_Channel_GetPaused(channel, paused)
        ERRCHECK (result)
        
        If paused Then
            result = FMOD_Channel_SetPaused(channel, 0)
            ERRCHECK (result)
            
            cmdPlay.Caption = "Pause"
        Else
            result = FMOD_Channel_SetPaused(channel, 1)
            ERRCHECK (result)
            
            cmdPlay.Caption = "Play"
        End If
    Else
        result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound, 0, channel)
        ERRCHECK (result)
        
        cmdPlay.Caption = "Pause"
        
        playstarted = True
    End If
End Sub

Private Sub cmdNext_Click()
    Dim result As FMOD_RESULT
    
    currenttrack = currenttrack + 1
    If currenttrack >= numtracks Then
        currenttrack = 0
    End If
    result = FMOD_Sound_GetSubSound(cdsound, currenttrack, sound)
    ERRCHECK (result)
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound, 0, channel)
    ERRCHECK (result)
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim ms As Long
    Dim lenms As Long
    Dim lenmscd As Long
    Dim playing As Long
    Dim paused As Long
    
    If channel Then
        result = FMOD_Channel_GetPaused(channel, paused)
        ERRCHECK (result)
        result = FMOD_Channel_IsPlaying(channel, playing)
        ERRCHECK (result)
        result = FMOD_Channel_GetPosition(channel, ms, FMOD_TIMEUNIT_MS)
        ERRCHECK (result)
        result = FMOD_Sound_GetLength(sound, lenms, FMOD_TIMEUNIT_MS)
        ERRCHECK (result)
    End If
    
    If cdsound Then
        'Get length of entire CD.  Did you know you can also play 'cdsound' and it will play the whole CD without gaps?
        result = FMOD_Sound_GetLength(cdsound, lenmscd, FMOD_TIMEUNIT_MS)
        ERRCHECK (result)
    End If
    
    StatusBar.SimpleText = "Total CD length: " & CInt(lenmscd / 1000 / 60) & ":" & CInt(lenmscd / 1000 Mod 60) & ":" & CInt(lenmscd / 10 Mod 100) & _
                           " Track " & (currenttrack + 1) & "/" & numtracks & " : " & CInt(ms / 1000 / 60) & ":" & CInt(ms / 1000 Mod 60) & _
                           ":" & (ms / 10 Mod 100) & "/" & CInt(lenms / 1000 / 60) & ":" & CInt(lenms / 1000 Mod 60) & ":" & CInt(lenms / 10 Mod 100) & _
                           IIf(paused, " Paused", IIf(playing, " Playing", " Stopped"))
    
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

Private Function NextString(startstr As String) As String
    Dim pos As Long

    pos = InStr(startstr, Chr(0))
  
    If pos Then
       NextString = Mid(startstr, 1, pos - 1)
       startstr = Mid(startstr, pos + 1, Len(startstr))
    End If
End Function
