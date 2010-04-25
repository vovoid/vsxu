VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Spectrum Example"
   ClientHeight    =   5025
   ClientLeft      =   885
   ClientTop       =   1215
   ClientWidth     =   8055
   LinkTopic       =   "Form1"
   ScaleHeight     =   5025
   ScaleWidth      =   8055
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   3480
      TabIndex        =   5
      Top             =   4440
      Width           =   975
   End
   Begin VB.Timer Timer 
      Interval        =   10
      Left            =   5760
      Top             =   4440
   End
   Begin VB.CommandButton cmdPause 
      Caption         =   "Pause"
      Height          =   375
      Left            =   4080
      TabIndex        =   1
      Top             =   3720
      Width           =   1335
   End
   Begin VB.CommandButton cmdPlay 
      Caption         =   "Play"
      Height          =   375
      Left            =   2520
      TabIndex        =   0
      Top             =   3720
      Width           =   1335
   End
   Begin VB.Frame Frame1 
      Caption         =   "Audio file to play"
      Height          =   1215
      Left            =   120
      TabIndex        =   2
      Top             =   3000
      Width           =   7815
      Begin VB.CommandButton cmdFileOpen 
         Caption         =   "..."
         Height          =   375
         Left            =   7080
         TabIndex        =   4
         Top             =   240
         Width           =   495
      End
      Begin VB.TextBox Text1 
         Height          =   375
         Left            =   240
         TabIndex        =   3
         Top             =   240
         Width           =   6735
      End
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ===============================================================================================
' Spectrum Example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to draw a spectrum as well as an oscilliscope of the wave data
'
' ===============================================================================================

Option Explicit

Dim System As Long
Dim Sound As Long
Dim Channel As Long
Dim szFile(20481) As Byte
Dim szFileTitle(4096) As Byte

Dim brushBlack As Long
Dim brushWhite As Long
Dim brushGreen As Long

Private Const GRAPHICWINDOW_WIDTH As Long = 540
Private Const GRAPHICWINDOW_HEIGHT As Long = 200


Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim Version As Long
    
    ' Create the brushes we will be using
    brushBlack = CreateSolidBrush(RGB(0, 0, 0))
    brushWhite = CreateSolidBrush(RGB(255, 255, 255))
    brushGreen = CreateSolidBrush(RGB(0, 255, 0))
    
    ' Create a System object and initialize.
    result = FMOD_System_Create(System)
    ERRCHECK (result)
    
    result = FMOD_System_GetVersion(System, Version)
    ERRCHECK (result)

    If Version <> FMOD_VERSION Then
        MsgBox "Error!  You are using an old version of FMOD " & Hex$(Version) & ". " & _
               "This program requires " & Hex$(FMOD_VERSION)
    End If

    result = FMOD_System_Init(System, 32, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)

End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim result As FMOD_RESULT
    
    ' Shut down
    If Sound Then
        result = FMOD_Sound_Release(Sound)
        ERRCHECK (result)
    End If
    
    If System Then
        result = FMOD_System_Close(System)
        ERRCHECK (result)
        
        result = FMOD_System_Release(System)
        ERRCHECK (result)
    End If
End Sub

Private Sub cmdFileOpen_Click()
    Dim result As FMOD_RESULT
    Dim ofn As OPENFILENAME
                    
    ofn.lStructSize = Len(ofn)
    ofn.hwndOwner = Me.Hwnd
    ofn.lpstrTitle = "Open"
    
    ofn.lpstrFilter = "All audio files (*.*)"
    ofn.nFilterIndex = 1
    ofn.lpstrFile = szFile
    ofn.nMaxFile = 20480
    ofn.lpstrFileTitle = szFileTitle
    ofn.nMaxFileTitle = 2048
    ofn.lpstrInitialDir = "."
    ofn.Flags = OFN_EXPLORER Or OFN_PATHMUSTEXIST Or OFN_FILEMUSTEXIST
    
    If Sound Then
        If Channel Then
            Call FMOD_Channel_Stop(Channel)
            Channel = 0
        End If
        
        Call FMOD_Sound_Release(Sound)
        Sound = 0
    End If
    
    If GetOpenFileName(ofn) Then
        Text1.Text = ofn.lpstrFile
    
        ' Create the stream
        result = FMOD_System_CreateStream(System, ofn.lpstrFile, FMOD_2D Or FMOD_SOFTWARE, Sound)
        ERRCHECK (result)
    End If
    
    cmdPlay.Caption = "Play"
End Sub

Private Sub cmdPlay_Click()
    Dim result As FMOD_RESULT
    Dim Isplaying As Long
    
    If Channel Then
        Call FMOD_Channel_IsPlaying(Channel, Isplaying)
    End If
    
    If Sound And Isplaying = 0 Then
        result = FMOD_System_PlaySound(System, FMOD_CHANNEL_FREE, Sound, 0, Channel)
        ERRCHECK (result)
        
        cmdPlay.Caption = "Stop"
    Else
        If Channel Then
            Call FMOD_Channel_Stop(Channel)
            Channel = 0
        End If
        
        cmdPlay.Caption = "Play"
    End If
End Sub

Private Sub cmdPause_Click()
    Dim result As FMOD_RESULT
    Dim Paused As Long
    
    If Channel Then
        result = FMOD_Channel_GetPaused(Channel, Paused)
        ERRCHECK (result)
        
        If Paused Then
            cmdPause.Caption = "Pause"
            result = FMOD_Channel_SetPaused(Channel, 0)
        Else
            cmdPause.Caption = "Unpause"
            result = FMOD_Channel_SetPaused(Channel, 1)
        End If
    End If
End Sub

Private Sub DrawSpectrum(hdcbuffer As Long)
    Dim result As FMOD_RESULT
    Dim spectrum(512) As Single
    Dim count As Long
    Dim count2 As Long
    Dim Numchannels As Long
    Dim line As RECT
    Dim max As Single
    
    result = FMOD_System_GetSoftwareFormat(System, 0, 0, Numchannels, 0, 0, 0)
    ERRCHECK (result)
    
    '
    ' Draw Spectrum
    '
    For count = 0 To Numchannels - 1
        result = FMOD_System_GetSpectrum(System, spectrum(0), 512, count, FMOD_DSP_FFT_WINDOW_TRIANGLE)
        ERRCHECK (result)
        
        For count2 = 0 To 255
            If max < spectrum(count2) Then
                max = spectrum(count2)
            End If
        Next
        
        ' Draw the actual spectrum
        ' The upper band of frequencies at 44khz is pretty boring (ie 11-22khz), so we are only
        ' going to display the first 256 frequencies, or (0-11khz)
        For count2 = 0 To 255
            Dim height As Single
            
            height = spectrum(count2) / max * GRAPHICWINDOW_HEIGHT
            
            If height >= GRAPHICWINDOW_HEIGHT Then
                height = height - 1#
            End If
            
            If height < 0 Then
                height = 0#
            End If
            
            height = GRAPHICWINDOW_HEIGHT - height
            
            line.Bottom = GRAPHICWINDOW_HEIGHT
            line.Top = height
            line.Left = count2
            line.Right = count2 + 1

            FillRect hdcbuffer, line, brushGreen
        Next
    Next
End Sub

Private Sub DrawOscilliscope(hdcbuffer As Long)
    Dim result As FMOD_RESULT
    Dim wavedata(256) As Single
    Dim count As Long
    Dim count2 As Long
    Dim Numchannels As Long
    Dim line As RECT

    result = FMOD_System_GetSoftwareFormat(System, 0, 0, Numchannels, 0, 0, 0)
    ERRCHECK (result)
        
    '
    ' Draw Wave Data
    '
    For count = 0 To Numchannels - 1
        result = FMOD_System_GetWaveData(System, wavedata(0), 256, count)
        ERRCHECK (result)
        
        ' Draw the oscilliscope
        For count2 = 0 To 255
            Dim Y As Single

            Y = ((wavedata(count2) + 1#) / 2#) * GRAPHICWINDOW_HEIGHT
            
            line.Bottom = Y
            line.Top = Y + 1
            line.Left = count2 + GRAPHICWINDOW_WIDTH / 2
            line.Right = count2 + GRAPHICWINDOW_WIDTH / 2 + 1
            
            FillRect hdcbuffer, line, brushWhite
        Next
    Next
End Sub

Private Sub Timer_Timer()
    Dim result As FMOD_RESULT
    Dim hdc As Long
    Dim hdcbuffer As Long
    Dim hdcmem As Long
    Dim hbmold As Long
    Dim hbmbuffer As Long
    Dim hbmoldbuffer As Long
    Dim rectangle As RECT
            
    hdc = GetDC(Me.Hwnd)
    hdcbuffer = CreateCompatibleDC(hdc)
    hbmbuffer = CreateCompatibleBitmap(hdc, GRAPHICWINDOW_WIDTH, GRAPHICWINDOW_HEIGHT)
    hbmoldbuffer = SelectObject(hdcbuffer, hbmbuffer)
    hdcmem = CreateCompatibleDC(hdc)
    hbmold = SelectObject(hdcmem, 0)
    
    GetClientRect Me.Hwnd, rectangle
    
    FillRect hdcbuffer, rectangle, brushBlack
    
    If System Then
        DrawSpectrum (hdcbuffer)
        DrawOscilliscope (hdcbuffer)
        
        result = FMOD_System_Update(System)
        ERRCHECK (result)
    End If
    
    BitBlt hdc, rectangle.Left, rectangle.Top, GRAPHICWINDOW_WIDTH, GRAPHICWINDOW_HEIGHT, hdcbuffer, rectangle.Left, rectangle.Top, vbSrcCopy

    SelectObject hdcmem, hbmold
    DeleteDC hdcmem
    SelectObject hdcbuffer, hbmoldbuffer
    DeleteObject hbmbuffer
    DeleteDC hdcbuffer
    ReleaseDC Me.Hwnd, hdc
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
