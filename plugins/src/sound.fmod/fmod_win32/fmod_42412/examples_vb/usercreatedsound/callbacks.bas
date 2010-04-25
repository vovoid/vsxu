Attribute VB_Name = "callbacks"
Function pcmreadcallback(ByVal Sound As Long, ByVal data As Long, ByVal datalen As Long) As FMOD_RESULT
    Dim count As Long
    Dim t1, t2 As Single
    Dim v1, v2 As Single
    
    Dim stereo16bitbuffer() As Integer

    ReDim stereo16bitbuffer(datalen * 2)
    
    Do While count < (datalen * 2)
        stereo16bitbuffer(count) = Sin(t1) * 32767
        count = count + 1
        stereo16bitbuffer(count) = Sin(t2) * 32767
        count = count + 1
      
        t1 = t1 + 0.01 + v2
        t2 = t2 + 0.0142 + v2
        v1 = v1 + Sin(t1) * 0.002
        v2 = v2 + Sin(t2) * 0.002
    Loop

    CopyMemory ByVal data, stereo16bitbuffer(0), (datalen)

    pcmreadcallback = FMOD_OK
End Function

Function pcmsetposcallback(ByVal Sound As Long, ByVal subsound As Long, ByVal pcmoffset As Long, ByVal postype As FMOD_TIMEUNIT) As FMOD_RESULT

    ' This is useful if the user calls FMOD_Sound_SetTime or FMOD_Sound_SetPosition and you want to seek your data accordingly.
    
    pcmseekcallback = FMOD_OK
End Function
