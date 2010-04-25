VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Record To Disk Example"
   ClientHeight    =   2985
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   3615
   LinkTopic       =   "Form1"
   ScaleHeight     =   2985
   ScaleWidth      =   3615
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdStop 
      Caption         =   "Stop"
      Height          =   495
      Left            =   1920
      TabIndex        =   6
      Top             =   1320
      Width           =   1455
   End
   Begin VB.CommandButton cmdStart 
      Caption         =   "Start recording to record.wav"
      Height          =   495
      Left            =   240
      TabIndex        =   5
      Top             =   1320
      Width           =   1575
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1200
      TabIndex        =   4
      Top             =   2040
      Width           =   975
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   2640
      Top             =   2040
   End
   Begin VB.ComboBox ComboRecord 
      Height          =   315
      Left            =   120
      TabIndex        =   2
      Text            =   "Select RECORD driver"
      Top             =   840
      Width           =   3375
   End
   Begin VB.ComboBox ComboOutput 
      Height          =   315
      Left            =   120
      TabIndex        =   1
      Text            =   "Select OUTPUT type"
      Top             =   480
      Width           =   3375
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   3
      Top             =   2610
      Width           =   3615
      _ExtentX        =   6376
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
' Record to disk example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to do a streaming record to disk.
'===============================================================================================
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
Dim Soundlength As Long
Dim Lastrecordpos As Long
Dim Datalength As Long
Dim FileHandle As Long
Dim Recording As Boolean

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    
    ComboRecord.Enabled = False
    cmdStart.Enabled = False
    Recording = False
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
    Dim count As Long
    Dim Numdrivers As Long
    Dim name(128) As Byte
    Dim GUID As FMOD_GUID
    
    Select Case ComboOutput.ListIndex
        Case Is = 0
            result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_DSOUND)
        Case Is = 1
            result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_WINMM)
        Case Is = 2
            result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_ASIO)
    End Select
    ERRCHECK (result)
    
    ComboRecord.Enabled = True
    
    ' Get record drivers
    result = FMOD_System_GetRecordNumDrivers(system, Numdrivers)
    ERRCHECK (result)
    
    For count = 0 To Numdrivers - 1
        result = FMOD_System_GetRecordDriverInfo(system, count, name(0), 128, GUID)
        ERRCHECK (result)
        
        ComboRecord.AddItem StrConv(name, vbUnicode)
    Next
End Sub

Private Sub ComboRecord_Click()
    Dim result As FMOD_RESULT
    Dim exinfo As FMOD_CREATESOUNDEXINFO
    
    result = FMOD_System_SetRecordDriver(system, ComboRecord.ListIndex)
    ERRCHECK (result)

    ComboOutput.Enabled = False
    ComboRecord.Enabled = False
    
    ' Initialise
    result = FMOD_System_Init(system, 32, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    ' Create a sound to record to
    exinfo.cbsize = LenB(exinfo)
    exinfo.Numchannels = 2
    exinfo.Format = FMOD_SOUND_FORMAT_PCM16
    exinfo.defaultfrequency = 44100
    exinfo.Length = exinfo.defaultfrequency * 2 * exinfo.Numchannels * 2
    
    result = FMOD_System_CreateSoundEx(system, 0, (FMOD_2D Or FMOD_SOFTWARE Or FMOD_OPENUSER), exinfo, Sound)
    ERRCHECK (result)
    
    Timer.Enabled = True
    cmdStart.Enabled = True
End Sub

Private Sub cmdStart_Click()
    Dim result As FMOD_RESULT
    
    Soundlength = 0
    Lastrecordpos = 0
    Datalength = 0
    
    result = FMOD_System_RecordStart(system, Sound, 1)
    ERRCHECK (result)
    
    ' Create the file
    Open "record.wav" For Random As #1
    Close #1
    
    FileHandle = lOpen("record.wav", 1)
    
    ' Write out the wav header.  As we don't know the length yet it will be 0.
    WriteWavHeader (0)
    
    cmdStart.Enabled = False
    Recording = True
End Sub

Private Sub cmdStop_Click()
    
    If Recording Then
    
        Recording = False
    
        WriteWavHeader (Datalength)
    
        lClose (FileHandle)
        
        cmdStart.Enabled = True
    End If

End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    
    If Recording Then
        Dim recordpos As Long

        result = FMOD_System_GetRecordPosition(system, recordpos)
        ERRCHECK (result)

        If recordpos <> Lastrecordpos Then
            Dim Ptr1 As Long
            Dim Ptr2 As Long
            Dim blocklength As Long
            Dim Len1 As Long
            Dim Len2 As Long
                            
            blocklength = recordpos - Lastrecordpos
            If blocklength < 0 Then
                blocklength = blocklength + Soundlength
            End If


            ' Lock the sound to get access to the raw data.
            
            result = FMOD_Sound_Lock(Sound, Lastrecordpos * 4, blocklength * 4, Ptr1, Ptr2, Len1, Len2) ' *4 = stereo 16bit.  1 sample = 4 bytes.

                    
            ' Write it to disk.
            
            If Ptr1 And Len1 > 0 Then
                Call lWrite(FileHandle, Ptr1, Len1)
                    
                Datalength = Datalength + Len1
            End If
            
            If Ptr2 And Len2 > 0 Then
                Call lWrite(FileHandle, Ptr2, Len2)
                      
                Datalength = Datalength + Len2
            End If
            
            '  Unlock the sound to allow FMOD to use it again.
            
            result = FMOD_Sound_Unlock(Sound, Ptr1, Ptr1, Len1, Len2)
        End If
            
        Lastrecordpos = recordpos
        
        StatusBar.SimpleText = "Record buffer pos = " & recordpos & " Record time = " & CInt(Datalength / 44100 / 4 / 60) & ":" & CInt(Datalength / 44100 / 4) Mod 60

    End If
    
    If system Then
        result = FMOD_System_Update(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub WriteWavHeader(ByVal Length As Long)
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
    dataChnk.chunk.size = Length

    wavHdr.chunk.Id(0) = Asc("R")
    wavHdr.chunk.Id(1) = Asc("I")
    wavHdr.chunk.Id(2) = Asc("F")
    wavHdr.chunk.Id(3) = Asc("F")
    wavHdr.chunk.size = 32 + Length    ' sizeof(fmtChunk) + sizeof(riffChunk) + lenbytes
    wavHdr.rifftype(0) = Asc("W")
    wavHdr.rifftype(1) = Asc("A")
    wavHdr.rifftype(2) = Asc("V")
    wavHdr.rifftype(3) = Asc("E")
    
    ' Write wave header
    
    Call SetFilePointer(FileHandle, 0, 0, 0)
    
    Call lWrite(FileHandle, VarPtr(wavHdr), 12)
    Call lWrite(FileHandle, VarPtr(fmtChnk), 24)
    Call lWrite(FileHandle, VarPtr(dataChnk), 8)
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
