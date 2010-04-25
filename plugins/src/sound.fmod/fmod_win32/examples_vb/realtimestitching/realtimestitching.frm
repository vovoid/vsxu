VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Real-time Stitching Example"
   ClientHeight    =   1830
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   1830
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdPause 
      Caption         =   "Pause / Resume"
      Height          =   495
      Left            =   120
      TabIndex        =   3
      Top             =   720
      Width           =   2415
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   495
      Left            =   2760
      TabIndex        =   2
      Top             =   720
      Width           =   1695
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   0
      Top             =   0
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   1
      Top             =   1455
      Width           =   4680
      _ExtentX        =   8255
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
' ===============================================================================================
' Real-time stitching example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how you can create your own multi-subsound stream, then in realtime replace
' each the subsound as it plays them.  Using a looping sentence, it will seamlessly stich between
' 2 subsounds in this example, and each time it switches to a new sound, it will replace the old
' one with another sound in our list.
'
' These sounds can go on forever as long as they are the same bitdepth (when decoded) and number
' of channels (ie mono / stereo).  The reason for this is the hardware channel cannot change
' formats mid sentence, and using different hardware channels would mean it wouldn't be gapless.
' ===============================================================================================
Option Explicit

Private Const numsounds As Long = 32

Dim system As Long
Dim Sound As Long
Dim Subsound(0 To 1) As Long
Dim subsoundid As Long
Dim sentenceid As Long
Dim channel As Long
Dim soundname As Variant

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim exinfo As FMOD_CREATESOUNDEXINFO
    Dim soundlist(0 To 1) As Long
    
    exinfo.cbsize = LenB(exinfo)
    
    soundname = Array( _
            "../../examples/media/e.ogg", "../../examples/media/d.ogg", _
            "../../examples/media/c.ogg", "../../examples/media/d.ogg", _
            "../../examples/media/e.ogg", "../../examples/media/e.ogg", _
            "../../examples/media/e.ogg", "../../examples/media/e.ogg", _
            "../../examples/media/d.ogg", "../../examples/media/d.ogg", _
            "../../examples/media/d.ogg", "../../examples/media/d.ogg", _
            "../../examples/media/e.ogg", "../../examples/media/e.ogg", _
            "../../examples/media/e.ogg", "../../examples/media/e.ogg", _
            "../../examples/media/e.ogg", "../../examples/media/d.ogg", _
            "../../examples/media/c.ogg", "../../examples/media/d.ogg", _
            "../../examples/media/e.ogg", "../../examples/media/e.ogg", _
            "../../examples/media/e.ogg", "../../examples/media/e.ogg", _
            "../../examples/media/d.ogg", "../../examples/media/d.ogg", _
            "../../examples/media/e.ogg", "../../examples/media/d.ogg", _
            "../../examples/media/c.ogg", "../../examples/media/c.ogg", _
            "../../examples/media/c.ogg", "../../examples/media/c.ogg")
            
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
    
   
    ' Set up the FMOD_CREATESOUNDEXINFO structure for the user stream with room for 2 subsounds. (our subsound double buffer)
    exinfo.cbsize = LenB(exinfo)
    exinfo.defaultfrequency = 44100
    exinfo.Numsubsounds = 2
    exinfo.Numchannels = 1
    exinfo.Format = FMOD_SOUND_FORMAT_PCM16

    ' create the 'parent' stream that contains the substreams.  Set it to loop so that it loops between subsound 0 and 1.
    result = FMOD_System_CreateStreamEx(system, 0, FMOD_LOOP_NORMAL Or FMOD_OPENUSER, exinfo, Sound)
    ERRCHECK (result)

    
    ' Add 2 of our streams as children of the parent.  They should be the same format (ie mono/stereo and bitdepth) as the parent sound.
    ' When subsound 0 has finished and it is playing subsound 1, we will swap subsound 0 with a new sound, and the same for when subsound 1 has finished,
    ' causing a continual double buffered flip, which means continuous sound.
    result = FMOD_System_CreateStream(system, soundname(0), FMOD_DEFAULT, Subsound(0))
    ERRCHECK (result)

    result = FMOD_System_CreateStream(system, soundname(0), FMOD_DEFAULT, Subsound(1))
    ERRCHECK (result)

    result = FMOD_Sound_SetSubSound(Sound, 0, Subsound(0))
    ERRCHECK (result)

    result = FMOD_Sound_SetSubSound(Sound, 1, Subsound(1))
    ERRCHECK (result)

    
    ' Set up the gapless sentence to contain these first 2 streams.
    soundlist(0) = 0
    soundlist(1) = 1

    result = FMOD_Sound_SetSubSoundSentence(Sound, soundlist(0), 2)
    ERRCHECK (result)

    subsoundid = 0
    sentenceid = 2     ' The next sound to be appeneded to the stream.

    ' Play the sound.
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, Sound, 0, channel)
    ERRCHECK (result)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    ' Shut down
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

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    
    ' Replace the subsound that just finished with a new subsound, to create endless seamless stitching!
    
    ' Note that this polls the currently playing subsound using the FMOD_TIMEUNIT_BUFFERED flag.
    ' Remember streams are decoded / buffered ahead in advance!
    ' Don 't use the 'audible time' which is FMOD_TIMEUNIT_SENTENCE_SUBSOUND by itself.  When streaming, sound is
    ' processed ahead of time, and things like stream buffer / sentence manipulation (as done below) is required
    ' to be in 'buffered time', or else there will be synchronization problems and you might end up releasing a
    ' sub-sound that is still playing!
    If channel Then
        Dim currentsubsoundid As Long
    
        result = FMOD_Channel_GetPosition(channel, currentsubsoundid, FMOD_TIMEUNIT_SENTENCE_SUBSOUND Or FMOD_TIMEUNIT_BUFFERED)
        ERRCHECK (result)
    
        If currentsubsoundid <> subsoundid Then
            ' Release the sound that isn't playing any more.
            result = FMOD_Sound_Release(Subsound(subsoundid))
            ERRCHECK (result)
    
            ' Replace it with a new sound in our list.
            result = FMOD_System_CreateStream(system, soundname(sentenceid), FMOD_DEFAULT, Subsound(subsoundid))
            ERRCHECK (result)
    
            result = FMOD_Sound_SetSubSound(Sound, subsoundid, Subsound(subsoundid))
            ERRCHECK (result)

            sentenceid = sentenceid + 1
            If sentenceid >= numsounds Then
                sentenceid = 0
            End If
    
            subsoundid = currentsubsoundid
        End If
    End If
    
    If system Then
        result = FMOD_System_Update(system)
        ERRCHECK (result)
    End If
End Sub

Private Sub cmdPause_Click()
    Dim result As FMOD_RESULT
    Dim paused As Long
    
    If channel Then
        result = FMOD_Channel_GetPaused(channel, paused)
        ERRCHECK (result)
     
        If paused Then
            result = FMOD_Channel_SetPaused(channel, 0)
        Else
            result = FMOD_Channel_SetPaused(channel, 1)
        End If
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
