VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Effects Example"
   ClientHeight    =   3165
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   3675
   LinkTopic       =   "Form1"
   ScaleHeight     =   3165
   ScaleWidth      =   3675
   StartUpPosition =   3  'Windows Default
   Begin VB.CheckBox chkParameq 
      Caption         =   "parameq"
      Height          =   375
      Left            =   2280
      TabIndex        =   9
      Top             =   1320
      Width           =   1095
   End
   Begin VB.CheckBox chkChorus 
      Caption         =   "chorus"
      Height          =   375
      Left            =   2280
      TabIndex        =   8
      Top             =   960
      Width           =   975
   End
   Begin VB.CheckBox chkDistortion 
      Caption         =   "distortion"
      Height          =   375
      Left            =   2280
      TabIndex        =   7
      Top             =   600
      Width           =   1095
   End
   Begin VB.CheckBox chkFlange 
      Caption         =   "flange"
      Height          =   375
      Left            =   360
      TabIndex        =   6
      Top             =   1680
      Width           =   1215
   End
   Begin VB.CheckBox chkEcho 
      Caption         =   "echo"
      Height          =   375
      Left            =   360
      TabIndex        =   5
      Top             =   1320
      Width           =   1215
   End
   Begin VB.CheckBox chkHighPass 
      Caption         =   "highpass"
      Height          =   375
      Left            =   360
      TabIndex        =   4
      Top             =   960
      Width           =   1095
   End
   Begin VB.CheckBox chkLowPass 
      Caption         =   "lowpass"
      Height          =   375
      Left            =   360
      TabIndex        =   3
      Top             =   600
      Width           =   1095
   End
   Begin VB.CommandButton cmdPause 
      Caption         =   "Pause / Resume"
      Height          =   375
      Left            =   480
      TabIndex        =   2
      Top             =   2160
      Width           =   2655
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1320
      TabIndex        =   1
      Top             =   2640
      Width           =   975
   End
   Begin VB.Label copyright 
      Alignment       =   2  'Center
      Caption         =   "Copyright (c) Firelight Technologies 2004-2005"
      Height          =   375
      Left            =   -120
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
' effects example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.
'
' This example shows how to apply some of the built in software effects to sounds.
' This example filters the global mix.  All software sounds played here would be filtered in the
' same way.
' To filter per channel, and not have other channels affected, simply replace system->addDSP with
' channel->addDSP.
' Note in this example you don't have to add and remove units each time, you could simply add them
' all at the start then use DSP::setActive to toggle them on and off.
' ===============================================================================================
Option Explicit

Dim system As Long
Dim Sound As Long
Dim channel As Long
Dim dsplowpass As Long
Dim dsphighpass As Long
Dim dspecho As Long
Dim dspflange As Long
Dim dspdistortion As Long
Dim dspchorus As Long
Dim dspparameq As Long

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
        
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
    
    result = FMOD_System_CreateSound(system, "../../examples/media/drumloop.wav", FMOD_SOFTWARE, Sound)
    ERRCHECK (result)
    
    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, Sound, 0, channel)
    ERRCHECK (result)

    
    '    Create some effects to play with.
    
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_LOWPASS, dsplowpass)
    ERRCHECK (result)
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_HIGHPASS, dsphighpass)
    ERRCHECK (result)
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_ECHO, dspecho)
    ERRCHECK (result)
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_FLANGE, dspflange)
    ERRCHECK (result)
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_DISTORTION, dspdistortion)
    ERRCHECK (result)
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_CHORUS, dspchorus)
    ERRCHECK (result)
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_PARAMEQ, dspparameq)
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

Private Sub chkChorus_Click()
    Dim result As FMOD_RESULT
    Dim active As Long

    result = FMOD_DSP_GetActive(dspchorus, active)
    ERRCHECK (result)

    If active Then
        result = FMOD_DSP_Remove(dspchorus)
        ERRCHECK (result)
    Else
        result = FMOD_System_AddDSP(system, dspchorus, 0)
        ERRCHECK (result)
    End If
End Sub

Private Sub chkDistortion_Click()
    Dim result As FMOD_RESULT
    Dim active As Long

    result = FMOD_DSP_GetActive(dspdistortion, active)
    ERRCHECK (result)

    If active Then
        result = FMOD_DSP_Remove(dspdistortion)
        ERRCHECK (result)
    Else
        result = FMOD_System_AddDSP(system, dspdistortion, 0)
        ERRCHECK (result)
    End If
End Sub

Private Sub chkEcho_Click()
    Dim result As FMOD_RESULT
    Dim active As Long

    result = FMOD_DSP_GetActive(dspecho, active)
    ERRCHECK (result)

    If active Then
        result = FMOD_DSP_Remove(dspecho)
        ERRCHECK (result)
    Else
        result = FMOD_System_AddDSP(system, dspecho, 0)
        ERRCHECK (result)
    End If
End Sub

Private Sub chkFlange_Click()
    Dim result As FMOD_RESULT
    Dim active As Long

    result = FMOD_DSP_GetActive(dspflange, active)
    ERRCHECK (result)

    If active Then
        result = FMOD_DSP_Remove(dspflange)
        ERRCHECK (result)
    Else
        result = FMOD_System_AddDSP(system, dspflange, 0)
        ERRCHECK (result)
    End If
End Sub

Private Sub chkHighPass_Click()
    Dim result As FMOD_RESULT
    Dim active As Long

    result = FMOD_DSP_GetActive(dsphighpass, active)
    ERRCHECK (result)

    If active Then
        result = FMOD_DSP_Remove(dsphighpass)
        ERRCHECK (result)
    Else
        result = FMOD_System_AddDSP(system, dsphighpass, 0)
        ERRCHECK (result)
    End If
End Sub

Private Sub chkLowPass_Click()
    Dim result As FMOD_RESULT
    Dim active As Long

    result = FMOD_DSP_GetActive(dsplowpass, active)
    ERRCHECK (result)

    If active Then
        result = FMOD_DSP_Remove(dsplowpass)
        ERRCHECK (result)
    Else
        result = FMOD_System_AddDSP(system, dsplowpass, 0)
        ERRCHECK (result)
    End If
End Sub

Private Sub chkParameq_Click()
    Dim result As FMOD_RESULT
    Dim active As Long

    result = FMOD_DSP_GetActive(dspparameq, active)
    ERRCHECK (result)

    If active Then
        result = FMOD_DSP_Remove(dspparameq)
        ERRCHECK (result)
    Else
        result = FMOD_System_AddDSP(system, dspparameq, 0)
        ERRCHECK (result)
    End If
End Sub

Private Sub cmdPause_Click()
    Dim result As FMOD_RESULT
    Dim paused As Long

    result = FMOD_Channel_GetPaused(channel, paused)
    ERRCHECK (result)

    If paused Then
        result = FMOD_Channel_SetPaused(channel, 0)
        ERRCHECK (result)
    Else
        result = FMOD_Channel_SetPaused(channel, 1)
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
