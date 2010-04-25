VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Pitch Detection Example"
   ClientHeight    =   2595
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   6285
   LinkTopic       =   "Form1"
   ScaleHeight     =   2595
   ScaleWidth      =   6285
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   2640
      TabIndex        =   5
      Top             =   1800
      Width           =   975
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   5640
      Top             =   960
   End
   Begin VB.ComboBox ComboOutput 
      Height          =   315
      Left            =   1440
      TabIndex        =   3
      Text            =   "Select OUTPUT type"
      Top             =   600
      Width           =   3375
   End
   Begin VB.ComboBox ComboPlayback 
      Height          =   315
      Left            =   1440
      TabIndex        =   2
      Text            =   "Select PLAYBACK driver"
      Top             =   960
      Width           =   3375
   End
   Begin VB.ComboBox ComboRecord 
      Height          =   315
      Left            =   1440
      TabIndex        =   1
      Text            =   "Select RECORD driver"
      Top             =   1320
      Width           =   3375
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   4
      Top             =   2220
      Width           =   6285
      _ExtentX        =   11086
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
      Left            =   1440
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
' ===============================================================================================
' Pitch detection example.
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example combines recording with spectrum analysis to determine the pitch of the sound
' being recorded.
' ===============================================================================================
Option Explicit

Private Const OUTPUTRATE As Long = 48000
Private Const SPECTRUMSIZE As Long = 8192
Private Const SPECTRUMRANGE As Single = OUTPUTRATE / 2
Private Const BINSIZE As Single = (SPECTRUMRANGE / SPECTRUMSIZE)

Dim system As Long
Dim Sound As Long
Dim channel As Long
Dim note As Variant
Dim notefreq As Variant
Dim bin As Long

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim Numdrivers As Long
    Dim count As Long
    Dim name(128) As Byte
    Dim GUID As FMOD_GUID
    
    Dim temp As Single
    
    ' Initialise note and notefreq arrays
    note = Array( _
            "C 0", "C#0", "D 0", "D#0", "E 0", "F 0", "F#0", "G 0", "G#0", "A 0", "A#0", "B 0", _
            "C 1", "C#1", "D 1", "D#1", "E 1", "F 1", "F#1", "G 1", "G#1", "A 1", "A#1", "B 1", _
            "C 2", "C#2", "D 2", "D#2", "E 2", "F 2", "F#2", "G 2", "G#2", "A 2", "A#2", "B 2", _
            "C 3", "C#3", "D 3", "D#3", "E 3", "F 3", "F#3", "G 3", "G#3", "A 3", "A#3", "B 3", _
            "C 4", "C#4", "D 4", "D#4", "E 4", "F 4", "F#4", "G 4", "G#4", "A 4", "A#4", "B 4", _
            "C 5", "C#5", "D 5", "D#5", "E 5", "F 5", "F#5", "G 5", "G#5", "A 5", "A#5", "B 5", _
            "C 6", "C#6", "D 6", "D#6", "E 6", "F 6", "F#6", "G 6", "G#6", "A 6", "A#6", "B 6", _
            "C 7", "C#7", "D 7", "D#7", "E 7", "F 7", "F#7", "G 7", "G#7", "A 7", "A#7", "B 7", _
            "C 8", "C#8", "D 8", "D#8", "E 8", "F 8", "F#8", "G 8", "G#8", "A 8", "A#8", "B 8", _
            "C 9", "C#9", "D 9", "D#9", "E 9", "F 9", "F#9", "G 9", "G#9", "A 9", "A#9", "B 9")
    
    notefreq = Array( _
            16.35, 17.32, 18.35, 19.45, 20.6, 21.83, 23.12, 24.5, 25.96, 27.5, 29.14, 30.87, _
            32.7, 34.65, 36.71, 38.89, 41.2, 43.65, 46.25, 49, 51.91, 55, 58.27, 61.74, _
            65.41, 69.3, 73.42, 77.78, 82.41, 87.31, 92.5, 98, 103.83, 110, 116.54, 123.47, _
            130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185, 196, 207.65, 220, 233.08, 246.94, _
            261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392, 415.3, 440, 466.16, 493.88, _
            523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880, 932.33, 987.77, _
            1046.5, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760, 1864.66, 1975.53, _
            2093, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520, 3729.31, 3951.07, _
            4186.01, 4434.92, 4698.64, 4978.03, 5274.04, 5587.65, 5919.91, 6271.92, 6644.87, 7040, 7458.62, 7902.13, _
            8372.01, 8869.84, 9397.27, 9956.06, 10548.08, 11175.3, 11839.82, 12543.85, 13289.75, 14080, 14917.24, 15804.26, 0)
    
    ComboPlayback.Enabled = False
    ComboRecord.Enabled = False
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
        result = FMOD_System_GetDriverInfo(system, count, name(0), 128, GUID)
        ERRCHECK (result)
        
        ComboPlayback.AddItem StrConv(name, vbUnicode)
    Next
    
    ' Get record drivers
    result = FMOD_System_GetRecordNumDrivers(system, Numdrivers)
    ERRCHECK (result)
    
    For count = 0 To Numdrivers - 1
        result = FMOD_System_GetRecordDriverInfo(system, count, name(0), 128, GUID)
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
    Dim outputfreq As Long
    Dim Format As FMOD_SOUND_FORMAT
    
    result = FMOD_System_SetRecordDriver(system, ComboRecord.ListIndex)
    ERRCHECK (result)
    
    result = FMOD_System_SetSoftwareFormat(system, OUTPUTRATE, FMOD_SOUND_FORMAT_PCM16, 1, 0, FMOD_DSP_RESAMPLER_LINEAR)
    ERRCHECK (result)

    ' Initialise
    result = FMOD_System_Init(system, 32, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    result = FMOD_System_GetSoftwareFormat(system, outputfreq, Format, 0, 0, 0, 0)
    ERRCHECK (result)
    
    ' Create a sound to record to
    exinfo.cbsize = LenB(exinfo)
    exinfo.Numchannels = 1
    exinfo.Format = Format
    exinfo.defaultfrequency = OUTPUTRATE
    exinfo.Length = exinfo.defaultfrequency * 2 * exinfo.Numchannels * 5
    
    result = FMOD_System_CreateSoundEx(system, 0, (FMOD_2D Or FMOD_SOFTWARE Or FMOD_LOOP_NORMAL Or FMOD_OPENUSER), exinfo, Sound)
    ERRCHECK (result)
    
    ' Start recording
    result = FMOD_System_RecordStart(system, Sound, 1)
    ERRCHECK (result)
    
    Sleep (200)  ' Give it some time to record something
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_REUSE, Sound, 0, channel)
    ERRCHECK (result)
    
    ' Dont hear what is being recorded otherwise it will feedback.  Spectrum analysis is done before volume scaling in the DSP chain
    result = FMOD_Channel_SetVolume(channel, 0)
    ERRCHECK (result)
    
    Timer.Enabled = True
    ComboOutput.Enabled = False
    ComboPlayback.Enabled = False
    ComboRecord.Enabled = False
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim dominanthz As Single
    Dim max As Single
    Dim dominantnote As Long
    Dim binsze As Single
    Dim count As Long
    Dim spectrum(SPECTRUMSIZE) As Single
    
    binsze = BINSIZE
    
    If channel Then
        result = FMOD_Channel_GetSpectrum(channel, spectrum(0), SPECTRUMSIZE, 0, FMOD_DSP_FFT_WINDOW_TRIANGLE)
        
        For count = 0 To SPECTRUMSIZE - 1
            If spectrum(count) > 0.01 And spectrum(count) > max Then
                max = spectrum(count)
                bin = count
            End If
        Next
        
        dominanthz = bin * BINSIZE        ' dominant frequency min
    
        dominantnote = 0
        
        For count = 0 To 119
            If dominanthz >= notefreq(count) And dominanthz < notefreq(count + 1) Then

                ' which is it closer to.  This note or the next note
                If Abs(dominanthz - notefreq(count)) < Abs(dominanthz - notefreq(count + 1)) Then
                    dominantnote = count
                Else
                    dominantnote = count + 1
                End If
                GoTo break
            End If
        Next
    End If
    
break:
    StatusBar.SimpleText = "Detected rate " & FormatNumber(dominanthz, 1, vbFalse, vbFalse, vbFalse) & " -> " & _
                            FormatNumber(((bin + 0.99) * BINSIZE), 1, vbFalse, vbFalse, vbFalse) & _
                           " hz. Detected musical note. " & note(dominantnote) & " (" & _
                            notefreq(dominantnote) & " hz)"
    
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
