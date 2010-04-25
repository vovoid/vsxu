Attribute VB_Name = "callbacks"
Public gSoundType As FMOD_SOUND_TYPE
Public gUpdateFileName As Boolean
Public gCurrentTrackArtist As String
Public gCurrentTrackTitle As String
Public gOutputFileName As String
Private gFileHandle As Long
Public gBuffer As Long
Public gSizebytes As Long
Public gMyopen As Boolean
Public gMyclose As Boolean
Public gMyread As Boolean

Public Function myopen(ByVal name As Long, ByVal unicode As Long, ByVal filesize As Long, ByRef Handle As Long, ByRef userdata As Long) As FMOD_RESULT
    
    gMyopen = True  ' Signals opencallback() function to be called from main loop
    
    myopen = FMOD_OK
End Function

Public Function myclose(ByVal Handle As Long, ByVal userdata As Long) As FMOD_RESULT

    gMyclose = True ' Signals closecallback() function to be called from main loop
    
    myclose = FMOD_OK
End Function

Public Function myread(ByVal Handle As Long, ByVal Buffer As Long, ByVal sizebytes As Long, ByRef bytesread As Long, ByRef userdata) As FMOD_RESULT
        
    gBuffer = Buffer
    gSizebytes = sizebytes
    
    gMyread = True ' Signals readcallback() function to be called from main loop

    myread = FMOD_OK
End Function

Public Function opencallback()
    ' Create the file
    Open gOutputFileName For Random As #1
    Close #1
     
    gFileHandle = lOpen(gOutputFileName, 1)
    
    gMyopen = False
End Function

Public Function closecallback()
    lClose (gFileHandle)
    
    gMyclose = False
End Function

Public Function readcallback()
'    If gUpdateFileName Then
'        Dim ext As String
  
'        gUpdateFileName = False

'        lClose (gFileHandle)

'        Select Case gSoundType
'            Case FMOD_SOUND_TYPE_MPEG:       ' MP2/MP3
'                ext = ".mp3"
   
'            Case FMOD_SOUND_TYPE_OGGVORBIS:  ' Ogg vorbis
'                ext = ".ogg"
      
'            Case Else:
'                ext = ".unknown"
'        End Select
  
        ' If it is the 'temp file name' then rename the file and append to it instead of starting a new file
'        If gOutputFileName = "output.mp3" Then
'            gOutputFileName = gCurrentTrackArtist & IIf(gCurrentTrackTitle = "", " - ", "") & gCurrentTrackTitle & ext
     
'            Name "output.mp3" As gOutputFileName
       
'            gFileHandle = lOpen(gOutputFileName, 1)
'        Else
'            gOutputFileName = gCurrentTrackArtist & IIf(gCurrentTrackTitle = "", " - ", "") & gCurrentTrackTitle & ext
'            gFileHandle = lOpen(gOutputFileName, 1)
'        End If
'    End If
  
    Call lWrite(gFileHandle, gBuffer, gSizebytes)
    
    gMyread = False
End Function
