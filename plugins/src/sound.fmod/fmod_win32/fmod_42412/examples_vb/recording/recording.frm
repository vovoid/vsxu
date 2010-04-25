VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Recording Example"
   ClientHeight    =   5595
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4095
   LinkTopic       =   "Form1"
   ScaleHeight     =   5595
   ScaleWidth      =   4095
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdSave 
      Caption         =   "Save the 5 second segment to a wav file"
      Height          =   495
      Left            =   360
      TabIndex        =   10
      Top             =   4200
      Width           =   3375
   End
   Begin VB.CommandButton cmdStop 
      Caption         =   "Stop recording and playback"
      Height          =   495
      Left            =   360
      TabIndex        =   9
      Top             =   3600
      Width           =   3375
   End
   Begin VB.CommandButton cmdLoop 
      Caption         =   "Turn looping on/off"
      Height          =   495
      Left            =   360
      TabIndex        =   8
      Top             =   3000
      Width           =   3375
   End
   Begin VB.CommandButton cmdPlay 
      Caption         =   "Play the 5 second segment of audio"
      Height          =   495
      Left            =   360
      TabIndex        =   7
      Top             =   2400
      Width           =   3375
   End
   Begin VB.CommandButton cmdRecord 
      Caption         =   "Record a 5 second segment of audio"
      Height          =   495
      Left            =   360
      TabIndex        =   6
      Top             =   1800
      Width           =   3375
   End
   Begin VB.ComboBox ComboRecord 
      Height          =   315
      Left            =   360
      TabIndex        =   5
      Text            =   "Select RECORD driver"
      Top             =   1320
      Width           =   3375
   End
   Begin VB.ComboBox ComboPlayback 
      Height          =   315
      Left            =   360
      TabIndex        =   4
      Text            =   "Select PLAYBACK driver"
      Top             =   960
      Width           =   3375
   End
   Begin VB.ComboBox ComboOutput 
      Height          =   315
      Left            =   360
      TabIndex        =   3
      Text            =   "Select OUTPUT type"
      Top             =   600
      Width           =   3375
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1560
      TabIndex        =   1
      Top             =   4800
      Width           =   975
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   3000
      Top             =   4800
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   0
      Top             =   5220
      Width           =   4095
      _ExtentX        =   7223
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
      TabIndex        =   2
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
' Record example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to record a sound, then write it to a wav file.
' It then shows how to play a sound while it is being recorded to.  Because it is recording, the
' sound playback has to be delayed a little bit so that the playback doesn't play part of the
' buffer that is still being written to.
' ===============================================================================================
Option Explicit

'
' WAV Structures
'
Private Type RiffChunk
    Id(0 To 3) As Byte
    size As Long
End Type

Private Type FmtChunk
    chunk As RiffChunk
    wFormatTag As Integer   ' format type
    nChannels As Integer    ' number of channels (i.e. mono, stereo...)
    nSamplesPerSec As Long  ' sample rate
    nAvgBytesPerSec As Long ' for buffer estimation
    nBlockAlign As Integer  ' block size of data
    wBitsPerSample As Integer  ' number of bits per sample of mono data
End Type

Private Type DataChunk
    chunk As RiffChunk
End Type

Private Type WavHeader
    chunk As RiffChunk
    rifftype(0 To 3) As Byte
End Type

Dim system As Long
Dim Sound As Long
Dim channel As Long
Dim looping As Boolean

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim Numdrivers As Long
    Dim count As Long
    Dim name(128) As Byte
    Dim guid As FMOD_GUID
    
    looping = False
    ComboPlayback.Enabled = False
    ComboRecord.Enabled = False
    cmdRecord.Enabled = False
    cmdPlay.Enabled = False
    cmdStop.Enabled = False
    cmdSave.Enabled = False
    Timer.Enabled = False
    
    ' Create a System object and initialize.
    result = FMOD_System_Create(system)
    ERRCHECK (result)

    result = FMOD_System_GetVersion(system, version)
    ERRCHECK (result)

    If version < FMOD_VERSION Then
        MsgBox "Error!  You are using an old version of FMOD " & Hex$(version) & ". " & _
               "This program requires " & Hex$(FMOD_VERSION)
    End If
    
    ' Get output modes
    ComboOutput.AddItem "DirectSound"
    ComboOutput.AddItem "Windows Multimedia WaveOut"
    ComboOutput.AddItem "ASIO"
    
    ' Get playback drivers
    result = FMOD_System_GetNumDrivers(system, Numdrivers)
    ERRCHECK (result)
    
    For count = 0 To Numdrivers - 1
        result = FMOD_System_GetDriverInfo(system, count, name(0), 128, guid)
        ERRCHECK (result)
        
        ComboPlayback.AddItem StrConv(name, vbUnicode)
    Next
    
    ' Get record drivers
    result = FMOD_System_GetRecordNumDrivers(system, Numdrivers)
    ERRCHECK (result)
    
    For count = 0 To Numdrivers - 1
        result = FMOD_System_GetRecordDriverInfo(system, count, name(0), 128, guid)
        ERRCHECK (result)
        
        ComboRecord.AddItem StrConv(name, vbUnicode)
    Next
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    ' Shutdown
    If Sound Then
        result = FMOD_Sound_Release(Sound)
        ERRCHECK (result)
    End If
    If system Then
        result = FMOD_System_Close(system)
        ERRCHECK (result)
        
        result = FMOD_System_Release(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub ComboOutput_Click()
    Dim result As FMOD_RESULT
   
    Select Case ComboOutput.ListIndex
        Case Is = 0
            result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_DSOUND)
        Case Is = 1
            result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_WINMM)
        Case Is = 2
            result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_ASIO)
    End Select
    ERRCHECK (result)
    
    ComboPlayback.Enabled = True
End Sub

Private Sub ComboPlayback_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_SetDriver(system, ComboOutput.ListIndex)
    ERRCHECK (result)

    ComboRecord.Enabled = True
End Sub

Private Sub ComboRecord_Click()
    Dim result As FMOD_RESULT
    Dim exinfo As FMOD_CREATESOUNDEXINFO
    
    result = FMOD_System_SetRecordDriver(system, ComboRecord.ListIndex)
    ERRCHECK (result)

    ComboOutput.Enabled = False
    ComboPlayback.Enabled = False
    ComboRecord.Enabled = False
    
    ' Initialise
    result = FMOD_System_Init(system, 32, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    ' Create a sound to record to
    exinfo.cbsize = LenB(exinfo)
    exinfo.Numchannels = 1
    exinfo.Format = FMOD_SOUND_FORMAT_PCM16
    exinfo.defaultfrequency = 44100
    exinfo.Length = exinfo.defaultfrequency * 2 * exinfo.Numchannels * 5
    
    result = FMOD_System_CreateSoundEx(system, 0, (FMOD_2D Or FMOD_SOFTWARE Or FMOD_OPENUSER), exinfo, Sound)
    ERRCHECK (result)
    
    Timer.Enabled = True
    cmdRecord.Enabled = True
    cmdPlay.Enabled = True
    cmdStop.Enabled = True
    cmdSave.Enabled = True
End Sub

Private Sub cmdRecord_Click()
    Dim result As FMOD_RESULT
    
    If looping Then
        result = FMOD_System_RecordStart(system, Sound, 1)
    Else
        result = FMOD_System_RecordStart(system, Sound, 0)
    End If
    ERRCHECK (result)
End Sub

Private Sub cmdPlay_Click()
    Dim result As FMOD_RESULT
    
    If looping Then
        result = FMOD_Sound_SetMode(Sound, FMOD_LOOP_NORMAL)
    Else
        result = FMOD_Sound_SetMode(Sound, FMOD_LOOP_OFF)
    End If
    ERRCHECK (result)

    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_REUSE, Sound, 0, channel)
    ERRCHECK (result)
End Sub

Private Sub cmdLoop_Click()
   looping = Not looping
End Sub

Private Sub cmdStop_Click()
    Dim result As FMOD_RESULT
    
    result = FMOD_System_RecordStop(system)
    ERRCHECK (result)

    If channel Then
        result = FMOD_Channel_Stop(channel)
        channel = 0
    End If
End Sub

Private Sub cmdSave_Click()
    Call SaveToWave
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim Length As Long
    Dim recordpos As Long
    Dim playpos As Long
    Dim Recording As Long
    Dim playing As Long
    
    If system And Sound Then
        result = FMOD_Sound_GetLength(Sound, Length, FMOD_TIMEUNIT_PCM)
        result = FMOD_System_IsRecording(system, Recording)
        result = FMOD_System_GetRecordPosition(system, recordpos)

        If channel Then
            result = FMOD_Channel_IsPlaying(channel, playing)
            result = FMOD_Channel_GetPosition(channel, playpos, FMOD_TIMEUNIT_PCM)
        End If
    End If
    
    StatusBar.SimpleText = "State: " & IIf(Recording, IIf(playing, "Recording / playing", "Recording"), IIf(playing, "Playing", "Idle")) & _
                            " Record pos = " & recordpos & " Play pos = " & playpos & " Loop " & IIf(looping, "On", "Off")
    
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
        
Private Sub SaveToWave()
    Dim result As FMOD_RESULT
    Dim fmtChnk As FmtChunk
    Dim dataChnk As DataChunk
    Dim wavHdr As WavHeader
    Dim Soundtype As FMOD_SOUND_TYPE
    Dim Format As FMOD_SOUND_FORMAT
    Dim Channels As Long
    Dim Bits As Long
    Dim rate As Single
    Dim Lenbytes As Long
    Dim fp As Long
    Dim Ptr1 As Long
    Dim Ptr2 As Long
    Dim Len1 As Long
    Dim Len2 As Long
    
    result = FMOD_Sound_GetFormat(Sound, Soundtype, Format, Channels, Bits)
    ERRCHECK (result)
    result = FMOD_Sound_GetDefaults(Sound, rate, 0, 0, 0)
    ERRCHECK (result)
    result = FMOD_Sound_GetLength(Sound, Lenbytes, FMOD_TIMEUNIT_PCMBYTES)
    ERRCHECK (result)
            
    fmtChnk.chunk.Id(0) = Asc("f")
    fmtChnk.chunk.Id(1) = Asc("m")
    fmtChnk.chunk.Id(2) = Asc("t")
    fmtChnk.chunk.Id(3) = Asc(" ")
    fmtChnk.chunk.size = 16          ' sizeof(fmtChunk) - sizeof(riffChunk)
    fmtChnk.wFormatTag = 1
    fmtChnk.nChannels = Channels
    fmtChnk.nSamplesPerSec = rate
    fmtChnk.nAvgBytesPerSec = rate * Channels * Bits / 8
    fmtChnk.nBlockAlign = 1 * Channels * Bits / 8
    fmtChnk.wBitsPerSample = Bits
    
    dataChnk.chunk.Id(0) = Asc("d")
    dataChnk.chunk.Id(1) = Asc("a")
    dataChnk.chunk.Id(2) = Asc("t")
    dataChnk.chunk.Id(3) = Asc("a")
    dataChnk.chunk.size = Lenbytes

    wavHdr.chunk.Id(0) = Asc("R")
    wavHdr.chunk.Id(1) = Asc("I")
    wavHdr.chunk.Id(2) = Asc("F")
    wavHdr.chunk.Id(3) = Asc("F")
    wavHdr.chunk.size = 441032    ' sizeof(fmtChunk) + sizeof(riffChunk) + lenbytes
    wavHdr.rifftype(0) = Asc("W")
    wavHdr.rifftype(1) = Asc("A")
    wavHdr.rifftype(2) = Asc("V")
    wavHdr.rifftype(3) = Asc("E")
    
    ' Create, open file and write wave header
    Open "output.wav" For Random As #1
    Close #1
    
    fp = lOpen("output.wav", 1)
    
    Call lWrite(fp, VarPtr(wavHdr), 12)
    Call lWrite(fp, VarPtr(fmtChnk), 24)
    Call lWrite(fp, VarPtr(dataChnk), 8)
    
    ' Lock the sound to get access to the raw data
    result = FMOD_Sound_Lock(Sound, 0, Lenbytes, Ptr1, Ptr2, Len1, Len2)
    ERRCHECK (result)
    
    ' Write it to disk
    Call lWrite(fp, Ptr1, Len1)
    
    ' Unlock the sound to allow FMOD to use it again.
    result = FMOD_Sound_Unlock(Sound, Ptr1, Ptr2, Len1, Len2)
    ERRCHECK (result)
    
    Call lClose(fp)
    
    MsgBox "Written to output.wav"
End Sub
