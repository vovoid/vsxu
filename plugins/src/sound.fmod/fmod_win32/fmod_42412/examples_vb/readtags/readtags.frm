VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Read Tags Example"
   ClientHeight    =   4005
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   5580
   LinkTopic       =   "Form1"
   ScaleHeight     =   4005
   ScaleWidth      =   5580
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox ListBox 
      Height          =   2790
      Left            =   120
      TabIndex        =   2
      Top             =   480
      Width           =   5295
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   2280
      TabIndex        =   1
      Top             =   3480
      Width           =   975
   End
   Begin VB.Label copyright 
      Alignment       =   2  'Center
      Caption         =   "Copyright (c) Firelight Technologies 2004-2005"
      Height          =   495
      Left            =   840
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
' readtags example
' Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.
'
' This example shows how to read tags from sound files
' ===============================================================================================
Option Explicit

Dim system As Long
Dim sound As Long

Private Sub Form_Load()
    Dim result As FMOD_RESULT
    Dim version As Long
    Dim tag As FMOD_TAG
    Dim numtags As Long
    Dim numtagsupdated As Long
    Dim count As Long
    Dim tagname As String
    Dim tagdata As String
    Dim nullstring As String
    
    ' Create a System object and initialize.
    result = FMOD_System_Create(system)
    ERRCHECK (result)
    
    result = FMOD_System_GetVersion(system, version)
    ERRCHECK (result)

    If version <> FMOD_VERSION Then
        MsgBox "Error!  You are using an old version of FMOD " & Hex$(version) & ". " & _
               "This program requires " & Hex$(FMOD_VERSION)
    End If
    
    result = FMOD_System_Init(system, 100, FMOD_INIT_NORMAL, 0)
    ERRCHECK (result)
    
    ' Open the specified file. Use FMOD_CREATESTREAM and FMOD_DONTPREBUFFER so it opens quickly
    result = FMOD_System_CreateSound(system, "../../examples/media/wave.mp3", (FMOD_SOFTWARE Or FMOD_2D Or FMOD_CREATESTREAM Or FMOD_OPENONLY), sound)
    ERRCHECK (result)

    
    ' Read and display all tags associated with this file
    Do While True
        ' An index of -1 means "get the first tag that's new or updated".
        ' If no tags are new or updated then getTag will return FMOD_ERR_TAGNOTFOUND.
        ' This is the first time we've read any tags so they'll all be new but after we've read them,
        ' they won 't be new any more.
        result = FMOD_Sound_GetTag(sound, nullstring, -1, tag)
        If result <> FMOD_OK Then
            GoTo break
        End If
        
        tagname = Space(lstrlen(tag.name))
        Call lstrcpy(tagname, tag.name)
        
        If tag.datatype = FMOD_TAGDATATYPE_STRING Then
            tagdata = Space(tag.datalen - 1)
            Call lstrcpy(tagdata, tag.data)
            
            ListBox.AddItem tagname & " = " & tagdata & " (" & tag.datalen & " bytes)"
        Else
            ListBox.AddItem tagname & " = <binary>" & " (" & tag.datalen & " bytes)"
        End If
    Loop

break:

    ListBox.AddItem " "

    ' Read all the tags regardless of whether they're updated or not. Also show the tag type.
    result = FMOD_Sound_GetNumTags(sound, numtags, numtagsupdated)
    ERRCHECK (result)
    Do While count < numtags
        Dim tagtext As String
        
        result = FMOD_Sound_GetTag(sound, nullstring, count, tag)
        ERRCHECK (result)

        Select Case tag.type
            Case FMOD_TAGTYPE_UNKNOWN:
                tagtext = "FMOD_TAGTYPE_UNKNOWN  "

            Case FMOD_TAGTYPE_ID3V1:
                tagtext = "FMOD_TAGTYPE_ID3V1  "

            Case FMOD_TAGTYPE_ID3V2:
                tagtext = "FMOD_TAGTYPE_ID3V2  "

            Case FMOD_TAGTYPE_VORBISCOMMENT:
                tagtext = "FMOD_TAGTYPE_VORBISCOMMENT  "

            Case FMOD_TAGTYPE_SHOUTCAST:
                tagtext = "FMOD_TAGTYPE_SHOUTCAST  "
            
            Case FMOD_TAGTYPE_ICECAST:
                tagtext = "FMOD_TAGTYPE_ICECAST  "

            Case FMOD_TAGTYPE_ASF:
                tagtext = "FMOD_TAGTYPE_ASF  "

            Case FMOD_TAGTYPE_FMOD:
                tagtext = "FMOD_TAGTYPE_FMOD  "

            Case FMOD_TAGTYPE_USER:
                tagtext = "FMOD_TAGTYPE_USER  "
        End Select

        tagname = Space(lstrlen(tag.name))
        Call lstrcpy(tagname, tag.name)
        
        If tag.datatype = FMOD_TAGDATATYPE_STRING Then
            tagdata = Space(tag.datalen - 1)
            Call lstrcpy(tagdata, tag.data)
            
            ListBox.AddItem tagtext & tagname & " = " & tagdata & " (" & tag.datalen & " bytes)"
        Else
            ListBox.AddItem tagtext & tagname & " = ??? (" & tag.datalen & " bytes)"
        End If
        count = count + 1
    Loop

    ListBox.AddItem " "

    ' Find a specific tag by name. Specify an index > 0 to get access to multiple tags of the same name.
    result = FMOD_Sound_GetTag(sound, "ARTIST", 0, tag)
    ERRCHECK (result)

    tagname = Space(lstrlen(tag.name))
    Call lstrcpy(tagname, tag.name)
    tagdata = Space(tag.datalen - 1)
    Call lstrcpy(tagdata, tag.data)
    
    ListBox.AddItem tagname & " = " & tagdata & " (" & tag.datalen & " bytes)"
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
