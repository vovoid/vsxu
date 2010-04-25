(* ============================================================================================= *)
(* FMOD Ex - Main C/C++ header file. Copyright (c), Firelight Technologies Pty, Ltd. 2004-2008.  *)
(*                                                                                               *)
(* This header is the base header for all other FMOD headers.  If you are programming in C       *)
(* use this exclusively, or if you are programming C++ use this in conjunction with FMOD.HPP     *)
(*                                                                                               *)
(* ============================================================================================= *)

unit fmodtypes;

{$I fmod.inc}

interface

(*
    FMOD version number.  Check this against FMOD_System_GetVersion
*)

const
  FMOD_VERSION = $00041305;

(*
    FMOD types.
*)

type
  FMOD_SYSTEM       = Pointer;
  FMOD_SOUND        = Pointer;
  FMOD_CHANNEL      = Pointer;
  FMOD_CHANNELGROUP = Pointer;
  FMOD_DSP          = Pointer;
  FMOD_BOOL         = LongBool;
  FMOD_POLYGON		= Pointer;
  FMOD_GEOMETRY     = Pointer;
  FMOD_SYNCPOINT	= Pointer;


(*
[STRUCTURE] 
[
    [DESCRIPTION]   
    Structure describing a point in 3D space.

    [REMARKS]
    FMOD uses a left handed co-ordinate system by default.
    To use a right handed co-ordinate system specify FMOD_INIT_3D_RIGHTHANDED from FMOD_INITFLAGS in FMOD_System_Init.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_System_Set3DListenerAttributes
    FMOD_System_Get3DListenerAttributes
    FMOD_Channel_Set3DAttributes
    FMOD_Channel_Get3DAttributes
    FMOD_Geometry_AddPolygon
    FMOD_Geometry_SetPolygonVertex
    FMOD_Geometry_GetPolygonVertex
    FMOD_Geometry_SetRotation
    FMOD_Geometry_GetRotation
    FMOD_Geometry_SetPosition
    FMOD_Geometry_GetPosition
    FMOD_Geometry_SetScale
    FMOD_Geometry_GetScale
    FMOD_INITFLAGS
]
*)
type
  FMOD_VECTOR = record
	  x, y, z: Single;
  end;


(*
[ENUM]
[
    [DESCRIPTION]
    error codes.  Returned from every function.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
]
*)
type
  FMOD_RESULT =
  (
    FMOD_OK,                        (* No errors. *)
    FMOD_ERR_ALREADYLOCKED,         (* Tried to call lock a second time before unlock was called. *)
    FMOD_ERR_BADCOMMAND,            (* Tried to call a function on a data type that does not allow this type of functionality (ie calling FMOD_Sound_Lock on a streaming sound). *)
    FMOD_ERR_CDDA_DRIVERS,          (* Neither NTSCSI nor ASPI could be initialised. *)
    FMOD_ERR_CDDA_INIT,             (* An error occurred while initialising the CDDA subsystem. *)
    FMOD_ERR_CDDA_INVALID_DEVICE,   (* Couldn't find the specified device. *)
    FMOD_ERR_CDDA_NOAUDIO,          (* No audio tracks on the specified disc. *)
    FMOD_ERR_CDDA_NODEVICES,        (* No CD/DVD devices were found. *) 
    FMOD_ERR_CDDA_NODISC,           (* No disc present in the specified drive. *)
    FMOD_ERR_CDDA_READ,             (* A CDDA read error occurred. *)
    FMOD_ERR_CHANNEL_ALLOC,         (* Error trying to allocate a channel. *)
    FMOD_ERR_CHANNEL_STOLEN,        (* The specified channel has been reused to play another sound. *)
    FMOD_ERR_COM,                   (* A Win32 COM related error occured. COM failed to initialize or a QueryInterface failed meaning a Windows codec or driver was not installed properly. *)
    FMOD_ERR_DMA,                   (* DMA Failure.  See debug output for more information. *)
    FMOD_ERR_DSP_CONNECTION,        (* DSP connection error.  Connection possibly caused a cyclic dependancy. *)
    FMOD_ERR_DSP_FORMAT,            (* DSP Format error.  A DSP unit may have attempted to connect to this network with the wrong format. *)
    FMOD_ERR_DSP_NOTFOUND,          (* DSP connection error.  Couldn't find the DSP unit specified. *)
    FMOD_ERR_DSP_RUNNING,           (* DSP error.  Cannot perform this operation while the network is in the middle of running.  This will most likely happen if a connection or disconnection is attempted in a DSP callback. *)
    FMOD_ERR_DSP_TOOMANYCONNECTIONS,(* DSP connection error.  The unit being connected to or disconnected should only have 1 input or output. *)
    FMOD_ERR_FILE_BAD,              (* Error loading file. *)
    FMOD_ERR_FILE_COULDNOTSEEK,     (* Couldn't perform seek operation.  This is a limitation of the medium (ie netstreams) or the file format. *)
    FMOD_ERR_FILE_DISKEJECTED,      (* Media was ejected while reading. *)
    FMOD_ERR_FILE_EOF,              (* End of file unexpectedly reached while trying to read essential data (truncated data?). *)
    FMOD_ERR_FILE_NOTFOUND,         (* File not found. *)
    FMOD_ERR_FILE_UNWANTED,         (* Unwanted file access occured. *)
    FMOD_ERR_FORMAT,                (* Unsupported file or audio format. *)
    FMOD_ERR_HTTP,                  (* A HTTP error occurred. This is a catch-all for HTTP errors not listed elsewhere. *)
    FMOD_ERR_HTTP_ACCESS,           (* The specified resource requires authentication or is forbidden. *)
    FMOD_ERR_HTTP_PROXY_AUTH,       (* Proxy authentication is required to access the specified resource. *)
    FMOD_ERR_HTTP_SERVER_ERROR,     (* A HTTP server error occurred. *)
    FMOD_ERR_HTTP_TIMEOUT,          (* The HTTP request timed out. *)
    FMOD_ERR_INITIALIZATION,        (* FMOD was not initialized correctly to support this function. *)
    FMOD_ERR_INITIALIZED,           (* Cannot call this command after FMOD_System_Init. *)
    FMOD_ERR_INTERNAL,              (* An error occured that wasnt supposed to.  Contact support. *)
    FMOD_ERR_INVALID_ADDRESS,       (* On Xbox 360, this memory address passed to FMOD must be physical, (ie allocated with XPhysicalAlloc.) *)
    FMOD_ERR_INVALID_FLOAT,         (* Value passed in was a NaN, Inf or denormalized float. *)
    FMOD_ERR_INVALID_HANDLE,        (* An invalid object handle was used. *)
    FMOD_ERR_INVALID_PARAM,         (* An invalid parameter was passed to this function. *)
    FMOD_ERR_INVALID_SPEAKER,       (* An invalid speaker was passed to this function based on the current speaker mode. *)
    FMOD_ERR_INVALID_VECTOR,        (* The vectors passed in are not unit length, or perpendicular. *)
    FMOD_ERR_IRX,                   (* PS2 only.  fmodex.irx failed to initialize.  This is most likely because you forgot to load it. *)
    FMOD_ERR_MAXAUDIBLE,            (* Reached maximum audible playback count for this sound's soundgroup. *)
    FMOD_ERR_MEMORY,                (* Not enough memory or resources. *)
    FMOD_ERR_MEMORY_CANTPOINT,      (* Can't use FMOD_OPENMEMORY_POINT on non PCM source data, or non mp3/xma/adpcm data if FMOD_CREATECOMPRESSEDSAMPLE was used. *)
    FMOD_ERR_MEMORY_IOP,            (* PS2 only.  Not enough memory or resources on PlayStation 2 IOP ram. *)
    FMOD_ERR_MEMORY_SRAM,           (* Not enough memory or resources on console sound ram. *)
    FMOD_ERR_NEEDS2D,               (* Tried to call a command on a 3d sound when the command was meant for 2d sound. *)
    FMOD_ERR_NEEDS3D,               (* Tried to call a command on a 2d sound when the command was meant for 3d sound. *)
    FMOD_ERR_NEEDSHARDWARE,         (* Tried to use a feature that requires hardware support.  (ie trying to play a VAG compressed sound in software on PS2). *)
    FMOD_ERR_NEEDSSOFTWARE,         (* Tried to use a feature that requires the software engine.  Software engine has either been turned off, or command was executed on a hardware channel which does not support this feature. *)
    FMOD_ERR_NET_CONNECT,           (* Couldn't connect to the specified host. *)
    FMOD_ERR_NET_SOCKET_ERROR,      (* A socket error occurred.  This is a catch-all for socket-related errors not listed elsewhere. *)
    FMOD_ERR_NET_URL,               (* The specified URL couldn't be resolved. *)
    FMOD_ERR_NET_WOULD_BLOCK,       (* Operation on a non-blocking socket could not complete immediately. *)
    FMOD_ERR_NOTREADY,              (* Operation could not be performed because specified sound is not ready. *)
    FMOD_ERR_OUTPUT_ALLOCATED,      (* Error initializing output device, but more specifically, the output device is already in use and cannot be reused. *)
    FMOD_ERR_OUTPUT_CREATEBUFFER,   (* Error creating hardware sound buffer. *)
    FMOD_ERR_OUTPUT_DRIVERCALL,     (* A call to a standard soundcard driver failed, which could possibly mean a bug in the driver or resources were missing or exhausted. *)
    FMOD_ERR_OUTPUT_ENUMERATION,    (* Error enumerating the available driver list. List may be inconsistent due to a recent device addition or removal. *)
    FMOD_ERR_OUTPUT_FORMAT,         (* Soundcard does not support the minimum features needed for this soundsystem (16bit stereo output). *)
    FMOD_ERR_OUTPUT_INIT,           (* Error initializing output device. *)
    FMOD_ERR_OUTPUT_NOHARDWARE,     (* FMOD_HARDWARE was specified but the sound card does not have the resources nescessary to play it. *)
    FMOD_ERR_OUTPUT_NOSOFTWARE,     (* Attempted to create a software sound but no software channels were specified in FMOD_System_Init. *)
    FMOD_ERR_PAN,                   (* Panning only works with mono or stereo sound sources. *)
    FMOD_ERR_PLUGIN,                (* An unspecified error has been returned from a 3rd party plugin. *)
    FMOD_ERR_PLUGIN_INSTANCES,      (* The number of allowed instances of a plugin has been exceeded *)
    FMOD_ERR_PLUGIN_MISSING,        (* A requested output, dsp unit type or codec was not available. *)
    FMOD_ERR_PLUGIN_RESOURCE,       (* A resource that the plugin requires cannot be found. (ie the DLS file for MIDI playback) *)
    FMOD_ERR_RECORD,                (* An error occured trying to initialize the recording device. *)
    FMOD_ERR_REVERB_INSTANCE,       (* Specified Instance in FMOD_REVERB_PROPERTIES couldn't be set. Most likely because another application has locked the EAX4 FX slot.*)
    FMOD_ERR_SUBSOUND_ALLOCATED,    (* This subsound is already being used by another sound, you cannot have more than one parent to a sound.  Null out the other parent's entry first. *)
    FMOD_ERR_SUBSOUND_MODE,         (* The subsound's mode bits do not match with the parent sound's mode bits.  See documentation for function that it was called with. *)
    FMOD_ERR_SUBSOUNDS,             (* The error occured because the sound referenced contains subsounds.  (ie you cannot play the parent sound as a static sample, only its subsounds.) *)
    FMOD_ERR_TAGNOTFOUND,           (* The specified tag could not be found or there are no tags. *)
    FMOD_ERR_TOOMANYCHANNELS,       (* The sound created exceeds the allowable input channel count.  This can be increased using the maxinputchannels parameter in FMOD_System_SetSoftwareFormat. *)
    FMOD_ERR_UNIMPLEMENTED,         (* Something in FMOD hasn't been implemented when it should be! contact support! *)
    FMOD_ERR_UNINITIALIZED,         (* This command failed because FMOD_System_Init or FMOD_System_SetDriver was not called. *)
    FMOD_ERR_UNSUPPORTED,           (* A command issued was not supported by this object.  Possibly a plugin without certain callbacks specified. *)
    FMOD_ERR_UPDATE,                (* An error caused by FMOD_System_Update occured. *)
    FMOD_ERR_VERSION,               (* The version number of this file format is not supported. *)
    
    FMOD_ERR_EVENT_FAILED,          (* An Event failed to be retrieved, most likely due to 'just fail' being specified as the max playbacks behaviour. *)
    FMOD_ERR_EVENT_INTERNAL,        (* An error occured that wasn't supposed to.  See debug log for reason. *)
    FMOD_ERR_EVENT_INFOONLY,        (* Cant execute this command on an EVENT_INFOONLY event. *)
    FMOD_ERR_EVENT_MAXSTREAMS,      (* Event failed because 'Max streams' was hit when FMOD_INIT_FAIL_ON_MAXSTREAMS was specified. *)
    FMOD_ERR_EVENT_MISMATCH,        (* FSB mis-matches the FEV it was compiled with. *)
    FMOD_ERR_EVENT_NAMECONFLICT,    (* A category with the same name already exists. *)
    FMOD_ERR_EVENT_NOTFOUND         (* The requested event, event group, event category or event property could not be found. *)
  );


(*
[ENUM]
[
    [DESCRIPTION]   
    These output types are used with FMOD_System_SetOutput/FMOD_System_GetOutput, to choose which output method to use.
  
    [REMARKS]
    To pass information to the driver when initializing fmod use the extradriverdata parameter in System::init for the following reasons.
    - FMOD_OUTPUTTYPE_WAVWRITER - extradriverdata is a pointer to a char * filename that the wav writer will output to.
    - FMOD_OUTPUTTYPE_WAVWRITER_NRT - extradriverdata is a pointer to a char * filename that the wav writer will output to.
    - FMOD_OUTPUTTYPE_DSOUND - extradriverdata is a pointer to a HWND so that FMOD can set the focus on the audio for a particular window.
    - FMOD_OUTPUTTYPE_GC - extradriverdata is a pointer to a FMOD_ARAMBLOCK_INFO struct. This can be found in fmodgc.h.
    - FMOD_OUTPUTTYPE_ALSA - extradriverdata is a pointer to a char * argument if required by the chosen ALSA driver. 
    Currently these are the only FMOD drivers that take extra information.  Other unknown plugins may have different requirements.
    
    Note! If FMOD_OUTPUTTYPE_NOSOUND_NRT or FMOD_OUTPUTTYPE_NOSOUND_NRT are used, and if the System::update function is being called
    very quickly (ie for a non realtime decode) it may be being called too quickly for the FMOD streamer thread to respond to.  
    The result will be a skipping/stuttering output in the captured audio.
    To remedy this, disable the FMOD Ex streamer thread, and use FMOD_INIT_STREAM_FROM_UPDATE can be used to avoid skipping in 
    the output stream, as it will lock the mixer and the streamer together in the same thread.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_SetOutput
    FMOD_System_GetOutput
    FMOD_System_SetSoftwareFormat
    FMOD_System_GetSoftwareFormat
    FMOD_System_Init
    FMOD_System_Update
    FMOD_INITFLAGS
]
*)
type
  FMOD_OUTPUTTYPE =
  (
    FMOD_OUTPUTTYPE_AUTODETECT,      (* Picks the best output mode for the platform.  This is the default. *)
                                     
    FMOD_OUTPUTTYPE_UNKNOWN,         (* All         - 3rd party plugin, unknown.  This is for use with System::getOutput only. *)
    FMOD_OUTPUTTYPE_NOSOUND,         (* All         - All calls in this mode succeed but make no sound. *)
    FMOD_OUTPUTTYPE_WAVWRITER,       (* All         - Writes output to fmodoutput.wav by default.  Use the 'extradriverdata' parameter in System::init, by simply passing the filename as a string, to set the wav filename. *)
    FMOD_OUTPUTTYPE_NOSOUND_NRT,     (* All         - Non-realtime version of FMOD_OUTPUTTYPE_NOSOUND.  User can drive mixer with System::update at whatever rate they want. *)
    FMOD_OUTPUTTYPE_WAVWRITER_NRT,   (* All         - Non-realtime version of FMOD_OUTPUTTYPE_WAVWRITER.  User can drive mixer with System::update at whatever rate they want. *)
                                     
    FMOD_OUTPUTTYPE_DSOUND,          (* Win32/Win64 - DirectSound output.  Use this to get hardware accelerated 3d audio and EAX Reverb support. (Default on Windows) *)
    FMOD_OUTPUTTYPE_WINMM,           (* Win32/Win64 - Windows Multimedia output. *)
    FMOD_OUTPUTTYPE_OPENAL,          (* Win32/Win64 - OpenAL 1.1 output. Use this for lower CPU overhead than FMOD_OUTPUTTYPE_DSOUND, and also Vista H/W support with Creative Labs cards. *)
    FMOD_OUTPUTTYPE_WASAPI,          (* Win32       - Windows Audio Session API. (Default on Windows Vista) *)
    FMOD_OUTPUTTYPE_ASIO,            (* Win32       - Low latency ASIO driver. *)
    FMOD_OUTPUTTYPE_OSS,             (* Linux       - Open Sound System output. (Default on Linux) *)
    FMOD_OUTPUTTYPE_ALSA,            (* Linux       - Advanced Linux Sound Architecture output. *)
    FMOD_OUTPUTTYPE_ESD,             (* Linux       - Enlightment Sound Daemon output. *)
    FMOD_OUTPUTTYPE_SOUNDMANAGER,    (* Mac         - Macintosh SoundManager output.  (Default on Mac carbon library)*)
    FMOD_OUTPUTTYPE_COREAUDIO,       (* Mac         - Macintosh CoreAudio output.  (Default on Mac OSX library) *)
    FMOD_OUTPUTTYPE_XBOX,            (* Xbox        - Native hardware output. (Default on Xbox) *)
    FMOD_OUTPUTTYPE_PS2,             (* PS2         - Native hardware output. (Default on PS2) *)
    FMOD_OUTPUTTYPE_PS3,             (* PS3         - Native hardware output. (Default on PS3) *)
    FMOD_OUTPUTTYPE_GC,              (* GameCube    - Native hardware output. (Default on GameCube) *)
    FMOD_OUTPUTTYPE_XBOX360,         (* Xbox 360    - Native hardware output. (Default on Xbox 360) *)
    FMOD_OUTPUTTYPE_PSP,             (* PSP         - Native hardware output. (Default on PSP) *)
    FMOD_OUTPUTTYPE_WII,			 (* Wii			- Native hardware output. (Default on Wii) *)

    FMOD_OUTPUTTYPE_MAX              (* Maximum number of output types supported. *)
  );


(*
[ENUM] 
[
    [DESCRIPTION]   
    Bit fields to use with FMOD_System_GetDriverCaps to determine the capabilities of a card / output device.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_GetDriverCaps
]
*)
const
    FMOD_CAPS_NONE                    = $00000000;    (* Device has no special capabilities *)
    FMOD_CAPS_HARDWARE                = $00000001;    (* Device supports hardware mixing. *)
    FMOD_CAPS_HARDWARE_EMULATED       = $00000002;    (* User has device set to 'Hardware acceleration = off' in control panel, and now extra 200ms latency is incurred.*)
    FMOD_CAPS_OUTPUT_MULTICHANNEL     = $00000004;    (* Device can do multichannel output, ie greater than 2 channels *)
    FMOD_CAPS_OUTPUT_FORMAT_PCM8      = $00000008;    (* Device can output to 8bit integer PCM *)
    FMOD_CAPS_OUTPUT_FORMAT_PCM16     = $00000010;    (* Device can output to 16bit integer PCM *)
    FMOD_CAPS_OUTPUT_FORMAT_PCM24     = $00000020;    (* Device can output to 24bit integer PCM *)
    FMOD_CAPS_OUTPUT_FORMAT_PCM32     = $00000040;    (* Device can output to 32bit integer PCM *)
    FMOD_CAPS_OUTPUT_FORMAT_PCMFLOAT  = $00000080;    (* Device can output to 32bit floating point PCM *)
    FMOD_CAPS_REVERB_EAX2             = $00000100;    (* Device supports EAX2 reverb. *)
    FMOD_CAPS_REVERB_EAX3             = $00000200;    (* Device supports EAX3 reverb. *)
    FMOD_CAPS_REVERB_EAX4             = $00000400;    (* Device supports EAX4 reverb  *)
    FMOD_CAPS_REVERB_EAX5			  = $00000800;    (* Device supports EAX5 reverb  *)
    FMOD_CAPS_REVERB_I3DL2            = $00001000;    (* Device supports I3DL2 reverb. *)
    FMOD_CAPS_REVERB_LIMITED          = $00002000;    (* Device supports some form of limited hardware reverb, maybe parameterless and only selectable by environment. *)


(*
[ENUM]
[
    [DESCRIPTION]   
    These are speaker types defined for use with the FMOD_System_SetSpeakerMode or FMOD_System_GetSpeakerMode command.

    [REMARKS]
    These are important notes on speaker modes in regards to sounds created with FMOD_SOFTWARE.
    Note below the phrase 'sound channels' is used.  These are the subchannels inside a sound, they are not related and 
    have nothing to do with the FMOD class "FMOD_CHANNEL".
    For example a mono sound has 1 sound channel, a stereo sound has 2 sound channels, and an AC3 or 6 channel wav file have 6 "sound channels".
    
    FMOD_SPEAKERMODE_NONE
    ---------------------
    This mode is for output devices that are not specifically mono/stereo/quad/surround/5.1 or 7.1, but are multichannel.
    Sound channels map to speakers sequentially, so a mono sound maps to output speaker 0, stereo sound maps to output speaker 0 & 1.
    The user assumes knowledge of the speaker order.  FMOD_SPEAKER enumerations may not apply, so raw channel indicies should be used.
    Multichannel sounds map input channels to output channels 1:1. 
    Channel::setPan and Channel::setSpeakerMix do not work.
    Speaker levels must be manually set with Channel::setSpeakerLevels.
    
    FMOD_SPEAKERMODE_MONO
    ---------------------
    This mode is for a 1 speaker arrangement.
    Panning does not work in this speaker mode.
    Mono, stereo and multichannel sounds have each sound channel played on the one speaker unity.
    Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.
    Channel::setSpeakerMix does not work.
    
    FMOD_SPEAKERMODE_STEREO
    -----------------------
    This mode is for 2 speaker arrangements that have a left and right speaker.
    - Mono sounds default to an even distribution between left and right.  They can be panned with Channel::setPan.
    - Stereo sounds default to the middle, or full left in the left speaker and full right in the right speaker.  
    - They can be cross faded with Channel::setPan.
    - Multichannel sounds have each sound channel played on each speaker at unity.
    - Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.
    - Channel::setSpeakerMix works but only front left and right parameters are used, the rest are ignored.
    
    FMOD_SPEAKERMODE_QUAD
    ------------------------
    This mode is for 4 speaker arrangements that have a front left, front right, rear left and a rear right speaker.
    - Mono sounds default to an even distribution between front left and front right.  They can be panned with Channel::setPan.
    - Stereo sounds default to the left sound channel played on the front left, and the right sound channel played on the front right.
    - They can be cross faded with Channel::setPan.
    - Multichannel sounds default to all of their sound channels being played on each speaker in order of input.
    - Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.
    - Channel::setSpeakerMix works but side left, side right, center and lfe are ignored.
    
    FMOD_SPEAKERMODE_SURROUND
    ------------------------
    This mode is for 4 speaker arrangements that have a front left, front right, front center and a rear center.
    - Mono sounds default to the center speaker.  They can be panned with Channel::setPan.
    - Stereo sounds default to the left sound channel played on the front left, and the right sound channel played on the front right.  
    - They can be cross faded with Channel::setPan.
    - Multichannel sounds default to all of their sound channels being played on each speaker in order of input.
    - Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.
    - Channel::setSpeakerMix works but side left, side right and lfe are ignored, and rear left / rear right are averaged into the rear speaker.
    
    FMOD_SPEAKERMODE_5POINT1
    ------------------------
    This mode is for 5.1 speaker arrangements that have a left/right/center/rear left/rear right and a subwoofer speaker.
    - Mono sounds default to the center speaker.  They can be panned with Channel::setPan.
    - Stereo sounds default to the left sound channel played on the front left, and the right sound channel played on the front right.  
    - They can be cross faded with Channel::setPan.
    - Multichannel sounds default to all of their sound channels being played on each speaker in order of input.  
    - Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.
    - Channel::setSpeakerMix works but side left / side right are ignored.
    
    FMOD_SPEAKERMODE_7POINT1
    ------------------------
    This mode is for 7.1 speaker arrangements that have a left/right/center/rear left/rear right/side left/side right 
    and a subwoofer speaker.
    - Mono sounds default to the center speaker.  They can be panned with Channel::setPan.
    - Stereo sounds default to the left sound channel played on the front left, and the right sound channel played on the front right.  
    - They can be cross faded with Channel::setPan.
    - Multichannel sounds default to all of their sound channels being played on each speaker in order of input.  
    - Mix behaviour for multichannel sounds can be set with Channel::setSpeakerLevels.
    - Channel::setSpeakerMix works and every parameter is used to set the balance of a sound in any speaker.
    
    FMOD_SPEAKERMODE_PROLOGIC
    ------------------------------------------------------
    This mode is for mono, stereo, 5.1 and 7.1 speaker arrangements, as it is backwards and forwards compatible with stereo, 
    but to get a surround effect a Dolby Prologic or Prologic 2 hardware decoder / amplifier is needed.
    Pan behaviour is the same as FMOD_SPEAKERMODE_5POINT1.
    
    If this function is called the numoutputchannels setting in System::setSoftwareFormat is overwritten.
    
    For 3D sounds, panning is determined at runtime by the 3D subsystem based on the speaker mode to determine which speaker the 
    sound should be placed in.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_SetSpeakerMode
    FMOD_System_GetSpeakerMode
    FMOD_System_GetDriverCaps
    FMOD_Channel_SetSpeakerLevels
]
*)
type
  FMOD_SPEAKERMODE =
  (
    FMOD_SPEAKERMODE_RAW,              (* There is no specific speakermode.  Sound channels are mapped in order of input to output.  See remarks for more information. *)
    FMOD_SPEAKERMODE_MONO,             (* The speakers are monaural. *)
    FMOD_SPEAKERMODE_STEREO,           (* The speakers are stereo (DEFAULT). *)
    FMOD_SPEAKERMODE_QUAD,             (* 4 speaker setup.  This includes front left, front right, rear left, rear right.  *)
    FMOD_SPEAKERMODE_SURROUND,         (* 4 speaker setup.  This includes front left, front right, center, rear center (rear left/rear right are averaged). *)
    FMOD_SPEAKERMODE_5POINT1,          (* 5.1 speaker setup.  This includes front left, front right, center, rear left, rear right and a subwoofer. *)
    FMOD_SPEAKERMODE_7POINT1,          (* 7.1 speaker setup.  This includes front left, front right, center, rear left, rear right, side left, side right and a subwoofer. *)
    FMOD_SPEAKERMODE_PROLOGIC          (* Stereo output, but data is encoded in a way that is picked up by a Prologic/Prologic2 decoder and split into a 5.1 speaker setup. *)
  );


(*
[ENUM]
[
    [DESCRIPTION]   
    These are speaker types defined for use with the FMOD_Channel_SetSpeakerLevels command.
    It can also be used for speaker placement in the FMOD_System_Set3DSpeakerPosition command.

    [REMARKS]
    If you are using FMOD_SPEAKERMODE_NONE and speaker assignments are meaningless, just cast a raw integer value to this type.
    For example (FMOD_SPEAKER)7 would use the 7th speaker (also the same as FMOD_SPEAKER_SIDE_RIGHT).  
    Values higher than this can be used if an output system has more than 8 speaker types / output channels.  15 is the current maximum.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_SPEAKERMODE
    FMOD_Channel_SetSpeakerLevels
    FMOD_Channel_GetSpeakerLevels
    FMOD_System_Set3DSpeakerPosition
    FMOD_System_Get3DSpeakerPosition
]
*)
type
  FMOD_SPEAKER =
  (
    FMOD_SPEAKER_FRONT_LEFT,
    FMOD_SPEAKER_FRONT_RIGHT,
    FMOD_SPEAKER_FRONT_CENTER,
    FMOD_SPEAKER_LOW_FREQUENCY,
    FMOD_SPEAKER_BACK_LEFT,
    FMOD_SPEAKER_BACK_RIGHT,
    FMOD_SPEAKER_SIDE_LEFT,
    FMOD_SPEAKER_SIDE_RIGHT,
    FMOD_SPEAKER_MAX
  );


(*
[ENUM]
[
    [DESCRIPTION]   
    These are plugin types defined for use with the FMOD_System_GetNumPlugins, 
    FMOD_System_GetPluginInfo and FMOD_System_UnloadPlugin functions.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_GetNumPlugins
    FMOD_System_GetPluginInfo
    FMOD_System_UnloadPlugin
]
*)
type
  FMOD_PLUGINTYPE =
  (
    FMOD_PLUGINTYPE_OUTPUT,          (* The plugin type is an output module.  FMOD mixed audio will play through one of these devices *)
    FMOD_PLUGINTYPE_CODEC,           (* The plugin type is a file format codec.  FMOD will use these codecs to load file formats for playback. *)
    FMOD_PLUGINTYPE_DSP              (* The plugin type is a DSP unit.  FMOD will use these plugins as part of its DSP network to apply effects to output or generate sound in realtime. *)
  );


(*
[ENUM] 
[
    [DESCRIPTION]   
    Initialization flags.  Use them with FMOD_System_Init in the flags parameter to change various behaviour.  

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_Init
]
*)
const
    FMOD_INIT_NORMAL                   = $00000000;   (* All platforms - Initialize normally *)
    FMOD_INIT_STREAM_FROM_UPDATE       = $00000001;   (* All platforms - No stream thread is created internally.  Streams are driven from System::update.  Mainly used with non-realtime outputs. *)
    FMOD_INIT_3D_RIGHTHANDED           = $00000002;   (* All platforms - FMOD will treat +X as left, +Y as up and +Z as forwards. *)
    FMOD_INIT_SOFTWARE_DISABLE         = $00000004;   (* All platforms - Disable software mixer to save memory.  Anything created with FMOD_SOFTWARE will fail and DSP will not work. *)
    FMOD_INIT_SOFTWARE_OCCLUSION       = $00000008;   (* All platforms - All FMOD_SOFTWARE with FMOD_3D based voices will add a software lowpass filter effect into the DSP chain which is automatically used when Channel::set3DOcclusion is used or the geometry API. *)
    FMOD_INIT_SOFTWARE_HRTF            = $00000010;   (* All platforms - All FMOD_SOFTWARE with FMOD_3D based voices will add a software lowpass filter effect into the DSP chain which causes sounds to sound duller when the sound goes behind the listener. *)
    FMOD_INIT_VOL0_BECOMES_VIRTUAL     = $00000080;   (* All platforms - Any sounds that are 0 volume will go virtual and not be processed except for having their positions updated virtually.  Use System::setAdvancedSettings to adjust what volume besides zero to switch to virtual at. *)
    FMOD_INIT_WASAPI_EXCLUSIVE         = $00000100;   (* Win32 Vista only - for WASAPI output - Enable exclusive access to hardware, lower latency at the expense of excluding other applications from accessing the audio hardware. *)
    FMOD_INIT_DSOUND_HRTFNONE          = $00000200;   (* Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use simple stereo panning/doppler/attenuation when 3D hardware acceleration is not present. *)
    FMOD_INIT_DSOUND_HRTFLIGHT         = $00000400;   (* Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use a slightly higher quality algorithm when 3D hardware acceleration is not present. *)
    FMOD_INIT_DSOUND_HRTFFULL          = $00000800;   (* Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use full quality 3D playback when 3d hardware acceleration is not present. *)
    FMOD_INIT_PS2_DISABLECORE0REVERB   = $00010000;   (* PS2 only - Disable reverb on CORE 0 to regain SRAM. *)
    FMOD_INIT_PS2_DISABLECORE1REVERB   = $00020000;   (* PS2 only - Disable reverb on CORE 1 to regain SRAM. *)
    FMOD_INIT_PS2_DONTUSESCRATCHPAD    = $00040000;   (* PS2 only - FMOD's usage of the scratchpad between System::update and System::updateFinished. *)
    FMOD_INIT_PS2_SWAPDMACHANNELS      = $00080000;   (* PS2 only - Changes FMOD from using SPU DMA channel 0 for software mixing, and 1 for sound data upload/file streaming, to 1 and 0 respectively. *)
    FMOD_INIT_XBOX_REMOVEHEADROOM      = $00100000;   (* Xbox only - By default DirectSound attenuates all sound by 6db to avoid clipping/distortion.  CAUTION.  If you use this flag you are responsible for the final mix to make sure clipping / distortion doesn't happen. *)
    FMOD_INIT_360_MUSICMUTENOTPAUSE    = $00200000;   (* Xbox 360 only - The "music" channelgroup which by default pauses when custom 360 dashboard music is played, can be changed to mute (therefore continues playing) instead of pausing, by using this flag. *)



(*
[ENUM]
[
    [DESCRIPTION]   
    These definitions describe the type of song being played.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_Sound_GetFormat
]
*)
type
  FMOD_SOUND_TYPE =
  (
    FMOD_SOUND_TYPE_UNKNOWN,         (* 3rd party / unknown plugin format. *)
    FMOD_SOUND_TYPE_AAC,             (* AAC. *)
    FMOD_SOUND_TYPE_AIFF,            (* AIFF. *)
    FMOD_SOUND_TYPE_ASF,             (* Microsoft Advanced Systems Format (ie WMA/ASF/WMV). *)
    FMOD_SOUND_TYPE_AT3,
    FMOD_SOUND_TYPE_CDDA,            (* Digital CD audio. *)
    FMOD_SOUND_TYPE_DLS,             (* Sound font / downloadable sound bank. *)
    FMOD_SOUND_TYPE_FLAC,            (* FLAC lossless codec. *)
    FMOD_SOUND_TYPE_FSB,             (* FMOD Sample Bank. *)
    FMOD_SOUND_TYPE_GCADPCM,         (* GameCube ADPCM *)
    FMOD_SOUND_TYPE_IT,              (* Impulse Tracker. *)
    FMOD_SOUND_TYPE_MIDI,            (* MIDI. *)
    FMOD_SOUND_TYPE_MOD,             (* Protracker / Fasttracker MOD. *)
    FMOD_SOUND_TYPE_MPEG,            (* MP2/MP3 MPEG. *)
    FMOD_SOUND_TYPE_OGGVORBIS,       (* Ogg vorbis. *)
    FMOD_SOUND_TYPE_PLAYLIST,        (* Information only from ASX/PLS/M3U/WAX playlists *)
    FMOD_SOUND_TYPE_RAW,             (* Raw PCM data. *)
    FMOD_SOUND_TYPE_S3M,             (* ScreamTracker 3. *)
    FMOD_SOUND_TYPE_SF2,             (* Sound font 2 format. *)
    FMOD_SOUND_TYPE_USER,            (* User created sound. *)
    FMOD_SOUND_TYPE_WAV,             (* Microsoft WAV. *)
    FMOD_SOUND_TYPE_XM,              (* FastTracker 2 XM. *)
    FMOD_SOUND_TYPE_XMA,
    FMOD_SOUND_TYPE_VAG
  );


(*
[ENUM]
[
    [DESCRIPTION]   
    These definitions describe the native format of the hardware or software buffer that will be used.

    [REMARKS]
    This is the format the native hardware or software buffer will be or is created in.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_CreateSound
    FMOD_Sound_GetFormat
]
*)
type
  FMOD_SOUND_FORMAT =
  (
    FMOD_SOUND_FORMAT_NONE,             (* Unitialized / unknown. *)
    FMOD_SOUND_FORMAT_PCM8,             (* 8bit integer PCM data. *)
    FMOD_SOUND_FORMAT_PCM16,            (* 16bit integer PCM data. *)
    FMOD_SOUND_FORMAT_PCM24,            (* 24bit integer PCM data. *)
    FMOD_SOUND_FORMAT_PCM32,            (* 32bit integer PCM data. *)
    FMOD_SOUND_FORMAT_PCMFLOAT,         (* 32bit floating point PCM data. *)
    FMOD_SOUND_FORMAT_GCADPCM,          (* Compressed GameCube DSP data. *)
    FMOD_SOUND_FORMAT_IMAADPCM,         (* Compressed IMA ADPCM / Xbox ADPCM data. *)
    FMOD_SOUND_FORMAT_VAG,              (* Compressed PlayStation 2 ADPCM data *)
    FMOD_SOUND_FORMAT_XMA,              (* Compressed Xbox360 data. *)
    FMOD_SOUND_FORMAT_MPEG              (* Compressed MPEG layer 2 or 3 data. *)
  );
  PFMOD_SOUND_FORMAT = ^FMOD_SOUND_FORMAT;


(*
[ENUM]
[  
    [DESCRIPTION]   
    Sound description bitfields, bitwise OR them together for loading and describing sounds.

    [REMARKS]
    By default a sound will open as a static sound that is decompressed fully into memory.  To have a sound stream instead, use FMOD_CREATESTREAM.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_System_CreateSound
    FMOD_System_CreateStream
    FMOD_Sound_GetMode
    FMOD_Sound_GetMode
    FMOD_Channel_SetMode
    FMOD_Channel_GetMode
]
*)
const
    FMOD_DEFAULT                = $00000000;  (* FMOD_DEFAULT is a default sound type.  Equivalent to all the defaults listed below.  FMOD_LOOP_OFF, FMOD_2D, FMOD_HARDWARE. *)
    FMOD_LOOP_OFF               = $00000001;  (* For non looping sounds. (default).  Overrides FMOD_LOOP_NORMAL / FMOD_LOOP_BIDI. *)
    FMOD_LOOP_NORMAL            = $00000002;  (* For forward looping sounds. *)
    FMOD_LOOP_BIDI              = $00000004;  (* For bidirectional looping sounds. (only works on software mixed static sounds). *)
    FMOD_2D                     = $00000008;  (* Ignores any 3d processing. (default). *)
    FMOD_3D                     = $00000010;  (* Makes the sound positionable in 3D.  Overrides FMOD_2D. *)
    FMOD_HARDWARE               = $00000020;  (* Attempts to make sounds use hardware acceleration. (default). *)
    FMOD_SOFTWARE               = $00000040;  (* Makes sound reside in software.  Overrides FMOD_HARDWARE.  Use this for FFT, DSP, 2D multi speaker support and other software related features. *)
    FMOD_CREATESTREAM           = $00000080;  (* Decompress at runtime, streaming from the source provided (standard stream).  Overrides FMOD_CREATESAMPLE. *)
    FMOD_CREATESAMPLE           = $00000100;  (* Decompress at loadtime, decompressing or decoding whole file into memory as the target sample format. (standard sample). *)
    FMOD_CREATECOMPRESSEDSAMPLE = $00000200;  (* Load MP2, MP3, IMAADPCM or XMA into memory and leave it compressed.  During playback the FMOD software mixer will decode it in realtime as a 'compressed sample'.  Can only be used in combination with FMOD_SOFTWARE.  Overrides FMOD_CREATESAMPLE. *)
    FMOD_OPENUSER               = $00000400;  (* Opens a user created static sample or stream. Use FMOD_CREATESOUNDEXINFO to specify format and/or read callbacks. *)
    FMOD_OPENMEMORY             = $00000800;  (* "name_or_data" will be interpreted as a pointer to memory instead of filename for creating sounds. *)
    FMOD_OPENMEMORY_POINT       = $10000000;  (* "name_or_data" will be interpreted as a pointer to memory instead of filename for creating sounds.  Use FMOD_CREATESOUNDEXINFO to specify length.  This differs to FMOD_OPENMEMORY in that it uses the memory as is, without duplicating the memory into its own buffers.  FMOD_SOFTWARE only.  Doesn't work with FMOD_HARDWARE, as sound hardware cannot access main ram on a lot of platforms.  Cannot be freed after open, only after Sound::release.   Will not work if the data is compressed and FMOD_CREATECOMPRESSEDSAMPLE is not used. *)
    FMOD_OPENRAW                = $00001000;  (* Will ignore file format and treat as raw pcm.  Use FMOD_CREATESOUNDEXINFO to specify format. *)
    FMOD_OPENONLY               = $00002000;  (* Just open the file, dont prebuffer or read.  Good for fast opens for info, or when FMOD_Sound_ReadData is to be used. *)
    FMOD_ACCURATETIME           = $00004000;  (* For FMOD_CreateSound - for accurate FMOD_Sound_GetLength / FMOD_Channel_SetPosition on VBR MP3 or AAC.  Scans file first, so takes longer to open. FMOD_OPENONLY does not affect this. *)
    FMOD_MPEGSEARCH             = $00008000;  (* For corrupted / bad MP3 files.  This will search all the way through the file until it hits a valid MPEG header.  Normally only searches for 4k. *)
    FMOD_NONBLOCKING            = $00010000;  (* For opening sounds asyncronously, return value from open function must be polled for when it is ready. *)
    FMOD_UNIQUE                 = $00020000;  (* Unique sound, can only be played one at a time *)
    FMOD_3D_HEADRELATIVE        = $00040000;  (* Make the sound's position, velocity and orientation relative to the listener. *)
    FMOD_3D_WORLDRELATIVE       = $00080000;  (* Make the sound's position, velocity and orientation absolute (relative to the world). (DEFAULT) *)
    FMOD_3D_LOGROLLOFF          = $00100000;  (* This sound will follow the standard logarithmic rolloff model where mindistance = full volume, maxdistance = where sound stops attenuating, and rolloff is fixed according to the global rolloff factor.  (default) *)
    FMOD_3D_LINEARROLLOFF       = $00200000;  (* This sound will follow a linear rolloff model where mindistance = full volume, maxdistance = silence.  *)
    FMOD_3D_CUSTOMROLLOFF       = $04000000;  (* This sound will follow a rolloff model defined by FMOD_Sound_Set3DCustomRolloff / FMOD_Channel_Set3DCustomRolloff. *)
    FMOD_3D_IGNOREGEOMETRY      = $40000000;  (* Is not affect by geometry occlusion.  If not specified in Sound::setMode, or Channel::setMode, the flag is cleared and it is affected by geometry again. *)
    FMOD_CDDA_FORCEASPI         = $00400000;  (* For CDDA sounds only - use ASPI instead of NTSCSI to access the specified CD/DVD device. *)
    FMOD_CDDA_JITTERCORRECT     = $00800000;  (* For CDDA sounds only - perform jitter correction. Jitter correction helps produce a more accurate CDDA stream at the cost of more CPU time. *)
    FMOD_UNICODE                = $01000000;  (* Filename is double-byte unicode. *)
    FMOD_IGNORETAGS             = $02000000;  (* Skips id3v2/asf/etc tag checks when opening a sound, to reduce seek/read overhead when opening files (helps with CD performance). *)
    FMOD_LOWMEM                 = $08000000;  (* Removes some features from samples to give a lower memory overhead, like Sound::getName. *)
    FMOD_LOADSECONDARYRAM       = $20000000;  (* Load sound into the secondary RAM of supported platform.  On PS3, sounds will be loaded into RSX/VRAM. *)
    FMOD_VIRTUAL_PLAYFROMSTART  = $80000000;  (* For sounds that start virtual (due to being quiet or low importance), instead of swapping back to audible, and playing at the correct offset according to time, this flag makes the sound play from the start. *)

(*
[ENUM]
[
    [DESCRIPTION]   
    These values describe what state a sound is in after FMOD_NONBLOCKING has been used to open it.

    [REMARKS]    

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_Sound_GetOpenState
    FMOD_MODE
]
*)
type
  FMOD_OPENSTATE =
  (
    FMOD_OPENSTATE_READY,           (* Opened and ready to play *)
    FMOD_OPENSTATE_LOADING,         (* Initial load in progress *)
    FMOD_OPENSTATE_ERROR,           (* Failed to open - file not found, out of memory etc. *)
    FMOD_OPENSTATE_CONNECTING,      (* Connecting to remote host (internet sounds only) *)
    FMOD_OPENSTATE_BUFFERING,       (* Buffering data *)
    FMOD_OPENSTATE_SEEKING          (* Seeking to subsound and re-flushing stream buffer. *)
    FMOD_OPENSTATE_STREAMING,       (* Ready and playing, but not possible to release at this time without stalling the main thread. *)
  );


(*
[ENUM]
[
    [DESCRIPTION]   
    These callback types are used with FMOD_Channel_SetCallback.

    [REMARKS]
    Each callback has commanddata parameters passed int unique to the type of callback.
    See reference to FMOD_CHANNEL_CALLBACK to determine what they might mean for each type of callback.

    Note!  Currently the user must call System::update for these callbacks to trigger!

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_Channel_SetCallback
    FMOD_CHANNEL_CALLBACK
    FMOD_System_Update
]
*)
type 
  FMOD_CHANNEL_CALLBACKTYPE =
  (
    FMOD_CHANNEL_CALLBACKTYPE_END,				    (* Called when a sound ends. *)
    FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE,	        (* Called when a voice is swapped out or swapped in. *)
    FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT				(* Called when a syncpoint is encountered.  Can be from wav file markers. *)
  );


(* 
    FMOD Callbacks
*)
type
  FMOD_CHANNEL_CALLBACK       = function (channel: FMOD_CHANNEL; callbacktype: FMOD_CHANNEL_CALLBACKTYPE; command: Integer; commanddata1:word; commanddata2:word ): FMOD_RESULT; stdcall;

  FMOD_SOUND_NONBLOCKCALLBACK = function (sound: FMOD_SOUND; result: FMOD_RESULT): FMOD_RESULT; stdcall;
  FMOD_SOUND_PCMREADCALLBACK  = function (sound: FMOD_SOUND; data: Pointer; datalen: Cardinal): FMOD_RESULT; stdcall;
  FMOD_SOUND_PCMSEEKCALLBACK  = function (sound: FMOD_SOUND; subsound: Integer; pcmoffset: Cardinal): FMOD_RESULT; stdcall;

  FMOD_FILE_OPENCALLBACK      = function (const name: PChar; unicode: Integer; filesize: Cardinal; handle: Pointer; var userdata: Pointer): FMOD_RESULT; stdcall;
  FMOD_FILE_CLOSECALLBACK     = function (handle: Pointer; userdata: Pointer): FMOD_RESULT; stdcall;
  FMOD_FILE_READCALLBACK      = function (handle, buffer: Pointer; sizebytes: Cardinal; var bytesread: Cardinal; userdata: Pointer): FMOD_RESULT; stdcall;
  FMOD_FILE_SEEKCALLBACK      = function (handle: Pointer; pos, type_: Integer; userdata: Pointer): FMOD_RESULT; stdcall;

  FMOD_MEMORY_ALLOCCALLBACK   = function (size: Cardinal): Pointer; stdcall;
  FMOD_MEMORY_REALLOCCALLBACK = function (ptr: Pointer; size: Cardinal): Pointer; stdcall;
  FMOD_MEMORY_FREECALLBACK    = procedure (ptr: Pointer); stdcall;


(*
[ENUM]
[
    [DESCRIPTION]   
    List of windowing methods used in spectrum analysis to reduce leakage / transient signals intefering with the analysis.
    This is a problem with analysis of continuous signals that only have a small portion of the signal sample (the fft window size).
    Windowing the signal with a curve or triangle tapers the sides of the fft window to help alleviate this problem.

    [REMARKS]
    Cyclic signals such as a sine wave that repeat their cycle in a multiple of the window size do not need windowing.
    I.e. If the sine wave repeats every 1024, 512, 256 etc samples and the FMOD fft window is 1024, then the signal would not need windowing.
    Not windowing is the same as FMOD_DSP_FFT_WINDOW_RECT, which is the default.
    If the cycle of the signal (ie the sine wave) is not a multiple of the window size, it will cause frequency abnormalities, so a different windowing method is needed.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_System_GetSpectrum
    FMOD_Channel_GetSpectrum
]
*)
type 
  FMOD_DSP_FFT_WINDOW =
  (
    FMOD_DSP_FFT_WINDOW_RECT,           (* w[n] = 1.0                                                                                            *)
    FMOD_DSP_FFT_WINDOW_TRIANGLE,       (* w[n] = TRI(2n/N)                                                                                      *)
    FMOD_DSP_FFT_WINDOW_HAMMING,        (* w[n] = 0.54 - (0.46 * COS(n/N) )                                                                      *)
    FMOD_DSP_FFT_WINDOW_HANNING,        (* w[n] = 0.5 *  (1.0  - COS(n/N) )                                                                      *)
    FMOD_DSP_FFT_WINDOW_BLACKMAN,       (* w[n] = 0.42 - (0.5  * COS(n/N) ) + (0.08 * COS(2.0 * n/N) )                                           *)
    FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS, (* w[n] = 0.35875 - (0.48829 * COS(1.0 * n/N)) + (0.14128 * COS(2.0 * n/N)) - (0.01168 * COS(3.0 * n/N)) *)
    FMOD_DSP_FFT_WINDOW_MAX
  );


(*
[ENUM]
[
    [DESCRIPTION]   
    List of interpolation types that the FMOD Ex software mixer supports.  

    [REMARKS]
    The default resampler type is FMOD_DSP_RESAMPLER_LINEAR.
    Use System::setSoftwareFormat to tell FMOD the resampling quality you require for FMOD_SOFTWARE based sounds.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    System::setSoftwareFormat
    System::getSoftwareFormat
]
*)
type
  FMOD_DSP_RESAMPLER = 
  (
    FMOD_DSP_RESAMPLER_NOINTERP,        (* No interpolation.  High frequency aliasing hiss will be audible depending on the sample rate of the sound. *)
    FMOD_DSP_RESAMPLER_LINEAR,          (* Linear interpolation (default method).  Fast and good quality, causes very slight lowpass effect on low frequency sounds. *)
    FMOD_DSP_RESAMPLER_CUBIC,           (* Cubic interoplation.  Slower than linear interpolation but better quality. *)
    FMOD_DSP_RESAMPLER_SPLINE,          (* 5 point spline interoplation.  Slowest resampling method but best quality. *)
    FMOD_DSP_RESAMPLER_MAX
  );



(*
[ENUM]
[
    [DESCRIPTION]   
    List of tag types that could be stored within a sound.  These include id3 tags, metadata from netstreams and vorbis/asf data.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_Sound_GetTag
]
*)
type 
  FMOD_TAGTYPE =
  (
    FMOD_TAGTYPE_UNKNOWN,
    FMOD_TAGTYPE_ID3V1,
    FMOD_TAGTYPE_ID3V2,
    FMOD_TAGTYPE_VORBISCOMMENT,
    FMOD_TAGTYPE_SHOUTCAST,
    FMOD_TAGTYPE_ICECAST,
    FMOD_TAGTYPE_ASF,
    FMOD_TAGTYPE_MIDI,
    FMOD_TAGTYPE_PLAYLIST,
    FMOD_TAGTYPE_FMOD,
    FMOD_TAGTYPE_USER,
    FMOD_TAGTYPE_MAX
  );


(*
[ENUM]
[
    [DESCRIPTION]   
    List of data types that can be returned by FMOD_Sound_GetTag

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_Sound_GetTag
]
*)
type
  FMOD_TAGDATATYPE =
  (
    FMOD_TAGDATATYPE_BINARY,
    FMOD_TAGDATATYPE_INT,
    FMOD_TAGDATATYPE_FLOAT,
    FMOD_TAGDATATYPE_STRING,
    FMOD_TAGDATATYPE_STRING_UTF16,
    FMOD_TAGDATATYPE_STRING_UTF16BE,
    FMOD_TAGDATATYPE_STRING_UTF8,
    FMOD_TAGDATATYPE_CDTOC
  );


(*
[STRUCTURE] 
[
    [DESCRIPTION]   
    Structure describing a piece of tag data.

    [REMARKS]
    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_Sound_GetTag
    FMOD_TAGTYPE
    FMOD_TAGDATATYPE
]
*)
type
  FMOD_TAG = record
    tagtype: FMOD_TAGTYPE;      (* [out] The type of this tag. *)
    datatype: FMOD_TAGDATATYPE; (* [out] The type of data that this tag contains *)
    name: PChar;                (* [out] The name of this tag i.e. "TITLE", "ARTIST" etc. *)
    data: Pointer;              (* [out] Pointer to the tag data - its format is determined by the datatype member *)
    datalen: Cardinal;          (* [out] Length of the data contained in this tag *)
    updated: FMOD_BOOL;         (* [out] True if this tag has been updated since last being accessed with FMOD_Sound_GetTag *)
  end;


(*
[STRUCTURE]
[
    [DESCRIPTION]   
    Structure describing a CD/DVD table of contents

    [REMARKS]
    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_Sound_GetTag
]
*)
type
  FMOD_CDTOC = record
    numtracks: Integer;                 (* [out] The number of tracks on the CD *)
    min: array [0..99] of Integer;      (* [out] The start offset of each track in minutes *)
    sec: array [0..99] of Integer;      (* [out] The start offset of each track in seconds *)
    frame: array [0..99] of Integer;    (* [out] The start offset of each track in frames *)
  end;


(*
[ENUM]
[
    [DESCRIPTION]   
    List of time types that can be returned by FMOD_Sound_GetLength and used with FMOD_Channel_SetPosition or FMOD_Channel_GetPosition.

    [REMARKS]
    FMOD_TIMEUNIT_SUBSOUND_MS, FMOD_TIMEUNIT_SUBSOUND_PCM, FMOD_TIMEUNIT_SUBSOUND_PCMBYTES, FMOD_TIMEUNIT_SUBSOUND and FMOD_TIMEUNIT_SUBSOUND_BUFFERED are only supported by Channel::getPosition.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_Sound_GetLength
    FMOD_Channel_SetPosition
    FMOD_Channel_GetPosition
]
*)
const
    FMOD_TIMEUNIT_MS                 = $00000001;  (* Milliseconds. *)
    FMOD_TIMEUNIT_PCM                = $00000002;  (* PCM Samples, related to milliseconds * samplerate / 1000. *)
    FMOD_TIMEUNIT_PCMBYTES           = $00000004;  (* Bytes, related to PCM samples * channels * datawidth (ie 16bit = 2 bytes). *)
    FMOD_TIMEUNIT_RAWBYTES           = $00000008;  (* Raw file bytes of (compressed) sound data (does not include headers).  Only used by FMOD_Sound_GetLength and FMOD_Channel_GetPosition. *)
    FMOD_TIMEUNIT_MODORDER           = $00000100;  (* MOD/S3M/XM/IT.  Order in a sequenced module format.  Use FMOD_Sound_GetFormat to determine the format. *)
    FMOD_TIMEUNIT_MODROW             = $00000200;  (* MOD/S3M/XM/IT.  Current row in a sequenced module format.  FMOD_Sound_GetLength will return the number if rows in the currently playing or seeked to pattern. *)
    FMOD_TIMEUNIT_MODPATTERN         = $00000400;  (* MOD/S3M/XM/IT.  Current pattern in a sequenced module format.  Sound::getLength will return the number of patterns in the song and Channel::getPosition will return the currently playing pattern. *)
    FMOD_TIMEUNIT_SENTENCE_MS        = $00010000;  (* Currently playing subsound in a sentence time in milliseconds. *)
    FMOD_TIMEUNIT_SENTENCE_PCM       = $00020000;  (* Currently playing subsound in a sentence time in PCM Samples, related to milliseconds * samplerate / 1000. *)
    FMOD_TIMEUNIT_SENTENCE_PCMBYTES  = $00040000;  (* Currently playing subsound in a sentence time in bytes, related to PCM samples * channels * datawidth (ie 16bit = 2 bytes). *)
    FMOD_TIMEUNIT_SENTENCE           = $00080000;  (* Currently playing subsound in a sentence according to the channel. For display.  *)
    FMOD_TIMEUNIT_SENTENCE_SUBSOUND  = $00100000;  (* Currently playing subsound in a sentence according to the buffered ahead of time sound.  For sentence processing ahead of time. *)
    FMOD_TIMEUNIT_BUFFERED           = $10000000;  (* Time value as seen by buffered stream.  This is always ahead of audible time, and is only used for processing. *)


(*
[ENUM]
[
    [DESCRIPTION]
    When creating a multichannel sound, FMOD will pan them to their default speaker locations, for example a 6 channel sound will default to one channel per 5.1 output speaker.
    Another example is a stereo sound.  It will default to left = front left, right = front right.

    This is for sounds that are not 'default'.  For example you might have a sound that is 6 channels but actually made up of 3 stereo pairs, that should all be located in front left, front right only.

    [REMARKS]
    For full flexibility of speaker assignments, use Channel::setSpeakerLevels.  This functionality is cheaper, uses less memory and easier to use.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    [SEE_ALSO]
    FMOD_CREATESOUNDEXINFO
    FMOD_Channel_SetSpeakerLevels
]
*)
type
  FMOD_SPEAKERMAPTYPE =
  (
    FMOD_SPEAKERMAPTYPE_DEFAULT,     (* This is the default, and just means FMOD decides which speakers it puts the source channels. *)
    FMOD_SPEAKERMAPTYPE_ALLMONO,     (* This means the sound is made up of all mono sounds.  All voices will be panned to the front center by default in this case.  *)
    FMOD_SPEAKERMAPTYPE_ALLSTEREO    (* This means the sound is made up of all stereo sounds.  All voices will be panned to front left and front right alternating every second channel.  *)
  );


(*
[STRUCTURE] 
[
    [DESCRIPTION]
    Use this structure with FMOD_System_CreateSound when more control is needed over loading.
    The possible reasons to use this with FMOD_System_CreateSound are:
    - Loading a file from memory.
    - Loading a file from within another file, giving an offset and length.
    - To create a user created / non file based sound.
    - To specify a starting subsound to seek to within a multi-sample sounds (ie FSB/DLS/SF2) when created as a stream.
    - To specify which subsounds to load for multi-sample sounds (ie FSB/DLS/SF2) so that memory is saved and only a subset is actually loaded/read from disk.
    - To specify 'piggyback' read and seek callbacks for capture of sound data as fmod reads and decodes it.  Useful for ripping decoded PCM data from sounds as they are loaded / played.
    - To specify a MIDI DLS/SF2 sample set file to load when opening a MIDI file.
    See below on what members to fill for each of the above types of sound you want to create.

    [REMARKS]
    This structure is optional!  Specify 0 or NULL in FMOD_System_CreateSound if you don't need it!

    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.

    Loading a file from memory.
    ---------------------------
    - Create the sound using the FMOD_OPENMEMORY flag.
    - Mandatory.  Specify 'length' for the size of the memory block in bytes.
    - Other flags are optional.
        
    Loading a file from within another larger (possibly wad/pak) file, by giving the loader an offset and length.
    -------------------------------------------------------------------------------------------------------------
    - Mandatory.  Specify 'fileoffset' and 'length'.
    - Other flags are optional.
        
    To create a user created / non file based sound.
    ------------------------------------------------
    - Create the sound using the FMOD_OPENUSER flag.
    - Mandatory.  Specify 'defaultfrequency, 'numchannels' and 'format'.
    - Other flags are optional.    
    
    To specify a starting subsound to seek to and flush with, within a multi-sample stream (ie FSB/DLS/SF2).
    --------------------------------------------------------------------------------------------------------   
    - Mandatory.  Specify 'initialsubsound'.    
    
    To specify which subsounds to load for multi-sample sounds (ie FSB/DLS/SF2) so that memory is saved and only a subset is actually loaded/read from disk.
    --------------------------------------------------------------------------------------------------------------------------------------------------------
    - Mandatory.  Specify 'inclusionlist' and 'inclusionlistnum'.    
    
    To specify 'piggyback' read and seek callbacks for capture of sound data as fmod reads and decodes it.  Useful for ripping decoded PCM data from sounds as they are loaded / played.
    ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    
    - Mandatory.  Specify 'pcmreadcallback' and 'pcmseekcallback'.
        
    To specify a MIDI DLS/SF2 sample set file to load when opening a MIDI file.
    ---------------------------------------------------------------------------    
    - Mandatory.  Specify 'dlsname'.    
    
    Setting the 'decodebuffersize' is for cpu intensive codecs that may be causing stuttering, not file intensive codecs (ie those from CD or netstreams) which are normally altered with System::setStreamBufferSize.  As an example of cpu intensive codecs, an mp3 file will take more cpu to decode than a PCM wav file.
    If you have a stuttering effect, then it is using more cpu than the decode buffer playback rate can keep up with.  Increasing the decode buffersize will most likely solve this problem.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_CreateSound
]
*)
type
  FMOD_CREATESOUNDEXINFO = record
    cbsize: Integer;                                   (* [in] Size of this structure.  This is used so the structure can be expanded in the future and still work on older versions of FMOD Ex. *)
    length: Cardinal;                                  (* [in] Optional. Specify 0 to ignore. Size in bytes of file to load, or sound to create (in this case only if FMOD_OPENUSER is used).  Required if loading from memory.  If 0 is specified, then it will use the size of the file (unless loading from memory then an error will be returned). *)
    fileoffset: Cardinal;                              (* [in] Optional. Specify 0 to ignore. Offset from start of the file to start loading from.  This is useful for loading files from inside big data files. *)
    numchannels: Integer;                              (* [in] Optional. Specify 0 to ignore. Number of channels in a sound specified only if FMOD_OPENUSER is used. *)
    defaultfrequency: Integer;                         (* [in] Optional. Specify 0 to ignore. Default frequency of sound in a sound specified only if FMOD_OPENUSER is used.  Other formats use the frequency determined by the file format. *)
    format: FMOD_SOUND_FORMAT;                         (* [in] Optional. Specify 0 or FMOD_SOUND_FORMAT_NONE to ignore. Format of the sound specified only if FMOD_OPENUSER is used.  Other formats use the format determined by the file format.   *)
    decodebuffersize: Integer;                         (* [in] Optional. Specify 0 to ignore. For streams.  This determines the size of the double buffer (in PCM samples) that a stream uses.  Use this for user created streams if you want to determine the size of the callback buffer passed to you.  Specify 0 to use FMOD's default size which is currently equivalent to 400ms of the sound format created/loaded. *)
    initialsubsound: Integer;                          (* [in] Optional. Specify 0 to ignore. In a multi-sample file format such as .FSB/.DLS/.SF2, specify the initial subsound to seek to, only if FMOD_CREATESTREAM is used. *)
    numsubsounds: Integer;                             (* [in] Optional. Specify 0 to ignore or have no subsounds.  In a user created multi-sample sound, specify the number of subsounds within the sound that are accessable with FMOD_SoundGetSubSound. *)
    inclusionlist: ^Integer;                           (* [in] Optional. Specify 0 to ignore. In a multi-sample format such as .FSB/.DLS/.SF2 it may be desirable to specify only a subset of sounds to be loaded out of the whole file.  This is an array of subsound indicies to load into memory when created. *)
    inclusionlistnum: Integer;                         (* [in] Optional. Specify 0 to ignore. This is the number of integers contained within the *)
    pcmreadcallback: FMOD_SOUND_PCMREADCALLBACK;       (* [in] Optional. Specify 0 to ignore. Callback to 'piggyback' on FMOD's read functions and accept or even write PCM data while FMOD is opening the sound.  Used for user sounds created with FMOD_OPENUSER or for capturing decoded data as FMOD reads it. *)
    pcmseekcallback: FMOD_SOUND_PCMSEEKCALLBACK;       (* [in] Optional. Specify 0 to ignore. Callback for when the user calls a seeking function such as FMOD_Channel_SetPosition within a multi-sample sound, and for when it is opened.*)
    nonblockcallback: FMOD_SOUND_NONBLOCKCALLBACK;     (* [in] Optional. Specify 0 to ignore. Callback for successful completion, or error while loading a sound that used the FMOD_NONBLOCKING flag.*)
    dlsname: PChar;                                    (* [in] Optional. Specify 0 to ignore. Filename for a DLS or SF2 sample set when loading a MIDI file.   If not specified, on windows it will attempt to open /windows/system32/drivers/gm.dls, otherwise the MIDI will fail to open.  *)
    encryptionkey: PChar;                              (* [in] Optional. Specify 0 to ignore. Key for encrypted FSB file.  Without this key an encrypted FSB file will not load. *)
    maxpolyphony: Integer;                             (* [in] Optional. Specify 0 to ignore. For sequenced formats with dynamic channel allocation such as .MID and .IT, this specifies the maximum voice count allowed while playing.  .IT defaults to 64.  .MID defaults to 32. *)
    userdata: Pointer;                                 (* [in] Optional. Specify 0 to ignore. This is user data to be attached to the sound during creation.  Access via Sound::getUserData. *)
    suggestedsoundtype: FMOD_SOUND_TYPE;               (* [in] Optional. Specify 0 or FMOD_SOUND_TYPE_UNKNOWN to ignore.  Instead of scanning all codec types, use this to speed up loading by making it jump straight to this codec. *)
    useropen:  FMOD_FILE_OPENCALLBACK;                 (* [in] Optional. Specify 0 to ignore. Callback for opening this file. *)
    userclose: FMOD_FILE_CLOSECALLBACK;                (* [in] Optional. Specify 0 to ignore. Callback for closing this file. *)
    userread: FMOD_FILE_READCALLBACK;                  (* [in] Optional. Specify 0 to ignore. Callback for reading from this file. *)
    userseek: FMOD_FILE_SEEKCALLBACK;                  (* [in] Optional. Specify 0 to ignore. Callback for seeking within this file. *)
    speakermap: FMOD_SPEAKERMAPTYPE;                   (* [in] Optional. Specify 0 to ignore. Use this to differ the way fmod maps multichannel sounds to speakers.  See FMOD_SPEAKERMAPTYPE for more. *)
    initialsoundgroup: Integer;                        (* [in] Optional. Specify 0 to ignore. Specify a sound group if required, to put sound in as it is created. *)
    initialseekposition: Integer;                      (* [in] Optional. Specify 0 to ignore. For streams. Specify an initial position to seek the stream to. *)
    initialseekpostype: FMOD_TIMEUNIT;                 (* [in] Optional. Specify 0 to ignore. For streams. Specify the time unit for the position set in initialseekposition. *)
  end;
  PFMOD_CREATESOUNDEXINFO = ^FMOD_CREATESOUNDEXINFO; 


(*
[STRUCTURE] 
[
    [DESCRIPTION]
    Structure defining a reverb environment.

    For more indepth descriptions of the reverb properties under win32, please see the EAX2 and EAX3
    documentation at http://developer.creative.com/ under the 'downloads' section.
    If they do not have the EAX3 documentation, then most information can be attained from
    the EAX2 documentation, as EAX3 only adds some more parameters and functionality on top of
    EAX2.

    [REMARKS]
    Note the default reverb properties are the same as the FMOD_PRESET_GENERIC preset.
    Note that integer values that typically range from -10,000 to 1000 are represented in 
    decibels, and are of a logarithmic scale, not linear, wheras float values are always linear.
    PORTABILITY: Each member has the platform it supports in braces ie (win32/xbox).  
    Some reverb parameters are only supported in win32 and some only on xbox. If all parameters are set then
    the reverb should product a similar effect on either platform.
    Win32/Win64 - This is only supported with FMOD_OUTPUTTYPE_DSOUND and EAX compatible sound cards. 
    Macintosh - Currently unsupported.
    Linux - Currently unsupported.
    Xbox - Only a subset of parameters are supported.  
    PlayStation 2 - Only the Environment and Flags paramenters are supported. 
    GameCube - Only a subset of parameters are supported.
    
    The numerical values listed below are the maximum, minimum and default values for each variable respectively.
    
    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.

    [PLATFORMS]
    Win32, Win64, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable

    [SEE_ALSO]
    FMOD_System_SetReverbProperties
    FMOD_System_GetReverbProperties
    FMOD_REVERB_PRESETS
    FMOD_REVERB_FLAGS
]
*)
type
  FMOD_REVERB_PROPERTIES = record           (*          MIN     MAX     DEFAULT  DESCRIPTION *)
    Instance: Integer;                      (* [in/out] 0     , 2     , 0      , EAX4 only. Environment Instance. 3 seperate reverbs simultaneously are possible. This specifies which one to set. (win32 only) *)
    Environment: Cardinal;                  (* [in/out] 0     , 25    , 0      , sets all listener properties (win32/ps2) *)
    EnvSize: Single;                        (* [in/out] 1.0   , 100.0 , 7.5    , environment size in meters (win32 only) *)
    EnvDiffusion: Single;                   (* [in/out] 0.0   , 1.0   , 1.0    , environment diffusion (win32/xbox) *)
    Room: Integer;                          (* [in/out] -10000, 0     , -1000  , room effect level (at mid frequencies) (win32/xbox) *)
    RoomHF: Integer;                        (* [in/out] -10000, 0     , -100   , relative room effect level at high frequencies (win32/xbox) *)
    RoomLF: Integer;                        (* [in/out] -10000, 0     , 0      , relative room effect level at low frequencies (win32 only) *)
    DecayTime: Single;                      (* [in/out] 0.1   , 20.0  , 1.49   , reverberation decay time at mid frequencies (win32/xbox) *)
    DecayHFRatio: Single;                   (* [in/out] 0.1   , 2.0   , 0.83   , high-frequency to mid-frequency decay time ratio (win32/xbox) *)
    DecayLFRatio: Single;                   (* [in/out] 0.1   , 2.0   , 1.0    , low-frequency to mid-frequency decay time ratio (win32 only) *)
    Reflections: Integer;                   (* [in/out] -10000, 1000  , -2602  , early reflections level relative to room effect (win32/xbox) *)
    ReflectionsDelay: Single;               (* [in/out] 0.0   , 0.3   , 0.007  , initial reflection delay time (win32/xbox) *)
    ReflectionsPan: array [0..2] of Single; (* [in/out]       ,       , [0,0,0], early reflections panning vector (win32 only) *)
    Reverb: Integer;                        (* [in/out] -10000, 2000  , 200    , late reverberation level relative to room effect (win32/xbox) *)
    ReverbDelay: Single;                    (* [in/out] 0.0   , 0.1   , 0.011  , late reverberation delay time relative to initial reflection (win32/xbox) *)
    ReverbPan: array [0..2] of Single;      (* [in/out]       ,       , [0,0,0], late reverberation panning vector (win32 only) *)
    EchoTime: Single;                       (* [in/out] .075  , 0.25  , 0.25   , echo time (win32 only) *)
    EchoDepth: Single;                      (* [in/out] 0.0   , 1.0   , 0.0    , echo depth (win32 only) *)
    ModulationTime: Single;                 (* [in/out] 0.04  , 4.0   , 0.25   , modulation time (win32 only) *)
    ModulationDepth: Single;                (* [in/out] 0.0   , 1.0   , 0.0    , modulation depth (win32 only) *)
    AirAbsorptionHF: Single;                (* [in/out] -100  , 0.0   , -5.0   , change in level per meter at high frequencies (win32 only) *)
    HFReference: Single;                    (* [in/out] 1000.0, 20000 , 5000.0 , reference high frequency (hz) (win32/xbox) *)
    LFReference: Single;                    (* [in/out] 20.0  , 1000.0, 250.0  , reference low frequency (hz) (win32 only) *)
    RoomRolloffFactor: Single;              (* [in/out] 0.0   , 10.0  , 0.0    , like FMOD_3D_Listener_SetRolloffFactor but for room effect (win32/xbox) *)
    Diffusion: Single;                      (* [in/out] 0.0   , 100.0 , 100.0  , Value that controls the echo density in the late reverberation decay. (xbox only) *)
    Density: Single;                        (* [in/out] 0.0   , 100.0 , 100.0  , Value that controls the modal density in the late reverberation decay (xbox only) *)
    Flags: Cardinal;                        (* [in/out] FMOD_REVERB_FLAGS - modifies the behavior of above properties (win32/ps2) *)
  end;


(*
[DEFINE] 
[
    [NAME]
    FMOD_REVERB_FLAGS

    [DESCRIPTION]
    Values for the Flags member of the FMOD_REVERB_PROPERTIES structure.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_REVERB_PROPERTIES
]
*)
const
  FMOD_REVERB_FLAGS_DECAYTIMESCALE        = $00000001; (* 'EnvSize' affects reverberation decay time *)
  FMOD_REVERB_FLAGS_REFLECTIONSSCALE      = $00000002; (* 'EnvSize' affects reflection level *)
  FMOD_REVERB_FLAGS_REFLECTIONSDELAYSCALE = $00000004; (* 'EnvSize' affects initial reflection delay time *)
  FMOD_REVERB_FLAGS_REVERBSCALE           = $00000008; (* 'EnvSize' affects reflections level *)
  FMOD_REVERB_FLAGS_REVERBDELAYSCALE      = $00000010; (* 'EnvSize' affects late reverberation delay time *)
  FMOD_REVERB_FLAGS_DECAYHFLIMIT          = $00000020; (* AirAbsorptionHF affects DecayHFRatio *)
  FMOD_REVERB_FLAGS_ECHOTIMESCALE         = $00000040; (* 'EnvSize' affects echo time *)
  FMOD_REVERB_FLAGS_MODULATIONTIMESCALE   = $00000080; (* 'EnvSize' affects modulation time *)
  FMOD_REVERB_FLAGS_DEFAULT               = (FMOD_REVERB_FLAGS_DECAYTIMESCALE or
                                                FMOD_REVERB_FLAGS_REFLECTIONSSCALE or
                                                FMOD_REVERB_FLAGS_REFLECTIONSDELAYSCALE or
                                                FMOD_REVERB_FLAGS_REVERBSCALE or
                                                FMOD_REVERB_FLAGS_REVERBDELAYSCALE or
                                                FMOD_REVERB_FLAGS_DECAYHFLIMIT);
(* [DEFINE_END] *)


(*
[STRUCTURE] 
[
    [DESCRIPTION]
    Structure defining the properties for a reverb source, related to a FMOD channel.

    For more indepth descriptions of the reverb properties under win32, please see the EAX3
    documentation at http://developer.creative.com/ under the 'downloads' section.
    If they do not have the EAX3 documentation, then most information can be attained from
    the EAX2 documentation, as EAX3 only adds some more parameters and functionality on top of 
    EAX2.

    Note the default reverb properties are the same as the FMOD_PRESET_GENERIC preset.
    Note that integer values that typically range from -10,000 to 1000 are represented in 
    decibels, and are of a logarithmic scale, not linear, wheras float values are typically linear.
    PORTABILITY: Each member has the platform it supports in braces ie (win32/xbox).  
    Some reverb parameters are only supported in win32 and some only on xbox. If all parameters are set then
    the reverb should product a similar effect on either platform.
    Linux and FMODCE do not support the reverb api.

    The numerical values listed below are the maximum, minimum and default values for each variable respectively.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_Channel_SetReverbProperties
    FMOD_Channel_GetReverbProperties
    FMOD_REVERB_CHANNELFLAGS
]
*)
type
  FMOD_REVERB_CHANNELPROPERTIES = record  (* MIN     MAX    DEFAULT *)
    Direct: Integer;                      (* -10000, 1000,  0,    direct path level (at low and mid frequencies) (win32/xbox) *)
    DirectHF: Integer;                    (* -10000, 0,     0,    relative direct path level at high frequencies (win32/xbox) *)
    Room: Integer;                        (* -10000, 1000,  0,    room effect level (at low and mid frequencies) (win32/xbox) *)
    RoomHF: Integer;                      (* -10000, 0,     0,    relative room effect level at high frequencies (win32/xbox) *)
    Obstruction: Integer;                 (* -10000, 0,     0,    main obstruction control (attenuation at high frequencies)  (win32/xbox) *)
    ObstructionLFRatio: Single;           (* 0.0,    1.0,   0.0,  obstruction low-frequency level re. main control (win32/xbox) *)
    Occlusion: Integer;                   (* -10000, 0,     0,    main occlusion control (attenuation at high frequencies) (win32/xbox) *)
    OcclusionLFRatio: Single;             (* 0.0,    1.0,   0.25, occlusion low-frequency level re. main control (win32/xbox) *)
    OcclusionRoomRatio: Single;           (* 0.0,    10.0,  1.5,  relative occlusion control for room effect (win32) *)
    OcclusionDirectRatio: Single;         (* 0.0,    10.0,  1.0,  relative occlusion control for direct path (win32) *)
    Exclusion: Integer;                   (* -10000, 0,     0,    main exlusion control (attenuation at high frequencies) (win32) *)
    ExclusionLFRatio: Single;             (* 0.0,    1.0,   1.0,  exclusion low-frequency level re. main control (win32) *)
    OutsideVolumeHF: Integer;             (* -10000, 0,     0,    outside sound cone level at high frequencies (win32) *)
    DopplerFactor: Single;                (* 0.0,    10.0,  0.0,  like DS3D flDopplerFactor but per source (win32) *)
    RolloffFactor: Single;                (* 0.0,    10.0,  0.0,  like DS3D flRolloffFactor but per source (win32) *)
    RoomRolloffFactor: Single;            (* 0.0,    10.0,  0.0,  like DS3D flRolloffFactor but for room effect (win32/xbox) *)
    AirAbsorptionFactor: Single;          (* 0.0,    10.0,  1.0,  multiplies AirAbsorptionHF member of FMOD_REVERB_PROPERTIES (win32) *)
    Flags: Integer;                       (* FMOD_REVERB_CHANNELFLAGS - modifies the behavior of properties (win32) *)
  end;


(*
[DEFINE] 
[
    [NAME] 
    FMOD_REVERB_CHANNELFLAGS

    [DESCRIPTION]
    Values for the Flags member of the FMOD_REVERB_CHANNELPROPERTIES structure.

    [REMARKS]
    For EAX4 support with multiple reverb environments, set FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0,
    FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1 or/and FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT2 in the flags member 
    of FMOD_REVERB_CHANNELPROPERTIES to specify which environment instance(s) to target.
    Only up to 2 environments to target can be specified at once. Specifying three will result in an error.
    If the sound card does not support EAX4, the environment flag is ignored.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_REVERB_CHANNELPROPERTIES
]
*)
const
  FMOD_REVERB_CHANNELFLAGS_DIRECTHFAUTO  = $00000001; (* Automatic setting of 'Direct'  due to distance from listener *)
  FMOD_REVERB_CHANNELFLAGS_ROOMAUTO      = $00000002; (* Automatic setting of 'Room'  due to distance from listener *)
  FMOD_REVERB_CHANNELFLAGS_ROOMHFAUTO    = $00000004; (* Automatic setting of 'RoomHF' due to distance from listener *)
  FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0  = $00000008; (* EAX4 only. Specify channel to target reverb instance 0. *)
  FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1  = $00000010; (* EAX4 only. Specify channel to target reverb instance 1. *)
  FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT2  = $00000020; (* EAX4 only. Specify channel to target reverb instance 2. *)
  FMOD_REVERB_CHANNELFLAGS_DEFAULT       = (FMOD_REVERB_CHANNELFLAGS_DIRECTHFAUTO or
                                                FMOD_REVERB_CHANNELFLAGS_ROOMAUTO or
                                                FMOD_REVERB_CHANNELFLAGS_ROOMHFAUTO or
                                                FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0);
(* [DEFINE_END] *)



(*
[STRUCTURE] 
[
    [DESCRIPTION]
    Settings for advanced features like configuring memory and cpu usage for the FMOD_CREATECOMPRESSEDSAMPLE feature.

    [REMARKS]
    maxMPEGcodecs / maxADPCMcodecs / maxXMAcodecs will determine the maximum cpu usage of playing realtime samples.  Use this to lower potential excess cpu usage and also control memory usage.<br>

    Memory will be allocated for codecs 'up front' (during System::init) if these values are specified as non zero.  If any are zero, it allocates memory for the codec whenever a file of the type in question is loaded.  So if maxMPEGcodecs is 0 for example, it will allocate memory for the mpeg codecs the first time an mp3 is loaded or an mp3 based .FSB file is loaded.

    Due to inefficient encoding techniques on certain .wav based ADPCM files, FMOD can can need an extra 29720 bytes per codec.  This means for lowest memory consumption.  Use FSB as it uses an optimal/small ADPCM block size.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_SetAdvancedSettings
    FMOD_System_GetAdvancedSettings
    FMOD_System_Init
    FMOD_MODE
]
*)
type
  FMOD_ADVANCEDSETTINGS = record
    cbsize: Integer;                (* Size of structure.  Use sizeof(FMOD_ADVANCEDSETTINGS) *)
    maxMPEGcodecs: Integer;         (* For use with FMOD_CREATECOMPRESSEDSAMPLE only.  Mpeg  codecs consume 29.076 bytes per instance and this number will determine how many mpeg channels can be played simultaneously.  Default = 16. *)
    maxADPCMcodecs: Integer;        (* For use with FMOD_CREATECOMPRESSEDSAMPLE only.  ADPCM codecs consume ?? bytes per instance and this number will determine how many ADPCM channels can be played simultaneously.  Default = 32. *)
    maxXMAcodecs: Integer;          (* For use with FMOD_CREATECOMPRESSEDSAMPLE only.  XMA   codecs consume ?? bytes per instance and this number will determine how many XMA channels can be played simultaneously.  Default = 32.  *)
    ASIONumChannels: Integer;       (* [in/out] *)
    ASIOChannelList: Pointer;       (* [in/out] *)
    max3DReverbDSPs: Integer;       (* [in/out] The max number of 3d reverb DSP's in the system. *)
    HRTFMinAngle: Single;           (* [in/out] For use with FMOD_INIT_SOFTWARE_HRTF.  The angle (0-360) of a 3D sound from the listener's forward vector at which the HRTF function begins to have an effect.  Default = 180.0. *)
    HRTFMaxAngle: Single;           (* [in/out] For use with FMOD_INIT_SOFTWARE_HRTF.  The angle (0-360) of a 3D sound from the listener's forward vector at which the HRTF function begins to have maximum effect.  Default = 360.0.  *)
    HRTFFreq: Single;               (* [in/out] For use with FMOD_INIT_SOFTWARE_HRTF.  The cutoff frequency of the HRTF's lowpass filter function when at maximum effect. (i.e. at HRTFMaxAngle).  Default = 4000.0. *)
  end;


(*
[ENUM]
[
    [DESCRIPTION]
    Special channel index values for FMOD functions.

    [REMARKS]
    To get 'all' of the channels, use System::getChannelGroup.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    System::playSound
    System::playDSP
    System::getChannel
]
*)
const
  FMOD_CHANNEL_FREE           = -1;      (* Value to play on any free channel. *)
  FMOD_CHANNEL_REUSE          = -2;      (* For a channel index, re-use the channel handle that was passed in. *)


(* =============================================================================================== *)
(* FMOD Ex - codec development header file. Copyright (c), Firelight Technologies Pty, Ltd. 2004.  *)
(*                                                                                                 *)
(* Use this header if you are wanting to develop your own file format plugin to use with           *)
(* FMOD's codec system.  With this header you can make your own fileformat plugin that FMOD        *)
(* can register and use.  See the documentation and examples on how to make a working plugin.      *)
(*                                                                                                 *)
(* =============================================================================================== *)

type
  FMOD_CODEC_STATE = ^tagFMOD_CODEC_STATE;


(*
    Codec callbacks
*)
(* type- Cannot use the type keyword here because the declaration of FMOD_CODEC_STATE requires tagFMOD_CODEC_STATE in the same type block *)
  FMOD_CODEC_OPENCALLBACK        = function (codec: FMOD_CODEC_STATE; usermode: Cardinal; userfrequency: Integer; userformat: FMOD_SOUND_FORMAT): FMOD_RESULT; stdcall;
  FMOD_CODEC_CLOSECALLBACK       = function (codec: FMOD_CODEC_STATE): FMOD_RESULT; stdcall;
  FMOD_CODEC_READCALLBACK        = function (codec: FMOD_CODEC_STATE; buffer: Pointer; sizebytes: Cardinal; var bytesread: Cardinal): FMOD_RESULT; stdcall;
  FMOD_CODEC_GETLENGTHCALLBACK   = function (codec: FMOD_CODEC_STATE; length: Cardinal; lengthtype: Integer): FMOD_RESULT; stdcall;
  FMOD_CODEC_SETPOSITIONCALLBACK = function (codec: FMOD_CODEC_STATE; subsound: Integer; pcm: Cardinal): FMOD_RESULT; stdcall;
  FMOD_CODEC_GETPOSITIONCALLBACK = function (codec: FMOD_CODEC_STATE; var subsound: Integer; var position: Cardinal; postype: Integer): FMOD_RESULT; stdcall;
  FMOD_CODEC_SOUNDCREATECALLBACK = function (codec: FMOD_CODEC_STATE; subsound: Integer; sound: FMOD_SOUND): FMOD_RESULT; stdcall;
  FMOD_CODEC_METADATACALLBACK    = function (codec: FMOD_CODEC_STATE; tagtype: FMOD_TAGTYPE; name: PChar; data: Pointer; datalen: Cardinal; datatype: FMOD_TAGDATATYPE; unique: Integer): FMOD_RESULT; stdcall;


(*
[STRUCTURE]
[
    [DESCRIPTION]
    When creating a codec, declare one of these and provide the relevant callbacks and name for FMOD to use when it opens and reads a file.

    [REMARKS]
    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
]
*)
(* type- Cannot use the type keyword here because the declaration of FMOD_CODEC_STATE requires tagFMOD_CODEC_STATE in the same type block *)
  FMOD_CODEC_DESCRIPTION = record
    name: PChar;                                    (* [in] Name of the codec. *)
    version: Cardinal;                              (* [in] Plugin writer's version number. *)
    timeunits: Integer;                             (* [in] When setposition codec is called, only these time formats will be passed to the codec. Use bitwise OR to accumulate different types. *)
    open: FMOD_CODEC_OPENCALLBACK;                  (* [in] Open callback for the codec for when FMOD tries to open a sound using this codec. *)
    close: FMOD_CODEC_CLOSECALLBACK;                (* [in] Close callback for the codec for when FMOD tries to close a sound using this codec.  *)
    read: FMOD_CODEC_READCALLBACK;                  (* [in] Read callback for the codec for when FMOD tries to read some data from the file to the destination format (specified in the open callback). *)
    getlength: FMOD_CODEC_GETLENGTHCALLBACK;        (* [in] Callback to return the length of the song in whatever format required when Sound::getLength is called. *)
    setposition: FMOD_CODEC_SETPOSITIONCALLBACK;    (* [in] Seek callback for the codec for when FMOD tries to seek within the file with Sound::setTime or Sound::setPosition. *)
    getposition: FMOD_CODEC_GETPOSITIONCALLBACK;    (* [in] Tell callback for the codec for when FMOD tries to get the current position within the with Sound::getTime or Sound::getPosition. *)
    soundcreate: FMOD_CODEC_SOUNDCREATECALLBACK;    (* [in] Sound creation callback for the codec when FMOD finishes creating the sound.  (So the codec can set more parameters for the related created sound, ie loop points/mode or 3D attributes etc). *)
  end;


(*
[STRUCTURE] 
[
    [DESCRIPTION]
    Set these values marked 'in' to tell fmod what sort of sound to create.
    The format, channels and frequency tell FMOD what sort of hardware buffer to create when you initialize your code.  So if you wrote an MP3 codec that decoded to stereo 16bit integer PCM, you would specify FMOD_SOUND_FORMAT_PCM16, and channels would be equal to 2.
    Members marked as 'out' are set by fmod.  Do not modify these.  Simply specify 0 for these values when declaring the structure, FMOD will fill in the values for you after creation with the correct function pointers.

    [REMARKS]
    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.
    
    An FMOD file might be from disk, memory or network, however the file may be opened by the user.
    
    'numsubsounds' should be 0 if the file is a normal single sound stream or sound.  Examples of this would be .WAV, .WMA, .MP3, .AIFF.
    'numsubsounds' should be 1+ if the file is a container format, and does not contain wav data itself.  Examples of these types would be CDDA (multiple CD tracks), FSB (contains multiple sounds), MIDI/MOD/S3M/XM/IT (contain instruments).
    The arrays of format, channel, frequency, length and blockalign should point to arrays of information based on how many subsounds are in the format.  If the number of subsounds is 0 then it should point to 1 of each attribute, the same as if the number of subsounds was 1.  If subsounds was 100 for example, each pointer should point to an array of 100 of each attribute.
    When a sound has 1 or more subsounds, you must play the individual sounds specified by first obtaining the subsound with FMOD_Sound_GetSubSound.
    
    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_SOUND_FORMAT
    FMOD_FILE_READCALLBACK
    FMOD_FILE_SEEKCALLBACK
    FMOD_CODEC_METADATACALLBACK
    FMOD_Sound_GetSubSound
    FMOD_Sound_GetNumSubSounds
]
*)
(* type- Cannot use the type keyword here because the declaration of FMOD_CODEC_STATE requires tagFMOD_CODEC_STATE in the same type block *)
  FMOD_CODEC_WAVEFORMAT = record
    name:  array [0..255] of Char;    (* [in] Name of sound. *)
    format: FMOD_SOUND_FORMAT;        (* [in] format for codec output, ie FMOD_SOUND_FORMAT_PCM8, FMOD_SOUND_FORMAT_PCM16.*)
    channels: Integer;                (* [in] number of channels used by codec, ie mono = 1, stereo = 2. *)
    frequency: Integer;               (* [in] default frequency in hz of the codec, ie 44100. *)
    lengthpcm: Cardinal;              (* [in] length in decompressed, PCM samples of the file, ie length in seconds * frequency. *)
    blockalign: Integer;              (* [in] blockalign in decompressed, PCM samples of the optimal decode chunk size for this format.  The codec read callback will be called in multiples of this value. *)
    loopstart: Integer;               (* [in] loopstart in decompressed, PCM samples of file. *)
    loopend: Integer;                 (* [in] loopend in decompressed, PCM samples of file. *)
    mode: Cardinal;                   (* [in] mode to determine whether the sound should by default load as looping, non looping, 2d or 3d. *)
    channelmask: Cardinal;            (* [in] Microsoft speaker channel mask, as defined for WAVEFORMATEXTENSIBLE and is found in ksmedia.h.  Leave at 0 to play in natural speaker order. *)
  end;
  PFMOD_CODEC_WAVEFORMAT = ^FMOD_CODEC_WAVEFORMAT;

(*
[STRUCTURE]
[
    [DESCRIPTION]
    Codec plugin structure that is passed into each callback.
    
    Set these numsubsounds and waveformat members when called in FMOD_CODEC_OPENCALLBACK to tell fmod what sort of sound to create.
    
    The format, channels and frequency tell FMOD what sort of hardware buffer to create when you initialize your code.  So if you wrote an MP3 codec that decoded to stereo 16bit integer PCM, you would specify FMOD_SOUND_FORMAT_PCM16, and channels would be equal to 2.

    [REMARKS]
    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.


    An FMOD file might be from disk, memory or network, however the file may be opened by the user.

    'numsubsounds' should be 0 if the file is a normal single sound stream or sound.  Examples of this would be .WAV, .WMA, .MP3, .AIFF.
    'numsubsounds' should be 1+ if the file is a container format, and does not contain wav data itself.  Examples of these types would be CDDA (multiple CD tracks), FSB (contains multiple sounds), MIDI/MOD/S3M/XM/IT (contain instruments).
    The arrays of format, channel, frequency, length and blockalign should point to arrays of information based on how many subsounds are in the format.  If the number of subsounds is 0 then it should point to 1 of each attribute, the same as if the number of subsounds was 1.  If subsounds was 100 for example, each pointer should point to an array of 100 of each attribute.
    When a sound has 1 or more subsounds, you must play the individual sounds specified by first obtaining the subsound with FMOD_Sound_GetSubSound.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_SOUND_FORMAT
    FMOD_FILE_READCALLBACK      
    FMOD_FILE_SEEKCALLBACK      
    FMOD_CODEC_METADATACALLBACK
    FMOD_Sound_GetSubSound
    FMOD_Sound_GetNumSubSounds
]
*)
(* type- Cannot use the type keyword here because the declaration of FMOD_CODEC_STATE requires tagFMOD_CODEC_STATE in the same type block *)
  tagFMOD_CODEC_STATE = record
    numsubsounds: Integer;                  (* [in] Number of 'subsounds' in this sound.  Anything other than 0 makes it a 'container' format (ie CDDA/FSB/MIDI/MOD/etc which contain 1 or more subsounds).  For most normal, single sound codec such as WAV/AIFF/MP3, this should be 0 as they are not a container for subsounds, they are the sound itself. *)
    waveformat: PFMOD_CODEC_WAVEFORMAT;     (* [in] Pointer to an array of format structures containing information about each sample.  The number of entries here must equal the number of subsounds defined in the subsound parameter. If numsubsounds = 0 then there should be 1 instance of this structure. *)
    plugindata: Pointer;                    (* [in/out] User created data the codec plugin writer wants to attach to this object. *)

    filehandle: Pointer;                    (* [out] - Do not modify.  This will return an internal FMOD file handle to use with the callbacks provided.  *)
    filesize: Cardinal;                     (* [out] - Do not modify.  This will return an internal FMOD file handle to use with the callbacks provided.  *)
    fileread: FMOD_FILE_READCALLBACK;       (* [out] - Do not modify.  This will return a callable FMOD file function to use from codec. *)
    fileseek: FMOD_FILE_SEEKCALLBACK;       (* [out] - Do not modify.  This will return a callable FMOD file function to use from codec.  *)
    metadata: FMOD_CODEC_METADATACALLBACK;  (* [out] - Do not modify.  This will return a callable FMOD metadata function to use from codec.  *)
  end;


(* ========================================================================================== *)
(* FMOD Ex - DSP header file. Copyright (c), Firelight Technologies Pty, Ltd. 2004-2005.      *)
(*                                                                                            *)
(* Use this header if you are interested in delving deeper into the FMOD software mixing /    *)
(* DSP engine.  In this header you can find parameter structures for FMOD system reigstered   *)
(* DSP effects and generators.                                                                *)
(* Also use this header if you are wanting to develop your own DSP plugin to use with FMOD's  *)
(* dsp system.  With this header you can make your own DSP plugin that FMOD can               *)
(* register and use.  See the documentation and examples on how to make a working plugin.     *)
(*                                                                                            *)
(* ========================================================================================== *)

(*
[STRUCTURE]
[
    [DESCRIPTION]
    DSP plugin structure that is passed into each callback.

    [REMARKS]
    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable

    [SEE_ALSO]
    FMOD_DSP_DESCRIPTION
]
*)
  tagFMOD_DSP_STATE = record
    instance: FMOD_DSP;     (* [out] Handle to the DSP hand the user created.  Not to be modified.  C++ users cast to FMOD::DSP to use.  *)
    plugindata: Pointer;    (* [in] Plugin writer created data the output author wants to attach to this object. *)
  end;

type
  FMOD_DSP_STATE = ^tagFMOD_DSP_STATE;

(*
    DSP callbacks
*)
(* type- Cannot use the type keyword here because the declaration of FMOD_CODEC_STATE requires tagFMOD_CODEC_STATE in the same type block *)
  FMOD_DSP_CREATECALLBACK       = function (dsp: FMOD_DSP_STATE): FMOD_RESULT; stdcall;
  FMOD_DSP_RELEASECALLBACK      = function (dsp: FMOD_DSP_STATE): FMOD_RESULT; stdcall;
  FMOD_DSP_RESETCALLBACK        = function (dsp: FMOD_DSP_STATE): FMOD_RESULT; stdcall;
  FMOD_DSP_READCALLBACK         = function (dsp: FMOD_DSP_STATE; var inbuffer: Single; var outbuffer: Single; length: Cardinal; inchannels: Integer; outchannels: Integer): FMOD_RESULT; stdcall;
  FMOD_DSP_SETPOSITIONCALLBACK  = function (dsp: FMOD_DSP_STATE; position: Cardinal): FMOD_RESULT; stdcall;
  FMOD_DSP_SETPARAMCALLBACK     = function (dsp: FMOD_DSP_STATE; index: Integer; value: Single): FMOD_RESULT; stdcall;
  FMOD_DSP_GETPARAMCALLBACK     = function (dsp: FMOD_DSP_STATE; index: Integer; var value: Single; valuestr: PString): FMOD_RESULT; stdcall;
  FMOD_DSP_DIALOGCALLBACK       = function (dsp: FMOD_DSP_STATE; hwnd: Pointer; show: Integer): FMOD_RESULT; stdcall;

(*
[ENUM]
[
    [DESCRIPTION]   
    These definitions can be used for creating FMOD defined special effects or DSP units.

    [REMARKS]
    To get them to be active, first create the unit, then add it somewhere into the DSP network, either at the front of the network near the soundcard unit to affect the global output (by using FMOD_System_GetDSPHead, or on a single channel (using FMOD_Channel_GetDSPHead).

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_CreateDSPByType
]
*)
type
  FMOD_DSP_TYPE =
  (
    FMOD_DSP_TYPE_UNKNOWN,            (* This unit was created via a non FMOD plugin so has an unknown purpose. *)
    FMOD_DSP_TYPE_MIXER,              (* This unit does nothing but take inputs and mix them together then feed the result to the soundcard unit *)
    FMOD_DSP_TYPE_OSCILLATOR,         (* This unit generates sine/square/saw/triangle or noise tones. *)
    FMOD_DSP_TYPE_LOWPASS,            (* This unit filters sound using a high quality, resonant lowpass filter algorithm but consumes more CPU time. *)
    FMOD_DSP_TYPE_ITLOWPASS,          (* This unit filters sound using a resonant lowpass filter algorithm that is used in Impulse Tracker. *)
    FMOD_DSP_TYPE_HIGHPASS,           (* This unit filters sound using a resonant highpass filter algorithm. *)
    FMOD_DSP_TYPE_ECHO,               (* This unit produces an echo on the sound and fades out at the desired rate. *)
    FMOD_DSP_TYPE_FLANGE,             (* This unit produces a flange effect on the sound. *)
    FMOD_DSP_TYPE_DISTORTION,         (* This unit distorts the sound. *)
    FMOD_DSP_TYPE_NORMALIZE,          (* This unit normalizes or amplifies the sound to a certain level. *)
    FMOD_DSP_TYPE_PARAMEQ,            (* This unit attenuates or amplifies a selected frequency range. *)
    FMOD_DSP_TYPE_PITCHSHIFT,         (* This unit bends the pitch of a sound without changing the speed of playback. *)
    FMOD_DSP_TYPE_CHORUS,             (* This unit produces a chorus effect on the sound. *)
    FMOD_DSP_TYPE_REVERB,             (* This unit produces a reverb effect on the sound. *)
    FMOD_DSP_TYPE_VSTPLUGIN,          (* This unit allows the use of Steinberg VST plugins *)
    FMOD_DSP_TYPE_WINAMPPLUGIN,       (* This unit allows the use of Nullsoft Winamp plugins *)
    FMOD_DSP_TYPE_ITECHO,             (* This unit produces an echo on the sound and fades out at the desired rate as is used in Impulse Tracker. *)
    FMOD_DSP_TYPE_COMPRESSOR,         (* This unit implements dynamic compression (linked multichannel, wideband) *)
    FMOD_DSP_TYPE_SFXREVERB,          (* This unit implements SFX reverb *)
    FMOD_DSP_TYPE_LOWPASS_SIMPLE      (* This unit filters sound using a simple lowpass with no resonance, but has flexible cutoff and is fast. *)
  );


(*
[STRUCTURE] 
[
    [DESCRIPTION]

    [REMARKS]
    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.
    
    The step parameter tells the gui or application that the parameter has a certain granularity.
    For example in the example of cutoff frequency with a range from 100.0 to 22050.0 you might only want the selection to be in 10hz increments.  For this you would simply use 10.0 as the step value.
    For a boolean, you can use min = 0.0, max = 1.0, step = 1.0.  This way the only possible values are 0.0 and 1.0.
    Some applications may detect min = 0.0, max = 1.0, step = 1.0 and replace a graphical slider bar with a checkbox instead.
    A step value of 1.0 would simulate integer values only.
    A step value of 0.0 would mean the full floating point range is accessable.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]    
    FMOD_System_CreateDSP
    FMOD_DSP_SetParameter
]
*)
type
  FMOD_DSP_PARAMETERDESC = record
    min: Single;                     (* [in] Minimum value of the parameter (ie 100.0). *)
    max: Single;                     (* [in] Maximum value of the parameter (ie 22050.0). *)
    defaultval: Single;              (* [in] Default value of parameter. *)
    name:  array [0..15] of Char;    (* [in] Name of the parameter to be displayed (ie "Cutoff frequency"). *)
    plabel:  array [0..15] of Char;   (* [in] Short string to be put next to value to denote the unit type (ie "hz"). *)
    description: PChar;              (* [in] Description of the parameter to be displayed as a help item / tooltip for this parameter. *)
  end;
  PFMOD_DSP_PARAMETERDESC = ^FMOD_DSP_PARAMETERDESC;


(*
[STRUCTURE]
[
    [DESCRIPTION]
    Strcture to define the parameters for a DSP unit.

    [REMARKS]
    Members marked with [in] mean the variable can be written to.  The user can set the value.
    Members marked with [out] mean the variable is modified by FMOD and is for reading purposes only.  Do not change this value.

    There are 2 different ways to change a parameter in this architecture.
    One is to use FMOD_DSP_SetParameter / FMOD_DSP_GetParameter.  This is platform independant and is dynamic, so new unknown plugins can have their parameters enumerated and used.
    The other is to use FMOD_DSP_ShowConfigDialog.  This is platform specific and requires a GUI, and will display a dialog box to configure the plugin.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FMOD_System_CreateDSP
    FMOD_System_GetDSP
]
*)
type
  FMOD_DSP_DESCRIPTION = record
    name: array [0..31] of Char;                    (* [in] Name of the unit to be displayed in the network. *)
    version: Cardinal;                              (* [in] Plugin writer's version number. *)
    channels: Integer;                              (* [in] Number of channels.  Use 0 to process whatever number of channels is currently in the network.  >0 would be mostly used if the unit is a fixed format generator and not a filter. *)
    create: FMOD_DSP_CREATECALLBACK;                (* [in] Create callback.  This is called when DSP unit is created.  Can be null. *)
    release: FMOD_DSP_RELEASECALLBACK;              (* [in] Release callback.  This is called just before the unit is freed so the user can do any cleanup needed for the unit.  Can be null. *)
    reset: FMOD_DSP_RESETCALLBACK;                  (* [in] Reset callback.  This is called by the user to reset any history buffers that may need resetting for a filter, when it is to be used or re-used for the first time to its initial clean state.  Use to avoid clicks or artifacts. *)
    read: FMOD_DSP_READCALLBACK;                    (* [in] Read callback.  Processing is done here.  Can be null. *)
    setposition: FMOD_DSP_SETPOSITIONCALLBACK;      (* [in] Set position callback.  This is called if the unit wants to update its position info but not process data, or reset a cursor position internally if it is reading data from a certain source.  Can be null. *)
    numparameters: Integer;                         (* [in] Number of parameters used in this filter.  The user finds this with FMOD_DSP_GetNumParameters *)
    paramdesc: PFMOD_DSP_PARAMETERDESC;             (* [in] Variable number of parameter structures. *)
    setparameter: FMOD_DSP_SETPARAMCALLBACK;        (* [in] This is called when the user calls FMOD_DSP_SetParameter.  Can be null. *)
    getparameter: FMOD_DSP_GETPARAMCALLBACK;        (* [in] This is called when the user calls FMOD_DSP_GetParameter.  Can be null. *)
    config: FMOD_DSP_DIALOGCALLBACK;                (* [in] This is called when the user calls FMOD_DSP_ShowConfigDialog.  Can be used to display a dialog to configure the filter.  Can be null. *)
    configwidth: Integer;                           (* [in] Width of config dialog graphic if there is one.  0 otherwise. *)
    configheight: Integer;                          (* [in] Height of config dialog graphic if there is one.  0 otherwise. *)
  end;


(*
    ==============================================================================================================

    FMOD built in effect parameters.  
    Use FMOD_DSP_SetParameter with these enums for the 'index' parameter.

    ==============================================================================================================
*)

(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_OSCILLATOR filter.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_OSCILLATOR =
  (
    FMOD_DSP_OSCILLATOR_TYPE,   (* Waveform type.  0 = sine.  1 = square. 2 = sawup. 3 = sawdown. 4 = triangle. 5 = noise.  *)
    FMOD_DSP_OSCILLATOR_RATE    (* Frequency of the sinewave in hz.  1.0 to 22000.0.  Default = 220.0. *)
  );


(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_LOWPASS filter.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_LOWPASS =
  (
    FMOD_DSP_LOWPASS_CUTOFF,    (* Lowpass cutoff frequency in hz.   1.0 to 22000.0.  Default = 5000.0. *)
    FMOD_DSP_LOWPASS_RESONANCE  (* Lowpass resonance Q value. 1.0 to 10.0.  Default = 1.0. *)
  );


(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_LOWPASS2 filter.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_LOWPASS2 =
  (
    FMOD_DSP_LOWPASS2_CUTOFF,    (* Lowpass cutoff frequency in hz.  1.0 to 22000.0.  Default = 5000.0/ *)
    FMOD_DSP_LOWPASS2_RESONANCE  (* Lowpass resonance Q value.  0.0 to 127.0.  Default = 1.0. *)
  );


(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_HIGHPASS filter.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_HIGHPASS =
  (
    FMOD_DSP_HIGHPASS_CUTOFF,    (* Highpass cutoff frequency in hz.  10.0 to output 22000.0.  Default = 5000.0. *)
    FMOD_DSP_HIGHPASS_RESONANCE  (* Highpass resonance Q value.  1.0 to 10.0.  Default = 1.0. *)
  );


(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_ECHO filter.

    [REMARKS]
    Note.  Every time the delay is changed, the plugin re-allocates the echo buffer.  This means the echo will dissapear at that time while it refills its new buffer.
    Larger echo delays result in larger amounts of memory allocated.
    
    '<i>maxchannels</i>' also dictates the amount of memory allocated.  By default, the maxchannels value is 0.  If FMOD is set to stereo, the echo unit will allocate enough memory for 2 channels.  If it is 5.1, it will allocate enough memory for a 6 channel echo, etc.
    If the echo effect is only ever applied to the global mix (ie it was added with FMOD_System_AddDSP), then 0 is the value to set as it will be enough to handle all speaker modes.
    When the echo is added to a channel (ie FMOD_Channel_AddDSP) then the channel count that comes in could be anything from 1 to 8 possibly.  It is only in this case where you might want to increase the channel count above the output's channel count.
    If a channel echo is set to a lower number than the sound's channel count that is coming in, it will not echo the sound.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_ECHO =
  (
    FMOD_DSP_ECHO_DELAY,       (* Echo delay in ms.  10  to 5000.  Default = 500. *)
    FMOD_DSP_ECHO_DECAYRATIO,  (* Echo decay per delay.  0 to 1.  1.0 = No decay, 0.0 = total decay.  Default = 0.5. *)
    FMOD_DSP_ECHO_FEEDBACK,    (* 0 = Unit is a simple 1 line delay.  1 = Unit echos what has already been processed by echo. Default = 1. *)
    FMOD_DSP_ECHO_MAXCHANNELS  (* Maximum channels supported.  0 to 16.  0 = same as fmod's default output polyphony, 1 = mono, 2 = stereo etc.  See remarks for more.  Default = 0.  It is suggested to leave at 0! *)
  );


(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_FLANGE filter.

    [REMARKS]
    Flange is an effect where the signal is played twice at the same time, and one copy slides back and forth creating a whooshing or flanging effect.
    As there are 2 copies of the same signal, by default each signal is given 50% mix, so that the total is not louder than the original unaffected signal.
    
    Flange depth is a percentage of a 10ms shift from the original signal.  Anything above 10ms is not considered flange because to the ear it begins to 'echo' so 10ms is the highest value possible.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_FLANGE =
  (
    FMOD_DSP_FLANGE_DRYMIX,      (* Volume of original signal to pass to output.  0.0 to 1.0. Default = 0.45. *)
    FMOD_DSP_FLANGE_WETMIX,      (* Volume of flange signal to pass to output.  0.0 to 1.0. Default = 0.55. *)
    FMOD_DSP_FLANGE_DEPTH,       (* Flange depth.  0.01 to 1.0.  Default = 1.0. *)
    FMOD_DSP_FLANGE_RATE         (* Flange speed in hz.  0.0 to 20.0.  Default = 0.1. *)
  );


(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_DISTORTION filter.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_DISTORTION =
  (
    FMOD_DSP_DISTORTION_LEVEL    (* Distortion value.  0.0 to 1.0.  Default = 0.5. *)
  );


(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_NORMALIZE filter.

    [REMARKS]
    Normalize amplifies the sound based on the maximum peaks within the signal.
    For example if the maximum peaks in the signal were 50% of the bandwidth, it would scale the whole sound by 2.
    The lower threshold value makes the normalizer ignores peaks below a certain point, to avoid over-amplification if a loud signal suddenly came in, and also to avoid amplifying to maximum things like background hiss.
    
    Because FMOD is a realtime audio processor, it doesn't have the luxury of knowing the peak for the whole sound (ie it can't see into the future), so it has to process data as it comes in.
    To avoid very sudden changes in volume level based on small samples of new data, fmod fades towards the desired amplification which makes for smooth gain control.  The fadetime parameter can control this.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_NORMALIZE =
  (
    FMOD_DSP_NORMALIZE_FADETIME,    (* Time to ramp the silence to full in ms.  0.0 to 20000.0. Default = 5000.0. *)
    FMOD_DSP_NORMALIZE_THRESHHOLD,  (* Lower volume range threshold to ignore.  0.0 to 1.0.  Default = 0.1.  Raise higher to stop amplification of very quiet signals. *)
    FMOD_DSP_NORMALIZE_MAXAMP       (* Maximum amplification allowed.  1.0 to 100000.0.  Default = 20.0.  1.0 = no amplifaction, higher values allow more boost. *)
  );


(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_PARAMEQ filter.

    [REMARKS]
    Parametric EQ is a bandpass filter that attenuates or amplifies a selected frequency and its neighbouring frequencies.
    
    To create a multi-band EQ create multiple FMOD_DSP_TYPE_PARAMEQ units and set each unit to different frequencies, for example 1000hz, 2000hz, 4000hz, 8000hz, 16000hz with a range of 1 octave each.
    
    When a frequency has its gain set to 1.0, the sound will be unaffected and represents the original signal exactly.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_PARAMEQ =
  (
    FMOD_DSP_PARAMEQ_CENTER,     (* Frequency center.  20.0 to 22000.0.  Default = 8000.0. *)
    FMOD_DSP_PARAMEQ_BANDWIDTH,  (* Octave range around the center frequency to filter.  0.2 to 5.0.  Default = 1.0. *)
    FMOD_DSP_PARAMEQ_GAIN        (* Frequency Gain.  0.05 to 3.0.  Default = 1.0.  *)
  );



(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_PITCHSHIFT filter.

    [REMARKS]
    This pitch shifting unit can be used to change the pitch of a sound without speeding it up or slowing it down.
    It can also be used for time stretching or scaling, for example if the pitch was doubled, and the frequency of the sound was halved, the pitch of the sound would sound correct but it would be twice as slow.
    
    <b>Warning!</b> This filter is very computationally expensive!  Similar to a vocoder, it requires several overlapping FFT and IFFT's to produce smooth output, and can require around 440mhz for 1 stereo 48khz signal using the default settings.
    Reducing the signal to mono will half the cpu usage, as will the overlap count.
    Reducing this will lower audio quality, but what settings to use are largely dependant on the sound being played.  A noisy polyphonic signal will need higher overlap and fft size compared to a speaking voice for example.
    
    This pitch shifter is based on the pitch shifter code at http://www.dspdimension.com, written by Stephan M. Bernsee.
    The original code is COPYRIGHT 1999-2003 Stephan M. Bernsee <smb@dspdimension.com>.
    
    '<i>maxchannels</i>' dictates the amount of memory allocated.  By default, the maxchannels value is 0.  If FMOD is set to stereo, the pitch shift unit will allocate enough memory for 2 channels.  If it is 5.1, it will allocate enough memory for a 6 channel pitch shift, etc.
    If the pitch shift effect is only ever applied to the global mix (ie it was added with FMOD_System_AddDSP), then 0 is the value to set as it will be enough to handle all speaker modes.
    When the pitch shift is added to a channel (ie FMOD_Channel_AddDSP) then the channel count that comes in could be anything from 1 to 8 possibly.  It is only in this case where you might want to increase the channel count above the output's channel count.
    If a channel pitch shift is set to a lower number than the sound's channel count that is coming in, it will not pitch shift the sound.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_PITCHSHIFT =
  (
    FMOD_DSP_PITCHSHIFT_PITCH,       (* Pitch value.  0.5 to 2.0.  Default = 1.0. 0.5 = one octave down, 2.0 = one octave up.  1.0 does not change the pitch. *)
    FMOD_DSP_PITCHSHIFT_FFTSIZE,     (* FFT window size.  256, 512, 1024, 2048, 4096.  Default = 1024.  Increase this to reduce 'smearing'.  This effect is a warbling sound similar to when an mp3 is encoded at very low bitrates. *)
    FMOD_DSP_PITCHSHIFT_OVERLAP,     (* Window overlap.  1 to 32.  Default = 4.  Increase this to reduce 'tremolo' effect.  Increasing it by a factor of 2 doubles the CPU usage. *)
    FMOD_DSP_PITCHSHIFT_MAXCHANNELS  (* Maximum channels supported.  0 to 16.  0 = same as fmod's default output polyphony, 1 = mono, 2 = stereo etc.  See remarks for more.  Default = 0.  It is suggested to leave at 0! *)
  );



(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_CHORUS filter.

    [REMARKS]
    Chrous is an effect where the sound is more 'spacious' due to 1 to 3 versions of the sound being played along side the original signal but with the pitch of each copy modulating on a sine wave.
    This is a highly configurable chorus unit.  It supports 3 taps, small and large delay times and also feedback.
    This unit also could be used to do a simple echo, or a flange effect. 

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_CHORUS =
  (
    FMOD_DSP_CHORUS_DRYMIX,   (* Volume of original signal to pass to output.  0.0 to 1.0. Default = 0.5. *)
    FMOD_DSP_CHORUS_WETMIX1,  (* Volume of 1st chorus tap.  0.0 to 1.0.  Default = 0.5. *)
    FMOD_DSP_CHORUS_WETMIX2,  (* Volume of 2nd chorus tap. This tap is 90 degrees out of phase of the first tap.  0.0 to 1.0.  Default = 0.5. *)
    FMOD_DSP_CHORUS_WETMIX3,  (* Volume of 3rd chorus tap. This tap is 90 degrees out of phase of the second tap.  0.0 to 1.0.  Default = 0.5. *)
    FMOD_DSP_CHORUS_DELAY,    (* Chorus delay in ms.  0.1 to 100.0.  Default = 40.0 ms. *)
    FMOD_DSP_CHORUS_RATE,     (* Chorus modulation rate in hz.  0.0 to 20.0.  Default = 0.8 hz. *)
    FMOD_DSP_CHORUS_DEPTH,    (* Chorus modulation depth.  0.0 to 1.0.  Default = 0.03. *)
    FMOD_DSP_CHORUS_FEEDBACK  (* Chorus feedback.  Controls how much of the wet signal gets fed back into the chorus buffer.  0.0 to 1.0.  Default = 0.0. *)
  );

(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_REVERB filter.

    [REMARKS]
    Based on freeverb by Jezar at Dreampoint - http://www.dreampoint.co.uk.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_REVERB =
  (
    FMOD_DSP_REVERB_ROOMSIZE, (* Roomsize. 0.0 to 1.0.  Default = 0.5 *)
    FMOD_DSP_REVERB_DAMP,     (* Damp.     0.0 to 1.0.  Default = 0.5 *)
    FMOD_DSP_REVERB_WETMIX,   (* Wet mix.  0.0 to 1.0.  Default = 0.33 *)
    FMOD_DSP_REVERB_DRYMIX,   (* Dry mix.  0.0 to 1.0.  Default = 0.66 *)
    FMOD_DSP_REVERB_WIDTH,    (* Width.    0.0 to 1.0.  Default = 1.0 *)
    FMOD_DSP_REVERB_MODE      (* Mode.     0 (normal), 1 (freeze).  Default = 0 *)
  );


(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_ITECHO filter.
    This is effectively a software based echo filter that emulates the DirectX DMO echo effect.  Impulse tracker files can support this, and FMOD will produce the effect on ANY platform, not just those that support DirectX effects!

    [REMARKS]
    Note.  Every time the delay is changed, the plugin re-allocates the echo buffer.  This means the echo will dissapear at that time while it refills its new buffer.
    Larger echo delays result in larger amounts of memory allocated.
    
    As this is a stereo filter made mainly for IT playback, it is targeted for stereo signals.
    With mono signals only the FMOD_DSP_ITECHO_LEFTDELAY is used.
    For multichannel signals (>2) there will be no echo on those channels.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]      
    FMOD_DSP_SetParameter
    FMOD_DSP_GetParameter
    FMOD_DSP_TYPE
    FMOD_System_AddDSP
]
*)
type
  FMOD_DSP_ITECHO =
  (
    FMOD_DSP_ITECHO_WETDRYMIX,      (* Ratio of wet (processed) signal to dry (unprocessed) signal. Must be in the range from 0.0 through 100.0 (all wet). The default value is 50. *)
    FMOD_DSP_ITECHO_FEEDBACK,       (* Percentage of output fed back into input, in the range from 0.0 through 100.0. The default value is 50. *)
    FMOD_DSP_ITECHO_LEFTDELAY,      (* Delay for left channel, in milliseconds, in the range from 1.0 through 2000.0. The default value is 500 ms. *)
    FMOD_DSP_ITECHO_RIGHTDELAY,     (* Delay for right channel, in milliseconds, in the range from 1.0 through 2000.0. The default value is 500 ms. *)
    FMOD_DSP_ITECHO_PANDELAY        (* Value that specifies whether to swap left and right delays with each successive echo. The default value is zero, meaning no swap. Possible values are defined as 0.0 (equivalent to FALSE) and 1.0 (equivalent to TRUE).  CURRENTLY NOT SUPPORTED. *)
  );



(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_COMPRESSOR unit.
    This is a simple linked multichannel software limiter that is uniform across the whole spectrum.<br>

    [REMARKS]
    The limiter is not guaranteed to catch every peak above the threshold level,
    because it cannot apply gain reduction instantaneously - the time delay is
    determined by the attack time. However setting the attack time too short will
    distort the sound, so it is a compromise. High level peaks can be avoided by
    using a short attack time - but not too short, and setting the threshold a few
    decibels below the critical level.
    <br>

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    [SEE_ALSO]      
    DSP::SetParameter
    DSP::GetParameter
    FMOD_DSP_TYPE
    System::addDSP
]
*)
type
  FMOD_DSP_COMPRESSOR =
  (
    FMOD_DSP_COMPRESSOR_THRESHOLD,  (* Threshold level (dB) in the range from -60 through 0. The default value is 0. *) 
    FMOD_DSP_COMPRESSOR_ATTACK,     (* Gain reduction attack time (milliseconds), in the range from 10 through 200. The default value is 50. *)
    FMOD_DSP_COMPRESSOR_RELEASE,    (* Gain reduction release time (milliseconds), in the range from 20 through 1000. The default value is 50. *)
    FMOD_DSP_COMPRESSOR_GAINMAKEUP  (* Make-up gain (dB) applied after limiting, in the range from 0 through 30. The default value is 0. *)
  );

(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_SFXREVERB unit.<br>
    
    [REMARKS]
    This is a high quality I3DL2 based reverb which improves greatly on FMOD_DSP_REVERB.<br>
    On top of the I3DL2 property set, "Dry Level" is also included to allow the dry mix to be changed.<br>
    <br>
    Currently FMOD_DSP_SFXREVERB_REFLECTIONSLEVEL, FMOD_DSP_SFXREVERB_REFLECTIONSDELAY and FMOD_DSP_SFXREVERB_REVERBDELAY are not enabled but will come in future versions.<br>
    <br>
    These properties can be set with presets in FMOD_REVERB_PRESETS.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    [SEE_ALSO]      
    DSP::SetParameter
    DSP::GetParameter
    FMOD_DSP_TYPE
    System::addDSP
    FMOD_REVERB_PRESETS
]
*)
type
  FMOD_DSP_SFXREVERB =
  (
    FMOD_DSP_SFXREVERB_DRYLEVEL,            (* Dry Level      : Mix level of dry signal in output in mB.  Ranges from -10000.0 to 0.0.  Default is 0. *)
    FMOD_DSP_SFXREVERB_ROOM,                (* Room           : Room effect level at low frequencies in mB.  Ranges from -10000.0 to 0.0.  Default is 0.0. *)
    FMOD_DSP_SFXREVERB_ROOMHF,              (* Room HF        : Room effect high-frequency level re. low frequency level in mB.  Ranges from -10000.0 to 0.0.  Default is 0.0. *)
    FMOD_DSP_SFXREVERB_ROOMROLLOFFFACTOR,   (* Room Rolloff   : Like DS3D flRolloffFactor but for room effect.  Ranges from 0.0 to 10.0. Default is 10.0 *)
    FMOD_DSP_SFXREVERB_DECAYTIME,           (* Decay Time     : Reverberation decay time at low-frequencies in seconds.  Ranges from 0.1 to 20.0. Default is 1.0. *)
    FMOD_DSP_SFXREVERB_DECAYHFRATIO,        (* Decay HF Ratio : High-frequency to low-frequency decay time ratio.  Ranges from 0.1 to 2.0. Default is 0.5. *)
    FMOD_DSP_SFXREVERB_REFLECTIONSLEVEL,    (* Reflections    : Early reflections level relative to room effect in mB.  Ranges from -10000.0 to 1000.0.  Default is -10000.0. *)
    FMOD_DSP_SFXREVERB_REFLECTIONSDELAY,    (* Reflect Delay  : Delay time of first reflection in seconds.  Ranges from 0.0 to 0.3.  Default is 0.02. *)
    FMOD_DSP_SFXREVERB_REVERBLEVEL,         (* Reverb         : Late reverberation level relative to room effect in mB.  Ranges from -10000.0 to 2000.0.  Default is 0.0. *)
    FMOD_DSP_SFXREVERB_REVERBDELAY,         (* Reverb Delay   : Late reverberation delay time relative to first reflection in seconds.  Ranges from 0.0 to 0.1.  Default is 0.04. *)
    FMOD_DSP_SFXREVERB_DIFFUSION,           (* Diffusion      : Reverberation diffusion (echo density) in percent.  Ranges from 0.0 to 100.0.  Default is 100.0. *)
    FMOD_DSP_SFXREVERB_DENSITY,             (* Density        : Reverberation density (modal density) in percent.  Ranges from 0.0 to 100.0.  Default is 100.0. *)
    FMOD_DSP_SFXREVERB_HFREFERENCE          (* HF Reference   : Reference high frequency in Hz.  Ranges from 20.0 to 20000.0. Default is 5000.0. *)
  );

(*
[ENUM]
[  
    [DESCRIPTION]   
    Parameter types for the FMOD_DSP_TYPE_LOWPASS_SIMPLE filter.<br>
    This is a very simple low pass filter, based on two single-pole RC time-constant modules.
    The emphasis is on speed rather than accuracy, so this should not be used for task requiring critical filtering.<br> 

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

    [SEE_ALSO]      
    DSP::setParameter
    DSP::getParameter
    FMOD_DSP_TYPE
]
*)
type
  FMOD_DSP_LOWPASS_SIMPLE =
  (
    FMOD_DSP_LOWPASS_SIMPLE_CUTOFF     (* Lowpass cutoff frequency in hz.  10.0 to 22000.0.  Default = 5000.0 *)
  );




implementation

end.