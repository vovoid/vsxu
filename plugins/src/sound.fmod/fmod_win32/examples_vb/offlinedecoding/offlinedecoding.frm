VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Offline Decoding Example"
   ClientHeight    =   1875
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   3975
   LinkTopic       =   "Form1"
   ScaleHeight     =   1875
   ScaleWidth      =   3975
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   1440
      TabIndex        =   2
      Top             =   1080
      Width           =   975
   End
   Begin VB.CommandButton cmdStart 
      Caption         =   "Start"
      Height          =   495
      Left            =   960
      TabIndex        =   1
      Top             =   480
      Width           =   1935
   End
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   0
      Top             =   1500
      Width           =   3975
      _ExtentX        =   7011
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
      TabIndex        =   3
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
' Offline Decoding Example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how decode a file to PCM, without playing it.
' It writes out the data as a raw data file.
' The FMOD_System_CreateSound function uses FMOD_OPENONLY so that FMOD does not read any data
' itself.
' If this is uses then it is up to the user to use FMOD_Sound_ReadData to get the data out of
' the file and into the destination buffer.
' ===============================================================================================

Option Explicit

Private Sub cmdStart_Click()
    Dim System As Long
    Dim Sound As Long
    Dim result As FMOD_RESULT
    Dim Version As Long
    
    Dim Data(0 To 4095) As Byte
    Dim Length As Long
    Dim Read As Long
    Dim bytesread As Long
    Dim outfp As Long
    
    ' Create a System object and initialize.
    result = FMOD_System_Create(System)
    ERRCHECK (result)
    
    result = FMOD_System_GetVersion(System, Version)
    ERRCHECK (result)

    If Version <> FMOD_VERSION Then
        MsgBox "Error!  You are using an old version of FMOD " & Hex$(Version) & ". " & _
               "This program requires " & Hex$(FMOD_VERSION)
    End If

    result = FMOD_System_Init(System, 1, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    result = FMOD_System_CreateStream(System, "../../examples/media/wave.mp3", FMOD_OPENONLY Or FMOD_ACCURATETIME, Sound)
    ERRCHECK (result)
    
    '
    '   Decode the sound and write it to a .raw file.
    '
    result = FMOD_Sound_GetLength(Sound, Length, FMOD_TIMEUNIT_PCMBYTES)
    ERRCHECK (result)

    Open "output.raw" For Random As #1
    Close #1
    outfp = lOpen("output.raw", 1)
    
    bytesread = 0
    
    Do
        result = FMOD_Sound_ReadData(Sound, GetAddrOf(Data(0)), 4096, Read)
    
        bytesread = bytesread + Read
        
        Call lWrite(outfp, GetAddrOf(Data(0)), Read)
        
        StatusBar.SimpleText = "writing " & bytesread & " bytes of " & Length & " to output.raw"
    Loop While (result = FMOD_OK And Read = 4096)
    
    StatusBar.SimpleText = "done"
    
    lClose (outfp)
    
    ' Shut down
    result = FMOD_Sound_Release(Sound)
    ERRCHECK (result)
    
    result = FMOD_System_Close(System)
    ERRCHECK (result)
    
    result = FMOD_System_Release(System)
    ERRCHECK (result)
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
