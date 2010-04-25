Attribute VB_Name = "fmodex"
' ============================================================================================ '
'                                                                                              '
' FMOD Ex - Visual Basic Module.    Copyright (c), Firelight Technologies Pty, Ltd. 2004-2009. '
'                                                                                              '
' ============================================================================================ '

'
'   FMOD version number.  Check this against FMOD::System::getVersion.
'   0xaaaabbcc -> aaaa = major version number.  bb = minor version number.  cc = development version number.
'

Public Const FMOD_VERSION As Long = &H42412

'
'[Structure]
'[
    '[description]
    'Structure describing a point in 3D space.

    '[REMARKS]
    'FMOD uses a left handed co-ordinate system by default.<br>
    'To use a right handed co-ordinate system specify FMOD_INIT_3D_RIGHTHANDED from FMOD_INITFLAGS in System::init.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_System_Set3DListenerAttributes
    'FMOD_System_Get3DListenerAttributes
    'FMOD_Channel_Set3DAttributes
    'FMOD_Channel_Get3DAttributes
    'FMOD_Geometry_AddPolygon
    'FMOD_Geometry_SetPolygonVertex
    'FMOD_Geometry_GetPolygonVertex
    'FMOD_Geometry_SetRotation
    'FMOD_Geometry_GetRotation
    'FMOD_Geometry_SetPosition
    'FMOD_Geometry_GetPosition
    'FMOD_Geometry_SetScale
    'FMOD_Geometry_GetScale
    'FMOD_INITFLAGS
']
'
Public Type FMOD_VECTOR
    X As Single
    Y As Single
    z As Single
End Type


'
'[STRUCTURE]
'[
    '[DESCRIPTION]
    'Structure describing a globally unique identifier.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii, Solaris

    '[SEE_ALSO]
    'System::getDriverInfo
']
'
Public Type FMOD_GUID
    Data1 As Long       ' Specifies the first 8 hexadecimal digits of the GUID '
    Data2 As Integer    ' Specifies the first group of 4 hexadecimal digits.   '
    Data3 As Integer    ' Specifies the second group of 4 hexadecimal digits.  '
    Data4(8) As Byte    ' Array of 8 bytes. The first 2 bytes contain the third group of 4 hexadecimal digits. The remaining 6 bytes contain the final 12 hexadecimal digits. '
End Type


'
'[ENUM]
'[
    '[Description]
    'error codes.  Returned from every function.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
']
'
Public Enum FMOD_RESULT
    FMOD_OK                        ' No errors. */
    FMOD_ERR_ALREADYLOCKED         ' Tried to call lock a second time before unlock was called. */
    FMOD_ERR_BADCOMMAND            ' Tried to call a function on a data type that does not allow this type of functionality (ie calling Sound::lock on a streaming sound). */
    FMOD_ERR_CDDA_DRIVERS          ' Neither NTSCSI nor ASPI could be initialised. */
    FMOD_ERR_CDDA_INIT             ' An error occurred while initialising the CDDA subsystem. */
    FMOD_ERR_CDDA_INVALID_DEVICE   ' Couldn't find the specified device. */
    FMOD_ERR_CDDA_NOAUDIO          ' No audio tracks on the specified disc. */
    FMOD_ERR_CDDA_NODEVICES        ' No CD/DVD devices were found. */
    FMOD_ERR_CDDA_NODISC           ' No disc present in the specified drive. */
    FMOD_ERR_CDDA_READ             ' A CDDA read error occurred. */
    FMOD_ERR_CHANNEL_ALLOC         ' Error trying to allocate a channel. */
    FMOD_ERR_CHANNEL_STOLEN        ' The specified channel has been reused to play another sound. */
    FMOD_ERR_COM                   ' A Win32 COM related error occured. COM failed to initialize or a QueryInterface failed meaning a Windows codec or driver was not installed properly. */
    FMOD_ERR_DMA                   ' DMA Failure.  See debug output for more information. */
    FMOD_ERR_DSP_CONNECTION        ' DSP connection error.  Connection possibly caused a cyclic dependancy. */
    FMOD_ERR_DSP_FORMAT            ' DSP Format error.  A DSP unit may have attempted to connect to this network with the wrong format. */
    FMOD_ERR_DSP_NOTFOUND          ' DSP connection error.  Couldn't find the DSP unit specified. */
    FMOD_ERR_DSP_RUNNING           ' DSP error.  Cannot perform this operation while the network is in the middle of running.  This will most likely happen if a connection or disconnection is attempted in a DSP callback. */
    FMOD_ERR_DSP_TOOMANYCONNECTIONS ' DSP connection error.  The unit being connected to or disconnected should only have 1 input or output. */
    FMOD_ERR_FILE_BAD              ' Error loading file. */
    FMOD_ERR_FILE_COULDNOTSEEK     ' Couldn't perform seek operation.  This is a limitation of the medium (ie netstreams) or the file format. */
    FMOD_ERR_FILE_DISKEJECTED      ' Media was ejected while reading. */
    FMOD_ERR_FILE_EOF              ' End of file unexpectedly reached while trying to read essential data (truncated data?). */
    FMOD_ERR_FILE_NOTFOUND         ' File not found. */
    FMOD_ERR_FILE_UNWANTED         ' Unwanted file access occured. */
    FMOD_ERR_FORMAT                ' Unsupported file or audio format. */
    FMOD_ERR_HTTP                  ' A HTTP error occurred. This is a catch-all for HTTP errors not listed elsewhere. */
    FMOD_ERR_HTTP_ACCESS           ' The specified resource requires authentication or is forbidden. */
    FMOD_ERR_HTTP_PROXY_AUTH       ' Proxy authentication is required to access the specified resource. */
    FMOD_ERR_HTTP_SERVER_ERROR     ' A HTTP server error occurred. */
    FMOD_ERR_HTTP_TIMEOUT          ' The HTTP request timed out. */
    FMOD_ERR_INITIALIZATION        ' FMOD was not initialized correctly to support this function. */
    FMOD_ERR_INITIALIZED           ' Cannot call this command after System::init. */
    FMOD_ERR_INTERNAL              ' An error occured that wasn't supposed to.  Contact support. */
    FMOD_ERR_INVALID_ADDRESS       ' On Xbox 360 this memory address passed to FMOD must be physical (ie allocated with XPhysicalAlloc.) */
    FMOD_ERR_INVALID_FLOAT         ' Value passed in was a NaN, Inf or denormalized float. */
    FMOD_ERR_INVALID_HANDLE        ' An invalid object handle was used. */
    FMOD_ERR_INVALID_PARAM         ' An invalid parameter was passed to this function. */
    FMOD_ERR_INVALID_SPEAKER       ' An invalid speaker was passed to this function based on the current speaker mode. */
    FMOD_ERR_INVALID_SYNCPOINT     ' The syncpoint did not come from this sound handle. */
    FMOD_ERR_INVALID_VECTOR        ' The vectors passed in are not unit length, or perpendicular. */
    FMOD_ERR_IRX                   ' PS2 only.  fmodex.irx failed to initialize.  This is most likely because you forgot to load it. */
    FMOD_ERR_MAXAUDIBLE            ' Reached maximum audible playback count for this sound's soundgroup. */
    FMOD_ERR_MEMORY                ' Not enough memory or resources. */
    FMOD_ERR_MEMORY_CANTPOINT      ' Can't use FMOD_OPENMEMORY_POINT on non PCM source data or non mp3/xma/adpcm data if FMOD_CREATECOMPRESSEDSAMPLE was used. */
    FMOD_ERR_MEMORY_IOP            ' PS2 only.  Not enough memory or resources on PlayStation 2 IOP ram. */
    FMOD_ERR_MEMORY_SRAM           ' Not enough memory or resources on console sound ram. */
    FMOD_ERR_NEEDS2D               ' Tried to call a command on a 3d sound when the command was meant for 2d sound. */
    FMOD_ERR_NEEDS3D               ' Tried to call a command on a 2d sound when the command was meant for 3d sound. */
    FMOD_ERR_NEEDSHARDWARE         ' Tried to use a feature that requires hardware support.  (ie trying to play a VAG compressed sound in software on PS2). */
    FMOD_ERR_NEEDSSOFTWARE         ' Tried to use a feature that requires the software engine.  Software engine has either been turned off, or command was executed on a hardware channel which does not support this feature. */
    FMOD_ERR_NET_CONNECT           ' Couldn't connect to the specified host. */
    FMOD_ERR_NET_SOCKET_ERROR      ' A socket error occurred.  This is a catch-all for socket-related errors not listed elsewhere. */
    FMOD_ERR_NET_URL               ' The specified URL couldn't be resolved. */
    FMOD_ERR_NET_WOULD_BLOCK       ' Operation on a non-blocking socket could not complete immediately. */
    FMOD_ERR_NOTREADY              ' Operation could not be performed because specified sound is not ready. */
    FMOD_ERR_OUTPUT_ALLOCATED      ' Error initializing output device, but more specifically, the output device is already in use and cannot be reused. */
    FMOD_ERR_OUTPUT_CREATEBUFFER   ' Error creating hardware sound buffer. */
    FMOD_ERR_OUTPUT_DRIVERCALL     ' A call to a standard soundcard driver failed, which could possibly mean a bug in the driver or resources were missing or exhausted. */
    FMOD_ERR_OUTPUT_ENUMERATION    ' Error enumerating the available driver list. List may be inconsistent due to a recent device addition or removal. */
    FMOD_ERR_OUTPUT_FORMAT         ' Soundcard does not support the minimum features needed for this soundsystem (16bit stereo output). */
    FMOD_ERR_OUTPUT_INIT           ' Error initializing output device. */
    FMOD_ERR_OUTPUT_NOHARDWARE     ' FMOD_HARDWARE was specified but the sound card does not have the resources nescessary to play it. */
    FMOD_ERR_OUTPUT_NOSOFTWARE     ' Attempted to create a software sound but no software channels were specified in System::init. */
    FMOD_ERR_PAN                   ' Panning only works with mono or stereo sound sources. */
    FMOD_ERR_PLUGIN                ' An unspecified error has been returned from a 3rd party plugin. */
    FMOD_ERR_PLUGIN_INSTANCES      ' The number of allowed instances of a plugin has been exceeded */
    FMOD_ERR_PLUGIN_MISSING        ' A requested output dsp unit type or codec was not available. */
    FMOD_ERR_PLUGIN_RESOURCE       ' A resource that the plugin requires cannot be found. (ie the DLS file for MIDI playback) */
    FMOD_ERR_RECORD                ' An error occured trying to initialize the recording device. */
    FMOD_ERR_REVERB_INSTANCE       ' Specified Instance in FMOD_REVERB_PROPERTIES couldn't be set. Most likely because another application has locked the EAX4 FX slot. */
    FMOD_ERR_SUBSOUND_ALLOCATED    ' This subsound is already being used by another sound, you cannot have more than one parent to a sound.  Null out the other parent's entry first. */
    FMOD_ERR_SUBSOUND_CANTMOVE     ' Shared subsounds cannot be replaced or moved from their parent stream, such as when the parent stream is an FSB file. */
    FMOD_ERR_SUBSOUND_MODE         ' The subsound's mode bits do not match with the parent sound's mode bits.  See documentation for function that it was called with. */
    FMOD_ERR_SUBSOUNDS             ' The error occured because the sound referenced contains subsounds.  (ie you cannot play the parent sound as a static sample, only its subsounds.) */
    FMOD_ERR_TAGNOTFOUND           ' The specified tag could not be found or there are no tags. */
    FMOD_ERR_TOOMANYCHANNELS       ' The sound created exceeds the allowable input channel count.  This can be increased using the maxinputchannels parameter in System::setSoftwareFormat. */
    FMOD_ERR_UNIMPLEMENTED         ' Something in FMOD hasn't been implemented when it should be! contact support! */
    FMOD_ERR_UNINITIALIZED         ' This command failed because System::init or System::setDriver was not called. */
    FMOD_ERR_UNSUPPORTED           ' A command issued was not supported by this object.  Possibly a plugin without certain callbacks specified. */
    FMOD_ERR_UPDATE                ' An error caused by System::update occured. */
    FMOD_ERR_VERSION               ' The version number of this file format is not supported. */

    FMOD_ERR_EVENT_FAILED          ' An Event failed to be retrieved, most likely due to 'just fail' being specified as the max playbacks behavior. */
    FMOD_ERR_EVENT_INFOONLY        ' Can't execute this command on an EVENT_INFOONLY event. */
    FMOD_ERR_EVENT_INTERNAL        ' An error occured that wasn't supposed to.  See debug log for reason. */
    FMOD_ERR_EVENT_MAXSTREAMS      ' Event failed because 'Max streams' was hit when FMOD_INIT_FAIL_ON_MAXSTREAMS was specified. */
    FMOD_ERR_EVENT_MISMATCH        ' FSB mis-matches the FEV it was compiled with. */
    FMOD_ERR_EVENT_NAMECONFLICT    ' A category with the same name already exists. */
    FMOD_ERR_EVENT_NOTFOUND        ' The requested event, event group, event category or event property could not be found. */
    FMOD_ERR_EVENT_NEEDSSIMPLE     ' Tried to call a function on a complex event that's only supported by simple events. */
    FMOD_ERR_EVENT_GUIDCONFLICT    ' An event with the same GUID already exists. */

    FMOD_ERR_MUSIC_UNINITIALIZED   ' Music system is not initialized probably because no music data is loaded. */
End Enum


'
'[ENUM]
'[
    '[Description]
    'These output types are used with FMOD_System_SetOutput/FMOD_System_GetOutput, to choose which output method to use.
  
    '[REMARKS]
    'To drive the output synchronously, and to disable FMOD's timing thread, use the FMOD_INIT_NONREALTIME flag.<br>
    '<br>
    'To pass information to the driver when initializing fmod use the extradriverdata parameter for the following reasons.<br>
    '<li>FMOD_OUTPUTTYPE_WAVWRITER - extradriverdata is a pointer to a char * filename that the wav writer will output to.
    '<li>FMOD_OUTPUTTYPE_WAVWRITER_NRT - extradriverdata is a pointer to a char * filename that the wav writer will output to.
    '<li>FMOD_OUTPUTTYPE_DSOUND - extradriverdata is a pointer to a HWND so that FMOD can set the focus on the audio for a particular window.
    '<li>FMOD_OUTPUTTYPE_GC - extradriverdata is a pointer to a FMOD_ARAMBLOCK_INFO struct. This can be found in fmodgc.h.
    'Currently these are the only FMOD drivers that take extra information.  Other unknown plugins may have different requirements.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'System:: setOutput
    'System:: getOutput
    'System:: setSoftwareFormat
    'System:: getSoftwareFormat
    'System:: init
']
'
Public Enum FMOD_OUTPUTTYPE
    FMOD_OUTPUTTYPE_AUTODETECT    ' Picks the best output mode for the platform.  This is the default.

    FMOD_OUTPUTTYPE_UNKNOWN       ' All         - 3rd party plugin, unknown.  This is for use with System::getOutput only.
    FMOD_OUTPUTTYPE_NOSOUND       ' All         - All calls in this mode succeed but make no sound.
    FMOD_OUTPUTTYPE_WAVWRITER     ' All         - Writes output to fmodout.wav by default.  Use System::setSoftwareFormat to set the filename.
    FMOD_OUTPUTTYPE_NOSOUND_NRT   ' All         - Non-realtime version of FMOD_OUTPUTTYPE_NOSOUND.  User can drive mixer with System::update at whatever rate they want. '
    FMOD_OUTPUTTYPE_WAVWRITER_NRT ' All         - Non-realtime version of FMOD_OUTPUTTYPE_WAVWRITER.  User can drive mixer with System::update at whatever rate they want. '
 
    FMOD_OUTPUTTYPE_DSOUND        ' Win32/Win64 - DirectSound output.  Use this to get hardware accelerated 3d audio and EAX Reverb support. (Default on Windows)
    FMOD_OUTPUTTYPE_WINMM         ' Win32/Win64 - Windows Multimedia output.
    FMOD_OUTPUTTYPE_OPENAL        ' Win32/Win64 - OpenAL 1.1 output. Use this for lower CPU overhead than FMOD_OUTPUTTYPE_DSOUND, and also Vista H/W support with Creative Labs cards. */
    FMOD_OUTPUTTYPE_WASAPI        ' Win32       - Windows Audio Session API. (Default on Windows Vista) */
    FMOD_OUTPUTTYPE_ASIO          ' Win32       - Low latency ASIO driver.
    FMOD_OUTPUTTYPE_OSS           ' Linux       - Open Sound System output.
    FMOD_OUTPUTTYPE_ALSA          ' Linux       - Advanced Linux Sound Architecture output.
    FMOD_OUTPUTTYPE_ESD           ' Linux       - Enlightment Sound Daemon output.
    FMOD_OUTPUTTYPE_SOUNDMANAGER  ' Mac         - Macintosh SoundManager output.
    FMOD_OUTPUTTYPE_COREAUDIO     ' Mac         - Macintosh CoreAudio output.
    FMOD_OUTPUTTYPE_XBOX          ' Xbox        - Native hardware output.
    FMOD_OUTPUTTYPE_PS2           ' PS2         - Native hardware output.
    FMOD_OUTPUTTYPE_GC            ' GameCube    - Native hardware output.
    FMOD_OUTPUTTYPE_XBOX360       ' Xbox 360    - Native hardware output.
    FMOD_OUTPUTTYPE_PSP           ' PSP         - Native hardware output.
    FMOD_OUTPUTTYPE_WII           ' Wii                 - Native hardware output. (Default on Wii) */

    FMOD_OUTPUTTYPE_MAX           ' Maximum number of output types supported. '
End Enum


'
'[ENUM]
'[
    '[Description]
    'Bit fields to use with System::getDriverCaps to determine the capabilities of a card / output device.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'System::getDriverCaps
']
'
Public Enum FMOD_CAPS
    FMOD_CAPS_NONE = &H0                             ' Device has no special capabilities.
    FMOD_CAPS_HARDWARE = &H1                         ' Device supports hardware mixing.
    FMOD_CAPS_HARDWARE_EMULATED = &H2                ' User has device set to 'Hardware acceleration = off' in control panel, and now extra 200ms latency is incurred.
    FMOD_CAPS_OUTPUT_MULTICHANNEL = &H4              ' Device can do multichannel output, ie greater than 2 channels.
    FMOD_CAPS_OUTPUT_FORMAT_PCM8 = &H8               ' Device can output to 8bit integer PCM.
    FMOD_CAPS_OUTPUT_FORMAT_PCM16 = &H10             ' Device can output to 16bit integer PCM.
    FMOD_CAPS_OUTPUT_FORMAT_PCM24 = &H20             ' Device can output to 24bit integer PCM.
    FMOD_CAPS_OUTPUT_FORMAT_PCM32 = &H40             ' Device can output to 32bit integer PCM.
    FMOD_CAPS_OUTPUT_FORMAT_PCMFLOAT = &H80          ' Device can output to 32bit floating point PCM.
    FMOD_CAPS_REVERB_EAX2 = &H100                    ' Device supports EAX2 reverb.
    FMOD_CAPS_REVERB_EAX3 = &H200                    ' Device supports EAX3 reverb.
    FMOD_CAPS_REVERB_EAX4 = &H400                    ' Device supports EAX4 reverb.
    FMOD_CAPS_REVERB_EAX5 = &H800                    ' Device supports EAX5 reverb.
    FMOD_CAPS_REVERB_I3DL2 = &H800                   ' Device supports I3DL2 reverb.
    FMOD_CAPS_REVERB_LIMITED = &H1000                ' Device supports some form of limited hardware reverb, maybe parameterless and only selectable by environment.
End Enum


'[DEFINE]
'[
'    [NAME]
'    FMOD_DEBUGLEVEL
'
'    [DESCRIPTION]
'    Bit fields to use with FMOD::Debug_SetLevel / FMOD::Debug_GetLevel to control the level of tty debug output with logging versions of FMOD (fmodL).
'
'    [REMARKS]
'
'    [PLATFORMS]
'    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii
'
'    [SEE_ALSO]
'    Debug_SetLevel
'    Debug_GetLevel
']
Public Enum FMOD_DEBUGLEVEL
    FMOD_DEBUG_LEVEL_NONE = &H0
    FMOD_DEBUG_LEVEL_LOG = &H1
    FMOD_DEBUG_LEVEL_ERROR = &H2
    FMOD_DEBUG_LEVEL_WARNING = &H4
    FMOD_DEBUG_LEVEL_HINT = &H8
    FMOD_DEBUG_LEVEL_ALL = &HFF
    FMOD_DEBUG_TYPE_MEMORY = &H100
    FMOD_DEBUG_TYPE_THREAD = &H200
    FMOD_DEBUG_TYPE_FILE = &H400
    FMOD_DEBUG_TYPE_NET = &H800
    FMOD_DEBUG_TYPE_EVENT = &H1000
    FMOD_DEBUG_TYPE_ALL = &HFFFF
    FMOD_DEBUG_DISPLAY_TIMESTAMPS = &H1000000
    FMOD_DEBUG_DISPLAY_LINENUMBERS = &H2000000
    FMOD_DEBUG_DISPLAY_COMPRESS = &H4000000
    FMOD_DEBUG_DISPLAY_THREAD = &H8000000
    FMOD_DEBUG_DISPLAY_ALL = &HF000000
    FMOD_DEBUG_ALL = &HFFFFFFFF
End Enum


'[DEFINE]
'[
'    [NAME]
'    FMOD_MEMORY_TYPE
'
'    [DESCRIPTION]
'    Bit fields for memory allocation type being passed into FMOD memory callbacks.
'
'    [REMARKS]
'
'    [PLATFORMS]
'    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii
'
'    [SEE_ALSO]
'    FMOD_MEMORY_ALLOCCALLBACK
'    FMOD_MEMORY_REALLOCCALLBACK
'    FMOD_MEMORY_FREECALLBACK
'    Memory_Initialize
']
Public Enum FMOD_MEMORY_TYPE
    FMOD_MEMORY_NORMAL = &H0                   ' Standard memory.
    FMOD_MEMORY_XBOX360_PHYSICAL = &H100000    ' Requires XPhysicalAlloc / XPhysicalFree.
    FMOD_MEMORY_PERSISTENT = &H200000          ' Persistent memory. Memory will be freed when System::release is called.
    FMOD_MEMORY_SECONDARY = &H400000           ' Secondary memory. Allocation should be in secondary memory. For example RSX on the PS3. */
End Enum


'
'[ENUM]
'[
    '[DESCRIPTION]
    'These are speaker types defined for use with the System::setSpeakerMode or System::getSpeakerMode command.

    '[REMARKS]
    'These are important notes on speaker modes in regards to sounds created with FMOD_SOFTWARE.<br>
    'Note below the phrase 'sound channels' is used.  These are the subchannels inside a sound, they are not related and
    'have nothing to do with the FMOD class "Channel".<br>
    'For example a mono sound has 1 sound channel, a stereo sound has 2 sound channels, and an AC3 or 6 channel wav file have 6 "sound channels".<br>
    '<br>
    'FMOD_SPEAKERMODE_RAW<br>
    '---------------------<br>
    'This mode is for output devices that are not specifically mono/stereo/quad/surround/5.1 or 7.1, but are multichannel.<br>
    'Sound channels map to speakers sequentially, so a mono sound maps to output speaker 0, stereo sound maps to output speaker 0 & 1.<br>
    'The user assumes knowledge of the speaker order.  FMOD_SPEAKER enumerations may not apply, so raw channel indicies should be used.<br>
    'Multichannel sounds map input channels to output channels 1:1. <br>
    'Channel::setPan and Channel::setSpeakerMix do not work.<br>
    'Speaker levels must be manually set with Channel::setSpeakerLevels.<br>
    '<br>
    'FMOD_SPEAKERMODE_MONO<br>
    '---------------------<br>
    'This mode is for a 1 speaker arrangement.<br>
    'Panning does not work in this speaker mode.<br>
    'Mono, stereo and multichannel sounds have each sound channel played on the one speaker unity.<br>
    'Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.<br>
    'Channel::setSpeakerMix does not work.<br>
    '<br>
    'FMOD_SPEAKERMODE_STEREO<br>
    '-----------------------<br>
    'This mode is for 2 speaker arrangements that have a left and right speaker.<br>
    '<li>Mono sounds default to an even distribution between left and right.  They can be panned with Channel::setPan.<br>
    '<li>Stereo sounds default to the middle, or full left in the left speaker and full right in the right speaker.
    '<li>They can be cross faded with Channel::setPan.<br>
    '<li>Multichannel sounds have each sound channel played on each speaker at unity.<br>
    '<li>Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.<br>
    '<li>Channel::setSpeakerMix works but only front left and right parameters are used, the rest are ignored.<br>
    '<br>
    'FMOD_SPEAKERMODE_QUAD<br>
    '------------------------<br>
    'This mode is for 4 speaker arrangements that have a front left, front right, rear left and a rear right speaker.<br>
    '<li>Mono sounds default to an even distribution between front left and front right.  They can be panned with Channel::setPan.<br>
    '<li>Stereo sounds default to the left sound channel played on the front left, and the right sound channel played on the front right.<br>
    '<li>They can be cross faded with Channel::setPan.<br>
    '<li>Multichannel sounds default to all of their sound channels being played on each speaker in order of input.<br>
    '<li>Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.<br>
    '<li>Channel::setSpeakerMix works but side left, side right, center and lfe are ignored.<br>
    '<br>
    'FMOD_SPEAKERMODE_SURROUND<br>
    '------------------------<br>
    'This mode is for 4 speaker arrangements that have a front left, front right, front center and a rear center.<br>
    '<li>Mono sounds default to the center speaker.  They can be panned with Channel::setPan.<br>
    '<li>Stereo sounds default to the left sound channel played on the front left, and the right sound channel played on the front right.
    '<li>They can be cross faded with Channel::setPan.<br>
    '<li>Multichannel sounds default to all of their sound channels being played on each speaker in order of input.<br>
    '<li>Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.<br>
    '<li>Channel::setSpeakerMix works but side left, side right and lfe are ignored, and rear left / rear right are averaged into the rear speaker.<br>
    '<br>
    'FMOD_SPEAKERMODE_5POINT1<br>
    '------------------------<br>
    'This mode is for 5.1 speaker arrangements that have a left/right/center/rear left/rear right and a subwoofer speaker.<br>
    '<li>Mono sounds default to the center speaker.  They can be panned with Channel::setPan.<br>
    '<li>Stereo sounds default to the left sound channel played on the front left, and the right sound channel played on the front right.
    '<li>They can be cross faded with Channel::setPan.<br>
    '<li>Multichannel sounds default to all of their sound channels being played on each speaker in order of input.
    '<li>Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.<br>
    '<li>Channel::setSpeakerMix works but side left / side right are ignored.<br>
    '<br>
    'FMOD_SPEAKERMODE_7POINT1<br>
    '------------------------<br>
    'This mode is for 7.1 speaker arrangements that have a left/right/center/rear left/rear right/side left/side right
    'and a subwoofer speaker.<br>
    '<li>Mono sounds default to the center speaker.  They can be panned with Channel::setPan.<br>
    '<li>Stereo sounds default to the left sound channel played on the front left, and the right sound channel played on the front right.
    '<li>They can be cross faded with Channel::setPan.<br>
    '<li>Multichannel sounds default to all of their sound channels being played on each speaker in order of input.
    '<li>Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.<br>
    '<li>Channel::setSpeakerMix works and every parameter is used to set the balance of a sound in any speaker.<br>
    '<br>
    'FMOD_SPEAKERMODE_PROLOGIC<br>
    '------------------------------------------------------<br>
    'This mode is for mono, stereo, 5.1 and 7.1 speaker arrangements, as it is backwards and forwards compatible with stereo,
    'but to get a surround effect a Dolby Prologic or Prologic 2 hardware decoder / amplifier is needed.<br>
    'Pan behaviour is the same as FMOD_SPEAKERMODE_5POINT1.<br>
    '<br>
    'If this function is called the numoutputchannels setting in System::setSoftwareFormat is overwritten.<br>
    '<br>
    'For 3D sounds, panning is determined at runtime by the 3D subsystem based on the speaker mode to determine which speaker the
    'sound should be placed in.<br>

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'System::setSpeakerMode
    'System::getSpeakerMode
    'System::getDriverCaps
    'Channel::setSpeakerLevels
']
'
Public Enum FMOD_SPEAKERMODE
    FMOD_SPEAKERMODE_RAW              ' There is no specific speakermode.  Sound channels are mapped in order of input to output.  See remarks for more information.
    FMOD_SPEAKERMODE_MONO             ' The speakers are monaural.
    FMOD_SPEAKERMODE_STEREO           ' The speakers are stereo (DEFAULT).
    FMOD_SPEAKERMODE_QUAD             ' 4 speaker setup.  This includes front left, front right, rear left, rear right.
    FMOD_SPEAKERMODE_SURROUND         ' 4 speaker setup.  This includes front left, front right, center, rear center (rear left/rear right are averaged).
    FMOD_SPEAKERMODE_5POINT1          ' 5.1 speaker setup.  This includes front left, front right, center, rear left, rear right and a subwoofer.
    FMOD_SPEAKERMODE_7POINT1          ' 7.1 speaker setup.  This includes front left, front right, center, rear left, rear right, side left, side right and a subwoofer.
    FMOD_SPEAKERMODE_PROLOGIC         ' Stereo output, but data is encoded in a way that is picked up by a Prologic/Prologic2 decoder and split into a 5.1 speaker setup.

    FMOD_SPEAKERMODE_MAX              ' Maximum number of speaker modes supported.
End Enum


'
'[ENUM]
'[
    '[DESCRIPTION]
    'These are speaker types defined for use with the Channel::setSpeakerLevels command.
    'It can also be used for speaker placement in the System::setSpeakerPosition command.

    '[REMARKS]
    'If you are using FMOD_SPEAKERMODE_RAW and speaker assignments are meaningless, just cast a raw integer value to this type.<br>
    'For example (FMOD_SPEAKER)7 would use the 7th speaker (also the same as FMOD_SPEAKER_SIDE_RIGHT).<br>
    'Values higher than this can be used if an output system has more than 8 speaker types / output channels.  15 is the current maximum.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_SPEAKERMODE
    'Channel::setSpeakerLevels
    'Channel::getSpeakerLevels
    'System::setSpeakerPosition
    'System::getSpeakerPosition
']
'
Public Enum FMOD_SPEAKER
    FMOD_SPEAKER_FRONT_LEFT
    FMOD_SPEAKER_FRONT_RIGHT
    FMOD_SPEAKER_FRONT_CENTER
    FMOD_SPEAKER_LOW_FREQUENCY
    FMOD_SPEAKER_BACK_LEFT
    FMOD_SPEAKER_BACK_RIGHT
    FMOD_SPEAKER_SIDE_LEFT
    FMOD_SPEAKER_SIDE_RIGHT
    
    FMOD_SPEAKER_MAX                                       ' Maximum number of speaker types supported.
    FMOD_SPEAKER_MONO = FMOD_SPEAKER_FRONT_LEFT            ' For use with FMOD_SPEAKERMODE_MONO and Channel::SetSpeakerLevels.  Mapped to same value as FMOD_SPEAKER_FRONT_LEFT.
    FMOD_SPEAKER_BACK_CENTER = FMOD_SPEAKER_LOW_FREQUENCY  ' For use with FMOD_SPEAKERMODE_SURROUND and Channel::SetSpeakerLevels only.  Mapped to same value as FMOD_SPEAKER_LOW_FREQUENCY.
End Enum


'
'[ENUM]
'[
    '[Description]
    'These are plugin types defined for use with the System::getNumPlugins,
    'System::getPluginInfo and System::unloadPlugin functions.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_System_GetNumPlugins
    'FMOD_System_GetPluginInfo
    'FMOD_System_UnloadPlugin
']
'
Public Enum FMOD_PLUGINTYPE
    FMOD_PLUGINTYPE_OUTPUT     ' The plugin type is an output module.  FMOD mixed audio will play through one of these devices.
    FMOD_PLUGINTYPE_CODEC      ' The plugin type is a file format codec.  FMOD will use these codecs to load file formats for playback.
    FMOD_PLUGINTYPE_DSP        ' The plugin type is a DSP unit.  FMOD will use these plugins as part of its DSP network to apply effects to output or generate sound in realtime.
End Enum


'
'[ENUM]
'[
    '[Description]
    'Initialization flags.  Use them with System::init in the flags parameter to change various behaviour.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_System_Init
']
'
Public Enum FMOD_INITFLAGS
    FMOD_INIT_NORMAL = &H0                           ' All platforms - Initialize normally.
    FMOD_INIT_STREAM_FROM_UPDATE = &H1               ' All platforms - No stream thread is created internally.  Streams are driven from System::update.  Mainly used with non-realtime outputs.
    FMOD_INIT_3D_RIGHTHANDED = &H2                   ' All platforms - FMOD will treat +X as left, +Y as up and +Z as forwards.
    FMOD_INIT_SOFTWARE_DISABLE = &H4                 ' All platforms - Disable software mixer to save memory.  Anything created with FMOD_SOFTWARE will fail and DSP will not work.
    FMOD_INIT_SOFTWARE_OCCLUSION = &H8               ' All platforms - All FMOD_SOFTWARE with FMOD_3D based voices will add a software lowpass filter effect into the DSP chain which is automatically used when Channel::set3DOcclusion is used or the geometry API.
    FMOD_INIT_SOFTWARE_HRTF = &H10                   ' All platforms - All FMOD_SOFTWARE with FMOD_3D based voices will add a software lowpass filter effect into the DSP chain which causes sounds to sound duller when the sound goes behind the listener.
    FMOD_INIT_SOFTWARE_REVERB_LOWMEM = &H40          ' All platforms - SFX reverb is run using 22/24khz delay buffers, halving the memory required. */
    FMOD_INIT_ENABLE_PROFILE = &H20                  ' All platforms - Enable TCP/IP based host which allows "DSPNet Listener.exe" to connect to it, and view the DSP dataflow network graph in real-time. */
    FMOD_INIT_VOL0_BECOMES_VIRTUAL = &H80            ' All platforms - Any sounds that are 0 volume will go virtual and not be processed except for having their positions updated virtually.  Use System::setAdvancedSettings to adjust what volume besides zero to switch to virtual at. */
    FMOD_INIT_WASAPI_EXCLUSIVE = &H100               ' Win32 Vista only - for WASAPI output - Enable exclusive access to hardware, lower latency at the expense of excluding other applications from accessing the audio hardware.
    FMOD_INIT_DSOUND_HRTFNONE = &H200                ' Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use simple stereo panning/doppler/attenuation when 3D hardware acceleration is not present.
    FMOD_INIT_DSOUND_HRTFLIGHT = &H400               ' Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use a slightly higher quality algorithm when 3D hardware acceleration is not present.
    FMOD_INIT_DSOUND_HRTFFULL = &H800                ' Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use full quality 3D playback when 3d hardware acceleration is not present.
    FMOD_INIT_PS2_DISABLECORE0REVERB = &H10000       ' PS2 only - Disable reverb on CORE 0 to regain SRAM.
    FMOD_INIT_PS2_DISABLECORE1REVERB = &H20000       ' PS2 only - Disable reverb on CORE 1 to regain SRAM.
    FMOD_INIT_PS2_DONTUSESCRATCHPAD = &H40000        ' PS2 only - Disable FMOD's usage of the scratchpad.
    FMOD_INIT_PS2_SWAPDMACHANNELS = &H80000          ' PS2 only - Changes FMOD from using SPU DMA channel 0 for software mixing, and 1 for sound data upload/file streaming, to 1 and 0 respectively.
    FMOD_INIT_XBOX_REMOVEHEADROOM = &H100000         ' XBox only - By default DirectSound attenuates all sound by 6db to avoid clipping/distortion.  CAUTION.  If you use this flag you are responsible for the final mix to make sure clipping / distortion doesn't happen.
    FMOD_INIT_360_MUSICMUTENOTPAUSE = &H200000       ' Xbox 360 only - The "music" channelgroup which by default pauses when custom 360 dashboard music is played, can be changed to mute (therefore continues playing) instead of pausing, by using this flag.
    FMOD_INIT_SYNCMIXERWITHUPDATE = &H400000         ' Win32/Wii/PS3/Xbox/Xbox 360 - FMOD Mixer thread is woken up to do a mix when System::update is called rather than waking periodically on its own timer. */
    FMOD_INIT_NEURALTHX = &H2000000                  ' Win32/Mac/Linux/Solaris - Use Neural THX downmixing from 7.1 if speakermode set to FMOD_SPEAKERMODE_STEREO or FMOD_SPEAKERMODE_5POINT1. */
End Enum


'
'[ENUM]
'[
    '[Description]
    'These definitions describe the type of song being played.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'Sound:: GetFormat
']
'
Public Enum FMOD_SOUND_TYPE
    FMOD_SOUND_TYPE_UNKNOWN         ' 3rd party / unknown plugin format.
    FMOD_SOUND_TYPE_AAC             ' AAC.  Currently unsupported.
    FMOD_SOUND_TYPE_AIFF            ' AIFF.
    FMOD_SOUND_TYPE_ASF             ' Microsoft Advanced Systems Format (ie WMA/ASF/WMV).
    FMOD_SOUND_TYPE_AT3             ' Sony ATRAC 3 format
    FMOD_SOUND_TYPE_CDDA            ' Digital CD audio.
    FMOD_SOUND_TYPE_DLS             ' Sound font / downloadable sound bank.
    FMOD_SOUND_TYPE_FLAC            ' FLAC lossless codec.
    FMOD_SOUND_TYPE_FSB             ' FMOD Sample Bank.
    FMOD_SOUND_TYPE_GCADPCM         ' GameCube ADPCM
    FMOD_SOUND_TYPE_IT              ' Impulse Tracker.
    FMOD_SOUND_TYPE_MIDI            ' MIDI.
    FMOD_SOUND_TYPE_MOD             ' Protracker / Fasttracker MOD.
    FMOD_SOUND_TYPE_MPEG            ' MP2/MP3 MPEG.
    FMOD_SOUND_TYPE_OGGVORBIS       ' Ogg vorbis.
    FMOD_SOUND_TYPE_PLAYLIST        ' Information only from ASX/PLS/M3U/WAX playlists
    FMOD_SOUND_TYPE_RAW             ' Raw PCM data.
    FMOD_SOUND_TYPE_S3M             ' ScreamTracker 3.
    FMOD_SOUND_TYPE_SF2             ' Sound font 2 format.
    FMOD_SOUND_TYPE_USER            ' User created sound.
    FMOD_SOUND_TYPE_WAV             ' Microsoft WAV.
    FMOD_SOUND_TYPE_XM              ' FastTracker 2 XM.
    FMOD_SOUND_TYPE_XMA             ' Xbox360 XMA
    FMOD_SOUND_TYPE_VAG             ' PlayStation 2 / PlayStation Portable adpcm VAG format.
End Enum


'
'[ENUM]
'[
    '[Description]
    'These definitions describe the native format of the hardware or software buffer that will be used.

    '[REMARKS]
    'This is the format the native hardware or software buffer will be or is created in.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'System::createSound
    'Sound::GetFormat
']
'
Public Enum FMOD_SOUND_FORMAT
    FMOD_SOUND_FORMAT_NONE      ' Unitialized / unknown.
    FMOD_SOUND_FORMAT_PCM8      ' 8bit integer PCM data.
    FMOD_SOUND_FORMAT_PCM16     ' 16bit integer PCM data.
    FMOD_SOUND_FORMAT_PCM24     ' 24bit integer PCM data.
    FMOD_SOUND_FORMAT_PCM32     ' 32bit integer PCM data.
    FMOD_SOUND_FORMAT_PCMFLOAT  ' 32bit floating point PCM data.
    FMOD_SOUND_FORMAT_GCADPCM   ' Compressed GameCube DSP data.
    FMOD_SOUND_FORMAT_IMAADPCM  ' Compressed XBox ADPCM data.
    FMOD_SOUND_FORMAT_VAG       ' Compressed PlayStation 2 ADPCM data.
    FMOD_SOUND_FORMAT_XMA       ' Compressed Xbox360 data.
    FMOD_SOUND_FORMAT_MPEG      ' Compressed MPEG layer 2 or 3 data.
    FMOD_SOUND_FORMAT_MAX       ' Maximum number of sound formats supported.
End Enum


'
'[DEFINE]
'[
    '[NAME]
    'FMOD_MODE

    '[DESCRIPTION]
    'Sound description bitfields, bitwise OR them together for loading and describing sounds.

    '[REMARKS]
    'By default a sound will open as a static sound that is decompressed fully into memory.<br>
    'To have a sound stream instead, use FMOD_CREATESTREAM.<br>
    'Some opening modes (ie FMOD_OPENUSER, FMOD_OPENMEMORY, FMOD_OPENRAW) will need extra information.<br>
    'This can be provided using the FMOD_CREATESOUNDEXINFO structure.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'System::createSound
    'System::createStream
    'Sound::setMode
    'Sound::getMode
    'Channel::setMode
    'Channel::getMode
']
'
Public Enum FMOD_MODE
    FMOD_DEFAULT = &H0                       ' FMOD_DEFAULT is a default sound type.  Equivalent to all the defaults listed below.  FMOD_LOOP_OFF, FMOD_2D, FMOD_HARDWARE.
    FMOD_LOOP_OFF = &H1                      ' For non looping sounds. (default).  Overrides FMOD_LOOP_NORMAL / FMOD_LOOP_BIDI.
    FMOD_LOOP_NORMAL = &H2                   ' For forward looping sounds.
    FMOD_LOOP_BIDI = &H4                     ' For bidirectional looping sounds. (only works on software mixed static sounds).
    FMOD_2D = &H8                            ' Ignores any 3d processing. (default).
    FMOD_3D = &H10                           ' Makes the sound positionable in 3D.  Overrides FMOD_2D.
    FMOD_HARDWARE = &H20                     ' Attempts to make sounds use hardware acceleration. (default).
    FMOD_SOFTWARE = &H40                     ' Makes sound reside in software.  Overrides FMOD_HARDWARE.  Use this for FFT,  DSP, 2D multi speaker support and other software related features.
    FMOD_CREATESTREAM = &H80                 ' Decompress at runtime, streaming from the source provided (standard stream).  Overrides FMOD_CREATESAMPLE.
    FMOD_CREATESAMPLE = &H100                ' Decompress at loadtime, decompressing or decoding whole file into memory as the target sample format. (standard sample).
    FMOD_CREATECOMPRESSEDSAMPLE = &H200      ' Load MP2, MP3, IMAADPCM or XMA into memory and leave it compressed.  During playback the FMOD software mixer will decode it in realtime as a 'compressed sample'.  Can only be used in combination with FMOD_SOFTWARE.
    FMOD_OPENUSER = &H400                    ' Opens a user created static sample or stream. Use FMOD_CREATESOUNDEXINFO to specify format and/or read callbacks.  If a user created 'sample' is created with no read callback, the sample will be empty.  Use FMOD_Sound_Lock and FMOD_Sound_Unlock to place sound data into the sound if this is the case.
    FMOD_OPENMEMORY = &H800                  ' "name_or_data" will be interpreted as a pointer to memory instead of filename for creating sounds.
    FMOD_OPENMEMORY_POINT = &H10000000       ' "name_or_data" will be interpreted as a pointer to memory instead of filename for creating sounds.  Use FMOD_CREATESOUNDEXINFO to specify length.  This differs to FMOD_OPENMEMORY in that it uses the memory as is, without duplicating the memory into its own buffers.  FMOD_SOFTWARE only.  Doesn't work with FMOD_HARDWARE, as sound hardware cannot access main ram on a lot of platforms.  Cannot be freed after open, only after Sound::release.   Will not work if the data is compressed and FMOD_CREATECOMPRESSEDSAMPLE is not used. */
    FMOD_OPENRAW = &H1000                    ' Will ignore file format and treat as raw pcm.  User may need to declare if data is FMOD_SIGNED or FMOD_UNSIGNED '
    FMOD_OPENONLY = &H2000                   ' Just open the file, dont prebuffer or read.  Good for fast opens for info, or when FMOD_Sound_ReadData is to be used.
    FMOD_ACCURATETIME = &H4000               ' For FMOD_System_CreateSound - for accurate FMOD_Sound_GetLength / FMOD_Channel_SetPosition on VBR MP3, AAC and MOD/S3M/XM/IT/MIDI files.  Scans file first, so takes longer to open. FMOD_OPENONLY does not affect this.
    FMOD_MPEGSEARCH = &H8000                 ' For corrupted / bad MP3 files.  This will search all the way through the file until it hits a valid MPEG header.  Normally only searches for 4k.
    FMOD_NONBLOCKING = &H10000               ' For opening sounds and getting streamed subsounds (seeking) asyncronously.  Use Sound::getOpenState to poll the state of the sound as it opens or retrieves the subsound in the background.
    FMOD_UNIQUE = &H20000                    ' Unique sound, can only be played one at a time '
    FMOD_3D_HEADRELATIVE = &H40000           ' Make the sound's position, velocity and orientation relative to the listener.
    FMOD_3D_WORLDRELATIVE = &H80000          ' Make the sound's position, velocity and orientation absolute (relative to the world). (DEFAULT)
    FMOD_3D_LOGROLLOFF = &H100000            ' This sound will follow the standard logarithmic rolloff model where mindistance = full volume, maxdistance = where sound stops attenuating, and rolloff is fixed according to the global rolloff factor.  (default)
    FMOD_3D_LINEARROLLOFF = &H200000         ' This sound will follow a linear rolloff model where mindistance = full volume, maxdistance = silence.
    FMOD_3D_CUSTOMROLLOFF = &H4000000        ' This sound will follow a rolloff model defined by FMOD_Sound_Set3DCustomRolloff / FMOD_Channel_Set3DCustomRolloff.
    FMOD_CDDA_FORCEASPI = &H400000           ' For CDDA sounds only - use ASPI instead of NTSCSI to access the specified CD/DVD device.
    FMOD_CDDA_JITTERCORRECT = &H800000       ' For CDDA sounds only - perform jitter correction. Jitter correction helps produce a more accurate CDDA stream at the cost of more CPU time.
    FMOD_UNICODE = &H1000000                 ' Filename is double-byte unicode.
    FMOD_IGNORETAGS = &H2000000              ' Skips id3v2/asf/etc tag checks when opening a sound, to reduce seek/read overhead when opening files (helps with CD performance).
    FMOD_LOWMEM = &H8000000                  ' Removes some features from samples to give a lower memory overhead, like FMOD_Sound_GetName.
    FMOD_LOADSECONDARYRAM = &H20000000       ' Load sound into the secondary RAM of supported platform.  On PS3, sounds will be loaded into RSX/VRAM. */
    FMOD_VIRTUAL_PLAYFROMSTART = &H80000000  ' For sounds that start virtual (due to being quiet or low importance), instead of swapping back to audible, and playing at the correct offset according to time, this flag makes the sound play from the start. */
End Enum


'
'[ENUM]
'[
    '[Description]
    'These values describe what state a sound is in after FMOD_NONBLOCKING has been used to open it.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'Sound:: getOpenState
    'FMOD_MODE
']
'
Public Enum FMOD_OPENSTATE
    FMOD_OPENSTATE_READY = 0        ' Opened and ready to play
    FMOD_OPENSTATE_LOADING          ' Initial load in progress
    FMOD_OPENSTATE_ERROR            ' Failed to open - file not found, out of memory etc.  See return value of Sound::getOpenState for what happened.
    FMOD_OPENSTATE_CONNECTING       ' Connecting to remote host (internet sounds only)
    FMOD_OPENSTATE_BUFFERING        ' Buffering data
    FMOD_OPENSTATE_SEEKING          ' Seeking to subsound and re-flushing stream buffer.
    FMOD_OPENSTATE_STREAMING        ' Ready and playing, but not possible to release at this time without stalling the main thread.
End Enum


'
'[ENUM]
'[
    '[DESCRIPTION]
    'These flags are used with SoundGroup::setMaxAudibleBehavior to determine what happens when more sounds
    'are played than are specified with SoundGroup::setMaxAudible.

    '[REMARKS]
    'When using FMOD_SOUNDGROUP_BEHAVIOR_MUTE, SoundGroup::setMuteFadeSpeed can be used to stop a sudden transition.
    'Instead, the time specified will be used to cross fade between the sounds that go silent and the ones that become audible.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'SoundGroup::setMaxAudibleBehavior
    'SoundGroup::getMaxAudibleBehavior
    'SoundGroup::setMaxAudible
    'SoundGroup::getMaxAudible
    'SoundGroup::setMuteFadeSpeed
    'SoundGroup::getMuteFadeSpeed
']
'*/
Public Enum FMOD_SOUNDGROUP_BEHAVIOR
    FMOD_SOUNDGROUP_BEHAVIOR_FAIL              ' Any sound played that puts the sound count over the SoundGroup::setMaxAudible setting, will simply fail during System::playSound. */
    FMOD_SOUNDGROUP_BEHAVIOR_MUTE              ' Any sound played that puts the sound count over the SoundGroup::setMaxAudible setting, will be silent, then if another sound in the group stops the sound that was silent before becomes audible again. */
    FMOD_SOUNDGROUP_BEHAVIOR_STEALLOWEST       ' Any sound played that puts the sound count over the SoundGroup::setMaxAudible setting, will steal the quietest / least important sound playing in the group. */
End Enum


'
'[ENUM]
'[
    '[Description]
    'These callback types are used with Channel::setCallback.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_Channel_SetCallback
    'FMOD_CHANNEL_CALLBACK
']
'
Public Enum FMOD_CHANNEL_CALLBACKTYPE
    FMOD_CHANNEL_CALLBACKTYPE_END                  ' Called when a sound ends.
    FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE         ' Called when a voice is swapped out or swapped in.
    FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT            ' Called when a syncpoint is encountered.  Can be from wav file markers.

    FMOD_CHANNEL_CALLBACKTYPE_MAX
End Enum


'
'[ENUM]
'[
    '[Description]
    'List of windowing methods used in spectrum analysis to reduce leakage / transient signals intefering with the analysis.<br>
    'This is a problem with analysis of continuous signals that only have a small portion of the signal sample (the fft window size).<br>
    'Windowing the signal with a curve or triangle tapers the sides of the fft window to help alleviate this problem.

    '[REMARKS]
    'Cyclic signals such as a sine wave that repeat their cycle in a multiple of the window size do not need windowing.<br>
    'I.e. If the sine wave repeats every 1024, 512, 256 etc samples and the FMOD fft window is 1024, then the signal would not need windowing.<br>
    'Not windowing is the same as FMOD_DSP_FFT_WINDOW_RECT, which is the default.<br>
    'If the cycle of the signal (ie the sine wave) is not a multiple of the window size, it will cause frequency abnormalities, so a different windowing method is needed.<br>
    '<exclude>
    '<br>
    'FMOD_DSP_FFT_WINDOW_RECT.<br>
    '<img src = "rectangle.gif"></img><br>
    '<br>
    'FMOD_DSP_FFT_WINDOW_TRIANGLE.<br>
    '<img src = "triangle.gif"></img><br>
    '<br>
    'FMOD_DSP_FFT_WINDOW_HAMMING.<br>
    '<img src = "hamming.gif"></img><br>
    '<br>
    'FMOD_DSP_FFT_WINDOW_HANNING.<br>
    '<img src = "hanning.gif"></img><br>
    '<br>
    'FMOD_DSP_FFT_WINDOW_BLACKMAN.<br>
    '<img src = "blackman.gif"></img><br>
    '<br>
    'FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS.<br>
    '<img src = "blackmanharris.gif"></img>
    '</exclude>
    
    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_System_GetSpectrum
    'FMOD_Channel_GetSpectrum
']
'
Public Enum FMOD_DSP_FFT_WINDOW
    FMOD_DSP_FFT_WINDOW_RECT            ' w[n] = 1.0                                                                                            '
    FMOD_DSP_FFT_WINDOW_TRIANGLE        ' w[n] = TRI(2n/N)                                                                                      '
    FMOD_DSP_FFT_WINDOW_HAMMING         ' w[n] = 0.54 - (0.46 * COS(n/N) )                                                                      '
    FMOD_DSP_FFT_WINDOW_HANNING         ' w[n] = 0.5 *  (1.0  - COS(n/N) )                                                                      '
    FMOD_DSP_FFT_WINDOW_BLACKMAN        ' w[n] = 0.42 - (0.5  * COS(n/N) ) + (0.08 * COS(2.0 * n/N) )                                           '
    FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS  ' w[n] = 0.35875 - (0.48829 * COS(1.0 * n/N)) + (0.14128 * COS(2.0 * n/N)) - (0.01168 * COS(3.0 * n/N)) '
    FMOD_DSP_FFT_WINDOW_MAX
End Enum


'
'[ENUM]
'[
    '[description]
    'List of interpolation types that the FMOD Ex software mixer supports.

    '[REMARKS]
    'The default resampler type is FMOD_DSP_RESAMPLER_LINEAR.<br>
    'Use System::setSoftwareFormat to tell FMOD the resampling quality you require for FMOD_SOFTWARE based sounds.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'System:: setSoftwareFormat
    'System:: getSoftwareFormat
']
'
Public Enum FMOD_DSP_RESAMPLER
    FMOD_DSP_RESAMPLER_NOINTERP        ' No interpolation.  High frequency aliasing hiss will be audible depending on the sample rate of the sound. '
    FMOD_DSP_RESAMPLER_LINEAR          ' Linear interpolation (default method).  Fast and good quality, causes very slight lowpass effect on low frequency sounds. '
    FMOD_DSP_RESAMPLER_CUBIC           ' Cubic interoplation.  Slower than linear interpolation but better quality. '
    FMOD_DSP_RESAMPLER_SPLINE          ' 5 point spline interoplation.  Slowest resampling method but best quality. '

    FMOD_DSP_RESAMPLER_MAX             ' Maximum number of resample methods supported. '
End Enum


'
'[ENUM]
'[
    '[Description]
    'List of tag types that could be stored within a sound.  These include id3 tags, metadata from netstreams and vorbis/asf data.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'Sound:: getTag
']
'
Public Enum FMOD_TAGTYPE
    FMOD_TAGTYPE_UNKNOWN = 0
    FMOD_TAGTYPE_ID3V1
    FMOD_TAGTYPE_ID3V2
    FMOD_TAGTYPE_VORBISCOMMENT
    FMOD_TAGTYPE_SHOUTCAST
    FMOD_TAGTYPE_ICECAST
    FMOD_TAGTYPE_ASF
    FMOD_TAGTYPE_MIDI
    FMOD_TAGTYPE_PLAYLIST
    FMOD_TAGTYPE_FMOD
    FMOD_TAGTYPE_USER
End Enum


'
'[ENUM]
'[
    '[Description]
    'List of data types that can be returned by Sound::getTag

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'Sound:: getTag
']
'
Public Enum FMOD_TAGDATATYPE
    FMOD_TAGDATATYPE_BINARY = 0
    FMOD_TAGDATATYPE_INT
    FMOD_TAGDATATYPE_FLOAT
    FMOD_TAGDATATYPE_STRING
    FMOD_TAGDATATYPE_STRING_UTF16
    FMOD_TAGDATATYPE_STRING_UTF16BE
    FMOD_TAGDATATYPE_STRING_UTF8
    FMOD_TAGDATATYPE_CDTOC
End Enum


'
'[ENUM]
'[
'[DESCRIPTION]
'Types of delay that can be used with Channel::setDelay / Channel::getDelay.

'[REMARKS]
'If you haven't called Channel::setDelay yet, if you call Channel::getDelay with FMOD_DELAYTYPE_DSPCLOCK_START it will return the
'equivalent global DSP clock value to determine when a channel started, so that you can use it for other channels to sync against.<br>
'<br>
'Use System::getDSPClock to also get the current dspclock time, a base for future calls to Channel::setDelay.<br>
'<br>
'Use FMOD_64BIT_ADD or FMOD_64BIT_SUB to add a hi/lo combination together and cope with wraparound.
'<br>
'If FMOD_DELAYTYPE_END_MS is specified, the value is not treated as a 64 bit number, just the delayhi value is used and it is treated as milliseconds.

'[PLATFORMS]
'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii, Solaris

'[SEE_ALSO]
'Channel::setDelay
'Channel::getDelay
'System::getDSPClock
']
'
Public Enum FMOD_DELAYTYPE
    FMOD_DELAYTYPE_END_MS              ' Delay at the end of the sound in milliseconds.  Use delayhi only.   Channel::isPlaying will remain true until this delay has passed even though the sound itself has stopped playing.*/
    FMOD_DELAYTYPE_DSPCLOCK_START      ' Time the sound started if Channel::getDelay is used, or if Channel::setDelay is used, the sound will delay playing until this exact tick. */
    FMOD_DELAYTYPE_DSPCLOCK_END        ' Time the sound should end. If this is non-zero, the channel will go silent at this exact tick. */

    FMOD_DELAYTYPE_MAX                 ' Maximum number of tag datatypes supported. */
End Enum


'#define FMOD_64BIT_ADD(_hi1, _lo1, _hi2, _lo2) _hi1 += ((_hi2) + ((((_lo1) + (_lo2)) < (_lo1)) ? 1 : 0)); (_lo1) += (_lo2);
'#define FMOD_64BIT_SUB(_hi1, _lo1, _hi2, _lo2) _hi1 -= ((_hi2) + ((((_lo1) - (_lo2)) > (_lo1)) ? 1 : 0)); (_lo1) -= (_lo2);


'
'[Structure]
'[
    '[Description]
    'Structure describing a piece of tag data.

    '[REMARKS]
    'Members marked with [in] mean the user sets the value before passing it to the function.<br>
    'Members marked with [out] mean FMOD sets the value to be used after the function exits.<br>

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'Sound:: getTag
    'FMOD_TAGTYPE
    'FMOD_TAGDATATYPE
']
'
Public Type FMOD_TAG
    type     As FMOD_TAGTYPE         ' [out] The type of this tag.
    datatype As FMOD_TAGDATATYPE     ' [out] The type of data that this tag contains '
    name     As Long                 ' [out] The name of this tag i.e. "TITLE", "ARTIST" etc.
    Data     As Long                 ' [out] Pointer to the tag data - its format is determined by the datatype member '
    datalen  As Long                 ' [out] Length of the data contained in this tag '
    udated   As Long              ' [out] True if this tag has been updated since last being accessed with Sound::getTag '
End Type


'
'[Structure]
'[
    '[Description]
    'Structure describing a CD/DVD table of contents

    '[REMARKS]
    'Members marked with [in] mean the user sets the value before passing it to the function.<br>
    'Members marked with [out] mean FMOD sets the value to be used after the function exits.<br>

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'Sound:: getTag
    ']
'
Public Type FMOD_CDTOC
    numtracks  As Long               ' [out] The number of tracks on the CD '
    min(100)   As Long               ' [out] The start offset of each track in minutes '
    sec(100)   As Long               ' [out] The start offset of each track in seconds '
    frame(100) As Long               ' [out] The start offset of each track in frames '
End Type


'
'[ENUM]
'[
    '[description]
    'List of time types that can be returned by Sound::getLength and used with Channel::setPosition or Channel::getPosition.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'Sound:: getLength
    'FMOD_Channel_SetPosition
    'FMOD_Channel_GetPosition
']
'
Public Enum FMOD_TIMEUNIT
    FMOD_TIMEUNIT_MS = &H1                        ' Milliseconds.
    FMOD_TIMEUNIT_PCM = &H2                       ' PCM Samples, related to milliseconds * samplerate / 1000.
    FMOD_TIMEUNIT_PCMBYTES = &H4                  ' Bytes, related to PCM samples * channels * datawidth (ie 16bit = 2 bytes).
    FMOD_TIMEUNIT_RAWBYTES = &H8                  ' Raw file bytes of (compressed) sound data (does not include headers).  Only used by Sound::getLength and Channel::getPosition.
    FMOD_TIMEUNIT_MODORDER = &H100                ' MOD/S3M/XM/IT.  Order in a sequenced module format.  Use Sound::getFormat to determine the format.
    FMOD_TIMEUNIT_MODROW = &H200                  ' MOD/S3M/XM/IT.  Current row in a sequenced module format.  Sound::getLength will return the number if rows in the currently playing or seeked to pattern.
    FMOD_TIMEUNIT_MODPATTERN = &H400              ' MOD/S3M/XM/IT.  Current pattern in a sequenced module format.  Sound::getLength will return the number of patterns in the song and Channel::getPosition will return the currently playing pattern.
    FMOD_TIMEUNIT_SENTENCE_MS = &H10000           ' Currently playing subsound in a sentence time in milliseconds.
    FMOD_TIMEUNIT_SENTENCE_PCM = &H20000          ' Currently playing subsound in a sentence time in PCM Samples, related to milliseconds * samplerate / 1000.
    FMOD_TIMEUNIT_SENTENCE_PCMBYTES = &H40000     ' Currently playing subsound in a sentence time in bytes, related to PCM samples * channels * datawidth (ie 16bit = 2 bytes).
    FMOD_TIMEUNIT_SENTENCE = &H80000              ' Currently playing sentence index according to the channel.
    FMOD_TIMEUNIT_SENTENCE_SUBSOUND = &H100000    ' Currently playing subsound index in a sentence.
    FMOD_TIMEUNIT_BUFFERED = &H10000000           ' Time value as seen by buffered stream.  This is always ahead of audible time, and is only used for processing.
End Enum


'
'[ENUM]
'[
    '[DESCRIPTION]
    'When creating a multichannel sound, FMOD will pan them to their default speaker locations, for example a 6 channel sound will default to one channel per 5.1 output speaker.<br>
    'Another example is a stereo sound.  It will default to left = front left, right = front right.<br>
    '<br>
    'This is for sounds that are not 'default'.  For example you might have a sound that is 6 channels but actually made up of 3 stereo pairs, that should all be located in front left, front right only.

    '[REMARKS]
    'For full flexibility of speaker assignments, use Channel::setSpeakerLevels.  This functionality is cheaper, uses less memory and easier to use.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_CREATESOUNDEXINFO
    'Channel::setSpeakerLevels
']
'
Public Enum FMOD_SPEAKERMAPTYPE
    FMOD_SPEAKERMAPTYPE_DEFAULT     ' This is the default, and just means FMOD decides which speakers it puts the source channels. */
    FMOD_SPEAKERMAPTYPE_ALLMONO     ' This means the sound is made up of all mono sounds.  All voices will be panned to the front center by default in this case.  */
    FMOD_SPEAKERMAPTYPE_ALLSTEREO   ' This means the sound is made up of all stereo sounds.  All voices will be panned to front left and front right alternating every second channel.  */
End Enum


'typedef FMOD_RESULT (F_CALLBACK *FMOD_SOUND_NONBLOCKCALLBACK)(FMOD_SOUND *sound, FMOD_RESULT result);
'typedef FMOD_RESULT (F_CALLBACK *FMOD_SOUND_PCMREADCALLBACK)(FMOD_SOUND *sound, void *data, unsigned int datalen);
'typedef FMOD_RESULT (F_CALLBACK *FMOD_SOUND_PCMSETPOSCALLBACK)(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype);


'
'[STRUCTURE]
'[
    '[DESCRIPTION]
    'Use this structure with System::createSound when more control is needed over loading.<br>
    'The possible reasons to use this with System::createSound are:<br>
    '<li>Loading a file from memory.
    '<li>Loading a file from within another larger (possibly wad/pak) file, by giving the loader an offset and length.
    '<li>To create a user created / non file based sound.
    '<li>To specify a starting subsound to seek to within a multi-sample sounds (ie FSB/DLS/SF2) when created as a stream.
    '<li>To specify which subsounds to load for multi-sample sounds (ie FSB/DLS/SF2) so that memory is saved and only a subset is actually loaded/read from disk.
    '<li>To specify 'piggyback' read and seek callbacks for capture of sound data as fmod reads and decodes it.  Useful for ripping decoded PCM data from sounds as they are loaded / played.
    '<li>To specify a MIDI DLS/SF2 sample set file to load when opening a MIDI file.
    'See below on what members to fill for each of the above types of sound you want to create.

    '[REMARKS]
    'This structure is optional!  Specify 0 or NULL in System::createSound if you don't need it!<br>
    '<br>
    'Members marked with [in] mean the user sets the value before passing it to the function.<br>
    'Members marked with [out] mean FMOD sets the value to be used after the function exits.<br>
    '<br>
    '<u>Loading a file from memory.</u><br>
    '<li>Create the sound using the FMOD_OPENMEMORY flag.<br>
    '<li>Mandantory.  Specify 'length' for the size of the memory block in bytes.
    '<li>Other flags are optional.
    '<br>
    '<br>
    '<u>Loading a file from within another larger (possibly wad/pak) file, by giving the loader an offset and length.</u><br>
    '<li>Mandantory.  Specify 'fileoffset' and 'length'.
    '<li>Other flags are optional.
    '<br>
    '<br>
    '<u>To create a user created / non file based sound.</u><br>
    '<li>Create the sound using the FMOD_OPENUSER flag.
    '<li>Mandantory.  Specify 'defaultfrequency, 'numchannels' and 'format'.
    '<li>Other flags are optional.
    '<br>
    '<br>
    '<u>To specify a starting subsound to seek to and flush with, within a multi-sample stream (ie FSB/DLS/SF2).</u><br>
    '<br>
    '<li>Mandantory.  Specify 'initialsubsound'.
    '<br>
    '<br>
    '<u>To specify which subsounds to load for multi-sample sounds (ie FSB/DLS/SF2) so that memory is saved and only a subset is actually loaded/read from disk.</u><br>
    '<br>
    '<li>Mandantory.  Specify 'inclusionlist' and 'inclusionlistnum'.
    '<br>
    '<br>
    '<u>To specify 'piggyback' read and seek callbacks for capture of sound data as fmod reads and decodes it.  Useful for ripping decoded PCM data from sounds as they are loaded / played.</u><br>
    '<br>
    '<li>Mandantory.  Specify 'pcmreadcallback' and 'pcmseekcallback'.
    '<br>
    '<br>
    '<u>To specify a MIDI DLS/SF2 sample set file to load when opening a MIDI file.</u><br>
    '<br>
    '<li>Mandantory.  Specify 'dlsname'.
    '<br>
    '<br>
    'Setting the 'decodebuffersize' is for cpu intensive codecs that may be causing stuttering, not file intensive codecs (ie those from CD or netstreams) which are normally altered with System::setStreamBufferSize.  As an example of cpu intensive codecs, an mp3 file will take more cpu to decode than a PCM wav file.<br>
    'If you have a stuttering effect, then it is using more cpu than the decode buffer playback rate can keep up with.  Increasing the decode buffersize will most likely solve this problem.<br>

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'System::createSound
    'System::setStreamBufferSize
    'FMOD_MODE
']
'
Public Type FMOD_CREATESOUNDEXINFO
    cbsize              As Long                 ' [in] Size of this structure.  This is used so the structure can be expanded in the future and still work on older versions of FMOD Ex.
    Length              As Long                 ' [in] Optional. Specify 0 to ignore. Size in bytes of file to load, or sound to create (in this case only if FMOD_OPENUSER is used).  Required if loading from memory.  If 0 is specified, then it will use the size of the file (unless loading from memory then an error will be returned).
    fileoffset          As Long                 ' [in] Optional. Specify 0 to ignore. Offset from start of the file to start loading from.  This is useful for loading files from inside big data files.
    Numchannels         As Long                 ' [in] Optional. Specify 0 to ignore. Number of channels in a sound specified only if FMOD_OPENUSER is used.
    defaultfrequency    As Long                 ' [in] Optional. Specify 0 to ignore. Default frequency of sound in a sound specified only if FMOD_OPENUSER is used.  Other formats use the frequency determined by the file format.
    Format              As FMOD_SOUND_FORMAT    ' [in] Optional. Specify 0 or FMOD_SOUND_FORMAT_NONE to ignore. Format of the sound specified only if FMOD_OPENUSER is used.  Other formats use the format determined by the file format.
    decodebuffersize    As Long                 ' [in] Optional. Specify 0 to ignore. For streams.  This determines the size of the double buffer (in PCM samples) that a stream uses.  Use this for user created streams if you want to determine the size of the callback buffer passed to you.  Specify 0 to use FMOD's default size which is currently equivalent to 400ms of the sound format created/loaded.
    initialsubsound     As Long                 ' [in] Optional. Specify 0 to ignore. In a multi-sample file format such as .FSB/.DLS/.SF2, specify the initial subsound to seek to, only if FMOD_CREATESTREAM is used.
    Numsubsounds        As Long                 ' [in] Optional. Specify 0 to ignore or have no subsounds.  In a user created multi-sample sound, specify the number of subsounds within the sound that are accessable with Sound::getSubSound.
    inclusionlist       As Long                 ' [in] Optional. Specify 0 to ignore. In a multi-sample format such as .FSB/.DLS/.SF2 it may be desirable to specify only a subset of sounds to be loaded out of the whole file.  This is an array of subsound indicies to load into memory when created.
    inclusionlistnum    As Long                 ' [in] Optional. Specify 0 to ignore. This is the number of integers contained within the inclusionlist array.
    pcmreadcallback     As Long                 ' [in] Optional. Specify 0 to ignore. Callback to 'piggyback' on FMOD's read functions and accept or even write PCM data while FMOD is opening the sound.  Used for user sounds created with FMOD_OPENUSER or for capturing decoded data as FMOD reads it.
    pcmsetposcallback   As Long                 ' [in] Optional. Specify 0 to ignore. Callback for when the user calls a seeking function such as Channel::setTime or Channel::setPosition within a multi-sample sound, and for when it is opened.
    nonblockcallback    As Long                 ' [in] Optional. Specify 0 to ignore. Callback for successful completion, or error while loading a sound that used the FMOD_NONBLOCKING flag.
    dlsname             As String               ' [in] Optional. Specify 0 to ignore. Filename for a DLS or SF2 sample set when loading a MIDI file.   If not specified, on windows it will attempt to open /windows/system32/drivers/gm.dls, otherwise the MIDI will fail to open.
    encryptionkey       As String               ' [in] Optional. Specify 0 to ignore. Key for encrypted FSB file.  Without this key an encrypted FSB file will not load.
    maxpolyphony        As Long                 ' [in] Optional. Specify 0 to ingore. For sequenced formats with dynamic channel allocation such as .MID and .IT, this specifies the maximum voice count allowed while playing.  .IT defaults to 64.  .MID defaults to 32.
    userdata            As Long                 ' [in] Optional. Specify 0 to ignore. This is user data to be attached to the sound during creation.  Access via Sound::getUserData.
    suggestedsoundtype  As FMOD_SOUND_TYPE      ' [in] Optional. Specify 0 or FMOD_SOUND_TYPE_UNKNOWN to ignore.  Instead of scanning all codec types, use this to speed up loading by making it jump straight to this codec.
    useropen            As Long                 ' [in] Optional. Specify 0 to ignore. Callback for opening this file.
    userclose           As Long                 ' [in] Optional. Specify 0 to ignore. Callback for closing this file.
    userread            As Long                 ' [in] Optional. Specify 0 to ignore. Callback for reading from this file.
    userseek            As Long                 ' [in] Optional. Specify 0 to ignore. Callback for seeking within this file.
    speakermap          As FMOD_SPEAKERMAPTYPE  ' [in] Optional. Specify 0 to ignore. Use this to differ the way fmod maps multichannel sounds to speakers.  See FMOD_SPEAKERMAPTYPE for more. */
    initialsoundgroup   As Long                 ' [in] Optional. Specify 0 to ignore. Specify a sound group if required, to put sound in as it is created.
    initialseekposition As Long                 ' [in] Optional. Specify 0 to ignore. For streams. Specify an initial position to seek the stream to.
    initialseekpostype  As FMOD_TIMEUNIT        ' [in] Optional. Specify 0 to ignore. For streams. Specify the time unit for the position set in initialseekposition.

End Type

'
'[STRUCTURE]
'[
    '[DESCRIPTION]
    'Structure defining a reverb environment.<br>
    '<br>
    'For more indepth descriptions of the reverb properties under win32, please see the EAX2 and EAX3
    'documentation at http://developer.creative.com/ under the 'downloads' section.<br>
    'If they do not have the EAX3 documentation, then most information can be attained from
    'the EAX2 documentation, as EAX3 only adds some more parameters and functionality on top of
    'EAX2.

    '[REMARKS]
    'Note the default reverb properties are the same as the FMOD_PRESET_GENERIC preset.<br>
    'Note that integer values that typically range from -10,000 to 1000 are represented in
    'decibels, and are of a logarithmic scale, not linear, wheras float values are always linear.<br>
    '<br>
    'The numerical values listed below are the maximum, minimum and default values for each variable respectively.<br>
    '<br>
    '<b>SUPPORTED</b> next to each parameter means the platform the parameter can be set on.  Some platforms support all parameters and some don't.<br>
    'EAX   means hardware reverb on FMOD_OUTPUTTYPE_DSOUND on windows only (must use FMOD_HARDWARE), on soundcards that support EAX 1 to 4.<br>
    'EAX4  means hardware reverb on FMOD_OUTPUTTYPE_DSOUND on windows only (must use FMOD_HARDWARE), on soundcards that support EAX 4.<br>
    'I3DL2 means hardware reverb on FMOD_OUTPUTTYPE_DSOUND on windows only (must use FMOD_HARDWARE), on soundcards that support I3DL2 non EAX native reverb.<br>
    'GC    means Nintendo Gamecube hardware reverb (must use FMOD_HARDWARE).<br>
    'WII   means Nintendo Wii hardware reverb (must use FMOD_HARDWARE).<br>
    'Xbox1 means the original Xbox hardware reverb (must use FMOD_HARDWARE).<br>
    'PS2   means Playstation 2 hardware reverb (must use FMOD_HARDWARE).<br>
    'SFX   means FMOD SFX software reverb.  This works on any platform that uses FMOD_SOFTWARE for loading sounds.<br>
    '<br>
    'Members marked with [in] mean the user sets the value before passing it to the function.<br>
    'Members marked with [out] mean FMOD sets the value to be used after the function exits.<br>

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'System::setReverbProperties
    'System::getReverbProperties
    'FMOD_REVERB_PRESETS
    'FMOD_REVERB_FLAGS
']
'
Public Type FMOD_REVERB_PROPERTIES
                                    '          MIN     MAX     DEFAULT  DESCRIPTION '
    Instance          As Long       ' [in]     0     , 2     , 0      , EAX4 only. Environment Instance. 3 seperate reverbs simultaneously are possible. This specifies which one to set. (win32 only) '
    Environment       As Long       ' [in/out] 0     , 25    , 0      , sets all listener properties (win32/ps2) '
    EnvSize           As Single     ' [in/out] 1.0   , 100.0 , 7.5    , environment size in meters (win32 only) '
    EnvDiffusion      As Single     ' [in/out] 0.0   , 1.0   , 1.0    , environment diffusion (win32/xbox) '
    Room              As Long       ' [in/out] -10000, 0     , -1000  , room effect level (at mid frequencies) (win32/xbox) '
    RoomHF            As Long       ' [in/out] -10000, 0     , -100   , relative room effect level at high frequencies (win32/xbox) '
    RoomLF            As Long       ' [in/out] -10000, 0     , 0      , relative room effect level at low frequencies (win32 only) '
    DecayTime         As Single     ' [in/out] 0.1   , 20.0  , 1.49   , reverberation decay time at mid frequencies (win32/xbox) '
    DecayHFRatio      As Single     ' [in/out] 0.1   , 2.0   , 0.83   , high-frequency to mid-frequency decay time ratio (win32/xbox) '
    DecayLFRatio      As Single     ' [in/out] 0.1   , 2.0   , 1.0    , low-frequency to mid-frequency decay time ratio (win32 only) '
    Reflections       As Long       ' [in/out] -10000, 1000  , -2602  , early reflections level relative to room effect (win32/xbox) '
    ReflectionsDelay  As Single     ' [in/out] 0.0   , 0.3   , 0.007  , initial reflection delay time (win32/xbox) '
    ReflectionsPan(3) As Single     ' [in/out]       ,       , [0,0,0], early reflections panning vector (win32 only) '
    reverb            As Long       ' [in/out] -10000, 2000  , 200    , late reverberation level relative to room effect (win32/xbox) '
    ReverbDelay       As Single     ' [in/out] 0.0   , 0.1   , 0.011  , late reverberation delay time relative to initial reflection (win32/xbox) '
    ReverbPan(3)      As Single     ' [in/out]       ,       , [0,0,0], late reverberation panning vector (win32 only) '
    EchoTime          As Single     ' [in/out] .075  , 0.25  , 0.25   , echo time (win32 only) '
    EchoDepth         As Single     ' [in/out] 0.0   , 1.0   , 0.0    , echo depth (win32 only) '
    ModulationTime    As Single     ' [in/out] 0.04  , 4.0   , 0.25   , modulation time (win32 only) '
    ModulationDepth   As Single     ' [in/out] 0.0   , 1.0   , 0.0    , modulation depth (win32 only) '
    AirAbsorptionHF   As Single     ' [in/out] -100  , 0.0   , -5.0   , change in level per meter at high frequencies (win32 only) '
    HFReference       As Single     ' [in/out] 1000.0, 20000 , 5000.0 , reference high frequency (hz) (win32/xbox) '
    LFReference       As Single     ' [in/out] 20.0  , 1000.0, 250.0  , reference low frequency (hz) (win32 only) '
    RoomRolloffFactor As Single     ' [in/out] 0.0   , 10.0  , 0.0    , like FMOD_3D_Listener_SetRolloffFactor but for room effect (win32/xbox) '
    Diffusion         As Single     ' [in/out] 0.0   , 100.0 , 100.0  , Value that controls the echo density in the late reverberation decay. (xbox only) '
    Density           As Single     ' [in/out] 0.0   , 100.0 , 100.0  , Value that controls the modal density in the late reverberation decay (xbox only) '
    Flags             As Long       ' [in/out] FMOD_REVERB_FLAGS - modifies the behavior of above properties (win32/ps2) '
End Type


'
'[define]
'[
    '[NAME]
    'FMOD_REVERB_FLAGS

    '[Description]
    'Values for the Flags member of the FMOD_REVERB_PROPERTIES structure.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_REVERB_PROPERTIES
']
'
Public Const FMOD_REVERB_FLAGS_DECAYTIMESCALE        As Long = &H1               ' 'EnvSize' affects reverberation decay time '
Public Const FMOD_REVERB_FLAGS_REFLECTIONSSCALE      As Long = &H2             ' 'EnvSize' affects reflection level '
Public Const FMOD_REVERB_FLAGS_REFLECTIONSDELAYSCALE As Long = &H4        ' 'EnvSize' affects initial reflection delay time '
Public Const FMOD_REVERB_FLAGS_REVERBSCALE           As Long = &H8        ' 'EnvSize' affects reflections level '
Public Const FMOD_REVERB_FLAGS_REVERBDELAYSCALE      As Long = &H10       ' 'EnvSize' affects late reverberation delay time '
Public Const FMOD_REVERB_FLAGS_DECAYHFLIMIT          As Long = &H20       ' AirAbsorptionHF affects DecayHFRatio '
Public Const FMOD_REVERB_FLAGS_ECHOTIMESCALE         As Long = &H40       ' 'EnvSize' affects echo time '
Public Const FMOD_REVERB_FLAGS_MODULATIONTIMESCALE   As Long = &H80       ' 'EnvSize' affects modulation time '
Public Const FMOD_REVERB_FLAGS_DEFAULT               As Long = (FMOD_REVERB_FLAGS_DECAYTIMESCALE Or FMOD_REVERB_FLAGS_REFLECTIONSSCALE Or FMOD_REVERB_FLAGS_REFLECTIONSDELAYSCALE Or FMOD_REVERB_FLAGS_REVERBSCALE Or FMOD_REVERB_FLAGS_REVERBDELAYSCALE Or FMOD_REVERB_FLAGS_DECAYHFLIMIT)
' [DEFINE_END] '


'
'[Structure]
'[
    '[Description]
    'Structure defining the properties for a reverb source, related to a FMOD channel.

    'For more indepth descriptions of the reverb properties under win32, please see the EAX3
    'documentation at http://developer.creative.com/ under the 'downloads' section.
    'If they do not have the EAX3 documentation, then most information can be attained from
    'the EAX2 documentation, as EAX3 only adds some more parameters and functionality on top of
    'EAX2.

    'Note the default reverb properties are the same as the FMOD_PRESET_GENERIC preset.
    'Note that integer values that typically range from -10,000 to 1000 are represented in
    'decibels, and are of a logarithmic scale, not linear, wheras FMOD_FLOAT values are typically linear.
    'PORTABILITY: Each member has the platform it supports in braces ie (win32/xbox).
    'Some reverb parameters are only supported in win32 and some only on xbox. If all parameters are set then
    'the reverb should product a similar effect on either platform.
    'Linux and FMODCE do not support the reverb api.

    'The numerical values listed below are the maximum, minimum and default values for each variable respectively.

    '[REMARKS]

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_Channel_SetReverbProperties
    'FMOD_Channel_GetReverbProperties
    'FMOD_REVERB_CHANNELFLAGS
']
'
Public Type FMOD_REVERB_CHANNELPROPERTIES
                                    '          MIN     MAX    DEFAULT  DESCRIPTION '
    Direct               As Long    ' [in/out] -10000, 1000,  0,       direct path level (at low and mid frequencies) (win32/xbox) '
    DirectHF             As Long    ' [in/out] -10000, 0,     0,       relative direct path level at high frequencies (win32/xbox) '
    Room                 As Long    ' [in/out] -10000, 1000,  0,       room effect level (at low and mid frequencies) (win32/xbox) '
    RoomHF               As Long    ' [in/out] -10000, 0,     0,       relative room effect level at high frequencies (win32/xbox) '
    Obstruction          As Long    ' [in/out] -10000, 0,     0,       main obstruction control (attenuation at high frequencies)  (win32/xbox) '
    ObstructionLFRatio   As Single  ' [in/out] 0.0,    1.0,   0.0,     obstruction low-frequency level re. main control (win32/xbox) '
    Occlusion            As Long    ' [in/out] -10000, 0,     0,       main occlusion control (attenuation at high frequencies) (win32/xbox) '
    OcclusionLFRatio     As Single  ' [in/out] 0.0,    1.0,   0.25,    occlusion low-frequency level re. main control (win32/xbox) '
    OcclusionRoomRatio   As Single  ' [in/out] 0.0,    10.0,  1.5,     relative occlusion control for room effect (win32) '
    OcclusionDirectRatio As Single  ' [in/out] 0.0,    10.0,  1.0,     relative occlusion control for direct path (win32) '
    Exclusion            As Long    ' [in/out] -10000, 0,     0,       main exlusion control (attenuation at high frequencies) (win32) '
    ExclusionLFRatio     As Single  ' [in/out] 0.0,    1.0,   1.0,     exclusion low-frequency level re. main control (win32) '
    OutsideVolumeHF      As Long    ' [in/out] -10000, 0,     0,       outside sound cone level at high frequencies (win32) '
    DopplerFactor        As Single  ' [in/out] 0.0,    10.0,  0.0,     like DS3D flDopplerFactor but per source (win32) '
    RolloffFactor        As Single  ' [in/out] 0.0,    10.0,  0.0,     like DS3D flRolloffFactor but per source (win32) '
    RoomRolloffFactor    As Single  ' [in/out] 0.0,    10.0,  0.0,     like DS3D flRolloffFactor but for room effect (win32/xbox) '
    AirAbsorptionFactor  As Single  ' [in/out] 0.0,    10.0,  1.0,     multiplies AirAbsorptionHF member of FMOD_REVERB_PROPERTIES (win32) '
    Flags                As Long    ' [in/out] FMOD_REVERB_CHANNELFLAGS - modifies the behavior of properties (win32) '
End Type


'
'[Structure]
'[
    '[Description]
    'Strcture to define the parameters for a DSP unit.

    '[REMARKS]
    'Members marked with '[in] mean the user sets the value before passing it to the function.<br>
    'Members marked with '[out] mean FMOD sets the value to be used after the function exits.<br>
    '<br>
    'There are 2 different ways to change a parameter in this architecture.<br>
    'One is to use DSP::setParameter / DSP::getParameter.  This is platform independant and is dynamic, so new unknown plugins can have their parameters enumerated and used.<br>
    'The other is to use DSP::showConfigDialog.  This is platform specific and requires a GUI, and will display a dialog box to configure the plugin.<br>

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_DSP_CATEGORY
    'FMOD_System_CreateDSP
    'FMOD_System_GetDSP
']
'
Public Type FMOD_DSP_DESCRIPTION
    name As String                  ' [in] Name of the unit to be displayed in the network.
    version As Long                 ' [in] Plugin writer's version number.
    Channels As Long                ' [in] Number of channels.  Use 0 to process whatever number of channels is currently in the network.  >0 would be mostly used if the unit is a fixed format generator and not a filter.
    create As Long                  ' [in] Create callback.  This is called when DSP unit is created.  Can be null.
    release As Long                 ' [in] Release callback.  This is called just before the unit is freed so the user can do any cleanup needed for the unit.  Can be null.
    reset As Long                   ' [in] Reset callback.  This is called by the user to reset any history buffers that may need resetting for a filter, when it is to be used or re-used for the first time to its initial clean state.  Use to avoid clicks or artifacts.
    Read As Long                    ' [in] Read callback.  Processing is done here.  Can be null.
    setpos As Long                  ' [in] Setposition callback.  This is called if the unit becomes virtualized and needs to simply update positions etc.  Can be null.
    
    numparameters As Long           ' [in] Number of parameters used in this filter.  The user finds this with DSP::getNumParameters '
    paramdesc As Long               ' [in] Variable number of parameter structures.
    setparameter As Long            ' [in] This is called when the user calls DSP::setParameter.  Can be null.
    getparameter As Long            ' [in] This is called when the user calls DSP::getParameter.  Can be null.
    config As Long                  ' [in] This is called when the user calls DSP::showConfigDialog.  Can be used to display a dialog to configure the filter.  Can be null.
    Configwidth As Long             ' [in] Width of config dialog graphic if there is one.  0 otherwise.
    Configheight As Long            ' [in] Height of config dialog graphic if there is one.  0 otherwise.
    userdata As Long                ' [in] Optional. Specify 0 to ignore. This is user data to be attached to the DSP unit during creation.  Access via DSP::getUserData.
End Type


'
'[define]
'[
    '[NAME]
    'FMOD_REVERB_CHANNELFLAGS

    '[Description]
    'Values for the Flags member of the FMOD_REVERB_CHANNELPROPERTIES structure.

    '[REMARKS]
    'For EAX4 support with multiple reverb environments, set FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0,
    'FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1 or/and FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT2 in the flags member
    'of FMOD_REVERB_CHANNELPROPERTIES to specify which environment instance(s) to target. <br>
    'Only up to 2 environments to target can be specified at once. Specifying three will result in an error.
    'If the sound card does not support EAX4, the environment flag is ignored.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_REVERB_CHANNELPROPERTIES
']
'
Public Const FMOD_REVERB_CHANNELFLAGS_DIRECTHFAUTO As Long = &H1        ' Automatic setting of 'Direct'  due to distance from listener '
Public Const FMOD_REVERB_CHANNELFLAGS_ROOMAUTO     As Long = &H2        ' Automatic setting of 'Room'  due to distance from listener '
Public Const FMOD_REVERB_CHANNELFLAGS_ROOMHFAUTO   As Long = &H4        ' Automatic setting of 'RoomHF' due to distance from listener '
Public Const FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0 As Long = &H8        ' EAX4 only. Specify channel to target reverb instance 0.
Public Const FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1 As Long = &H10       ' EAX4 only. Specify channel to target reverb instance 1.
Public Const FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT2 As Long = &H20       ' EAX4 only. Specify channel to target reverb instance 2.
Public Const FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT3 As Long = &H40       ' EAX5 only. Specify channel to target reverb instance 3.

Public Const FMOD_REVERB_CHANNELFLAGS_DEFAULT      As Long = (FMOD_REVERB_CHANNELFLAGS_DIRECTHFAUTO Or FMOD_REVERB_CHANNELFLAGS_ROOMAUTO Or FMOD_REVERB_CHANNELFLAGS_ROOMHFAUTO Or FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0)
' [DEFINE_END] '


'
'[STRUCTURE]
'[
    '[DESCRIPTION]
    'Settings for advanced features like configuring memory and cpu usage for the FMOD_CREATECOMPRESSEDSAMPLE feature.
   
       '[REMARKS]
       'maxMPEGcodecs / maxADPCMcodecs / maxXMAcodecs will determine the maximum cpu usage of playing realtime samples.  Use this to lower potential excess cpu usage and also control memory usage.<br>
   
       '[PLATFORMS]
       'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3
   
       '[SEE_ALSO]
       'System::setAdvancedSettings
       'System::getAdvancedSettings
']
'
Public Type FMOD_ADVANCEDSETTINGS
    cbsize As Long                   ' Size of structure.  Use sizeof(FMOD_ADVANCEDSETTINGS)
    maxMPEGcodecs As Long            ' For use with FMOD_CREATECOMPRESSEDSAMPLE only.  Mpeg  codecs consume 48,696 per instance and this number will determine how many mpeg channels can be played simultaneously.  Default = 16.
    maxADPCMcodecs As Long           ' For use with FMOD_CREATECOMPRESSEDSAMPLE only.  ADPCM codecs consume 1k per instance and this number will determine how many ADPCM channels can be played simultaneously.  Default = 32.
    maxXMAcodecs As Long             ' For use with FMOD_CREATECOMPRESSEDSAMPLE only.  XMA   codecs consume 8k per instance and this number will determine how many XMA channels can be played simultaneously.  Default = 32.
    maxPCMcodecs As Long             ' For use with PS3 only.                          PCM   codecs consume 12,672 bytes per instance and this number will determine how many streams and PCM voices can be played simultaneously. Default = 16 */
    ASIONumChannels As Long          ' [in/out]
    ASIOChannelList As Long          ' [in/out]
    ASIOSpeakerList As Long          ' [in/out] Optional. Specify 0 to ignore. Pointer to a list of speakers that the ASIO channels map to.  This can be called after System::init to remap ASIO output. */
    max3DReverbDSPs As Long          ' [in/out] The max number of 3d reverb DSP's in the system.
    HRTFMinAngle As Single           ' [in/out] For use with FMOD_INIT_SOFTWARE_HRTF.  The angle (0-360) of a 3D sound from the listener's forward vector at which the HRTF function begins to have an effect.  Default = 180.0. */
    HRTFMaxAngle As Single           ' [in/out] For use with FMOD_INIT_SOFTWARE_HRTF.  The angle (0-360) of a 3D sound from the listener's forward vector at which the HRTF function begins to have maximum effect.  Default = 360.0.  */
    HRTFFreq As Single               ' [in/out] For use with FMOD_INIT_SOFTWARE_HRTF.  The cutoff frequency of the HRTF's lowpass filter function when at maximum effect. (i.e. at HRTFMaxAngle).  Default = 4000.0. */
    vol0virtualvol As Single         ' [in/out] For use with FMOD_INIT_VOL0_BECOMES_VIRTUAL.  If this flag is used, and the volume is 0.0, then the sound will become virtual.  Use this value to raise the threshold to a different point where a sound goes virtual. */
    eventqueuesize As Long           ' [in/out] Optional. Specify 0 to ignore. For use with FMOD Event system only.  Specifies the number of slots available for simultaneous non blocking loads.  Default = 32. */
    defaultDecodeBufferSize As Long  ' [in/out] Optional. Specify 0 to ignore. For streams. This determines the default size of the double buffer (in milliseconds) that a stream uses.  Default = 400ms */
End Type


'
'[ENUM]
'[
    '[NAME]
    'FMOD_MISC_VALUES

    '[Description]
    'Miscellaneous values for FMOD functions.

    '[PLATFORMS]
    'Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    '[SEE_ALSO]
    'FMOD_System_Playsound
    'FMOD_System_PlayDSP
    'FMOD_System_GetChannel
']
'
Public Enum FMOD_CHANNELINDEX
    FMOD_CHANNEL_FREE = -1     ' For a channel index, FMOD chooses a free voice using the priority system.
    FMOD_CHANNEL_REUSE = -2    ' For a channel index, re-use the channel handle that was passed in.
End Enum

' [DEFINE_END] '

'
    'FMOD Callbacks
'
'typedef FMOD_RESULT (F_CALLBACK *FMOD_CHANNEL_CALLBACK)      (FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2);

'typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_OPENCALLBACK)     (const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata);
'typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_CLOSECALLBACK)    (void *handle, void *userdata);
'typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_READCALLBACK)     (void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata);
'typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_SEEKCALLBACK)     (void *handle, int pos, void *userdata);

'typedef void *      (F_CALLBACK *FMOD_MEMORY_ALLOCCALLBACK)  (unsigned int size, FMOD_MEMORY_TYPE type);
'typedef void *      (F_CALLBACK *FMOD_MEMORY_REALLOCCALLBACK)(void *ptr, unsigned int size, FMOD_MEMORY_TYPE type);
'typedef void        (F_CALLBACK *FMOD_MEMORY_FREECALLBACK)   (void *ptr, FMOD_MEMORY_TYPE type);


' ========================================================================================== '
' FUNCTION PROTOTYPES                                                                        '
' ========================================================================================== '

'
    'FMOD System memory functions (optional).
'

Public Declare Function FMOD_Memory_Initialize Lib "fmodex.dll" (ByVal poolmem As Long, ByVal poollen As Long, ByVal useralloc As Long, ByVal userrealloc As Long, ByVal userfree As Long) As FMOD_RESULT
Public Declare Function FMOD_Memory_GetStats Lib "fmodex.dll" (ByRef currentalloced As Long, ByRef maxalloced As Long) As FMOD_RESULT

Public Declare Function FMOD_Debug_SetLevel Lib "fmodex.dll" (ByVal level As FMOD_DEBUGLEVEL) As FMOD_RESULT
Public Declare Function FMOD_Debug_GetLevel Lib "fmodex.dll" (ByRef level As FMOD_DEBUGLEVEL) As FMOD_RESULT
Public Declare Function FMOD_File_SetDiskBusy Lib "fmodex.dll" (ByVal busy As Long) As FMOD_RESULT
Public Declare Function FMOD_File_GetDiskBusy Lib "fmodex.dll" (ByRef busy As Long) As FMOD_RESULT

'
    'FMOD System factory functions.  Use this to create an FMOD System Instance.  below you will see FMOD_System_Init/Close to get started.
'

Public Declare Function FMOD_System_Create Lib "fmodex.dll" (ByRef system As Long) As FMOD_RESULT
Public Declare Function FMOD_System_Release Lib "fmodex.dll" (ByVal system As Long) As FMOD_RESULT


'
    'System' API
'

'
     'Pre-init functions.
'

Public Declare Function FMOD_System_SetOutput Lib "fmodex.dll" (ByVal system As Long, ByVal output As FMOD_OUTPUTTYPE) As FMOD_RESULT
Public Declare Function FMOD_System_GetOutput Lib "fmodex.dll" (ByVal system As Long, ByRef output As FMOD_OUTPUTTYPE) As FMOD_RESULT
Public Declare Function FMOD_System_GetNumDrivers Lib "fmodex.dll" (ByVal system As Long, ByRef Numdrivers As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetDriverInfo Lib "fmodex.dll" (ByVal system As Long, ByVal Id As Long, ByRef name As Byte, ByVal namelen As Long, ByRef GUID As FMOD_GUID) As FMOD_RESULT
Public Declare Function FMOD_System_GetDriverCaps Lib "fmodex.dll" (ByVal system As Long, ByVal Id As Long, ByRef caps As FMOD_CAPS, ByRef minfrequency As Long, ByRef maxfrequency As Long, ByRef controlpanelspeakermode As FMOD_SPEAKERMODE) As FMOD_RESULT
Public Declare Function FMOD_System_SetDriver Lib "fmodex.dll" (ByVal system As Long, ByVal Driver As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetDriver Lib "fmodex.dll" (ByVal system As Long, ByRef Driver As Long) As FMOD_RESULT
Public Declare Function FMOD_System_SetHardwareChannels Lib "fmodex.dll" (ByVal system As Long, ByVal Min2d As Long, ByVal Max2d As Long, ByVal Min3d As Long, ByVal Max3d As Long) As FMOD_RESULT
Public Declare Function FMOD_System_SetSoftwareChannels Lib "fmodex.dll" (ByVal system As Long, ByVal Numsoftwarechannels As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetSoftwareChannels Lib "fmodex.dll" (ByVal system As Long, ByRef Numsoftwarechannels As Long) As FMOD_RESULT
Public Declare Function FMOD_System_SetSoftwareFormat Lib "fmodex.dll" (ByVal system As Long, ByVal Samplerate As Long, ByVal Format As FMOD_SOUND_FORMAT, ByVal Numoutputchannels As Long, ByVal Maxinputchannels As Long, ByVal Resamplemethod As FMOD_DSP_RESAMPLER) As FMOD_RESULT
Public Declare Function FMOD_System_GetSoftwareFormat Lib "fmodex.dll" (ByVal system As Long, ByRef Samplerate As Long, ByRef Format As FMOD_SOUND_FORMAT, ByRef Numoutputchannels As Long, ByRef Maxinputchannels As Long, ByRef Resamplemethod As FMOD_DSP_RESAMPLER, ByRef Bits As Long) As FMOD_RESULT
Public Declare Function FMOD_System_SetDSPBufferSize Lib "fmodex.dll" (ByVal system As Long, ByVal Bufferlength As Long, ByVal Numbuffers As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetDSPBufferSize Lib "fmodex.dll" (ByVal system As Long, ByRef Bufferlength As Long, ByRef Numbuffers As Long) As FMOD_RESULT
Public Declare Function FMOD_System_SetFileSystem Lib "fmodex.dll" (ByVal system As Long, ByVal useropen As Long, ByVal userclose As Long, ByVal userread As Long, ByVal userseek As Long, ByVal Buffersize As Long) As FMOD_RESULT
Public Declare Function FMOD_System_AttachFileSystem Lib "fmodex.dll" (ByVal system As Long, ByVal useropen As Long, ByVal userclose As Long, ByVal userread As Long, ByVal userseek As Long) As FMOD_RESULT
Public Declare Function FMOD_System_SetAdvancedSettings Lib "fmodex.dll" (ByVal system As Long, ByRef settings As FMOD_ADVANCEDSETTINGS) As FMOD_RESULT
Public Declare Function FMOD_System_GetAdvancedSettings Lib "fmodex.dll" (ByVal system As Long, ByRef settings As FMOD_ADVANCEDSETTINGS) As FMOD_RESULT
Public Declare Function FMOD_System_SetSpeakerMode Lib "fmodex.dll" (ByVal system As Long, ByVal Speakermode As FMOD_SPEAKERMODE) As FMOD_RESULT
Public Declare Function FMOD_System_GetSpeakerMode Lib "fmodex.dll" (ByVal system As Long, ByRef Speakermode As FMOD_SPEAKERMODE) As FMOD_RESULT

'
     'Plug-in support
'

Public Declare Function FMOD_System_SetPluginPath Lib "fmodex.dll" (ByVal system As Long, ByVal Path As String) As FMOD_RESULT
Public Declare Function FMOD_System_LoadPlugin Lib "fmodex.dll" (ByVal system As Long, ByVal Filename As String, ByRef Handle As Long, ByVal Priority As Long) As FMOD_RESULT
Public Declare Function FMOD_System_UnloadPlugin Lib "fmodex.dll" (ByVal system As Long, ByVal Handle As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetNumPlugins Lib "fmodex.dll" (ByVal system As Long, ByVal Plugintype As FMOD_PLUGINTYPE, ByRef Numplugins As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetPluginHandle Lib "fmodex.dll" (ByVal system As Long, ByVal Plugintype As FMOD_PLUGINTYPE, ByVal Index As Long, ByRef Handle As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetPluginInfo Lib "fmodex.dll" (ByVal system As Long, ByVal Handle As Long,  ByRef Plugintype As FMOD_PLUGINTYPE, ByRef name As Byte, ByVal namelen As Long, ByRef version As Long) As FMOD_RESULT

Public Declare Function FMOD_System_SetOutputByPlugin Lib "fmodex.dll" (ByVal system As Long, ByVal Handle As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetOutputByPlugin Lib "fmodex.dll" (ByVal system As Long, ByRef Handle As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateDSPByPlugin Lib "fmodex.dll" (ByVal system As Long, ByVal Handle As Long, ByRef dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateCodec Lib "fmodex.dll" (ByVal system As Long, ByVal CodecDescription As Long) As FMOD_RESULT
'
     'Init/Close
'

Public Declare Function FMOD_System_Init Lib "fmodex.dll" (ByVal system As Long, ByVal Maxchannels As Long, ByVal Flags As FMOD_INITFLAGS, ByVal Extradriverdata As Long) As FMOD_RESULT
Public Declare Function FMOD_System_Close Lib "fmodex.dll" (ByVal system As Long) As FMOD_RESULT

'
     'General post-init system functions
'

Public Declare Function FMOD_System_Update Lib "fmodex.dll" (ByVal system As Long) As FMOD_RESULT

Public Declare Function FMOD_System_Set3DSettings Lib "fmodex.dll" (ByVal system As Long, ByVal Dopplerscale As Single, ByVal Distancefactor As Single, ByVal Rolloffscale As Single) As FMOD_RESULT
Public Declare Function FMOD_System_Get3DSettings Lib "fmodex.dll" (ByVal system As Long, ByRef Dopplerscale As Single, ByRef Distancefactor As Single, ByRef Rolloffscale As Single) As FMOD_RESULT
Public Declare Function FMOD_System_Set3DNumListeners Lib "fmodex.dll" (ByVal system As Long, ByVal Numlisteners As Long) As FMOD_RESULT
Public Declare Function FMOD_System_Get3DNumListeners Lib "fmodex.dll" (ByVal system As Long, ByRef Numlisteners As Long) As FMOD_RESULT
Public Declare Function FMOD_System_Set3DListenerAttributes Lib "fmodex.dll" (ByVal system As Long, ByVal Listener As Long, ByRef Pos As FMOD_VECTOR, ByRef Vel As FMOD_VECTOR, ByRef Forward As FMOD_VECTOR, ByRef Up As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_System_Get3DListenerAttributes Lib "fmodex.dll" (ByVal system As Long, ByVal Listener As Long, ByRef Pos As FMOD_VECTOR, ByRef Vel As FMOD_VECTOR, ByRef Forward As FMOD_VECTOR, ByRef Up As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_System_Set3DRolloffCallback Lib "fmodex.dll" (ByVal system As Long, ByVal Callback As Long) As FMOD_RESULT
Public Declare Function FMOD_System_Set3DSpeakerPosition Lib "fmodex.dll" (ByVal system As Long, ByVal speaker As FMOD_SPEAKER, ByVal X As Single, ByVal Y As Single, ByVal active As Long) As FMOD_RESULT
Public Declare Function FMOD_System_Get3DSpeakerPosition Lib "fmodex.dll" (ByVal system As Long, ByRef speaker As FMOD_SPEAKER, ByRef X As Single, ByRef Y As Single, ByRef active As Long) As FMOD_RESULT

Public Declare Function FMOD_System_SetStreamBufferSize Lib "fmodex.dll" (ByVal system As Long, ByVal Filebuffersize As Long, ByVal Filebuffersizetype As FMOD_TIMEUNIT) As FMOD_RESULT
Public Declare Function FMOD_System_GetStreamBufferSize Lib "fmodex.dll" (ByVal system As Long, ByRef Filebuffersize As Long, ByRef Filebuffersizetype As FMOD_TIMEUNIT) As FMOD_RESULT

'
     'System information functions.
'

Public Declare Function FMOD_System_GetVersion Lib "fmodex.dll" (ByVal system As Long, ByRef version As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetOutputHandle Lib "fmodex.dll" (ByVal system As Long, ByRef Handle As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetChannelsPlaying Lib "fmodex.dll" (ByVal system As Long, ByRef Channels As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetHardwareChannels Lib "fmodex.dll" (ByVal system As Long, ByRef Num2d As Long, ByRef Num3d As Long, ByRef total As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetCPUUsage Lib "fmodex.dll" (ByVal system As Long, ByRef Dsp As Single, ByRef Stream As Single, ByRef Update As Single, ByRef total As Single) As FMOD_RESULT
Public Declare Function FMOD_System_GetSoundRAM Lib "fmodex.dll" (ByVal system As Long, ByRef currentalloced As Long, ByRef maxalloced As Long, ByRef total As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetNumCDROMDrives Lib "fmodex.dll" (ByVal system As Long, ByRef Numdrives As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetCDROMDriveName Lib "fmodex.dll" (ByVal system As Long, ByVal Drive As Long, ByRef Drivename As Byte, ByVal Drivenamelen As Long, ByRef Scsiname As Byte, ByVal Scsinamelen As Long, ByRef Devicename As Byte, ByVal Devicenamelen As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetSpectrum Lib "fmodex.dll" (ByVal system As Long, ByRef Spectrumarray As Single, ByVal Numvalues As Long, ByVal Channeloffset As Long, ByRef Windowtype As FMOD_DSP_FFT_WINDOW) As FMOD_RESULT
Public Declare Function FMOD_System_GetWaveData Lib "fmodex.dll" (ByVal system As Long, ByRef Wavearray As Single, ByVal Numvalues As Long, ByVal Channeloffset As Long) As FMOD_RESULT

'
     'Sound/DSP/Channel/FX creation and retrieval.
'

' ** FMOD_System_CreateSound and FMOD_System_CreateStream are wrapper functions that have been
' ** defined at the bottom of this bas file.

'Public Declare Function FMOD_System_CreateSound (ByVal system As Long, ByVal name_or_data As String, ByVal mode As FMOD_MODE, ByRef sound As Long) As FMOD_RESULT
'Public Declare Function FMOD_System_CreateStream (ByVal system As Long, ByVal name_or_data As String, ByVal mode As FMOD_MODE, ByRef sound As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateSoundEx Lib "fmodex.dll" Alias "FMOD_System_CreateSound" (ByVal system As Long, ByVal Name_or_data As String, ByVal Mode As FMOD_MODE, ByRef exinfo As FMOD_CREATESOUNDEXINFO, ByRef Sound As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateStreamEx Lib "fmodex.dll" Alias "FMOD_System_CreateStream" (ByVal system As Long, ByVal Name_or_data As String, ByVal Mode As FMOD_MODE, ByRef exinfo As FMOD_CREATESOUNDEXINFO, ByRef Sound As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateDSP Lib "fmodex.dll" (ByVal system As Long, ByRef description As FMOD_DSP_DESCRIPTION, ByRef Dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateDSPByType Lib "fmodex.dll" (ByVal system As Long, ByVal dsptype As FMOD_DSP_TYPE, ByRef Dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateDSPByIndex Lib "fmodex.dll" (ByVal system As Long, ByVal Index As Long, ByRef Dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateChannelGroup Lib "fmodex.dll" (ByVal system As Long, ByVal name As String, ByRef Channelgroup As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateSoundGroup Lib "fmodex.dll" (ByVal system As Long, ByRef name As Byte, ByRef soundgroup As Long) As FMOD_RESULT
Public Declare Function FMOD_System_CreateReverb Lib "fmodex.dll" (ByVal system As Long, ByRef reverb As Long) As FMOD_RESULT

Public Declare Function FMOD_System_PlaySound Lib "fmodex.dll" (ByVal system As Long, ByVal channelid As FMOD_CHANNELINDEX, ByVal Sound As Long, ByVal paused As Long, ByRef channel As Long) As FMOD_RESULT
Public Declare Function FMOD_System_PlayDSP Lib "fmodex.dll" (ByVal system As Long, ByVal channelid As FMOD_CHANNELINDEX, ByVal Dsp As Long, ByVal paused As Long, ByRef channel As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetChannel Lib "fmodex.dll" (ByVal system As Long, ByVal channelid As Long, ByRef channel As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetMasterChannelGroup Lib "fmodex.dll" (ByVal system As Long, ByRef Channelgroup As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetMasterSoundGroup Lib "fmodex.dll" (ByVal system As Long, ByRef soundgroup As Long) As FMOD_RESULT

'
     'Reverb API
'

Public Declare Function FMOD_System_SetReverbProperties Lib "fmodex.dll" (ByVal system As Long, ByRef Prop As FMOD_REVERB_PROPERTIES) As FMOD_RESULT
Public Declare Function FMOD_System_GetReverbProperties Lib "fmodex.dll" (ByVal system As Long, ByRef Prop As FMOD_REVERB_PROPERTIES) As FMOD_RESULT
Public Declare Function FMOD_System_SetReverbAmbientProperties Lib "fmodex.dll" (ByVal system As Long, ByRef Prop As FMOD_REVERB_PROPERTIES) As FMOD_RESULT
Public Declare Function FMOD_System_GetReverbAmbientProperties Lib "fmodex.dll" (ByVal system As Long, ByRef Prop As FMOD_REVERB_PROPERTIES) As FMOD_RESULT


'
     'System level DSP access.
'

Public Declare Function FMOD_System_GetDSPHead Lib "fmodex.dll" (ByVal system As Long, ByRef Dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_System_AddDSP Lib "fmodex.dll" (ByVal system As Long, ByVal Dsp As Long, ByRef dspconnection As Long) As FMOD_RESULT
Public Declare Function FMOD_System_LockDSP Lib "fmodex.dll" (ByVal system As Long) As FMOD_RESULT
Public Declare Function FMOD_System_UnlockDSP Lib "fmodex.dll" (ByVal system As Long) As FMOD_RESULT

'
     'Recording API
'

Public Declare Function FMOD_System_GetRecordNumDrivers Lib "fmodex.dll" (ByVal system As Long, ByRef Numdrivers As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetRecordDriverInfo Lib "fmodex.dll" (ByVal system As Long, ByVal Id As Long, ByRef name As Byte, ByVal namelen As Long, ByRef GUID As FMOD_GUID) As FMOD_RESULT
Public Declare Function FMOD_System_GetRecordPosition Lib "fmodex.dll" (ByVal system As Long, ByVal Id As Long, ByRef position As Long) As FMOD_RESULT

Public Declare Function FMOD_System_RecordStart Lib "fmodex.dll" (ByVal system As Long, ByVal Id As Long, ByVal Sound As Long, ByVal Loop_ As Long) As FMOD_RESULT
Public Declare Function FMOD_System_RecordStop Lib "fmodex.dll" (ByVal system As Long, ByVal Id As Long) As FMOD_RESULT
Public Declare Function FMOD_System_IsRecording Lib "fmodex.dll" (ByVal system As Long, ByVal Id As Long, ByRef Recording As Long) As FMOD_RESULT

'
     'Geometry API
'

Public Declare Function FMOD_System_CreateGeometry Lib "fmodex.dll" (ByVal system As Long, ByVal MaxPolygons As Long, ByVal MaxVertices As Long, ByRef Geometryf As Long) As FMOD_RESULT
Public Declare Function FMOD_System_SetGeometrySettings Lib "fmodex.dll" (ByVal system As Long, ByVal Maxworldsize As Single) As FMOD_RESULT
Public Declare Function FMOD_System_GetGeometrySettings Lib "fmodex.dll" (ByVal system As Long, ByRef Maxworldsize As Single) As FMOD_RESULT
Public Declare Function FMOD_System_LoadGeometry Lib "fmodex.dll" (ByVal system As Long, ByVal Data As Long, ByVal DataSize As Long, ByRef Geometry As Long) As FMOD_RESULT

'
     'Network functions
'

Public Declare Function FMOD_System_SetNetworkProxy Lib "fmodex.dll" (ByVal system As Long, ByVal Proxy As String) As FMOD_RESULT
Public Declare Function FMOD_System_GetNetworkProxy Lib "fmodex.dll" (ByVal system As Long, ByRef Proxy As Byte, ByVal Proxylen As Long) As FMOD_RESULT
Public Declare Function FMOD_System_SetNetworkTimeout Lib "fmodex.dll" (ByVal system As Long, ByVal timeout As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetNetworkTimeout Lib "fmodex.dll" (ByVal system As Long, ByRef timeout As Long) As FMOD_RESULT

'
     'Userdata set/get
'

Public Declare Function FMOD_System_SetUserData Lib "fmodex.dll" (ByVal system As Long, ByVal userdata As Long) As FMOD_RESULT
Public Declare Function FMOD_System_GetUserData Lib "fmodex.dll" (ByVal system As Long, ByRef userdata As Long) As FMOD_RESULT

'
    'Sound' API
'

Public Declare Function FMOD_Sound_Release Lib "fmodex.dll" (ByVal Sound As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetSystemObject Lib "fmodex.dll" (ByVal Sound As Long, ByRef system As Long) As FMOD_RESULT

'
     'Standard sound manipulation functions.
'

Public Declare Function FMOD_Sound_Lock Lib "fmodex.dll" (ByVal Sound As Long, ByVal Offset As Long, ByVal Length As Long, ByRef Ptr1 As Long, ByRef Ptr2 As Long, ByRef Len1 As Long, ByRef Len2 As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_Unlock Lib "fmodex.dll" (ByVal Sound As Long, ByVal Ptr1 As Long, ByVal Ptr2 As Long, ByVal Len1 As Long, ByVal Len2 As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_SetDefaults Lib "fmodex.dll" (ByVal Sound As Long, ByVal Frequency As Single, ByVal volume As Single, ByVal Pan As Single, ByVal Priority As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetDefaults Lib "fmodex.dll" (ByVal Sound As Long, ByRef Frequency As Single, ByRef volume As Single, ByRef Pan As Single, ByRef Priority As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_SetVariations Lib "fmodex.dll" (ByVal Sound As Long, ByVal Frequencyvar As Single, ByVal Volumevar As Single, ByVal Panvar As Single) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetVariations Lib "fmodex.dll" (ByVal Sound As Long, ByRef Frequencyvar As Single, ByRef Volumevar As Single, ByRef Panvar As Single) As FMOD_RESULT
Public Declare Function FMOD_Sound_Set3DMinMaxDistance Lib "fmodex.dll" (ByVal Sound As Long, ByVal min As Single, ByVal max As Single) As FMOD_RESULT
Public Declare Function FMOD_Sound_Get3DMinMaxDistance Lib "fmodex.dll" (ByVal Sound As Long, ByRef min As Single, ByRef max As Single) As FMOD_RESULT
Public Declare Function FMOD_Sound_Set3DConeSettings Lib "fmodex.dll" (ByVal Sound As Long, ByVal Insideconeangle As Single, ByVal Outsideconeangle As Single, ByVal Outsidevolume As Single) As FMOD_RESULT
Public Declare Function FMOD_Sound_Get3DConeSettings Lib "fmodex.dll" (ByVal Sound As Long, ByRef Insideconeangle As Single, ByRef Outsideconeangle As Single, ByRef Outsidevolume As Single) As FMOD_RESULT
Public Declare Function FMOD_Sound_Set3DCustomRolloff Lib "fmodex.dll" (ByVal Sound As Long, ByRef Points As FMOD_VECTOR, ByVal numpoints As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_Get3DCustomRolloff Lib "fmodex.dll" (ByVal Sound As Long, ByRef Points As Long, ByRef numpoints As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_SetSubSound Lib "fmodex.dll" (ByVal Sound As Long, ByVal Index As Long, ByVal Subsound As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetSubSound Lib "fmodex.dll" (ByVal Sound As Long, ByVal Index As Long, ByRef Subsound As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_SetSubSoundSentence Lib "fmodex.dll" (ByVal Sound As Long, ByRef Subsoundlist As Long, ByVal Numsubsounds As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetName Lib "fmodex.dll" (ByVal Sound As Long, ByRef name As Byte, ByVal namelen As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetLength Lib "fmodex.dll" (ByVal Sound As Long, ByRef Length As Long, ByVal Lengthtype As FMOD_TIMEUNIT) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetFormat Lib "fmodex.dll" (ByVal Sound As Long, ByRef Soundtype As FMOD_SOUND_TYPE, ByRef Format As FMOD_SOUND_FORMAT, ByRef Channels As Long, ByRef Bits As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetNumSubSounds Lib "fmodex.dll" (ByVal Sound As Long, ByRef Numsubsounds As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetNumTags Lib "fmodex.dll" (ByVal Sound As Long, ByRef Numtags As Long, ByRef Numtagsupdated As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetTag Lib "fmodex.dll" (ByVal Sound As Long, ByVal name As String, ByVal Index As Long, ByRef Tag As FMOD_TAG) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetOpenState Lib "fmodex.dll" (ByVal Sound As Long, ByRef Openstate As FMOD_OPENSTATE, ByRef Percentbuffered As Long, ByRef Starving As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_ReadData Lib "fmodex.dll" (ByVal Sound As Long, ByVal Buffer As Long, ByVal Lenbytes As Long, ByRef Read As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_SeekData Lib "fmodex.dll" (ByVal Sound As Long, ByVal Pcm As Long) As FMOD_RESULT

Public Declare Function FMOD_Sound_SetSoundGroup Lib "fmodex.dll" (ByVal Sound As Long, ByVal soundgroup As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetSoundGroup Lib "fmodex.dll" (ByVal Sound As Long, ByRef soundgroup As Long) As FMOD_RESULT

'
     'Synchronization point API.  These points can come from markers embedded in wav files, and can also generate channel callbacks.
'

Public Declare Function FMOD_Sound_GetNumSyncPoints Lib "fmodex.dll" (ByVal Sound As Long, ByRef Numsyncpoints As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetSyncPoint Lib "fmodex.dll" (ByVal Sound As Long, ByVal Index As Long, ByRef Point As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetSyncPointInfo Lib "fmodex.dll" (ByVal Sound As Long, ByVal Point As Long, ByRef name As Byte, ByVal namelen As Long, ByRef Offset As Long, ByVal Offsettype As FMOD_TIMEUNIT) As FMOD_RESULT
Public Declare Function FMOD_Sound_AddSyncPoint Lib "fmodex.dll" (ByVal Sound As Long, ByVal Offset As Long, ByVal Offsettype As FMOD_TIMEUNIT, ByVal name As String, ByRef Point As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_DeleteSyncPoint Lib "fmodex.dll" (ByVal Sound As Long, ByVal Point As Long) As FMOD_RESULT

'
     'Functions also in Channel class but here they are the 'default' to save having to change it in Channel all the time.
'

Public Declare Function FMOD_Sound_SetMode Lib "fmodex.dll" (ByVal Sound As Long, ByVal Mode As FMOD_MODE) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetMode Lib "fmodex.dll" (ByVal Sound As Long, ByRef Mode As FMOD_MODE) As FMOD_RESULT
Public Declare Function FMOD_Sound_SetLoopCount Lib "fmodex.dll" (ByVal Sound As Long, ByVal Loopcount As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetLoopCount Lib "fmodex.dll" (ByVal Sound As Long, ByRef Loopcount As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_SetLoopPoints Lib "fmodex.dll" (ByVal Sound As Long, ByVal Loopstart As Long, ByVal Loopstarttype As FMOD_TIMEUNIT, ByVal Loopend As Long, ByVal Loopendtype As FMOD_TIMEUNIT) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetLoopPoints Lib "fmodex.dll" (ByVal Sound As Long, ByRef Loopstart As Long, ByVal Loopstarttype As FMOD_TIMEUNIT, ByRef Loopend As Long, ByVal Loopendtype As FMOD_TIMEUNIT) As FMOD_RESULT

'
'For MOD/S3M/XM/IT/MID sequenced formats only.
'
Public Declare Function FMOD_Sound_GetMusicNumChannels Lib "fmodex.dll" (ByVal Sound As Long, ByRef numchannels As Long)
Public Declare Function FMOD_Sound_SetMusicChannelVolume Lib "fmodex.dll" (ByVal Sound As Long, ByVal channel As Long, ByVal volume As Single)
Public Declare Function FMOD_Sound_GetMusicChannelVolume Lib "fmodex.dll" (ByVal Sound As Long, ByVal channel As Long, ByRef volume As Single)

'
     'Userdata set/get.
'

Public Declare Function FMOD_Sound_SetUserData Lib "fmodex.dll" (ByVal Sound As Long, ByVal userdata As Long) As FMOD_RESULT
Public Declare Function FMOD_Sound_GetUserData Lib "fmodex.dll" (ByVal Sound As Long, ByRef userdata As Long) As FMOD_RESULT

'
    'Channel' API
'

Public Declare Function FMOD_Channel_GetSystemObject Lib "fmodex.dll" (ByVal channel As Long, ByRef system As Long) As FMOD_RESULT

Public Declare Function FMOD_Channel_Stop Lib "fmodex.dll" (ByVal channel As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetPaused Lib "fmodex.dll" (ByVal channel As Long, ByVal paused As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetPaused Lib "fmodex.dll" (ByVal channel As Long, ByRef paused As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetVolume Lib "fmodex.dll" (ByVal channel As Long, ByVal volume As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetVolume Lib "fmodex.dll" (ByVal channel As Long, ByRef volume As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetFrequency Lib "fmodex.dll" (ByVal channel As Long, ByVal Frequency As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetFrequency Lib "fmodex.dll" (ByVal channel As Long, ByRef Frequency As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetPan Lib "fmodex.dll" (ByVal channel As Long, ByVal Pan As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetPan Lib "fmodex.dll" (ByVal channel As Long, ByRef Pan As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetDelay Lib "fmodex.dll" (ByVal channel As Long, ByVal Startdelay As Long, ByVal Enddelay As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetDelay Lib "fmodex.dll" (ByVal channel As Long, ByRef Startdelay As Long, ByRef Enddelay As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetSpeakerMix Lib "fmodex.dll" (ByVal channel As Long, ByVal Frontleft As Single, ByVal Frontright As Single, ByVal Center As Single, ByVal Lfe As Single, ByVal Backleft As Single, ByVal Backright As Single, ByVal Sideleft As Single, ByVal Sideright As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetSpeakerMix Lib "fmodex.dll" (ByVal channel As Long, ByRef Frontleft As Single, ByRef Frontright As Single, ByRef Center As Single, ByRef Lfe As Single, ByRef Backleft As Single, ByRef Backright As Single, ByRef Sideleft As Single, ByRef Sideright As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetSpeakerLevels Lib "fmodex.dll" (ByVal channel As Long, ByVal speaker As FMOD_SPEAKER, ByRef levels As Single, ByVal numlevels As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetSpeakerLevels Lib "fmodex.dll" (ByVal channel As Long, ByVal speaker As FMOD_SPEAKER, ByRef levels As Single, ByVal numlevels As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetInputChannelMix Lib "fmodex.dll" (ByVal channel As Long, ByRef levels As Single, ByVal numlevels As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetInputChannelMix Lib "fmodex.dll" (ByVal channel As Long, ByRef levels As Single, ByVal numlevels As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetMute Lib "fmodex.dll" (ByVal channel As Long, ByVal mute As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetMute Lib "fmodex.dll" (ByVal channel As Long, ByRef mute As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetPriority Lib "fmodex.dll" (ByVal channel As Long, ByVal Priority As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetPriority Lib "fmodex.dll" (ByVal channel As Long, ByRef Priority As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetPosition Lib "fmodex.dll" (ByVal channel As Long, ByVal position As Long, ByVal Postype As FMOD_TIMEUNIT) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetPosition Lib "fmodex.dll" (ByVal channel As Long, ByRef position As Long, ByVal Postype As FMOD_TIMEUNIT) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetReverbProperties Lib "fmodex.dll" (ByVal channel As Long, ByRef Prop As FMOD_REVERB_CHANNELPROPERTIES) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetReverbProperties Lib "fmodex.dll" (ByVal channel As Long, ByRef Prop As FMOD_REVERB_CHANNELPROPERTIES) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetChannelGroup Lib "fmodex.dll" (ByVal channel As Long, ByVal Channelgroup As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetChannelGroup Lib "fmodex.dll" (ByVal channel As Long, ByRef Channelgroup As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetCallback Lib "fmodex.dll" (ByVal channel As Long, ByVal Callback As Long) As FMOD_RESULT

'
     '3D functionality.
'

Public Declare Function FMOD_Channel_Set3DAttributes Lib "fmodex.dll" (ByVal channel As Long, ByRef Pos As FMOD_VECTOR, ByRef Vel As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Channel_Get3DAttributes Lib "fmodex.dll" (ByVal channel As Long, ByRef Pos As FMOD_VECTOR, ByRef Vel As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Channel_Set3DMinMaxDistance Lib "fmodex.dll" (ByVal channel As Long, ByVal mindistance As Single, ByVal maxdistance As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Get3DMinMaxDistance Lib "fmodex.dll" (ByVal channel As Long, ByRef mindistance As Single, ByRef maxdistance As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Set3DConeSettings Lib "fmodex.dll" (ByVal channel As Long, ByVal Insideconeangle As Single, ByVal Outsideconeangle As Single, ByVal Outsidevolume As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Get3DConeSettings Lib "fmodex.dll" (ByVal channel As Long, ByRef Insideconeangle As Single, ByRef Outsideconeangle As Single, ByRef Outsidevolume As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Set3DConeOrientation Lib "fmodex.dll" (ByVal channel As Long, ByRef Orientation As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Channel_Get3DConeOrientation Lib "fmodex.dll" (ByVal channel As Long, ByRef Orientation As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Channel_Set3DCustomRolloff Lib "fmodex.dll" (ByVal channel As Long, ByRef Points As FMOD_VECTOR, ByVal numpoints As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_Get3DCustomRolloff Lib "fmodex.dll" (ByVal channel As Long, ByRef Points As Long, ByRef numpoints As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_Set3DOcclusion Lib "fmodex.dll" (ByVal channel As Long, ByVal DirectOcclusion As Single, ByVal ReverbOcclusion As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Get3DOcclusion Lib "fmodex.dll" (ByVal channel As Long, ByRef DirectOcclusion As Single, ByRef ReverbOcclusion As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Set3DSpread Lib "fmodex.dll" (ByVal channel As Long, ByVal angle As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Get3DSpread Lib "fmodex.dll" (ByVal channel As Long, ByRef angle As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Set3DPanLevel Lib "fmodex.dll" (ByVal channel As Long, ByVal float As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Get3DPanLevel Lib "fmodex.dll" (ByVal channel As Long, ByRef float As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Set3DDopplerLevel Lib "fmodex.dll" (ByVal channel As Long, ByVal level As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_Get3DDopplerLevel Lib "fmodex.dll" (ByVal channel As Long, ByRef level As Single) As FMOD_RESULT

'
     'DSP functionality only for channels playing sounds created with FMOD_SOFTWARE.
'

Public Declare Function FMOD_Channel_GetDSPHead Lib "fmodex.dll" (ByVal channel As Long, ByRef Dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_AddDSP Lib "fmodex.dll" (ByVal channel As Long, ByVal Dsp As Long, ByRef dspconnection As Long) As FMOD_RESULT

'
     'Information only functions.
'

Public Declare Function FMOD_Channel_IsPlaying Lib "fmodex.dll" (ByVal channel As Long, ByRef Isplaying As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_IsVirtual Lib "fmodex.dll" (ByVal channel As Long, ByRef Isvirtual As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetAudibility Lib "fmodex.dll" (ByVal channel As Long, ByRef Audibility As Single) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetCurrentSound Lib "fmodex.dll" (ByVal channel As Long, ByRef Sound As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetSpectrum Lib "fmodex.dll" (ByVal channel As Long, ByRef Spectrumarray As Single, ByVal Numvalues As Long, ByVal Channeloffset As Long, ByVal Windowtype As FMOD_DSP_FFT_WINDOW) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetWaveData Lib "fmodex.dll" (ByVal channel As Long, ByRef Wavearray As Single, ByVal Numvalues As Long, ByVal Channeloffset As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetIndex Lib "fmodex.dll" (ByVal channel As Long, ByRef Index As Long) As FMOD_RESULT

'
     'Functions also found in Sound class but here they can be set per channel.
'

Public Declare Function FMOD_Channel_SetMode Lib "fmodex.dll" (ByVal channel As Long, ByVal Mode As FMOD_MODE) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetMode Lib "fmodex.dll" (ByVal channel As Long, ByRef Mode As FMOD_MODE) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetLoopCount Lib "fmodex.dll" (ByVal channel As Long, ByVal Loopcount As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetLoopCount Lib "fmodex.dll" (ByVal channel As Long, ByRef Loopcount As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_SetLoopPoints Lib "fmodex.dll" (ByVal channel As Long, ByVal Loopstart As Long, ByVal Loopstarttype As FMOD_TIMEUNIT, ByVal Loopend As Long, ByVal Loopendtype As FMOD_TIMEUNIT) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetLoopPoints Lib "fmodex.dll" (ByVal channel As Long, ByRef Loopstart As Long, ByVal Loopstarttype As FMOD_TIMEUNIT, ByRef Loopend As Long, ByVal Loopendtype As FMOD_TIMEUNIT) As FMOD_RESULT

'
     'Userdata set/get.
'

Public Declare Function FMOD_Channel_SetUserData Lib "fmodex.dll" (ByVal channel As Long, ByVal userdata As Long) As FMOD_RESULT
Public Declare Function FMOD_Channel_GetUserData Lib "fmodex.dll" (ByVal channel As Long, ByRef userdata As Long) As FMOD_RESULT

'
    'ChannelGroup' API
'

Public Declare Function FMOD_ChannelGroup_Release Lib "fmodex.dll" (ByVal Channelgroup As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetSystemObject Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef system As Long) As FMOD_RESULT

'
     'Channelgroup scale values.  (scales the current volume or pitch of all channels and channel groups, DOESN'T overwrite)
'

Public Declare Function FMOD_ChannelGroup_SetVolume Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal volume As Single) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetVolume Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef volume As Single) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_SetPitch Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal Pitch As Single) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetPitch Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef Pitch As Single) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_SetPaused Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal paused As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetPaused Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef paused As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_SetMute Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal mute As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetMute Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef mute As Long) As FMOD_RESULT

'
     'Channelgroup override values.  (recursively overwrites whatever settings the channels had)
'

Public Declare Function FMOD_ChannelGroup_Stop Lib "fmodex.dll" (ByVal Channelgroup As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_OverrideVolume Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal volume As Single) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_OverrideFrequency Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal Frequency As Single) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_OverridePan Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal Pan As Single) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_OverrideReverbProperties Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef Prop As FMOD_REVERB_CHANNELPROPERTIES) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_Override3DAttributes Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef Pos As FMOD_VECTOR, ByRef Vel As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_OverrideSpeakerMix Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal Frontleft As Single, ByVal Frontright As Single, ByVal Center As Single, ByVal Lfe As Single, ByVal Backleft As Single, ByVal Backright As Single, ByVal Sideleft As Single, ByVal Sideright As Single) As FMOD_RESULT

'
      'Nested channel groups.
'
Public Declare Function FMOD_ChannelGroup_AddGroup Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal Group As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetNumGroups Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef Numgroups As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetGroup Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal Index As Long, ByRef Group As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetParentGroup Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef Group As Long) As FMOD_RESULT

'
     'DSP functionality only for channel groups playing sounds created with FMOD_SOFTWARE.
'

Public Declare Function FMOD_ChannelGroup_GetDSPHead Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef Dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_AddDSP Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal Dsp As Long, ByRef dspconnection As Long) As FMOD_RESULT

'
     'Information only functions.
'

Public Declare Function FMOD_ChannelGroup_GetName Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef name As Byte, ByVal namelen As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetNumChannels Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef Numchannels As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetChannel Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal Index As Long, ByRef channel As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetSpectrum Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef Spectrumarray As Single, ByVal Numvalues As Long, ByVal Channeloffset As Long, ByVal Windowtype As FMOD_DSP_FFT_WINDOW) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetWaveData Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef Wavearray As Single, ByVal Numvalues As Long, ByVal Channeloffset As Long) As FMOD_RESULT

'
     'Userdata set/get.
'

Public Declare Function FMOD_ChannelGroup_SetUserData Lib "fmodex.dll" (ByVal Channelgroup As Long, ByVal userdata As Long) As FMOD_RESULT
Public Declare Function FMOD_ChannelGroup_GetUserData Lib "fmodex.dll" (ByVal Channelgroup As Long, ByRef userdata As Long) As FMOD_RESULT

'
    'SoundGroup' API
'

Public Declare Function FMOD_SoundGroup_Release Lib "fmodex.dll" (ByVal soundgroup As Long) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_GetSystemObject Lib "fmodex.dll" (ByVal soundgroup As Long, ByRef system As Long) As FMOD_RESULT

'
    'SoundGroup control functions.
'

Public Declare Function FMOD_SoundGroup_SetMaxAudible Lib "fmodex.dll" (ByVal soundgroup As Long, ByVal maxaudible As Long) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_GetMaxAudible Lib "fmodex.dll" (ByVal soundgroup As Long, ByRef maxaudible As Long) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_SetMaxAudibleBehavior Lib "fmodex.dll" (ByVal soundgroup As Long, ByVal behavior As FMOD_SOUNDGROUP_BEHAVIOR) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_GetMaxAudibleBehavior Lib "fmodex.dll" (ByVal soundgroup As Long, ByRef behavior As FMOD_SOUNDGROUP_BEHAVIOR) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_SetMuteFadeSpeed Lib "fmodex.dll" (ByVal soundgroup As Long, ByVal speed As Single) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_GetMuteFadeSpeed Lib "fmodex.dll" (ByVal soundgroup As Long, ByRef speed As Single) As FMOD_RESULT

'
    'Information only functions.
'

Public Declare Function FMOD_SoundGroup_GetName Lib "fmodex.dll" (ByVal soundgroup As Long, ByRef name As Byte, ByVal namelen As Long) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_GetNumSounds Lib "fmodex.dll" (ByVal soundgroup As Long, ByRef numsounds As Long) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_GetSound Lib "fmodex.dll" (ByVal soundgroup As Long, ByVal Index As Long, ByRef Sound As Long) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_GetNumPlaying Lib "fmodex.dll" (ByVal soundgroup As Long, ByRef numplaying As Long) As FMOD_RESULT

'
    'Userdata set/get.
'

Public Declare Function FMOD_SoundGroup_SetUserData Lib "fmodex.dll" (ByVal soundgroup As Long, ByRef userdata As Long) As FMOD_RESULT
Public Declare Function FMOD_SoundGroup_GetUserData Lib "fmodex.dll" (ByVal soundgroup As Long, ByRef userdata As Long) As FMOD_RESULT


'
    'DSP' API
'

Public Declare Function FMOD_DSP_Release Lib "fmodex.dll" (ByVal Dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetSystemObject Lib "fmodex.dll" (ByVal Dsp As Long, ByRef system As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_AddInput Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Target As Long, ByRef dspconnection As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_DisconnectFrom Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Target As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_DisconnectAll Lib "fmodex.dll" (ByVal Dsp As Long, ByVal inputs As Long, ByVal outputs As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_Remove Lib "fmodex.dll" (ByVal Dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetNumInputs Lib "fmodex.dll" (ByVal Dsp As Long, ByRef Numinputs As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetNumOutputs Lib "fmodex.dll" (ByVal Dsp As Long, ByRef Numoutputs As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetInput Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Index As Long, ByRef input_ As Long, ByRef InputConnection As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetOutput Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Index As Long, ByRef output As Long, ByRef OutputConnection As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_SetActive Lib "fmodex.dll" (ByVal Dsp As Long, ByVal active As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetActive Lib "fmodex.dll" (ByVal Dsp As Long, ByRef active As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_SetBypass Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Bypass As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetBypass Lib "fmodex.dll" (ByVal Dsp As Long, ByRef Bypass As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_Reset Lib "fmodex.dll" (ByVal Dsp As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_SetParameter Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Index As Long, ByVal Value As Single) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetParameter Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Index As Long, ByRef Value As Single, ByRef Valuestr As Byte, ByVal Valuestrlen As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetNumParameters Lib "fmodex.dll" (ByVal Dsp As Long, ByRef Numparams As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetParameterInfo Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Index As Long, ByRef name As Byte, ByRef label As Byte, ByRef description As Byte, ByVal Descriptionlen As Long, ByRef min As Single, ByRef max As Single) As FMOD_RESULT
Public Declare Function FMOD_DSP_ShowConfigDialog Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Hwnd As Long, ByVal Show As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetInfo Lib "fmodex.dll" (ByVal Dsp As Long, ByRef name As Byte, ByRef version As Long, ByRef Channels As Long, ByRef Configwidth As Long, ByRef Configheight As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetType Lib "fmodex.dll" (ByVal Dsp As Long, ByRef type_ As FMOD_DSP_TYPE) As FMOD_RESULT
Public Declare Function FMOD_DSP_SetDefaults Lib "fmodex.dll" (ByVal Dsp As Long, ByVal Frequency As Single, ByVal volume As Single, ByVal Pan As Single, ByVal Priority As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetDefaults Lib "fmodex.dll" (ByVal Dsp As Long, ByRef Frequency As Single, ByRef volume As Single, ByRef Pan As Single, ByRef Priority As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_SetUserData Lib "fmodex.dll" (ByVal Dsp As Long, ByVal userdata As Long) As FMOD_RESULT
Public Declare Function FMOD_DSP_GetUserData Lib "fmodex.dll" (ByVal Dsp As Long, ByRef userdata As Long) As FMOD_RESULT

'
'DSP Connection API.
'

Public Declare Function FMOD_DSPConnection_GetInput Lib "fmodex.dll" (ByVal dspconnection As Long, ByRef input_ As Long)
Public Declare Function FMOD_DSPConnection_GetOutput Lib "fmodex.dll" (ByVal dspconnection As Long, ByRef output As Long)
Public Declare Function FMOD_DSPConnection_SetMix Lib "fmodex.dll" (ByVal dspconnection As Long, ByVal volume As Single)
Public Declare Function FMOD_DSPConnection_GetMix Lib "fmodex.dll" (ByVal dspconnection As Long, ByRef volume As Single)
Public Declare Function FMOD_DSPConnection_SetLevels Lib "fmodex.dll" (ByVal dspconnection As Long, ByVal speaker As FMOD_SPEAKER, ByRef levels As Single, ByVal numlevels As Long)
Public Declare Function FMOD_DSPConnection_GetLevels Lib "fmodex.dll" (ByVal dspconnection As Long, ByVal speaker As FMOD_SPEAKER, ByRef levels As Single, ByVal numlevels As Long)
Public Declare Function FMOD_DSPConnection_SetUserData Lib "fmodex.dll" (ByVal dspconnection As Long, ByVal userdata As Long) As FMOD_RESULT
Public Declare Function FMOD_DSPConnection_GetUserData Lib "fmodex.dll" (ByVal dspconnection As Long, ByRef userdata As Long) As FMOD_RESULT


'
    'Geometry' API
'

Public Declare Function FMOD_Geometry_Release Lib "fmodex.dll" (ByVal Gemoetry As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_AddPolygon Lib "fmodex.dll" (ByVal Geometry As Long, ByVal DirectOcclusion As Single, ByVal ReverbOcclusion As Single, ByVal DoubleSided As Long, ByVal NumVertices As Long, ByRef Vertices As FMOD_VECTOR, ByRef PolygonIndex As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetNumPolygons Lib "fmodex.dll" (ByVal Geometry As Long, ByRef NumPolygons As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetMaxPolygons Lib "fmodex.dll" (ByVal Geometry As Long, ByRef MaxPolygons As Long, ByRef MaxVertices As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetPolygonNumVertices Lib "fmodex.dll" (ByVal Geometry As Long, ByVal PolygonIndex As Long, ByRef NumVertices As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_SetPolygonVertex Lib "fmodex.dll" (ByVal Geometry As Long, ByVal PolygonIndex As Long, ByVal VertexIndex As Long, ByRef Vertex As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetPolygonVertex Lib "fmodex.dll" (ByVal Geometry As Long, ByVal PolygonIndex As Long, ByVal VertexIndex As Long, ByRef Vertex As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Geometry_SetPolygonAttributes Lib "fmodex.dll" (ByVal Geometry As Long, ByVal PolygonIndex As Long, ByVal DirectOcclusion As Single, ByVal ReverbOcclusion As Single, ByVal DoubleSided As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetPolygonAttributes Lib "fmodex.dll" (ByVal Geometry As Long, ByVal PolygonIndex As Long, ByRef DirectOcclusion As Single, ByRef ReverbOcclusion As Single, ByRef DoubleSided As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_SetActive Lib "fmodex.dll" (ByVal Geometry As Long, ByVal active As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetActive Lib "fmodex.dll" (ByVal Geometry As Long, ByRef active As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_SetRotation Lib "fmodex.dll" (ByVal Geometry As Long, ByRef Forward As FMOD_VECTOR, ByRef Up As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetRotation Lib "fmodex.dll" (ByVal Geometry As Long, ByRef Forward As FMOD_VECTOR, ByRef Up As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Geometry_SetPosition Lib "fmodex.dll" (ByVal Geometry As Long, ByRef position As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetPosition Lib "fmodex.dll" (ByVal Geometry As Long, ByRef position As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Geometry_SetScale Lib "fmodex.dll" (ByVal Geometry As Long, ByRef Scale_ As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetScale Lib "fmodex.dll" (ByVal Geometry As Long, ByRef Scale_ As FMOD_VECTOR) As FMOD_RESULT
Public Declare Function FMOD_Geometry_Save Lib "fmodex.dll" (ByVal Geometry As Long, ByVal Data As Long, ByRef DataSize As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_SetUserData Lib "fmodex.dll" (ByVal Geometry As Long, ByVal userdata As Long) As FMOD_RESULT
Public Declare Function FMOD_Geometry_GetUserData Lib "fmodex.dll" (ByVal Geometry As Long, ByRef userdata As Long) As FMOD_RESULT

'
    'Reverb' API
'

Public Declare Function FMOD_Reverb_Release Lib "fmodex.dll" (ByVal reverb As Long)

'
    'Reverb manipulation.
'

Public Declare Function FMOD_Reverb_Set3DAttributes Lib "fmodex.dll" (ByVal reverb As Long, ByRef position As FMOD_VECTOR, ByVal mindistance As Single, ByVal maxdistance As Single) As FMOD_RESULT
Public Declare Function FMOD_Reverb_Get3DAttributes Lib "fmodex.dll" (ByVal reverb As Long, ByRef position As FMOD_VECTOR, ByRef mindistance As Single, ByRef maxdistance As Single) As FMOD_RESULT
Public Declare Function FMOD_Reverb_SetProperties Lib "fmodex.dll" (ByVal reverb As Long, ByRef properties As FMOD_REVERB_PROPERTIES) As FMOD_RESULT
Public Declare Function FMOD_Reverb_GetProperties Lib "fmodex.dll" (ByVal reverb As Long, ByRef properties As FMOD_REVERB_PROPERTIES) As FMOD_RESULT
Public Declare Function FMOD_Reverb_SetActive Lib "fmodex.dll" (ByVal reverb As Long, ByVal active As Long) As FMOD_RESULT
Public Declare Function FMOD_Reverb_GetActive Lib "fmodex.dll" (ByVal reverb As Long, ByRef active As Long) As FMOD_RESULT

'
    'Userdata set/get.
'

Public Declare Function FMOD_Reverb_SetUserData Lib "fmodex.dll" (ByVal reverb As Long, ByRef userdata As Long) As FMOD_RESULT
Public Declare Function FMOD_Reverb_GetUserData Lib "fmodex.dll" (ByVal reverb As Long, ByRef userdata As Long) As FMOD_RESULT

' ========================================================================================== '
' EXTRA VB HELPER FUNCTIONS
' ========================================================================================== '

'
'   Windows Declarations
'

'Required for GetStringFromPointer
Private Declare Function ConvCStringToVBString Lib "kernel32" Alias "lstrcpyA" (ByVal lpsz As String, ByVal pt As Long) As Long ' Notice the As Long return value replacing the As String given by the API Viewer.

'Required for the getSpectrum/getWaveData functions
Private Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (Destination As Any, Source As Any, ByVal Length As Long)

'
'   Helper Functions
'

'Example: MyDriverName = GetStringFromPointer(namepointer)
Public Function GetStringFromPointer(ByVal lpString As Long) As String
Dim NullCharPos As Long
Dim szBuffer As String

    szBuffer = String(255, 0)
    ConvCStringToVBString szBuffer, lpString
    ' Look for the null char ending the C string
    NullCharPos = InStr(szBuffer, vbNullChar)
    GetStringFromPointer = Left(szBuffer, NullCharPos - 1)
End Function

Public Function GetSingleFromPointer(ByVal lpSingle As Long) As Single
'A Single is 4 bytes, so we copy 4 bytes
CopyMemory GetSingleFromPointer, ByVal lpSingle, 4
End Function


' WRAPPED FMODEX CREATESOUND FUNCTIONS
Public Function FMOD_System_CreateSound(ByVal system As Long, ByVal Name_or_data As String, ByVal Mode As FMOD_MODE, ByRef Sound As Long) As FMOD_RESULT
    Dim exinfo As FMOD_CREATESOUNDEXINFO
    Dim result As FMOD_RESULT
    
    exinfo.cbsize = LenB(exinfo)
    
    result = FMOD_System_CreateSoundEx(system, Name_or_data, Mode, exinfo, Sound)
    
    FMOD_System_CreateSound = result
End Function

Public Function FMOD_System_CreateStream(ByVal system As Long, ByVal Name_or_data As String, ByVal Mode As FMOD_MODE, ByRef Sound As Long) As FMOD_RESULT
    Dim exinfo As FMOD_CREATESOUNDEXINFO
    Dim result As FMOD_RESULT
    
    exinfo.cbsize = LenB(exinfo)
    
    result = FMOD_System_CreateStreamEx(system, Name_or_data, Mode, exinfo, Sound)
    
    FMOD_System_CreateStream = result
End Function
