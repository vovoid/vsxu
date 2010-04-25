(* ============================================================================================= *)
(* FMOD Ex - Main Pascal header file. Copyright (c), Firelight Technologies Pty, Ltd. 2004-2008. *)
(*                                                                                               *)
(* This header is for statically linking to the FMOD library.                                    *)
(*                                                                                               *)
(* ============================================================================================= *)

unit fmod;

{$I fmod.inc}

interface

uses
  fmodtypes;

(* ========================================================================================== *)
(* FUNCTION PROTOTYPES                                                                        *)
(* ========================================================================================== *)

(*
    FMOD System memory functions (optional).
*)

function FMOD_Memory_Initialize           (poolmem: Pointer; poollen: Integer; useralloc: FMOD_MEMORY_ALLOCCALLBACK; userrealloc: FMOD_MEMORY_REALLOCCALLBACK; userfree: FMOD_MEMORY_FREECALLBACK): FMOD_RESULT; stdcall;
function FMOD_Memory_GetStats             (var currentalloced: Integer; var maxalloced: Integer): FMOD_RESULT; stdcall;

(*
    FMOD System factory functions.  Use this to create an FMOD System Instance.  below you will see FMOD_System_Init/Close to get started.
*)

function FMOD_System_Create               (var system: FMOD_SYSTEM): FMOD_RESULT; stdcall; 
function FMOD_System_Release              (system: FMOD_SYSTEM): FMOD_RESULT; stdcall; 

(*
    'System' API
*)

(*
     Pre-init functions.                   
*)

function FMOD_System_SetOutput              (system: FMOD_SYSTEM; output: FMOD_OUTPUTTYPE): FMOD_RESULT; stdcall;
function FMOD_System_GetOutput              (system: FMOD_SYSTEM; var output: FMOD_OUTPUTTYPE): FMOD_RESULT; stdcall;
function FMOD_System_GetNumDrivers          (system: FMOD_SYSTEM; var numdrivers: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetDriverName          (system: FMOD_SYSTEM; id: Integer; name: PChar; namelen: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetDriverCaps          (system: FMOD_SYSTEM; id: Integer; var caps: Cardinal; var minfrequency, maxfrequency: Integer; var controlpanelspeakermode: FMOD_SPEAKERMODE): FMOD_RESULT; stdcall;
function FMOD_System_SetDriver              (system: FMOD_SYSTEM; driver: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetDriver              (system: FMOD_SYSTEM; var driver: Integer): FMOD_RESULT; stdcall;
function FMOD_System_SetHardwareChannels    (system: FMOD_SYSTEM; min2d: Integer; max2d: Integer; min3d: Integer; max3d: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetHardwareChannels    (system: FMOD_SYSTEM; var numhw2d: Integer; var numhw3d: Integer; var total: Integer): FMOD_RESULT; stdcall;
function FMOD_System_SetSoftwareChannels    (system: FMOD_SYSTEM; numsoftwarechannels: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetSoftwareChannels    (system: FMOD_SYSTEM; var numsoftwarechannels: Integer): FMOD_RESULT; stdcall;
function FMOD_System_SetSoftwareFormat      (system: FMOD_SYSTEM; samplerate: Integer; format: FMOD_SOUND_FORMAT; numoutputchannels: Integer; maxinputchannels: Integer; resamplemethod: FMOD_DSP_RESAMPLER): FMOD_RESULT; stdcall;
function FMOD_System_GetSoftwareFormat      (system: FMOD_SYSTEM; var samplerate: Integer; var format: FMOD_SOUND_FORMAT; var numoutputchannels: Integer; var maxinputchannels: Integer; var bits: Integer; var resamplemethod: FMOD_DSP_RESAMPLER): FMOD_RESULT; stdcall;
function FMOD_System_SetDSPBufferSize       (system: FMOD_SYSTEM; bufferlength: Cardinal; numbuffers: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetDSPBufferSize       (system: FMOD_SYSTEM; var bufferlength: Cardinal; var numbuffers: Integer): FMOD_RESULT; stdcall;
function FMOD_System_SetFileSystem          (system: FMOD_SYSTEM; useropen: FMOD_FILE_OPENCALLBACK; userclose: FMOD_FILE_CLOSECALLBACK; userread: FMOD_FILE_READCALLBACK; userseek: FMOD_FILE_SEEKCALLBACK): FMOD_RESULT; stdcall;
function FMOD_System_AttachFileSystem       (system: FMOD_SYSTEM; useropen: FMOD_FILE_OPENCALLBACK; userclose: FMOD_FILE_CLOSECALLBACK; userread: FMOD_FILE_READCALLBACK; userseek: FMOD_FILE_SEEKCALLBACK): FMOD_RESULT; stdcall;
function FMOD_System_SetAdvancedSettings    (system: FMOD_SYSTEM; var settings:FMOD_ADVANCEDSETTINGS): FMOD_RESULT; stdcall;
function FMOD_System_GetAdvancedSettings    (system: FMOD_SYSTEM; var settings:FMOD_ADVANCEDSETTINGS): FMOD_RESULT; stdcall;
function FMOD_System_SetSpeakerMode         (system: FMOD_SYSTEM; speakermode: FMOD_SPEAKERMODE): FMOD_RESULT; stdcall;
function FMOD_System_GetSpeakerMode         (system: FMOD_SYSTEM; var speakermode: FMOD_SPEAKERMODE): FMOD_RESULT; stdcall;

(*
     Plug-in support                       
*)

function FMOD_System_SetPluginPath          (system: FMOD_SYSTEM; const path: PChar): FMOD_RESULT; stdcall;
function FMOD_System_LoadPlugin             (system: FMOD_SYSTEM; const filename: PChar; var plugintype: FMOD_PLUGINTYPE; var index: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetNumPlugins          (system: FMOD_SYSTEM; plugintype: FMOD_PLUGINTYPE; var numplugins: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetPluginInfo          (system: FMOD_SYSTEM; plugintype: FMOD_PLUGINTYPE; index: Integer; name: PChar; namelen: Integer; version: Cardinal): FMOD_RESULT; stdcall;
function FMOD_System_UnloadPlugin           (system: FMOD_SYSTEM; plugintype: FMOD_PLUGINTYPE; index: Integer): FMOD_RESULT; stdcall;
function FMOD_System_SetOutputByPlugin      (system: FMOD_SYSTEM; index: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetOutputByPlugin      (system: FMOD_SYSTEM; var index: Integer): FMOD_RESULT; stdcall;
function FMOD_System_CreateCodec            (system: FMOD_SYSTEM; description:FMOD_CODEC_DESCRIPTION): FMOD_RESULT; stdcall;

(*
     Init/Close                            
*)

function FMOD_System_Init                   (system: FMOD_SYSTEM; maxchannels: Integer; flags: Cardinal; const extradriverdata: Pointer): FMOD_RESULT; stdcall;
function FMOD_System_Close                  (system: FMOD_SYSTEM): FMOD_RESULT; stdcall;

(*
     General post-init system functions    
*)

function FMOD_System_Update                 (system: FMOD_SYSTEM): FMOD_RESULT; stdcall;
function FMOD_System_UpdateFinished         (system: FMOD_SYSTEM): FMOD_RESULT; stdcall;

function FMOD_System_Set3DSettings          (system: FMOD_SYSTEM; dopplerscale, distancefactor, rolloffscale: Single): FMOD_RESULT; stdcall;
function FMOD_System_Get3DSettings          (system: FMOD_SYSTEM; var dopplerscale: Single; var distancefactor: Single; var rolloffscale: Single): FMOD_RESULT; stdcall;
function FMOD_System_Set3DNumListeners      (system: FMOD_SYSTEM; numlisteners: Integer): FMOD_RESULT; stdcall;
function FMOD_System_Get3DNumListeners      (system: FMOD_SYSTEM; var numlisteners: Integer): FMOD_RESULT; stdcall;
function FMOD_System_Set3DListenerAttributes(system: FMOD_SYSTEM; listener: Integer; const pos: FMOD_VECTOR; const vel: FMOD_VECTOR; const forward_: FMOD_VECTOR; const up: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_System_Get3DListenerAttributes(system: FMOD_SYSTEM; listener: Integer; var pos: FMOD_VECTOR; var vel: FMOD_VECTOR; var forward_: FMOD_VECTOR; var up: FMOD_VECTOR): FMOD_RESULT; stdcall;

function FMOD_System_SetSpeakerPosition     (system: FMOD_SYSTEM; speaker: FMOD_SPEAKER; x:Single; y:Single): FMOD_RESULT; stdcall;
function FMOD_System_GetSpeakerPosition     (system: FMOD_SYSTEM; speaker: FMOD_SPEAKER; var x:Single; var y:Single): FMOD_RESULT; stdcall;
function FMOD_System_SetStreamBufferSize    (system: FMOD_SYSTEM; filebuffersize: Cardinal; filebuffersizetype: Cardinal): FMOD_RESULT; stdcall;
function FMOD_System_GetStreamBufferSize    (system: FMOD_SYSTEM; var filebuffersize: Cardinal; filebuffersizetype: Cardinal): FMOD_RESULT; stdcall;

(*
     System information functions.
*)

function FMOD_System_GetVersion             (system: FMOD_SYSTEM; var version: Cardinal): FMOD_RESULT; stdcall;
function FMOD_System_GetOutputHandle        (system: FMOD_SYSTEM; var handle: Pointer): FMOD_RESULT; stdcall;
function FMOD_System_GetChannelsPlaying     (system: FMOD_SYSTEM; var channels: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetCPUUsage            (system: FMOD_SYSTEM; var dsp, stream, update, total: Single): FMOD_RESULT; stdcall;
function FMOD_System_GetSoundRAM            (system: FMOD_SYSTEM; var currentalloced: Integer; var maxalloced: Integer; var total: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetNumCDROMDrives      (system: FMOD_SYSTEM; var num: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetCDROMDriveName      (system: FMOD_SYSTEM; drivenum: Integer; name: PChar; namelen: Integer; scsiaddr: PChar; scsiaddrlen: Integer; devicename: PChar; devicenamelen: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetSpectrum            (system: FMOD_SYSTEM; var spectrumarray: Single; numvalues: Integer; channeloffset: Integer; windowtype: FMOD_DSP_FFT_WINDOW): FMOD_RESULT; stdcall;
function FMOD_System_GetWaveData            (system: FMOD_SYSTEM; var wavearray: Single; numvalues: Integer; channeloffset: Integer): FMOD_RESULT; stdcall;

(*
     Sound/DSP/Channel/FX creation and retrieval.       
*)

function FMOD_System_CreateSound            (system: FMOD_SYSTEM; const name_or_data: PChar; mode: Cardinal; exinfo: PFMOD_CREATESOUNDEXINFO; var sound: FMOD_SOUND): FMOD_RESULT; stdcall;
function FMOD_System_CreateStream           (system: FMOD_SYSTEM; const name_or_data: PChar; mode: Cardinal; exinfo: PFMOD_CREATESOUNDEXINFO; var sound: FMOD_SOUND): FMOD_RESULT; stdcall;
function FMOD_System_CreateDSP              (system: FMOD_SYSTEM; const description: FMOD_DSP_DESCRIPTION; var dsp: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_System_CreateDSPByType        (system: FMOD_SYSTEM; type_: FMOD_DSP_TYPE; var dsp: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_System_CreateDSPByIndex       (system: FMOD_SYSTEM; index: Integer; var dsp: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_System_CreateChannelGroup     (system: FMOD_SYSTEM; const name: PChar; var channelgroup: FMOD_CHANNELGROUP): FMOD_RESULT; stdcall;

function FMOD_System_PlaySound              (system: FMOD_SYSTEM; channelid: Integer; sound: FMOD_SOUND; paused: FMOD_BOOL; var channel: FMOD_CHANNEL): FMOD_RESULT; stdcall;
function FMOD_System_PlayDSP                (system: FMOD_SYSTEM; channelid: Integer; dsp: FMOD_DSP; paused: FMOD_BOOL; var channel: FMOD_CHANNEL): FMOD_RESULT; stdcall;
function FMOD_System_GetChannel             (system: FMOD_SYSTEM; channelid: Integer; var channel: FMOD_CHANNEL): FMOD_RESULT; stdcall;
function FMOD_System_GetMasterChannelGroup  (system: FMOD_SYSTEM; var channelgroup: FMOD_CHANNELGROUP): FMOD_RESULT; stdcall;

(*
     Reverb API
*)

function FMOD_System_SetReverbProperties    (system: FMOD_SYSTEM; const prop: FMOD_REVERB_PROPERTIES): FMOD_RESULT; stdcall;
function FMOD_System_GetReverbProperties    (system: FMOD_SYSTEM; var prop: FMOD_REVERB_PROPERTIES): FMOD_RESULT; stdcall;

(*
     System level DSP access.
*)

function FMOD_System_GetDSPHead             (system: FMOD_SYSTEM; var dsp: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_System_AddDSP                 (system: FMOD_SYSTEM; dsp: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_System_LockDSP                (system: FMOD_SYSTEM): FMOD_RESULT; stdcall;
function FMOD_System_UnlockDSP              (system: FMOD_SYSTEM): FMOD_RESULT; stdcall;

(*
     Recording API
*)

function FMOD_System_SetRecordDriver        (system: FMOD_SYSTEM; driver: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetRecordDriver        (system: FMOD_SYSTEM; var driver: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetRecordNumDrivers    (system: FMOD_SYSTEM; var numdrivers: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetRecordDriverName    (system: FMOD_SYSTEM; id: Integer; name: PChar; namelen: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetRecordPosition      (system: FMOD_SYSTEM; var position: Cardinal): FMOD_RESULT; stdcall;

function FMOD_System_RecordStart            (system: FMOD_SYSTEM; sound: FMOD_SOUND; loop: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_System_RecordStop             (system: FMOD_SYSTEM): FMOD_RESULT; stdcall;
function FMOD_System_IsRecording            (system: FMOD_SYSTEM; var recording: FMOD_BOOL): FMOD_RESULT; stdcall;

(*
     Geometry API.
*)

function FMOD_System_CreateGeometry         (system: FMOD_SYSTEM; maxPolygons: Integer; maxVertices: Integer; var geometry: FMOD_GEOMETRY): FMOD_RESULT; stdcall;
function FMOD_System_SetGeometrySettings    (system: FMOD_SYSTEM; maxWorldSize: Single): FMOD_RESULT; stdcall;
function FMOD_System_GetGeometrySettings    (system: FMOD_SYSTEM; var maxWorldSize: Single): FMOD_RESULT; stdcall;
function FMOD_System_LoadGeometry           (system: FMOD_SYSTEM; data: Pointer; dataSize: Integer; var geometry: FMOD_GEOMETRY): FMOD_RESULT; stdcall;

(*
     Network functions.
*)

function FMOD_System_SetNetworkProxy        (system: FMOD_SYSTEM; const proxy: PChar): FMOD_RESULT; stdcall;
function FMOD_System_GetNetworkProxy        (system: FMOD_SYSTEM; proxy: PChar; proxylen: Integer): FMOD_RESULT; stdcall;
function FMOD_System_SetNetworkTimeout      (system: FMOD_SYSTEM; timeout: Integer): FMOD_RESULT; stdcall;
function FMOD_System_GetNetworkTimeout      (system: FMOD_SYSTEM; var timeout: Integer): FMOD_RESULT; stdcall;

(*
     Userdata set/get.                       
*)

function FMOD_System_SetUserData            (system: FMOD_SYSTEM; userdata: Pointer): FMOD_RESULT; stdcall;
function FMOD_System_GetUserData            (system: FMOD_SYSTEM; var userdata: Pointer): FMOD_RESULT; stdcall;

(*
    'Sound' API
*)

function FMOD_Sound_Release                 (sound: FMOD_SOUND): FMOD_RESULT; stdcall;
function FMOD_Sound_GetSystemObject         (sound: FMOD_SOUND; var system: FMOD_SYSTEM): FMOD_RESULT; stdcall;

(*
     Standard sound manipulation functions.                                                
*)

function FMOD_Sound_Lock                    (sound: FMOD_SOUND; offset, length: Cardinal; var ptr1: Pointer; var ptr2: Pointer; var len1: Cardinal; var len2: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Sound_Unlock                  (sound: FMOD_SOUND; ptr1, ptr2: Pointer; len1, len2: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Sound_SetDefaults             (sound: FMOD_SOUND; frequency, volume, pan: Single; const levels: Single; priority: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_GetDefaults             (sound: FMOD_SOUND; var frequency: Single; var volume: Single; var pan: Single; var priority: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_SetVariations           (sound: FMOD_SOUND; frequency, volume, pan: Single): FMOD_RESULT; stdcall;
function FMOD_Sound_GetVariations           (sound: FMOD_SOUND; var frequency: Single; var volume: Single; var pan: Single): FMOD_RESULT; stdcall;
function FMOD_Sound_Set3DMinMaxDistance     (sound: FMOD_SOUND; min, max: Single): FMOD_RESULT; stdcall;
function FMOD_Sound_Get3DMinMaxDistance     (sound: FMOD_SOUND; var min: Single; var max: Single): FMOD_RESULT; stdcall;
function FMOD_Sound_Set3DConeSettings       (sound: FMOD_SOUND; insideconeangle: Single; outsideconeangle: Single; outsidevolume: Single): FMOD_RESULT; stdcall;
function FMOD_Sound_Get3DConeSettings       (sound: FMOD_SOUND; var insideconeangle: Single; var outsideconeangle: Single; var outsidevolume: Single): FMOD_RESULT; stdcall;
function FMOD_Sound_Set3DCustomRolloff      (sound: FMOD_SOUND; points: FMOD_VECTOR; numpoints: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_Get3DCustomRolloff      (sound: FMOD_SOUND; var points: FMOD_VECTOR; var numpoints: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_SetSubSound             (sound: FMOD_SOUND; index: Integer; subsound: FMOD_SOUND): FMOD_RESULT; stdcall;
function FMOD_Sound_GetSubSound             (sound: FMOD_SOUND; index: Integer; var subsound: FMOD_SOUND): FMOD_RESULT; stdcall;
function FMOD_Sound_SetSubSoundSentence     (sound: FMOD_SOUND; var subsoundlist: Integer; numsubsounds: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_GetName                 (sound: FMOD_SOUND; name: PChar; namelen: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_GetLength               (sound: FMOD_SOUND; var length: Cardinal; lengthtype: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Sound_GetFormat               (sound: FMOD_SOUND; var type_: FMOD_SOUND_TYPE; var format: FMOD_SOUND_FORMAT; var channels: Integer; var bits: Integer): FMOD_RESULT; stdcall; 
function FMOD_Sound_GetNumSubSounds         (sound: FMOD_SOUND; var numsubsounds: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_GetNumTags              (sound: FMOD_SOUND; var numtags: Integer; var numtagsupdated: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_GetTag                  (sound: FMOD_SOUND; const name: PChar; index: Integer; var tag: FMOD_TAG): FMOD_RESULT; stdcall;
function FMOD_Sound_GetOpenState            (sound: FMOD_SOUND; var openstate: FMOD_OPENSTATE; var percentbuffered: Cardinal; var starving: Boolean): FMOD_RESULT; stdcall;
function FMOD_Sound_ReadData                (sound: FMOD_SOUND; buffer: Pointer; lenbytes: Cardinal; var read: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Sound_SeekData                (sound: FMOD_SOUND; pcm: Cardinal): FMOD_RESULT; stdcall;

(*
     Synchronization point API.  These points can come from markers embedded in wav files, and can also generate channel callbacks.        
*)

function FMOD_Sound_GetNumSyncPoints        (sound: FMOD_SOUND; var numsyncpoints: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_GetSyncPoint            (sound: FMOD_SOUND; index: Integer; var point: FMOD_SYNCPOINT): FMOD_RESULT; stdcall;
function FMOD_Sound_GetSyncPointInfo        (sound: FMOD_SOUND; point: FMOD_SYNCPOINT; name: PChar; namelen: Integer; offset: Cardinal; offsettype: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Sound_AddSyncPoint            (sound: FMOD_SOUND; offset: Cardinal; offsettype: Cardinal; name: PChar; var point: FMOD_SYNCPOINT): FMOD_RESULT; stdcall;
function FMOD_Sound_DeleteSyncPoint         (sound: FMOD_SOUND; point: FMOD_SYNCPOINT): FMOD_RESULT; stdcall;

(*
     Functions also in Channel class but here they are the 'default' to save having to change it in Channel all the time.
*)

function FMOD_Sound_SetMode                 (sound: FMOD_SOUND; mode: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Sound_GetMode                 (sound: FMOD_SOUND; var mode: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Sound_SetLoopCount            (sound: FMOD_SOUND; loopcount: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_GetLoopCount            (sound: FMOD_SOUND; var loopcount: Integer): FMOD_RESULT; stdcall;
function FMOD_Sound_SetLoopPoints           (sound: FMOD_SOUND; loopstart: Cardinal; loopstarttype: Cardinal; loopend: Cardinal; loopendtype: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Sound_GetLoopPoints           (sound: FMOD_SOUND; var loopstart: Cardinal; loopstarttype: Cardinal; var loopend: Cardinal; loopendtype: Cardinal): FMOD_RESULT; stdcall;

(*
     Userdata set/get.
*)

function FMOD_Sound_SetUserData             (sound: FMOD_SOUND; userdata: Pointer): FMOD_RESULT; stdcall;
function FMOD_Sound_GetUserData             (sound: FMOD_SOUND; var userdata: Pointer): FMOD_RESULT; stdcall;

(*
    'Channel' API
*)

function FMOD_Channel_GetSystemObject       (channel: FMOD_CHANNEL; var system: FMOD_SYSTEM): FMOD_RESULT; stdcall;

function FMOD_Channel_Stop                  (channel: FMOD_CHANNEL): FMOD_RESULT; stdcall;
function FMOD_Channel_SetPaused             (channel: FMOD_CHANNEL; paused: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_Channel_GetPaused             (channel: FMOD_CHANNEL; var paused: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_Channel_SetVolume             (channel: FMOD_CHANNEL; volume: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_GetVolume             (channel: FMOD_CHANNEL; var volume: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_SetFrequency          (channel: FMOD_CHANNEL; frequency: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_GetFrequency          (channel: FMOD_CHANNEL; var frequency: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_SetPan                (channel: FMOD_CHANNEL; pan: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_GetPan                (channel: FMOD_CHANNEL; var pan: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_SetDelay              (channel: FMOD_CHANNEL; startdelay: Cardinal; enddelay: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Channel_GetDelay              (channel: FMOD_CHANNEL; var startdelay: Cardinal; var enddelay: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Channel_SetSpeakerMix         (channel: FMOD_CHANNEL; frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_GetSpeakerMix         (channel: FMOD_CHANNEL; var frontleft: Single; var frontright: Single; var center: Single; var lfe: Single; var backleft: Single; var backright: Single; var sideleft: Single; var sideright: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_SetSpeakerLevels      (channel: FMOD_CHANNEL; speaker: FMOD_SPEAKER; const levels: Single; numlevels: Integer): FMOD_RESULT; stdcall;
function FMOD_Channel_GetSpeakerLevels      (channel: FMOD_CHANNEL; speaker: FMOD_SPEAKER; var levels: Single; var numlevels: Integer): FMOD_RESULT; stdcall;
function FMOD_Channel_SetMute               (channel: FMOD_CHANNEL; mute: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_Channel_GetMute               (channel: FMOD_CHANNEL; var mute: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_Channel_SetPriority           (channel: FMOD_CHANNEL; priority: Integer): FMOD_RESULT; stdcall;
function FMOD_Channel_GetPriority           (channel: FMOD_CHANNEL; var priority: Integer): FMOD_RESULT; stdcall;
function FMOD_Channel_SetPosition           (channel: FMOD_CHANNEL; position: Cardinal; postype: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Channel_GetPosition           (channel: FMOD_CHANNEL; var position: Cardinal; postype: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Channel_SetReverbProperties   (channel: FMOD_CHANNEL; const prop: FMOD_REVERB_CHANNELPROPERTIES): FMOD_RESULT; stdcall;
function FMOD_Channel_GetReverbProperties   (channel: FMOD_CHANNEL; var prop: FMOD_REVERB_CHANNELPROPERTIES): FMOD_RESULT; stdcall;
function FMOD_Channel_SetChannelGroup       (channel: FMOD_CHANNEL; channelgroup: FMOD_CHANNELGROUP): FMOD_RESULT; stdcall;
function FMOD_Channel_GetChannelGroup       (channel: FMOD_CHANNEL; var channelgroup: FMOD_CHANNELGROUP): FMOD_RESULT; stdcall;
function FMOD_Channel_SetCallback           (channel: FMOD_CHANNEL; callbacktype: Cardinal; callback: FMOD_CHANNEL_CALLBACK; command: Integer): FMOD_RESULT; stdcall;

(*
     3D functionality.
*)

function FMOD_Channel_Set3DAttributes       (channel: FMOD_CHANNEL; const pos: FMOD_VECTOR; const vel: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Channel_Get3DAttributes       (channel: FMOD_CHANNEL; var pos: FMOD_VECTOR; var vel: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Channel_Set3DMinMaxDistance   (channel: FMOD_CHANNEL; mindistance, maxdistance: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_Get3DMinMaxDistance   (channel: FMOD_CHANNEL; var mindistance: Single; var maxdistance: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_Set3DConeSettings     (channel: FMOD_CHANNEL; insideconeangle: Single; outsideconeangle: Single; outsidevolume: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_Get3DConeSettings     (channel: FMOD_CHANNEL; var insideconeangle: Single; var outsideconeangle: Single; var outsidevolume: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_Set3DConeOrientation  (channel: FMOD_CHANNEL; var orientation: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Channel_Get3DConeOrientation  (channel: FMOD_CHANNEL; var orientation: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Channel_Set3DCustomRolloff    (channel: FMOD_CHANNEL; points: FMOD_VECTOR; numpoints: Integer): FMOD_RESULT; stdcall;
function FMOD_Channel_Get3DCustomRolloff    (channel: FMOD_CHANNEL; var points: FMOD_VECTOR; var numpoints: Integer): FMOD_RESULT; stdcall;
function FMOD_Channel_Set3DOcclusion        (channel: FMOD_CHANNEL; directOcclusion: Single; reverbOcclusion: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_Get3DOcclusion        (channel: FMOD_CHANNEL; var directOcclusion: Single; var reverbOcclusion: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_Set3DSpread           (channel: FMOD_CHANNEL; angle: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_Get3DSpread           (channel: FMOD_CHANNEL; var angle: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_Set3DPanLevel         (channel: FMOD_CHANNEL; level: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_Get3DPanLevel         (channel: FMOD_CHANNEL; var level: Single): FMOD_RESULT; stdcall;

(*
     DSP functionality only for channels playing sounds created with FMOD_SOFTWARE.
*)

function FMOD_Channel_GetDSPHead            (channel: FMOD_CHANNEL; var dsp: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_Channel_AddDSP                (channel: FMOD_CHANNEL; dsp: FMOD_DSP): FMOD_RESULT; stdcall;

(*
     Information only functions.
*)

function FMOD_Channel_IsPlaying             (channel: FMOD_CHANNEL; var isplaying: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_Channel_IsVirtual             (channel: FMOD_CHANNEL; var isvirtual: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_Channel_GetAudibility         (channel: FMOD_CHANNEL; var audibility: Single): FMOD_RESULT; stdcall;
function FMOD_Channel_GetCurrentSound       (channel: FMOD_CHANNEL; var sound: FMOD_SOUND): FMOD_RESULT; stdcall;
function FMOD_Channel_GetSpectrum           (channel: FMOD_CHANNEL; var spectrumarray: Single; numvalues: Integer; channeloffset: Integer; windowtype: FMOD_DSP_FFT_WINDOW): FMOD_RESULT; stdcall;
function FMOD_Channel_GetWaveData           (channel: FMOD_CHANNEL; var wavearray: Single; numvalues: Integer; channeloffset: Integer): FMOD_RESULT; stdcall;
function FMOD_Channel_GetIndex              (channel: FMOD_CHANNEL; var index: Integer): FMOD_RESULT; stdcall;

(*
     Functions also found in Sound class but here they can be set per channel.
*)

function FMOD_Channel_SetMode               (channel: FMOD_CHANNEL; mode: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Channel_GetMode               (channel: FMOD_CHANNEL; var mode: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Channel_SetLoopCount          (channel: FMOD_CHANNEL; loopcount: Integer): FMOD_RESULT; stdcall;
function FMOD_Channel_GetLoopCount          (channel: FMOD_CHANNEL; var loopcount: Integer): FMOD_RESULT; stdcall;
function FMOD_Channel_SetLoopPoints         (channel: FMOD_CHANNEL; loopstart: Cardinal; loopstarttype: Cardinal; loopend: Cardinal; loopendtype: Cardinal): FMOD_RESULT; stdcall;
function FMOD_Channel_GetLoopPoints         (channel: FMOD_CHANNEL; var loopstart: Cardinal; loopstarttype: Cardinal; var loopend: Cardinal; loopendtype: Cardinal): FMOD_RESULT; stdcall;

(*
     Userdata set/get.
*)

function FMOD_Channel_SetUserData           (channel: FMOD_CHANNEL; userdata: Pointer): FMOD_RESULT; stdcall;
function FMOD_Channel_GetUserData           (channel: FMOD_CHANNEL; var userdata: Pointer): FMOD_RESULT; stdcall;

(*
    'ChannelGroup' API
*)

function FMOD_ChannelGroup_Release          (channelgroup: FMOD_CHANNELGROUP): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetSystemObject  (channelgroup: FMOD_CHANNELGROUP; var system: FMOD_SYSTEM): FMOD_RESULT; stdcall;

(*
     Channelgroup scale values.  (scales the current volume or pitch of all channels and channel groups, DOESN'T overwrite)
*)

function FMOD_ChannelGroup_SetVolume        (channelgroup: FMOD_CHANNELGROUP; volume: Single): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetVolume        (channelgroup: FMOD_CHANNELGROUP; var volume: Single): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_SetPitch         (channelgroup: FMOD_CHANNELGROUP; pitch: Single): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetPitch         (channelgroup: FMOD_CHANNELGROUP; var pitch: Single): FMOD_RESULT; stdcall;

(*
     Channelgroup override values.  (recursively overwrites whatever settings the channels had)
*)

function FMOD_ChannelGroup_Stop                    (channelgroup: FMOD_CHANNELGROUP): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_OverridePaused          (channelgroup: FMOD_CHANNELGROUP; paused: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_OverrideVolume          (channelgroup: FMOD_CHANNELGROUP; volume : Single): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_OverrideFrequency       (channelgroup: FMOD_CHANNELGROUP; frequency: Single): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_OverridePan             (channelgroup: FMOD_CHANNELGROUP; pan: Single): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_OverrideMute            (channelgroup: FMOD_CHANNELGROUP; mute: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_OverrideReverbProperties(channelgroup: FMOD_CHANNELGROUP; const prop: FMOD_REVERB_CHANNELPROPERTIES): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_Override3DAttributes    (channelgroup: FMOD_CHANNELGROUP; const pos: FMOD_VECTOR; const vel: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_OverrideSpeakerMix      (channelgroup: FMOD_CHANNELGROUP; frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright: Single): FMOD_RESULT; stdcall;

(*
     Nested channel groups.
*)

function FMOD_ChannelGroup_AddGroup         (channelgroup: FMOD_CHANNELGROUP; group: FMOD_CHANNELGROUP): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetNumGroups     (channelgroup: FMOD_CHANNELGROUP; var numgroups: Integer): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetGroup         (channelgroup: FMOD_CHANNELGROUP; index: Integer; var group: FMOD_CHANNELGROUP): FMOD_RESULT; stdcall;

(*
     DSP functionality only for channel groups playing sounds created with FMOD_SOFTWARE.
*)

function FMOD_ChannelGroup_GetDSPHead       (channelgroup: FMOD_CHANNELGROUP; var dsp: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_AddDSP           (channelgroup: FMOD_CHANNELGROUP; dsp: FMOD_DSP): FMOD_RESULT; stdcall;

(*
     Information only functions.
*)

function FMOD_ChannelGroup_GetName          (channelgroup: FMOD_CHANNELGROUP; name: PChar; namelen: Integer): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetNumChannels   (channelgroup: FMOD_CHANNELGROUP; var numchannels: Integer): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetChannel       (channelgroup: FMOD_CHANNELGROUP; index: Integer; var channel: FMOD_CHANNEL): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetSpectrum      (channelgroup: FMOD_CHANNELGROUP; var spectrumarray: Single; numvalues: Integer; channeloffset: Integer; windowtype: FMOD_DSP_FFT_WINDOW ): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetWaveData      (channelgroup: FMOD_CHANNELGROUP; var wavearray: Single; numvalues: Integer; channeloffset: Integer): FMOD_RESULT; stdcall;

(*
     Userdata set/get.
*)

function FMOD_ChannelGroup_SetUserData      (channelgroup: FMOD_CHANNELGROUP; userdata: Pointer): FMOD_RESULT; stdcall;
function FMOD_ChannelGroup_GetUserData      (channelgroup: FMOD_CHANNELGROUP; var userdata: Pointer): FMOD_RESULT; stdcall;

(*
    'DSP' API
*)

function FMOD_DSP_Release                   (dsp: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_DSP_GetSystemObject           (dsp: FMOD_DSP; var system: FMOD_SYSTEM): FMOD_RESULT; stdcall;

(*
     Connection / disconnection / input and output enumeration.
*)

function FMOD_DSP_AddInput                  (dsp, target: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_DSP_DisconnectFrom            (dsp, target: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_DSP_Remove                    (dsp: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_DSP_GetNumInputs              (dsp: FMOD_DSP; var numinputs: Integer): FMOD_RESULT; stdcall;
function FMOD_DSP_GetNumOutputs             (dsp: FMOD_DSP; var numoutputs: Integer): FMOD_RESULT; stdcall;
function FMOD_DSP_GetInput                  (dsp: FMOD_DSP; index: Integer; var input: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_DSP_GetOutput                 (dsp: FMOD_DSP; index: Integer; var output: FMOD_DSP): FMOD_RESULT; stdcall;
function FMOD_DSP_SetInputMix               (dsp: FMOD_DSP; index: Integer; volume: Single): FMOD_RESULT; stdcall;
function FMOD_DSP_GetInputMix               (dsp: FMOD_DSP; index: Integer; var volume: Single): FMOD_RESULT; stdcall;
function FMOD_DSP_SetInputLevels            (dsp: FMOD_DSP; index: Integer; speaker: FMOD_SPEAKER; const levels: Single; numlevels: Integer): FMOD_RESULT; stdcall;
function FMOD_DSP_GetInputLevels            (dsp: FMOD_DSP; index: Integer; speaker: FMOD_SPEAKER; const levels: Single; numlevels: Integer): FMOD_RESULT; stdcall;
function FMOD_DSP_SetOutputMix              (dsp: FMOD_DSP; index: Integer; volume: Single): FMOD_RESULT; stdcall;
function FMOD_DSP_GetOutputMix              (dsp: FMOD_DSP; index: Integer; var volume: Single): FMOD_RESULT; stdcall;
function FMOD_DSP_SetOutputLevels           (dsp: FMOD_DSP; index: Integer; speaker: FMOD_SPEAKER; const levels: Single; numlevels: Integer): FMOD_RESULT; stdcall;
function FMOD_DSP_GetOutputLevels           (dsp: FMOD_DSP; index: Integer; speaker: FMOD_SPEAKER; const levels: Single; numlevels: Integer): FMOD_RESULT; stdcall;

(*
     DSP unit control
*)

function FMOD_DSP_SetActive                 (dsp: FMOD_DSP; active: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_DSP_GetActive                 (dsp: FMOD_DSP; var active: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_DSP_SetBypass                 (dsp: FMOD_DSP; bypass: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_DSP_GetBypass                 (dsp: FMOD_DSP; var bypass: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_DSP_Reset                     (dsp: FMOD_DSP): FMOD_RESULT; stdcall;

(*
     DSP parameter control
*)

function FMOD_DSP_SetParameter              (dsp: FMOD_DSP; index: Integer; value: Single): FMOD_RESULT; stdcall;
function FMOD_DSP_GetParameter              (dsp: FMOD_DSP; index: Integer; var value: Single; valuestr: PChar; valuestrlen: Integer): FMOD_RESULT; stdcall;
function FMOD_DSP_GetNumParameters          (dsp: FMOD_DSP; var numparams: Integer): FMOD_RESULT; stdcall;
function FMOD_DSP_GetParameterInfo          (dsp: FMOD_DSP; index: Integer; name: PChar; plabel: PChar; description: PChar; descriptionlen: Integer; var min: Single; var max: Single): FMOD_RESULT; stdcall;
function FMOD_DSP_ShowConfigDialog          (dsp: FMOD_DSP; hwnd: Pointer; show: FMOD_BOOL): FMOD_RESULT; stdcall;

(*
     DSP attributes.
*)

function FMOD_DSP_GetInfo                   (dsp: FMOD_DSP; name:PChar; var version: cardinal; var channels:integer; var configwidth:integer; var configheight:integer): FMOD_RESULT; stdcall; 
function FMOD_DSP_SetDefaults               (dsp: FMOD_DSP; frequency: Single; volume: Single; pan: Single; var levels: Single; priority: Integer): FMOD_RESULT; stdcall;
function FMOD_DSP_GetDefaults               (dsp: FMOD_DSP; var frequency: Single; var volume: Single; var pan: Single; var priority: Integer): FMOD_RESULT; stdcall;

(*
     Userdata set/get.
*)

function FMOD_DSP_SetUserData               (dsp: FMOD_DSP; userdata: Pointer): FMOD_RESULT; stdcall;
function FMOD_DSP_GetUserData               (dsp: FMOD_DSP; var userdata: Pointer): FMOD_RESULT; stdcall;

(*
    'Geometry' API
*)

function FMOD_Geometry_Release              (geometry: FMOD_GEOMETRY): FMOD_RESULT; stdcall;

function FMOD_Geometry_AddPolygon           (geometry: FMOD_GEOMETRY; directOcclusion: Single; reverbOcclusion: Single; doubleSided: FMOD_BOOL; numVertices: Integer; var vertices: FMOD_VECTOR; var polygonIndex: Integer): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetNumPolygons       (geometry: FMOD_GEOMETRY; var numPolygons: Integer): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetMaxPolygons       (geometry: FMOD_GEOMETRY; var maxPolygons: Integer; maxVertices: Integer): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetPolygonNumVertices(geometry: FMOD_GEOMETRY; polygonIndex: Integer; var numVertices: Integer): FMOD_RESULT; stdcall;
function FMOD_Geometry_SetPolygonVertex     (geometry: FMOD_GEOMETRY; polygonIndex: Integer; vertexIndex: Integer; var vertex: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetPolygonVertex     (geometry: FMOD_GEOMETRY; polygonIndex: Integer; vertexIndex: Integer; var vertex: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Geometry_SetPolygonAttributes (geometry: FMOD_GEOMETRY; polygonIndex: Integer; directOcclusion: Single; reverbOcclusion: Single; doubleSided: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetPolygonAttributes (geometry: FMOD_GEOMETRY; polygonIndex: Integer; var directOcclusion: Single; var reverbOcclusion: Single; var doubleSided: FMOD_BOOL): FMOD_RESULT; stdcall;

function FMOD_Geometry_SetActive            (geometry: FMOD_GEOMETRY; active: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetActive            (geometry: FMOD_GEOMETRY; var active: FMOD_BOOL): FMOD_RESULT; stdcall;
function FMOD_Geometry_SetRotation          (geometry: FMOD_GEOMETRY; var forward: FMOD_VECTOR; var up: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetRotation          (geometry: FMOD_GEOMETRY; var forward: FMOD_VECTOR; var up: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Geometry_SetPosition          (geometry: FMOD_GEOMETRY; var position: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetPosition          (geometry: FMOD_GEOMETRY; var position: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Geometry_SetScale             (geometry: FMOD_GEOMETRY; var scale: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetScale             (geometry: FMOD_GEOMETRY; var scale: FMOD_VECTOR): FMOD_RESULT; stdcall;
function FMOD_Geometry_Save                 (geometry: FMOD_GEOMETRY; data: Pointer; var datasize: Integer): FMOD_RESULT; stdcall;

(*
     Userdata set/get.
*)

function FMOD_Geometry_SetUserData          (geometry: FMOD_GEOMETRY; userdata: Pointer): FMOD_RESULT; stdcall;
function FMOD_Geometry_GetUserData          (geometry: FMOD_GEOMETRY; var userdata: Pointer): FMOD_RESULT; stdcall;

function FMOD_Load                          (const libname: PChar): FMOD_RESULT; stdcall;
function FMOD_Unload                        (): FMOD_RESULT; stdcall;

implementation

const
{$IFDEF MSWINDOWS}
  FMOD_DLL = 'fmodex.dll';
{$ELSE}
{$IFDEF LINUX}
  FMOD_DLL = 'libfmodex.so';
{$ENDIF}
{$ENDIF}

function FMOD_Load(const libname: PChar): FMOD_RESULT;
begin
  FMOD_Load := FMOD_OK;
end;

function FMOD_Unload(): FMOD_RESULT;
begin
  FMOD_Unload := FMOD_OK;
end;

(*                                          
    FMOD System memory functions (optional).
*)                                          

function FMOD_Memory_Initialize             ; external FMOD_DLL;
function FMOD_Memory_GetStats               ; external FMOD_DLL;

(*
    FMOD System factory functions.  Use this to create an FMOD System Instance.  below you will see FMOD_System_Init/Close to get started.
*)

function FMOD_System_Create                 ; external FMOD_DLL;
function FMOD_System_Release                ; external FMOD_DLL;
                                            
(*                                          
    'System' API                            
*)                                          
                                            
(*                                          
     Pre-init functions.                    
*)                                          
                                            
function FMOD_System_SetOutput              ; external FMOD_DLL;
function FMOD_System_GetOutput              ; external FMOD_DLL;
function FMOD_System_GetNumDrivers          ; external FMOD_DLL;
function FMOD_System_GetDriverName          ; external FMOD_DLL;
function FMOD_System_GetDriverCaps          ; external FMOD_DLL;
function FMOD_System_SetDriver              ; external FMOD_DLL;
function FMOD_System_GetDriver              ; external FMOD_DLL;
function FMOD_System_SetHardwareChannels    ; external FMOD_DLL;
function FMOD_System_GetHardwareChannels    ; external FMOD_DLL;
function FMOD_System_SetSoftwareChannels    ; external FMOD_DLL;
function FMOD_System_GetSoftwareChannels    ; external FMOD_DLL;
function FMOD_System_SetSoftwareFormat      ; external FMOD_DLL;
function FMOD_System_GetSoftwareFormat      ; external FMOD_DLL;
function FMOD_System_SetDSPBufferSize       ; external FMOD_DLL;
function FMOD_System_GetDSPBufferSize       ; external FMOD_DLL;
function FMOD_System_SetFileSystem          ; external FMOD_DLL;
function FMOD_System_AttachFileSystem       ; external FMOD_DLL;
function FMOD_System_SetAdvancedSettings    ; external FMOD_DLL;
function FMOD_System_GetAdvancedSettings    ; external FMOD_DLL;
function FMOD_System_SetSpeakerMode         ; external FMOD_DLL;
function FMOD_System_GetSpeakerMode         ; external FMOD_DLL;

(*                                          
     Plug-in support                        
*)                                          
                                           
function FMOD_System_SetPluginPath          ; external FMOD_DLL;
function FMOD_System_LoadPlugin             ; external FMOD_DLL;
function FMOD_System_GetNumPlugins          ; external FMOD_DLL;
function FMOD_System_GetPluginInfo          ; external FMOD_DLL;
function FMOD_System_UnloadPlugin           ; external FMOD_DLL;
function FMOD_System_SetOutputByPlugin      ; external FMOD_DLL;
function FMOD_System_GetOutputByPlugin      ; external FMOD_DLL;
function FMOD_System_CreateCodec            ; external FMOD_DLL;
                                            
(*                                          
     Init/Close                             
*)                                          
                                            
function FMOD_System_Init                   ; external FMOD_DLL;
function FMOD_System_Close                  ; external FMOD_DLL;                                            
                                           
(*                                          
     General post-init system functions     
*)                                          
                                            
function FMOD_System_Update                 ; external FMOD_DLL;
function FMOD_System_UpdateFinished         ; external FMOD_DLL;
                                            
function FMOD_System_Set3DSettings          ; external FMOD_DLL;
function FMOD_System_Get3DSettings          ; external FMOD_DLL;
function FMOD_System_Set3DNumListeners      ; external FMOD_DLL;
function FMOD_System_Get3DNumListeners      ; external FMOD_DLL;
function FMOD_System_Set3DListenerAttributes; external FMOD_DLL;
function FMOD_System_Get3DListenerAttributes; external FMOD_DLL;
                                            
function FMOD_System_SetSpeakerPosition     ; external FMOD_DLL;
function FMOD_System_GetSpeakerPosition     ; external FMOD_DLL;
function FMOD_System_SetStreamBufferSize    ; external FMOD_DLL;
function FMOD_System_GetStreamBufferSize    ; external FMOD_DLL;
                                            
(*                                          
     System information functions.          
*)                                          
                                            
function FMOD_System_GetVersion             ; external FMOD_DLL;
function FMOD_System_GetOutputHandle        ; external FMOD_DLL;
function FMOD_System_GetChannelsPlaying     ; external FMOD_DLL;
function FMOD_System_GetCPUUsage            ; external FMOD_DLL;
function FMOD_System_GetSoundRAM            ; external FMOD_DLL;
function FMOD_System_GetNumCDROMDrives      ; external FMOD_DLL;
function FMOD_System_GetCDROMDriveName      ; external FMOD_DLL;
function FMOD_System_GetSpectrum            ; external FMOD_DLL;
function FMOD_System_GetWaveData            ; external FMOD_DLL;
                                            
(*                                          
     Sound/DSP/Channel/FX creation and retrieval.       
*)                                          
                                            
function FMOD_System_CreateSound            ; external FMOD_DLL;
function FMOD_System_CreateStream           ; external FMOD_DLL;
function FMOD_System_CreateDSP              ; external FMOD_DLL;
function FMOD_System_CreateDSPByType        ; external FMOD_DLL;
function FMOD_System_CreateDSPByIndex       ; external FMOD_DLL;
function FMOD_System_CreateChannelGroup     ; external FMOD_DLL;
                                            
function FMOD_System_PlaySound              ; external FMOD_DLL;
function FMOD_System_PlayDSP                ; external FMOD_DLL;
function FMOD_System_GetChannel             ; external FMOD_DLL;
function FMOD_System_GetMasterChannelGroup  ; external FMOD_DLL;
                                            
(*                                          
     Reverb API                             
*)                                          
                                            
function FMOD_System_SetReverbProperties    ; external FMOD_DLL;
function FMOD_System_GetReverbProperties    ; external FMOD_DLL;
                                            
(*                                          
     System level DSP access.
*)                                          
                                            
function FMOD_System_GetDSPHead             ; external FMOD_DLL;
function FMOD_System_AddDSP                 ; external FMOD_DLL;
function FMOD_System_LockDSP                ; external FMOD_DLL;
function FMOD_System_UnlockDSP              ; external FMOD_DLL;
                                            
(*                                          
     Recording API
*)                                          
                                            
function FMOD_System_SetRecordDriver        ; external FMOD_DLL;
function FMOD_System_GetRecordDriver        ; external FMOD_DLL;
function FMOD_System_GetRecordNumDrivers    ; external FMOD_DLL;
function FMOD_System_GetRecordDriverName    ; external FMOD_DLL;
function FMOD_System_GetRecordPosition      ; external FMOD_DLL;

function FMOD_System_RecordStart            ; external FMOD_DLL;
function FMOD_System_RecordStop             ; external FMOD_DLL;
function FMOD_System_IsRecording            ; external FMOD_DLL;
                                            
(*
     Geometry API.
*)

function FMOD_System_CreateGeometry         ; external FMOD_DLL;
function FMOD_System_SetGeometrySettings    ; external FMOD_DLL;
function FMOD_System_GetGeometrySettings    ; external FMOD_DLL;
function FMOD_System_LoadGeometry           ; external FMOD_DLL;

(*                                          
     Network functions                      
*)                                          
                                            
function FMOD_System_SetNetworkProxy        ; external FMOD_DLL;
function FMOD_System_GetNetworkProxy        ; external FMOD_DLL;
function FMOD_System_SetNetworkTimeout      ; external FMOD_DLL;
function FMOD_System_GetNetworkTimeout      ; external FMOD_DLL;
                                            
(*
     Userdata set/get                         
*)

function FMOD_System_SetUserData            ; external FMOD_DLL;
function FMOD_System_GetUserData            ; external FMOD_DLL;
                                            
(*                                          
    'Sound' API                             
*)                                          
                                            
function FMOD_Sound_Release                 ; external FMOD_DLL;
function FMOD_Sound_GetSystemObject         ; external FMOD_DLL;
                                            
(*
     Standard sound manipulation functions.                                                
*)

function FMOD_Sound_Lock                    ; external FMOD_DLL;
function FMOD_Sound_Unlock                  ; external FMOD_DLL;
function FMOD_Sound_SetDefaults             ; external FMOD_DLL;
function FMOD_Sound_GetDefaults             ; external FMOD_DLL;
function FMOD_Sound_SetVariations           ; external FMOD_DLL;
function FMOD_Sound_GetVariations           ; external FMOD_DLL;
function FMOD_Sound_Set3DMinMaxDistance     ; external FMOD_DLL;
function FMOD_Sound_Get3DMinMaxDistance     ; external FMOD_DLL;
function FMOD_Sound_Set3DConeSettings       ; external FMOD_DLL;
function FMOD_Sound_Get3DConeSettings       ; external FMOD_DLL;
function FMOD_Sound_Set3DCustomRolloff      ; external FMOD_DLL;
function FMOD_Sound_Get3DCustomRolloff      ; external FMOD_DLL;
function FMOD_Sound_SetSubSound             ; external FMOD_DLL;
function FMOD_Sound_GetSubSound             ; external FMOD_DLL;
function FMOD_Sound_SetSubSoundSentence     ; external FMOD_DLL;
function FMOD_Sound_GetName                 ; external FMOD_DLL;
function FMOD_Sound_GetLength               ; external FMOD_DLL;
function FMOD_Sound_GetFormat               ; external FMOD_DLL;
function FMOD_Sound_GetNumSubSounds         ; external FMOD_DLL;
function FMOD_Sound_GetNumTags              ; external FMOD_DLL;
function FMOD_Sound_GetTag                  ; external FMOD_DLL;
function FMOD_Sound_GetOpenState            ; external FMOD_DLL;
function FMOD_Sound_ReadData                ; external FMOD_DLL;
function FMOD_Sound_SeekData                ; external FMOD_DLL;

(*
     Synchronization point API.  These points can come from markers embedded in wav files, and can also generate channel callbacks.        
*)

function FMOD_Sound_GetNumSyncPoints        ; external FMOD_DLL;
function FMOD_Sound_GetSyncPoint            ; external FMOD_DLL;
function FMOD_Sound_GetSyncPointInfo        ; external FMOD_DLL;
function FMOD_Sound_AddSyncPoint            ; external FMOD_DLL;
function FMOD_Sound_DeleteSyncPoint         ; external FMOD_DLL;
                                            
(*
     Functions also in Channel class but here they are the 'default' to save having to change it in Channel all the time.
*)

function FMOD_Sound_SetMode                 ; external FMOD_DLL;
function FMOD_Sound_GetMode                 ; external FMOD_DLL;
function FMOD_Sound_SetLoopCount            ; external FMOD_DLL;
function FMOD_Sound_GetLoopCount            ; external FMOD_DLL;
function FMOD_Sound_SetLoopPoints           ; external FMOD_DLL;
function FMOD_Sound_GetLoopPoints           ; external FMOD_DLL;
                                            
(*
     Userdata set/get.
*)

function FMOD_Sound_SetUserData             ; external FMOD_DLL;
function FMOD_Sound_GetUserData             ; external FMOD_DLL;
                                            
(*                                          
    'Channel' API                           
*)                                          
                                            
function FMOD_Channel_GetSystemObject       ; external FMOD_DLL;
                                            
function FMOD_Channel_Stop                  ; external FMOD_DLL;
function FMOD_Channel_SetPaused             ; external FMOD_DLL;
function FMOD_Channel_GetPaused             ; external FMOD_DLL;
function FMOD_Channel_SetVolume             ; external FMOD_DLL;
function FMOD_Channel_GetVolume             ; external FMOD_DLL;
function FMOD_Channel_SetFrequency          ; external FMOD_DLL;
function FMOD_Channel_GetFrequency          ; external FMOD_DLL;
function FMOD_Channel_SetPan                ; external FMOD_DLL;
function FMOD_Channel_GetPan                ; external FMOD_DLL;
function FMOD_Channel_SetDelay              ; external FMOD_DLL;
function FMOD_Channel_GetDelay              ; external FMOD_DLL;
function FMOD_Channel_SetSpeakerMix         ; external FMOD_DLL;
function FMOD_Channel_GetSpeakerMix         ; external FMOD_DLL;
function FMOD_Channel_SetSpeakerLevels      ; external FMOD_DLL;
function FMOD_Channel_GetSpeakerLevels      ; external FMOD_DLL;
function FMOD_Channel_SetMute               ; external FMOD_DLL;
function FMOD_Channel_GetMute               ; external FMOD_DLL;
function FMOD_Channel_SetPriority           ; external FMOD_DLL;
function FMOD_Channel_GetPriority           ; external FMOD_DLL;
function FMOD_Channel_SetPosition           ; external FMOD_DLL;
function FMOD_Channel_GetPosition           ; external FMOD_DLL;
function FMOD_Channel_SetReverbProperties   ; external FMOD_DLL;
function FMOD_Channel_GetReverbProperties   ; external FMOD_DLL;
function FMOD_Channel_SetChannelGroup       ; external FMOD_DLL;
function FMOD_Channel_GetChannelGroup       ; external FMOD_DLL;
function FMOD_Channel_SetCallback           ; external FMOD_DLL;

(*
     3D functionality.
*)

function FMOD_Channel_Set3DAttributes       ; external FMOD_DLL;
function FMOD_Channel_Get3DAttributes       ; external FMOD_DLL;
function FMOD_Channel_Set3DMinMaxDistance   ; external FMOD_DLL;
function FMOD_Channel_Get3DMinMaxDistance   ; external FMOD_DLL;
function FMOD_Channel_Set3DConeSettings     ; external FMOD_DLL;
function FMOD_Channel_Get3DConeSettings     ; external FMOD_DLL;
function FMOD_Channel_Set3DConeOrientation  ; external FMOD_DLL;
function FMOD_Channel_Get3DConeOrientation  ; external FMOD_DLL;
function FMOD_Channel_Set3DCustomRolloff    ; external FMOD_DLL;
function FMOD_Channel_Get3DCustomRolloff    ; external FMOD_DLL;
function FMOD_Channel_Set3DOcclusion        ; external FMOD_DLL;
function FMOD_Channel_Get3DOcclusion        ; external FMOD_DLL;
function FMOD_Channel_Set3DSpread           ; external FMOD_DLL;
function FMOD_Channel_Get3DSpread           ; external FMOD_DLL;
function FMOD_Channel_Set3DPanLevel         ; external FMOD_DLL;
function FMOD_Channel_Get3DPanLevel         ; external FMOD_DLL;

(*
     DSP functionality only for channels playing sounds created with FMOD_SOFTWARE.
*)

function FMOD_Channel_GetDSPHead            ; external FMOD_DLL;
function FMOD_Channel_AddDSP                ; external FMOD_DLL;

(*
     Information only functions.
*)

function FMOD_Channel_IsPlaying             ; external FMOD_DLL;
function FMOD_Channel_IsVirtual             ; external FMOD_DLL;
function FMOD_Channel_GetAudibility         ; external FMOD_DLL;
function FMOD_Channel_GetCurrentSound       ; external FMOD_DLL;
function FMOD_Channel_GetSpectrum           ; external FMOD_DLL;
function FMOD_Channel_GetWaveData           ; external FMOD_DLL;
function FMOD_Channel_GetIndex              ; external FMOD_DLL;
                                            
(*
     Functions also found in Sound class but here they can be set per channel.
*)

function FMOD_Channel_SetMode               ; external FMOD_DLL;
function FMOD_Channel_GetMode               ; external FMOD_DLL;
function FMOD_Channel_SetLoopCount          ; external FMOD_DLL;
function FMOD_Channel_GetLoopCount          ; external FMOD_DLL;
function FMOD_Channel_SetLoopPoints         ; external FMOD_DLL;
function FMOD_Channel_GetLoopPoints         ; external FMOD_DLL;
                                            
(*
     Userdata set/get.
*)

function FMOD_Channel_SetUserData           ; external FMOD_DLL;
function FMOD_Channel_GetUserData           ; external FMOD_DLL;
                                            
(*
    'ChannelGroup' API
*)

function FMOD_ChannelGroup_Release          ; external FMOD_DLL;
function FMOD_ChannelGroup_GetSystemObject  ; external FMOD_DLL;

(*
     Channelgroup scale values.  (scales the current volume or pitch of all channels and channel groups, DOESN'T overwrite)
*)

function FMOD_ChannelGroup_SetVolume        ; external FMOD_DLL;
function FMOD_ChannelGroup_GetVolume        ; external FMOD_DLL;
function FMOD_ChannelGroup_SetPitch         ; external FMOD_DLL;
function FMOD_ChannelGroup_GetPitch         ; external FMOD_DLL;

(*
     Channelgroup override values.  (recursively overwrites whatever settings the channels had)
*)

function FMOD_ChannelGroup_Stop                    ; external FMOD_DLL;
function FMOD_ChannelGroup_OverridePaused          ; external FMOD_DLL;
function FMOD_ChannelGroup_OverrideVolume          ; external FMOD_DLL;
function FMOD_ChannelGroup_OverrideFrequency       ; external FMOD_DLL;
function FMOD_ChannelGroup_OverridePan             ; external FMOD_DLL;
function FMOD_ChannelGroup_OverrideMute            ; external FMOD_DLL;
function FMOD_ChannelGroup_OverrideReverbProperties; external FMOD_DLL;
function FMOD_ChannelGroup_Override3DAttributes    ; external FMOD_DLL;
function FMOD_ChannelGroup_OverrideSpeakerMix      ; external FMOD_DLL;

(*
     Nested channel groups.
*)

function FMOD_ChannelGroup_AddGroup         ; external FMOD_DLL;
function FMOD_ChannelGroup_GetNumGroups     ; external FMOD_DLL;
function FMOD_ChannelGroup_GetGroup         ; external FMOD_DLL;

(*
     DSP functionality only for channel groups playing sounds created with FMOD_SOFTWARE.
*)

function FMOD_ChannelGroup_GetDSPHead       ; external FMOD_DLL;
function FMOD_ChannelGroup_AddDSP           ; external FMOD_DLL;

(*
     Information only functions.
*)

function FMOD_ChannelGroup_GetName          ; external FMOD_DLL;
function FMOD_ChannelGroup_GetNumChannels   ; external FMOD_DLL;
function FMOD_ChannelGroup_GetChannel       ; external FMOD_DLL;
function FMOD_ChannelGroup_GetSpectrum      ; external FMOD_DLL;
function FMOD_ChannelGroup_GetWaveData      ; external FMOD_DLL;

(*
     Userdata set/get.
*)

function FMOD_ChannelGroup_SetUserData      ; external FMOD_DLL;
function FMOD_ChannelGroup_GetUserData      ; external FMOD_DLL;

(*                                          
    'DSP' API                               
*)                                          
                                            
function FMOD_DSP_Release                   ; external FMOD_DLL;
function FMOD_DSP_GetSystemObject           ; external FMOD_DLL;
                                            
(*
     Connection / disconnection / input and output enumeration.
*)

function FMOD_DSP_AddInput                  ; external FMOD_DLL;
function FMOD_DSP_DisconnectFrom            ; external FMOD_DLL;
function FMOD_DSP_Remove                    ; external FMOD_DLL;
function FMOD_DSP_GetNumInputs              ; external FMOD_DLL;
function FMOD_DSP_GetNumOutputs             ; external FMOD_DLL;
function FMOD_DSP_GetInput                  ; external FMOD_DLL;
function FMOD_DSP_GetOutput                 ; external FMOD_DLL;
function FMOD_DSP_SetInputMix               ; external FMOD_DLL;
function FMOD_DSP_GetInputMix               ; external FMOD_DLL;
function FMOD_DSP_SetInputLevels            ; external FMOD_DLL;
function FMOD_DSP_GetInputLevels            ; external FMOD_DLL;
function FMOD_DSP_SetOutputMix              ; external FMOD_DLL;
function FMOD_DSP_GetOutputMix              ; external FMOD_DLL;
function FMOD_DSP_SetOutputLevels           ; external FMOD_DLL;
function FMOD_DSP_GetOutputLevels           ; external FMOD_DLL;

(*
     DSP unit control
*)

function FMOD_DSP_SetActive                 ; external FMOD_DLL;
function FMOD_DSP_GetActive                 ; external FMOD_DLL;
function FMOD_DSP_SetBypass                 ; external FMOD_DLL;
function FMOD_DSP_GetBypass                 ; external FMOD_DLL;
function FMOD_DSP_Reset                     ; external FMOD_DLL;

(*
     DSP parameter control
*)

function FMOD_DSP_SetParameter              ; external FMOD_DLL;
function FMOD_DSP_GetParameter              ; external FMOD_DLL;
function FMOD_DSP_GetNumParameters          ; external FMOD_DLL;
function FMOD_DSP_GetParameterInfo          ; external FMOD_DLL;
function FMOD_DSP_ShowConfigDialog          ; external FMOD_DLL;

(*
     DSP attributes.
*)

function FMOD_DSP_GetInfo                   ; external FMOD_DLL;
function FMOD_DSP_SetDefaults               ; external FMOD_DLL;
function FMOD_DSP_GetDefaults               ; external FMOD_DLL;

(*
     Userdata set/get.
*)

function FMOD_DSP_SetUserData               ; external FMOD_DLL;
function FMOD_DSP_GetUserData               ; external FMOD_DLL;

(*
    'Geometry' API
*)

function FMOD_Geometry_Release              ; external FMOD_DLL;

function FMOD_Geometry_AddPolygon           ; external FMOD_DLL;
function FMOD_Geometry_GetNumPolygons       ; external FMOD_DLL;
function FMOD_Geometry_GetMaxPolygons       ; external FMOD_DLL;
function FMOD_Geometry_GetPolygonNumVertices; external FMOD_DLL;
function FMOD_Geometry_SetPolygonVertex     ; external FMOD_DLL;
function FMOD_Geometry_GetPolygonVertex     ; external FMOD_DLL;
function FMOD_Geometry_SetPolygonAttributes ; external FMOD_DLL;
function FMOD_Geometry_GetPolygonAttributes ; external FMOD_DLL;

function FMOD_Geometry_SetActive            ; external FMOD_DLL;
function FMOD_Geometry_GetActive            ; external FMOD_DLL;
function FMOD_Geometry_SetRotation          ; external FMOD_DLL;
function FMOD_Geometry_GetRotation          ; external FMOD_DLL;
function FMOD_Geometry_SetPosition          ; external FMOD_DLL;
function FMOD_Geometry_GetPosition          ; external FMOD_DLL;
function FMOD_Geometry_SetScale             ; external FMOD_DLL;
function FMOD_Geometry_GetScale             ; external FMOD_DLL;
function FMOD_Geometry_Save                 ; external FMOD_DLL;

(*
     Userdata set/get.
*)

function FMOD_Geometry_SetUserData          ; external FMOD_DLL;
function FMOD_Geometry_GetUserData          ; external FMOD_DLL;

end.