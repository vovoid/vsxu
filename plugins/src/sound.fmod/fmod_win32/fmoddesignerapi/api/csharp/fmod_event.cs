/* ========================================================================================== */
/* FMOD Event - C# Wrapper . Copyright (c), Firelight Technologies Pty, Ltd. 2004-2007.       */
/*                                                                                            */
/*                                                                                            */
/* ========================================================================================== */

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace FMOD
{
    /*
        FMOD EventSystem version number.  Check this against FMOD::EventSystem::getVersion.
        0xaaaabbcc -> aaaa = major version number.  bb = minor version number.  cc = development version number.
    */
    public class EVENT_VERSION
    {
        public const int number = 0x00041300;
        public const string dll    = "fmod_event.dll";
    }

    /*
    [DEFINE]
    [
        [NAME]
        FMOD_EVENT_INITFLAGS

        [DESCRIPTION]   
        Initialization flags.  Use them with EventSystem::init in the eventflags parameter to change various behaviour.

        [REMARKS]

        [PLATFORMS]
        Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

        [SEE_ALSO]
        EventSystem::init
    ]
    */
    public enum EVENT_INIT
    {
        NORMAL              = 0x00000000, /* All platforms - Initialize normally */
        USER_ASSETMANAGER   = 0x00000001, /* All platforms - All wave data loading/freeing will be referred back to the user through the event callback */
        FAIL_ON_MAXSTREAMS  = 0x00000002, /* All platforms - Events will fail if "Max streams" was reached when playing streamed banks, instead of going virtual. */
        DONTUSENAMES        = 0x00000004  /* All platforms - All event/eventgroup/eventparameter/eventcategory/eventreverb names will be discarded on load. Use getXXXByIndex to access them. This may potentially save a lot of memory at runtime. */
    }


    /*
    [DEFINE]
    [
        [NAME] 
        FMOD_EVENT_MODE

        [DESCRIPTION]   
        Event data loading bitfields. Bitwise OR them together for controlling how event data is loaded.

        [REMARKS]

        [PLATFORMS]
        Win32, Win64, Linux, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

        [SEE_ALSO]
        EventGroup::loadEventData
        EventGroup::getEvent
        EventGroup::getEventByIndex
    ]
    */
    public enum EVENT_MODE
    {
        DEFAULT             = 0x00000000,  /* FMOD_EVENT_DEFAULT specifies default loading behaviour i.e. event data for the whole group is NOT cached and the function that initiated the loading process will block until loading is complete. */
        NONBLOCKING         = 0x00000001,  /* For loading event data asynchronously. FMOD will use a thread to load the data.  Use Event::getState to find out when loading is complete. */
        ERROR_ON_DISKACCESS = 0x00000002,  /* For EventGroup::getEvent / EventGroup::getEventByIndex.  If EventGroup::loadEventData has accidently been forgotten this flag will return an FMOD_ERR_FILE_UNWANTED if the getEvent function tries to load data. */
        INFOONLY            = 0x00000004   /* For EventGroup::getEvent / EventGroup::getEventByIndex.  Don't allocate instances or load data, just get a handle to allow user to get information from the event. */
    }


    /*
    [DEFINE]
    [
        [NAME] 
        FMOD_EVENT_STATE

        [DESCRIPTION]   
        These values describe what state an event is in.

        [REMARKS]    
        The flags below can be combined to set multiple states at once.  Use bitwise AND operations to test for these.
        An example of a combined flag set would be FMOD_EVENT_STATE_READY | FMOD_EVENT_STATE_PLAYING.

        [PLATFORMS]
        Win32, Win64, Linux, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

        [SEE_ALSO]
        Event::getState
        FMOD_EVENT_MODE
    ]
    */
    public enum EVENT_STATE
    {
        READY           = 0x00000001,  /* Event is ready to play. */
        LOADING         = 0x00000002,  /* Loading in progress. */
        ERROR           = 0x00000004,  /* Failed to open - file not found, out of memory etc.  See return value of Event::getState for what happened. */
        PLAYING         = 0x00000008,  /* Event has been started.  This will still be true even if there are no sounds active.  Event::stop must be called or the event must stop itself using a 'one shot and stop event' parameter mode. */
        CHANNELSACTIVE  = 0x00000010,  /* Event has active voices.  Use this if you want to detect if sounds are playing in the event or not. */
        INFOONLY        = 0x00000020,  /* Event was loaded with the FMOD_EVENT_INFOONLY flag. */
        STARVING        = 0x00000040,  /* Event is streaming but not being fed data in time, so may be stuttering. */
    }


    /*
    [ENUM]
    [
        [DESCRIPTION]
        Property indices for Event::getPropertyByIndex.
    
        [REMARKS]        

        [PLATFORMS]
        Win32, Win64, Linux, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

        [SEE_ALSO]
        Event::getPropertyByIndex
    ]
    */
    public enum EVENTPROPERTY
    {
        NAME = 0,              /* Type : char *    - Name of event. */
        VOLUME,                /* Type : float     - Relative volume of event. */
        VOLUMERANDOMIZATION,   /* Type : float     - Random deviation in volume of event. */
        PITCH,                 /* Type : float     - Relative pitch of event. */
        PITCHRANDOMIZATION,    /* Type : float     - Random deviation in pitch of event. */
        PRIORITY,              /* Type : int       - Playback priority of event. */
        MAX_PLAYBACKS,         /* Type : int       - Maximum simultaneous playbacks of event. */
        MAX_PLAYBACKS_BEHAVIOR,/* Type : int       - 1 = steal oldest, 2 = steal newest, 3 = steal quietest, 4 = just fail. */
        MODE,                  /* Type : FMOD_MODE - Either FMOD_3D or FMOD_2D. */
        _3D_ROLLOFF,           /* Type : FMOD_MODE - Either FMOD_3D_LOGROLLOFF, FMOD_3D_LINEARROLLOFF, or none for custom rolloff.. */
        _3D_MINDISTANCE,       /* Type : float     - Minimum 3d distance of event. */
        _3D_MAXDISTANCE,       /* Type : float     - Maximum 3d distance of event.  Means different things depending on EVENTPROPERTY_3D_ROLLOFF. */
        _3D_POSITION,          /* Type : FMOD_MODE - Either FMOD_3D_HEADRELATIVE or FMOD_3D_WORLDRELATIVE. */
        _3D_CONEINSIDEANGLE,   /* Type : float     - Event cone inside angle.  0 to 360. */
        _3D_CONEOUTSIDEANGLE,  /* Type : float     - Event cone outside angle.  0 to 360. */
        _3D_CONEOUTSIDEVOLUME, /* Type : float     - Event cone outside volume.  0 to 1.0. */
        _3D_DOPPLERSCALE,      /* Type : float     - Doppler scale where 0 = no doppler, 1.0 = normal doppler, 2.0 = double doppler etc. */
        _3D_SPEAKERSPREAD,     /* Type : float     - Angle of spread for stereo/mutlichannel source. 0 to 360. */
        _3D_PANLEVEL,          /* Type : float     - 0 = sound pans according to speaker levels, 1 = sound pans according to 3D position. */
        SPEAKER_L,             /* Type : float     - 2D event volume for front left speaker. */
        SPEAKER_C,             /* Type : float     - 2D event volume for front left speaker. */
        SPEAKER_R,             /* Type : float     - 2D event volume for front left speaker. */
        SPEAKER_LS,            /* Type : float     - 2D event volume for front left speaker. */
        SPEAKER_RS,            /* Type : float     - 2D event volume for front left speaker. */
        SPEAKER_LR,            /* Type : float     - 2D event volume for front left speaker. */
        SPEAKER_RR,            /* Type : float     - 2D event volume for front left speaker. */
        SPEAKER_LFE,           /* Type : float     - 2D event volume for front left speaker. */
        REVERBWETLEVEL,        /* Type : float     - Reverb gain for this event where 0 = full reverb, -60 = no reverb. */
        ONESHOT,               /* Type : int       - Oneshot event - stops when no channels playing */
        FADEIN,                /* Type : int       - Time in milliseconds over which to fade this event in when programmer starts it. 0 = no fade in. */
        FADEOUT,               /* Type : int       - Time in milliseconds over which to fade this event out when programmer stops it. 0 = no fade out. */
        REVERBDRYLEVEL,        /* Type : float     - Dry reverb gain for this event where 0 = full dry, -60 = no dry. */
        TIMEOFFSET,            /* Type : float     - Time offset of sound start in seconds (0 to 60.0f) */
		SPAWNINTENSITY,        /* Type : float     - Multiplier for spawn frequency of all sounds in this event. */
        WII_CONTROLLERSPEAKER, /* Type : int       - Wii only. Use 0 to 3 to specify a Wii controller speaker to play this event on, -1 to play on normal Wii speakers. */
        USER_BASE              /* User created events start from here onwards. */
    }


    /*
    [ENUM]
    [
        [DESCRIPTION]
        Flags to pass to EventGroup::loadEventData to determine what to load at the time of calling.
    
        [REMARKS]

        [PLATFORMS]
        Win32, Win64, Linux, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii

        [SEE_ALSO]
        EventGroup::loadEventData
    ]
    */
    public enum EVENT_RESOURCE
    {
        STREAMS_AND_SAMPLES,  /* Open all streams and load all banks into memory, under this group (recursive) */
        STREAMS,              /* Open all streams under this group (recursive).  No samples are loaded. */
        SAMPLES               /* Load all banks into memory, under this group (recursive).  No streams are opened. */
    }


    /*
    [ENUM]
    [
        [DESCRIPTION]   
        These callback types are used with FMOD_EVENT_CALLBACK.

        [REMARKS]
        <b>Note!</b>  Currently the user must call EventSystem::update for these callbacks to trigger!
        <br>
        An FMOD_EVENT_CALLBACKTYPE_SYNCPOINT callback is generated from 'markers' embedded in .wav files.
        These can be created by placing 'markers' in the original source wavs using a tool such as Sound Forge or Cooledit.<br>
        The wavs are then compiled into .FSB files when compiling the audio data using the FMOD designer tool.<br>
        Callbacks will be automatically generated at the correct place in the timeline when these markers are encountered
        which makes it useful for synchronization, lip syncing etc.<br>
        <br>
        An FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_START callback is generated each time a sound definition is played in an event.<br>
        This happens every time a sound definition starts due to the event parameter entering the region specified in the 
        layer created by the sound designer..<br>
        This also happens when sounds are randomly respawned using the random respawn feature in the sound definition 
        properties in FMOD designer.<br>
        <br>
        An FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_END callback is generated when a one-shot sound definition inside an event ends, 
        or when a looping sound definition stops due to the event parameter leaving the region specified in the layer created 
        by the sound designer.<br>
        <br>
        An FMOD_EVENT_CALLBACKTYPE_NET_MODIFIED callback is generated when someone has connected to your running application with 
        FMOD Designer and changed a property within this event, for example volume or pitch.<br>

        [PLATFORMS]
        Win32, Win64, Linux, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii, Wii

        [SEE_ALSO]      
        Event::setCallback
        FMOD_EVENT_CALLBACK
        EventSystem::update
    ]
    */
    public enum EVENT_CALLBACKTYPE
    {
        SYNCPOINT,        /* Called when a syncpoint is encountered.  Can be from wav file markers. */
        SOUNDDEF_START,   /* Called when a sound definition inside an event is triggered. */
        SOUNDDEF_END,     /* Called when a sound definition inside an event ends or is stopped. */
        STOLEN,           /* Called when an event runs out of instances and re-uses an existing event. */
        EVENTFINISHED,    /* Called when a non looping event parameter causes an event stop. */
        NET_MODIFIED,     /* Called when a property of the event has been modified by a network-connected host. */
        SOUNDDEF_CREATE,  /* Called when a programmer sound definition entry is loaded. */
        SOUNDDEF_RELEASE, /* Called when a programmer sound definition entry is unloaded. */
        SOUNDDEF_INFO,    /* Called when a sound definition entry is loaded. */
        EVENTSTARTED      /* Called when an event is started. */
    }


    /*
    [STRUCTURE]
    [
        [DESCRIPTION]
        Structure containing realtime information about a wavebank.

        [REMARKS]

        [PLATFORMS]
        Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

        [SEE_ALSO]      
        EventSystem::getInfo
        FMOD_EVENT_SYSTEMINFO
    ]
    */
    public struct EVENT_WAVEBANKINFO
    {
        public string        name;                  /* [out] Name of this wave bank. */
        public int           streamrefcnt;          /* [out] Number of stream references to this wave bank made by events in this event system. */
        public int           samplerefcnt;          /* [out] Number of sample references to this wave bank made by events in this event system. */
        public int           numstreams;            /* [out] Number of times this wave bank has been opened for streaming. */
        public int           maxstreams;            /* [out] Maximum number of times this wave bank will be opened for streaming. */
        public int           streamsinuse;          /* [out] Number of streams currently in use. */
        public uint          streammemory;          /* [out] Amount of memory (in bytes) used by streams. */
        public uint          samplememory;          /* [out] Amount of memory (in bytes) used by samples. */
    }


    /*
    [STRUCTURE]
    [
        [DESCRIPTION]
        Structure containing realtime information about an event system.

        [REMARKS]

        [PLATFORMS]
        Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

        [SEE_ALSO]      
        EventSystem::getInfo
        FMOD_EVENT_WAVEBANKINFO
    ]
    */
    public struct EVENT_SYSTEMINFO
    {
        public int                      numevents;        /* [out] Total number of events in all event groups in this event system. */
        public int                      eventmemory;      /* [out] Amount of memory (in bytes) used by event hierarchy classes. */
        public int                      numinstances;     /* [out] Total number of event instances in all event groups in this event system. */
        public int                      instancememory;   /* [out] Amount of memory (in bytes) used by all event instances in this event system. */
        public int                      dspmemory;        /* [out] Amount of memory (in bytes) used by event dsp networks. */
        public int                      numwavebanks;     /* [out] Number of wave banks known to this event system. */
        public IntPtr                   wavebankinfo;     /* [out] EVENT_WAVEBANKINFO Array of detailed information on each wave bank. */
        public int                      numplayingevents; /* [in/out] On entry, maximum number of entries in playingevents array. On exit, actual number of entries in playingevents array. Optional. Must be used in conjunction with playingevents. */
        public IntPtr                   playingevents;    /* [in/out] Pointer to an array that will be filled with the event handles of all playing events. Optional. Specify 0 if not needed. Must be used in conjunction with numplayingevents. */

    }


    /*
    [STRUCTURE]
    [
        [DESCRIPTION]
        Structure containing extended information about an event.

        [REMARKS]
        This structure is optional!  Specify 0 or NULL in Event::getInfo if you don't need it!<br>

        [PLATFORMS]
        Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

        [SEE_ALSO]      
        Event::getInfo
    ]
    */
    public struct EVENT_INFO
    {
        public int      memoryused;             /* [out] Amount of memory (in bytes) used by this event. */
        public int      positionms;             /* [out] Time passed in playback of this event instance in milliseconds. */
        public int      lengthms;               /* [out] Length in milliseconds of this event. Note: lengthms will be -1 if the length of the event can't be determined i.e. if it has looping sounds. */
        public int      channelsplaying;        /* [out] Number of channels currently playing in this event instance. */
        public int      instancesactive;        /* [out] Number of event instances currently in use. */
        public IntPtr   wavebanknames;          /* [out] An array containing the names of all wave banks that are referenced by this event. */
        public uint     projectid;              /* [out] The runtime 'EventProject' wide unique identifier for this event. */
        public uint     systemid;               /* [out] The runtime 'EventSystem' wide unique identifier for this event.  This is calculated when single or multiple projects are loaded. */
        public float    audibility;             /* [out] current audibility of event. */
		public int      numinstances;           /* [in/out] On entry, maximum number of entries in instances array. On exit, actual number of entries in instances array, or if instances is null, then it is just the number of instances of this event. Optional. */
		public IntPtr   instances;              /* [in/out] Pointer to an array that will be filled with the current reference-counted event handles of all instances of this event. Optional. Specify 0 if not needed. Must be used in conjunction with numinstances. Note: Due to reference counting, the event instance handles returned here may be different between subsequent calls to this function. If you use these event handles, make sure your code is prepared for them to be invalid! */
    }


    /*
    [STRUCTURE]
    [
        [DESCRIPTION]
        Use this structure with EventSystem::load when more control is needed over loading.

        [REMARKS]
        This structure is optional!  Specify 0 or NULL in EventSystem::load if you don't need it!<br>
        <br>
        Members marked with [in] mean the user sets the value before passing it to the function.<br>
        Members marked with [out] mean FMOD sets the value to be used after the function exits.<br>
        Use sounddefentrylimit to limit the number of sound definition entries - and therefore the amount of wave data - loaded for each sound definition. This feature allows the programmer to implement a "low detail" setting at runtime without needing a seperate "low detail" set of assets.

        [PLATFORMS]
        Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

        [SEE_ALSO]      
        EventSystem::load
    ]
    */
    public struct EVENT_LOADINFO
    {
        public uint     size;                  /* [in] Size of this structure.  This is used so the structure can be expanded in the future and still work on older versions of FMOD Ex. */
        public string   encryptionkey;         /* [in] Optional. Specify 0 to ignore. Key, or 'password' to decrypt a bank.  A sound designer may have encrypted the audio data to protect their sound data from 'rippers'. */
        public float    sounddefentrylimit;    /* [in] Optional. Specify 0 to ignore. A value between 0 -> 1 that is multiplied with the number of sound definition entries in each sound definition in the project being loaded in order to programmatically reduce the number of sound definition entries used at runtime. */
    }


    public delegate RESULT EVENT_CALLBACK (IntPtr eventraw, EVENT_CALLBACKTYPE type, IntPtr param1, IntPtr param2, IntPtr userdata);



    /*
        FMOD EventSystem factory functions.
    */
    public class Event_Factory
    {
        public static RESULT EventSystem_Create(ref EventSystem eventsystem)
        {
            RESULT      result           = RESULT.OK;
            IntPtr      eventsystemraw   = new IntPtr();
            EventSystem eventsystemnew   = null;

            result = FMOD_EventSystem_Create(ref eventsystemraw);
            if (result != RESULT.OK)
            {
                return result;
            }

            eventsystemnew = new EventSystem();
            eventsystemnew.setRaw(eventsystemraw);
            eventsystem = eventsystemnew;

            return result;
        }

        #region importfunctions

        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Create                      (ref IntPtr eventsystem);

        #endregion
    }


    /*
       'EventSystem' API
    */
    public class EventSystem
    {
        // Initialization / system functions.
        public RESULT init                      (int maxchannels, INITFLAG flags, IntPtr extradriverdata)
        {
            return FMOD_EventSystem_Init(eventsystemraw, maxchannels, flags, extradriverdata, EVENT_INIT.NORMAL);
        }
        public RESULT init                      (int maxchannels, INITFLAG flags, IntPtr extradriverdata, EVENT_INIT eventflags)
        {
            return FMOD_EventSystem_Init(eventsystemraw, maxchannels, flags, extradriverdata, eventflags);
        }
        public RESULT release                   ()
        {
            return FMOD_EventSystem_Release(eventsystemraw);
        }
        public RESULT update                    ()
        {
            return FMOD_EventSystem_Update(eventsystemraw);
        }
        public RESULT setMediaPath              (string path)
        {
            return FMOD_EventSystem_SetMediaPath(eventsystemraw, path);
        }
        public RESULT setPluginPath             (string path)
        {
            return FMOD_EventSystem_SetPluginPath(eventsystemraw, path);
        }
        public RESULT getVersion                (ref uint version)
        {
            return FMOD_EventSystem_GetVersion(eventsystemraw, ref version);
        }
        public RESULT getInfo                   (ref EVENT_SYSTEMINFO info)
        {
            return FMOD_EventSystem_GetInfo(eventsystemraw, ref info);
        }
        public RESULT getSystemObject           (ref System system)
        {
            RESULT result   = RESULT.OK;
            IntPtr systemraw   = new IntPtr();
            System systemnew   = null;

            try
            {
                result = FMOD_EventSystem_GetSystemObject(eventsystemraw, ref systemraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (system == null)
            {
                systemnew = new System();
                systemnew.setRaw(systemraw);
                system = systemnew;
            }
            else
            {
                system.setRaw(systemraw);
            }

            return result; 
        }
        public RESULT getMusicSystem(ref MusicSystem musicsystem)
        {
            RESULT result = RESULT.OK;
            IntPtr musicsystemraw = new IntPtr();
            MusicSystem musicsystemnew = null;

            try
            {
                result = FMOD_EventSystem_GetMusicSystem(eventsystemraw, ref musicsystemraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (musicsystem == null)
            {
                musicsystemnew = new MusicSystem();
                musicsystemnew.setRaw(musicsystemraw);
                musicsystem = musicsystemnew;
            }
            else
            {
                musicsystem.setRaw(musicsystemraw);
            }

            return result; 
        }


        // FEV load/unload.                                 
        public RESULT load                      (string filename, ref EVENT_LOADINFO loadinfo, ref EventProject project)
        {
            RESULT result                  = RESULT.OK;
            IntPtr eventprojectraw         = new IntPtr();
            EventProject eventprojectnew   = null;

            try
            {
                result = FMOD_EventSystem_Load(eventsystemraw, filename, ref loadinfo, ref eventprojectraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (project == null)
            {
                eventprojectnew = new EventProject();
                eventprojectnew.setRaw(eventprojectraw);
                project = eventprojectnew;
            }
            else
            {
                project.setRaw(eventprojectraw);
            }

            return result; 
        }
        public RESULT load                      (string filename)
        {
            return FMOD_EventSystem_Load(eventsystemraw, filename, (IntPtr)null, (IntPtr)null);
        }
        public RESULT unload                    ()
        {
            return FMOD_EventSystem_Unload(eventsystemraw);
        }
                                                            
        // Event,EventGroup,EventCategory Retrieval.        
        public RESULT getProject                (string name, ref EventProject project)
        {
            RESULT result                  = RESULT.OK;
            IntPtr eventprojectraw         = new IntPtr();
            EventProject eventprojectnew   = null;

            try
            {
                result = FMOD_EventSystem_GetProject(eventsystemraw, name, ref eventprojectraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (project == null)
            {
                eventprojectnew = new EventProject();
                eventprojectnew.setRaw(eventprojectraw);
                project = eventprojectnew;
            }
            else
            {
                project.setRaw(eventprojectraw);
            }

            return result; 
        }
        public RESULT getProjectByIndex         (int index, ref EventProject project)
        {
            RESULT result                  = RESULT.OK;
            IntPtr eventprojectraw         = new IntPtr();
            EventProject eventprojectnew   = null;

            try
            {
                result = FMOD_EventSystem_GetProjectByIndex(eventsystemraw, index, ref eventprojectraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (project == null)
            {
                eventprojectnew = new EventProject();
                eventprojectnew.setRaw(eventprojectraw);
                project = eventprojectnew;
            }
            else
            {
                project.setRaw(eventprojectraw);
            }

            return result; 
        }
        public RESULT getNumProjects            (ref int numprojects)
        {
            return FMOD_EventSystem_GetNumProjects(eventsystemraw, ref numprojects);
        }
        public RESULT getCategory               (string name, ref EventCategory category)
        {
            RESULT result                    = RESULT.OK;
            IntPtr eventcategoryraw          = new IntPtr();
            EventCategory eventcategorynew   = null;

            try
            {
                result = FMOD_EventSystem_GetCategory(eventsystemraw, name, ref eventcategoryraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (category == null)
            {
                eventcategorynew = new EventCategory();
                eventcategorynew.setRaw(eventcategoryraw);
                category = eventcategorynew;
            }
            else
            {
                category.setRaw(eventcategoryraw);
            }

            return result; 
        }
        public RESULT getCategoryByIndex        (int index, ref EventCategory category)
        {
            RESULT result                    = RESULT.OK;
            IntPtr eventcategoryraw          = new IntPtr();
            EventCategory eventcategorynew   = null;

            try
            {
                result = FMOD_EventSystem_GetCategoryByIndex(eventsystemraw, index, ref eventcategoryraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (category == null)
            {
                eventcategorynew = new EventCategory();
                eventcategorynew.setRaw(eventcategoryraw);
                category = eventcategorynew;
            }
            else
            {
                category.setRaw(eventcategoryraw);
            }

            return result; 
        }
        public RESULT getNumCategories          (ref int numcategories)
        {
            return FMOD_EventSystem_GetNumCategories(eventsystemraw, ref numcategories);
        }
        public RESULT getGroup                  (string name, bool cacheevents, ref EventGroup group)
        {
            RESULT result              = RESULT.OK;
            IntPtr eventgroupraw       = new IntPtr();
            EventGroup eventgroupnew   = null;

            try
            {
                result = FMOD_EventSystem_GetGroup(eventsystemraw, name, (cacheevents ? 1 : 0), ref eventgroupraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (group == null)
            {
                eventgroupnew = new EventGroup();
                eventgroupnew.setRaw(eventgroupraw);
                group = eventgroupnew;
            }
            else
            {
                group.setRaw(eventgroupraw);
            }

            return result; 
        }
        public RESULT getEvent                  (string name, EVENT_MODE mode, ref Event _event)
        {
            RESULT result   = RESULT.OK;
            IntPtr eventraw = new IntPtr();
            Event eventnew  = null;

            try
            {
                result = FMOD_EventSystem_GetEvent(eventsystemraw, name, mode, ref eventraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (_event == null)
            {
                eventnew = new Event();
                eventnew.setRaw(eventraw);
                _event = eventnew;
            }
            else
            {
                _event.setRaw(eventraw);
            }

            return result; 
        }
        public RESULT getEventBySystemID        (uint systemid, EVENT_MODE mode, ref Event _event)
        {
            RESULT result   = RESULT.OK;
            IntPtr eventraw = new IntPtr();
            Event eventnew  = null;

            try
            {
                result = FMOD_EventSystem_GetEventBySystemID(eventsystemraw, systemid, mode, ref eventraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (_event == null)
            {
                eventnew = new Event();
                eventnew.setRaw(eventraw);
                _event = eventnew;
            }
            else
            {
                _event.setRaw(eventraw);
            }

            return result;
        }
        public RESULT getNumEvents              (ref int numevents)
        {
            return FMOD_EventSystem_GetNumEvents(eventsystemraw, ref numevents);
        }

        // Reverb interfaces.
        public RESULT setReverbProperties       (ref REVERB_PROPERTIES prop)
        {
            return FMOD_EventSystem_SetReverbProperties(eventsystemraw, ref prop);
        }
        public RESULT getReverbProperties       (ref REVERB_PROPERTIES prop)
        {
            return FMOD_EventSystem_GetReverbProperties(eventsystemraw, ref prop);
        }

        public RESULT getReverbPreset           (string name, ref REVERB_PROPERTIES prop)
        {
            return FMOD_EventSystem_GetReverbPreset(eventsystemraw, name, ref prop);
        }
        public RESULT getReverbPresetByIndex    (int index,  ref REVERB_PROPERTIES prop)
        {
            return FMOD_EventSystem_GetReverbPresetByIndex(eventsystemraw, index, ref prop);
        }
        public RESULT getNumReverbPresets       (ref int numpresets)
        {
            return FMOD_EventSystem_GetNumReverbPresets(eventsystemraw, ref numpresets);
        }

        public RESULT createReverb              (ref EventReverb reverb)
        {
            RESULT      result    = RESULT.OK;
            IntPtr      reverbraw = new IntPtr();
            EventReverb reverbnew = null;

            try
            {
                result = FMOD_EventSystem_CreateReverb(eventsystemraw, ref reverbraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (reverb == null)
            {
                reverbnew = new EventReverb();
                reverbnew.setRaw(reverbraw);
                reverb = reverbnew;
            }
            else
            {
                reverb.setRaw(reverbraw);
            }

            return result;
        }
        public RESULT setReverbAmbientProperties(ref REVERB_PROPERTIES prop)
        {
            return FMOD_EventSystem_SetReverbAmbientProperties(eventsystemraw, ref prop);
        }
        public RESULT getReverbAmbientProperties(ref REVERB_PROPERTIES prop)
        {
            return FMOD_EventSystem_GetReverbAmbientProperties(eventsystemraw, ref prop);
        }

        // 3D Listener interface.
        public RESULT set3DNumListeners         (int numlisteners)
        {
            return FMOD_EventSystem_Set3DNumListeners(eventsystemraw, numlisteners);
        }
        public RESULT get3DNumListeners         (ref int numlisteners)
        {
            return FMOD_EventSystem_Get3DNumListeners(eventsystemraw, ref numlisteners);
        }
        public RESULT set3DListenerAttributes   (int listener, ref VECTOR pos, ref VECTOR vel, ref VECTOR forward, ref VECTOR up)
        {
            return FMOD_EventSystem_Set3DListenerAttributes(eventsystemraw, listener, ref pos, ref vel, ref forward, ref up);
        }
        public RESULT get3DListenerAttributes   (int listener, ref VECTOR pos, ref VECTOR vel, ref VECTOR forward, ref VECTOR up)
        {
            return FMOD_EventSystem_Get3DListenerAttributes(eventsystemraw, listener, ref pos, ref vel, ref forward, ref up);
        }

        // In memory FSB registration.
        public RESULT registerMemoryFSB         (string filename, IntPtr fsbdata, uint fsbdatalen)
        {
            return FMOD_EventSystem_RegisterMemoryFSB(eventsystemraw, filename, fsbdata, fsbdatalen);
        }
        public RESULT unregisterMemoryFSB       (string filename)
        {
            return FMOD_EventSystem_UnregisterMemoryFSB(eventsystemraw, filename);
        }

        #region importfunctions

        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Init              (IntPtr eventsystem, int maxchannels, INITFLAG flags, IntPtr extradriverdata, EVENT_INIT eventflags);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Release           (IntPtr eventsystem);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Update            (IntPtr eventsystem);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_SetMediaPath      (IntPtr eventsystem, string path);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_SetPluginPath     (IntPtr eventsystem, string path);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetVersion        (IntPtr eventsystem, ref uint version);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetInfo           (IntPtr eventsystem, ref EVENT_SYSTEMINFO info);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetSystemObject   (IntPtr eventsystem, ref IntPtr system);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetMusicSystem    (IntPtr eventsystem, ref IntPtr musicsystem);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Load              (IntPtr eventsystem, string filename, ref EVENT_LOADINFO loadinfo, ref IntPtr project);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Load              (IntPtr eventsystem, string filename, IntPtr loadinfo, IntPtr project);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Unload            (IntPtr eventsystem);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetProject        (IntPtr eventsystem, string name, ref IntPtr project);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetProjectByIndex (IntPtr eventsystem, int index, ref IntPtr project);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetNumProjects    (IntPtr eventsystem, ref int numprojects);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetCategory       (IntPtr eventsystem, string name, ref IntPtr category);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetCategoryByIndex(IntPtr eventsystem, int index, ref IntPtr category);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetNumCategories  (IntPtr eventsystem, ref int numcategories);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetGroup          (IntPtr eventsystem, string name, int cacheevents, ref IntPtr group);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetEvent          (IntPtr eventsystem, string name, EVENT_MODE mode, ref IntPtr _event);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetEventBySystemID(IntPtr eventsystem, uint systemid, EVENT_MODE mode, ref IntPtr _event);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetNumEvents      (IntPtr eventsystem, ref int numevents);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_SetReverbProperties(IntPtr eventsystem, ref REVERB_PROPERTIES prop);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetReverbProperties(IntPtr eventsystem, ref REVERB_PROPERTIES prop);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetReverbPreset   (IntPtr eventsystem, string name, ref REVERB_PROPERTIES prop);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetReverbPresetByIndex(IntPtr eventsystem, int index, ref REVERB_PROPERTIES prop);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetNumReverbPresets(IntPtr eventsystem, ref int numpresets);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_CreateReverb      (IntPtr eventsystem, ref IntPtr eventreverb);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_SetReverbAmbientProperties(IntPtr eventsystem, ref REVERB_PROPERTIES prop);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_GetReverbAmbientProperties(IntPtr eventsystem, ref REVERB_PROPERTIES prop);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Set3DNumListeners (IntPtr eventsystem, int numlisteners);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Get3DNumListeners (IntPtr eventsystem, ref int numlisteners);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Set3DListenerAttributes(IntPtr eventsystem, int listener, ref VECTOR pos, ref VECTOR vel, ref VECTOR forward, ref VECTOR up);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_Get3DListenerAttributes(IntPtr eventsystem, int listener, ref VECTOR pos, ref VECTOR vel, ref VECTOR forward, ref VECTOR up);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_RegisterMemoryFSB (IntPtr eventsystem, string filename, IntPtr fsbdata, uint fsbdatalen);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventSystem_UnregisterMemoryFSB(IntPtr eventsystem, string filename);

        #endregion

        #region wrapperinternal

        private IntPtr eventsystemraw;

        public void setRaw(IntPtr eventsystem)
        {
            eventsystemraw = new IntPtr();
            eventsystemraw = eventsystem;
        }

        public IntPtr getRaw()
        {
            return eventsystemraw;
        }

        #endregion
    }


    /*
       'EventProject' API
    */
    public class EventProject
    {
        public RESULT release            ()
        {
            return FMOD_EventProject_Release(eventprojectraw);
        }
        public RESULT getInfo            (ref int index, ref IntPtr name)
        {
            return FMOD_EventProject_GetInfo(eventprojectraw, ref index, ref name);
        }
        public RESULT getGroup           (string name, bool cacheevents, ref EventGroup group)
        {
            RESULT      result        = RESULT.OK;
            IntPtr      eventgroupraw = new IntPtr();
            EventGroup  eventgroupnew = null;

            try
            {
                result = FMOD_EventProject_GetGroup(eventprojectraw, name, (cacheevents ? 1 : 0), ref eventgroupraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (group == null)
            {
                eventgroupnew = new EventGroup();
                eventgroupnew.setRaw(eventgroupraw);
                group = eventgroupnew;
            }
            else
            {
                group.setRaw(eventgroupraw);
            }

            return result;
        }
        public RESULT getGroupByIndex    (int index, bool cacheevents, ref EventGroup group)
        {
            RESULT      result        = RESULT.OK;
            IntPtr      eventgroupraw = new IntPtr();
            EventGroup  eventgroupnew = null;

            try
            {
                result = FMOD_EventProject_GetGroupByIndex(eventprojectraw, index, (cacheevents ? 1 : 0), ref eventgroupraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (group == null)
            {
                eventgroupnew = new EventGroup();
                eventgroupnew.setRaw(eventgroupraw);
                group = eventgroupnew;
            }
            else
            {
                group.setRaw(eventgroupraw);
            }

            return result;
        }
        public RESULT getNumGroups       (ref int numgroups)
        {
            return FMOD_EventProject_GetNumGroups(eventprojectraw, ref numgroups);
        }
        public RESULT getEvent           (string name, EVENT_MODE mode, ref Event _event)
        {
            RESULT result   = RESULT.OK;
            IntPtr eventraw = new IntPtr();
            Event  eventnew = null;

            try
            {
                result = FMOD_EventProject_GetEvent(eventprojectraw, name, mode, ref eventraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (_event == null)
            {
                eventnew = new Event();
                eventnew.setRaw(eventraw);
                _event = eventnew;
            }
            else
            {
                _event.setRaw(eventraw);
            }

            return result;
        }
        public RESULT getEventByProjectID(uint projectid, EVENT_MODE mode, ref Event _event)
        {
            RESULT result   = RESULT.OK;
            IntPtr eventraw = new IntPtr();
            Event  eventnew = null;

            try
            {
                result = FMOD_EventProject_GetEventByProjectID(eventprojectraw, projectid, mode, ref eventraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (_event == null)
            {
                eventnew = new Event();
                eventnew.setRaw(eventraw);
                _event = eventnew;
            }
            else
            {
                _event.setRaw(eventraw);
            }

            return result;
        }
        public RESULT getNumEvents       (ref int numevents)
        {
            return FMOD_EventProject_GetNumEvents(eventprojectraw, ref numevents);
        }

        #region importfunctions
        
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventProject_Release          (IntPtr eventproject);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventProject_GetInfo          (IntPtr eventproject, ref int index, ref IntPtr name);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventProject_GetGroup         (IntPtr eventproject, string name, int cacheevents, ref IntPtr group);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventProject_GetGroupByIndex  (IntPtr eventproject, int index, int cacheevents, ref IntPtr group);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventProject_GetNumGroups     (IntPtr eventproject, ref int numgroups);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventProject_GetEvent         (IntPtr eventproject, string name, EVENT_MODE mode, ref IntPtr _event);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventProject_GetEventByProjectID(IntPtr eventproject, uint projectid, EVENT_MODE mode, ref IntPtr _event);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventProject_GetNumEvents     (IntPtr eventproject, ref int numevents);

        #endregion

        #region wrapperinternal

        private IntPtr eventprojectraw;

        public void setRaw(IntPtr eventproject)
        {
            eventprojectraw = new IntPtr();
            eventprojectraw = eventproject;
        }

        public IntPtr getRaw()
        {
            return eventprojectraw;
        }

        #endregion
    }


    /*
       'EventGroup' API
    */
    public class EventGroup
    {
        public RESULT getInfo            (ref int index, ref IntPtr name)
        {
            return FMOD_EventGroup_GetInfo(eventgroupraw, ref index, ref name);
        }
        public RESULT loadEventData      ()
        {
            return FMOD_EventGroup_LoadEventData(eventgroupraw, EVENT_RESOURCE.STREAMS_AND_SAMPLES, EVENT_MODE.DEFAULT);
        }
        public RESULT loadEventData      (EVENT_RESOURCE resource, EVENT_MODE mode)
        {
            return FMOD_EventGroup_LoadEventData(eventgroupraw, resource, mode);
        }
        public RESULT freeEventData      ()
        {
            return FMOD_EventGroup_FreeEventData(eventgroupraw, (IntPtr)null);
        }
        public RESULT freeEventData      (ref Event _event)
        {
            return FMOD_EventGroup_FreeEventData(eventgroupraw, _event.getRaw());
        }
        public RESULT getGroup           (string name, bool cacheevents, ref EventGroup group)
        {
            RESULT      result        = RESULT.OK;
            IntPtr      groupraw = new IntPtr();
            EventGroup  groupnew = null;

            try
            {
                result = FMOD_EventGroup_GetGroup(eventgroupraw, name, (cacheevents ? 1 : 0), ref groupraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (group == null)
            {
                groupnew = new EventGroup();
                groupnew.setRaw(groupraw);
                group = groupnew;
            }
            else
            {
                group.setRaw(groupraw);
            }

            return result;
        }
        public RESULT getGroupByIndex    (int index, bool cacheevents, ref EventGroup group)
        {
            RESULT      result        = RESULT.OK;
            IntPtr      groupraw = new IntPtr();
            EventGroup  groupnew = null;

            try
            {
                result = FMOD_EventGroup_GetGroupByIndex(eventgroupraw, index, (cacheevents ? 1 : 0), ref groupraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (group == null)
            {
                groupnew = new EventGroup();
                groupnew.setRaw(groupraw);
                group = groupnew;
            }
            else
            {
                group.setRaw(groupraw);
            }

            return result;
        }
        public RESULT getParentGroup     (ref EventGroup group)
        {
            RESULT      result        = RESULT.OK;
            IntPtr      groupraw = new IntPtr();
            EventGroup  groupnew = null;

            try
            {
                result = FMOD_EventGroup_GetParentGroup(eventgroupraw, ref groupraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (group == null)
            {
                groupnew = new EventGroup();
                groupnew.setRaw(groupraw);
                group = groupnew;
            }
            else
            {
                group.setRaw(groupraw);
            }

            return result;
        }
        public RESULT getParentProject   (ref EventProject project)
        {
            RESULT       result          = RESULT.OK;
            IntPtr       eventprojectraw = new IntPtr();
            EventProject eventprojectnew = null;

            try
            {
                result = FMOD_EventGroup_GetParentProject(eventgroupraw, ref eventprojectraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (project == null)
            {
                eventprojectnew = new EventProject();
                eventprojectnew.setRaw(eventprojectraw);
                project = eventprojectnew;
            }
            else
            {
                project.setRaw(eventprojectraw);
            }

            return result;
        }
        public RESULT getNumGroups       (ref int numgroups)
        {
            return FMOD_EventGroup_GetNumGroups(eventgroupraw, ref numgroups);
        }
        public RESULT getEvent           (string name, EVENT_MODE mode, ref Event _event)
        {
            RESULT result   = RESULT.OK;
            IntPtr eventraw = new IntPtr();
            Event  eventnew = null;

            try
            {
                result = FMOD_EventGroup_GetEvent(eventgroupraw, name, mode, ref eventraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (_event == null)
            {
                eventnew = new Event();
                eventnew.setRaw(eventraw);
                _event = eventnew;
            }
            else
            {
                _event.setRaw(eventraw);
            }

            return result;
        }
        public RESULT getEventByIndex    (int index, EVENT_MODE mode, ref Event _event)
        {
            RESULT result   = RESULT.OK;
            IntPtr eventraw = new IntPtr();
            Event  eventnew = null;

            try
            {
                result = FMOD_EventGroup_GetEventByIndex(eventgroupraw, index, mode, ref eventraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (_event == null)
            {
                eventnew = new Event();
                eventnew.setRaw(eventraw);
                _event = eventnew;
            }
            else
            {
                _event.setRaw(eventraw);
            }

            return result;
        }
        public RESULT getNumEvents       (ref int numevents)
        {
            return FMOD_EventGroup_GetNumEvents(eventgroupraw, ref numevents);
        }
        public RESULT getProperty        (string propertyname, IntPtr _value)
        {
            return FMOD_EventGroup_GetProperty(eventgroupraw, propertyname, _value);
        }
        public RESULT getPropertyByIndex (int propertyindex, IntPtr _value)
        {
            return FMOD_EventGroup_GetPropertyByIndex(eventgroupraw, propertyindex, _value);
        }
        public RESULT getNumProperties   (ref int numproperties)
        {
            return FMOD_EventGroup_GetNumProperties(eventgroupraw, ref numproperties);
        }
        public RESULT getState           (ref EVENT_STATE state)
        {
            return FMOD_EventGroup_GetState(eventgroupraw, ref state);
        }

        #region importfunctions
        
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetInfo            (IntPtr eventgroup, ref int index, ref IntPtr name);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_LoadEventData      (IntPtr eventgroup, EVENT_RESOURCE resource, EVENT_MODE mode);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_FreeEventData      (IntPtr eventgroup, IntPtr _event);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetGroup           (IntPtr eventgroup, string name, int cacheevents, ref IntPtr group);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetGroupByIndex    (IntPtr eventgroup, int index, int cacheevents, ref IntPtr group);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetParentGroup     (IntPtr eventgroup, ref IntPtr group);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetParentProject   (IntPtr eventgroup, ref IntPtr project);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetNumGroups       (IntPtr eventgroup, ref int numgroups);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetEvent           (IntPtr eventgroup, string name, EVENT_MODE mode, ref IntPtr _event);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetEventByIndex    (IntPtr eventgroup, int index, EVENT_MODE mode, ref IntPtr _event);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetNumEvents       (IntPtr eventgroup, ref int numevents);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetProperty        (IntPtr eventgroup, string propertyname, IntPtr _value);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetPropertyByIndex (IntPtr eventgroup, int propertyindex, IntPtr _value);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetNumProperties   (IntPtr eventgroup, ref int numproperties);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventGroup_GetState           (IntPtr eventgroup, ref EVENT_STATE state);

        #endregion

        #region wrapperinternal

        private IntPtr eventgroupraw;

        public void setRaw(IntPtr eventgroup)
        {
            eventgroupraw = new IntPtr();
            eventgroupraw = eventgroup;
        }

        public IntPtr getRaw()
        {
            return eventgroupraw;
        }

        #endregion
    }


    /*
       'EventCategory' API
    */
    public class EventCategory
    {
        public RESULT getInfo            (ref int index, ref IntPtr name)
        {
            return FMOD_EventCategory_GetInfo(eventcategoryraw, ref index, ref name);
        }
        public RESULT getCategory        (string name, ref EventCategory category)
        {
            RESULT      result        = RESULT.OK;
            IntPtr      categoryraw = new IntPtr();
            EventCategory  categorynew = null;

            try
            {
                result = FMOD_EventCategory_GetCategory(eventcategoryraw, name, ref categoryraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (category == null)
            {
                categorynew = new EventCategory();
                categorynew.setRaw(categoryraw);
                category = categorynew;
            }
            else
            {
                category.setRaw(categoryraw);
            }

            return result;
        }
        public RESULT getCategoryByIndex (int index, ref EventCategory category)
        {
            RESULT      result        = RESULT.OK;
            IntPtr      categoryraw = new IntPtr();
            EventCategory  categorynew = null;

            try
            {
                result = FMOD_EventCategory_GetCategoryByIndex(eventcategoryraw, index, ref categoryraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (category == null)
            {
                categorynew = new EventCategory();
                categorynew.setRaw(categoryraw);
                category = categorynew;
            }
            else
            {
                category.setRaw(categoryraw);
            }

            return result;
        }
        public RESULT getNumCategories   (ref int numcategories)
        {
            return FMOD_EventCategory_GetNumCategories(eventcategoryraw, ref numcategories);
        }
        public RESULT getEventByIndex    (int index, EVENT_MODE eventmode, ref Event _event)
        {
            RESULT result   = RESULT.OK;
            IntPtr eventraw = new IntPtr();
            Event  eventnew = null;

            try
            {
                result = FMOD_EventCategory_GetEventByIndex(eventcategoryraw, index, eventmode, ref eventraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (_event == null)
            {
                eventnew = new Event();
                eventnew.setRaw(eventraw);
                _event = eventnew;
            }
            else
            {
                _event.setRaw(eventraw);
            }

            return result;
        }
        public RESULT getNumEvents       (ref int numevents)
        {
            return FMOD_EventCategory_GetNumEvents(eventcategoryraw, ref numevents);
        }

        public RESULT stopAllEvents      ()
        {
            return FMOD_EventCategory_StopAllEvents(eventcategoryraw);
        }
        public RESULT setVolume          (float volume)
        {
            return FMOD_EventCategory_SetVolume(eventcategoryraw, volume);
        }
        public RESULT getVolume          (ref float volume)
        {
            return FMOD_EventCategory_GetVolume(eventcategoryraw, ref volume);
        }
        public RESULT setPitch           (float pitch)
        {
            return FMOD_EventCategory_SetPitch(eventcategoryraw, pitch);
        }
        public RESULT getPitch           (ref float pitch)
        {
            return FMOD_EventCategory_GetPitch(eventcategoryraw, ref pitch);
        }
        public RESULT setPaused          (bool paused)
        {
            return FMOD_EventCategory_SetPaused(eventcategoryraw, (paused ? 1 : 0));
        }
        public RESULT getPaused          (ref bool paused)
        {
            RESULT result = RESULT.OK;
            int p = 0;

            result = FMOD_EventCategory_GetPaused(eventcategoryraw, ref p);

            paused = (p != 0);

            return result;
        }
        public RESULT setMute            (bool mute)
        {
            return FMOD_EventCategory_SetMute(eventcategoryraw, (mute ? 1 : 0));
        }
        public RESULT getMute            (ref bool mute)
        {
            RESULT result = RESULT.OK;
            int m = 0;

            result = FMOD_EventCategory_GetMute(eventcategoryraw, ref m);

            mute = (m != 0);

            return result;
        }
        public RESULT getChannelGroup    (ref ChannelGroup channelgroup)
        {
            RESULT result   = RESULT.OK;
            IntPtr cgraw = new IntPtr();
            ChannelGroup  cgnew = null;

            try
            {
                result = FMOD_EventCategory_GetChannelGroup(eventcategoryraw, ref cgraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (channelgroup == null)
            {
                cgnew = new ChannelGroup();
                cgnew.setRaw(cgraw);
                channelgroup = cgnew;
            }
            else
            {
                channelgroup.setRaw(cgraw);
            }

            return result;
        }

        #region importfunctions

        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetInfo         (IntPtr eventcategory, ref int index, ref IntPtr name);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetCategory     (IntPtr eventcategory, string name, ref IntPtr category);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetCategoryByIndex(IntPtr eventcategory, int index, ref IntPtr category);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetNumCategories(IntPtr eventcategory, ref int numcategories);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetEventByIndex (IntPtr eventcategory, int index, EVENT_MODE eventmode, ref IntPtr _event);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetNumEvents    (IntPtr eventcategory, ref int numevents);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_StopAllEvents   (IntPtr eventcategory);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_SetVolume       (IntPtr eventcategory, float volume);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetVolume       (IntPtr eventcategory, ref float volume);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_SetPitch        (IntPtr eventcategory, float pitch);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetPitch        (IntPtr eventcategory, ref float pitch);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_SetPaused       (IntPtr eventcategory, int paused);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetPaused       (IntPtr eventcategory, ref int paused);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_SetMute         (IntPtr eventcategory, int mute);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetMute         (IntPtr eventcategory, ref int mute);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventCategory_GetChannelGroup (IntPtr eventcategory, ref IntPtr channelgroup);

        #endregion

        #region wrapperinternal

        private IntPtr eventcategoryraw;

        public void setRaw(IntPtr eventcategory)
        {
            eventcategoryraw = new IntPtr();
            eventcategoryraw = eventcategory;
        }

        public IntPtr getRaw()
        {
            return eventcategoryraw;
        }

        #endregion
    }


    /*
       'Event' API
    */
    public class Event
    {
        public RESULT start                      ()
        {
            return FMOD_Event_Start(eventraw);
        }
        public RESULT stop                       ()
        {
            return FMOD_Event_Stop(eventraw, 1);
        }
        public RESULT stop                       (bool immediate)
        {
            return FMOD_Event_Stop(eventraw, (immediate ? 1 : 0));
        }

        public RESULT getInfo                    (ref int index, ref IntPtr name, ref EVENT_INFO info)
        {
            return FMOD_Event_GetInfo(eventraw, ref index, ref name, ref info);
        }
        public RESULT getState                   (ref EVENT_STATE state)
        {
            return FMOD_Event_GetState(eventraw, ref state);
        }
        public RESULT getParentGroup             (ref EventGroup group)
        {
            RESULT result            = RESULT.OK;
            IntPtr eventgroupraw     = new IntPtr();
            EventGroup eventgroupnew = null;

            try
            {
                result = FMOD_Event_GetParentGroup(eventraw, ref eventgroupraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (group == null)
            {
                eventgroupnew = new EventGroup();
                eventgroupnew.setRaw(eventgroupraw);
                group = eventgroupnew;
            }
            else
            {
                group.setRaw(eventgroupraw);
            }

            return result;
        }
        public RESULT getChannelGroup            (ref ChannelGroup channelgroup)
        {
            RESULT result   = RESULT.OK;
            IntPtr cgraw = new IntPtr();
            ChannelGroup  cgnew = null;

            try
            {
                result = FMOD_Event_GetChannelGroup(eventraw, ref cgraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (channelgroup == null)
            {
                cgnew = new ChannelGroup();
                cgnew.setRaw(cgraw);
                channelgroup = cgnew;
            }
            else
            {
                channelgroup.setRaw(cgraw);
            }

            return result;
        }
        public RESULT setCallback                (EVENT_CALLBACK callback, IntPtr userdata)
        {
            return FMOD_Event_SetCallback(eventraw, callback, userdata);
        }

        public RESULT getParameter               (string name, ref EventParameter parameter)
        {
            RESULT result               = RESULT.OK;
            IntPtr parameterraw         = new IntPtr();
            EventParameter parameternew = null;

            try
            {
                result = FMOD_Event_GetParameter(eventraw, name, ref parameterraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (parameter == null)
            {
                parameternew = new EventParameter();
                parameternew.setRaw(parameterraw);
                parameter = parameternew;
            }
            else
            {
                parameter.setRaw(parameterraw);
            }

            return result;
        }
        public RESULT getParameterByIndex        (int index, ref EventParameter parameter)
        {
            RESULT result               = RESULT.OK;
            IntPtr parameterraw         = new IntPtr();
            EventParameter parameternew = null;

            try
            {
                result = FMOD_Event_GetParameterByIndex(eventraw, index, ref parameterraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (parameter == null)
            {
                parameternew = new EventParameter();
                parameternew.setRaw(parameterraw);
                parameter = parameternew;
            }
            else
            {
                parameter.setRaw(parameterraw);
            }

            return result;
        }
        public RESULT getNumParameters           (ref int numparameters)
        {
            return FMOD_Event_GetNumParameters(eventraw, ref numparameters);
        }
        public RESULT getProperty                (string propertyname, IntPtr _value, bool this_instance)
        {
            return FMOD_Event_GetProperty(eventraw, propertyname, _value, (this_instance ? 1 : 0));
        }
        public RESULT getPropertyByIndex         (int propertyindex, IntPtr _value, bool this_instance)
        {
            return FMOD_Event_GetPropertyByIndex(eventraw, propertyindex, _value, (this_instance ? 1 : 0));
        }
        public RESULT setProperty                (string propertyname, IntPtr _value, bool this_instance)
        {
            return FMOD_Event_SetProperty(eventraw, propertyname, _value, (this_instance ? 1 : 0));
        }
        public RESULT setPropertyByIndex         (int propertyindex, IntPtr _value, bool this_instance)
        {
            return FMOD_Event_SetPropertyByIndex(eventraw, propertyindex, _value, (this_instance ? 1 : 0));
        }
        public RESULT getNumProperties           (ref int numproperties)
        {
            return FMOD_Event_GetNumProperties(eventraw, ref numproperties);
        }
        public RESULT getCategory                (ref EventCategory category)
        {
            RESULT result             = RESULT.OK;
            IntPtr categoryraw        = new IntPtr();
            EventCategory categorynew = null;

            try
            {
                result = FMOD_Event_GetCategory(eventraw, ref categoryraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (category == null)
            {
                categorynew = new EventCategory();
                categorynew.setRaw(categoryraw);
                category = categorynew;
            }
            else
            {
                category.setRaw(categoryraw);
            }

            return result;
        }

        public RESULT setVolume                  (float volume)
        {
            return FMOD_Event_SetVolume(eventraw, volume);
        }
        public RESULT getVolume                  (ref float volume)
        {
            return FMOD_Event_GetVolume(eventraw, ref volume);
        }
        public RESULT setPitch                   (float pitch)
        {
            return FMOD_Event_SetPitch(eventraw, pitch);
        }
        public RESULT getPitch                   (ref float pitch)
        {
            return FMOD_Event_GetPitch(eventraw, ref pitch);
        }
        public RESULT setPaused                  (bool paused)
        {
            return FMOD_Event_SetPaused(eventraw, (paused ? 1 : 0));
        }
        public RESULT getPaused                  (ref bool paused)
        {
            RESULT result = RESULT.OK;
            int p = 0;

            result = FMOD_Event_GetPaused(eventraw, ref p);

            paused = (p != 0);

            return result;
        }
        public RESULT setMute                    (bool mute)
        {
            return FMOD_Event_SetMute(eventraw, (mute ? 1 : 0));
        }
        public RESULT getMute                    (ref bool mute)
        {
            RESULT result = RESULT.OK;
            int m = 0;

            result = FMOD_Event_GetMute(eventraw, ref m);

            mute = (m != 0);

            return result;
        }
        public RESULT set3DAttributes            (ref VECTOR position, ref VECTOR velocity)
        {
            return FMOD_Event_Set3DAttributes(eventraw, ref position, ref velocity, (IntPtr)null);
        }
        public RESULT get3DAttributes            (ref VECTOR position, ref VECTOR velocity)
        {
            return FMOD_Event_Get3DAttributes(eventraw, ref position, ref velocity, (IntPtr)null);
        }
        public RESULT set3DAttributes            (ref VECTOR position, ref VECTOR velocity, ref VECTOR orientation)
        {
            return FMOD_Event_Set3DAttributes(eventraw, ref position, ref velocity, ref orientation);
        }
        public RESULT get3DAttributes            (ref VECTOR position, ref VECTOR velocity, ref VECTOR orientation)
        {
            return FMOD_Event_Get3DAttributes(eventraw, ref position, ref velocity, ref orientation);
        }
        public RESULT set3DOcclusion             (float directocclusion, float reverbocclusion)
        {
            return FMOD_Event_Set3DOcclusion(eventraw, directocclusion, reverbocclusion);
        }
        public RESULT get3DOcclusion             (ref float directocclusion, ref float reverbocclusion)
        {
            return FMOD_Event_Get3DOcclusion(eventraw, ref directocclusion, ref reverbocclusion);
        }
        public RESULT setReverbProperties        (ref REVERB_CHANNELPROPERTIES prop)
        {
            return FMOD_Event_SetReverbProperties(eventraw, ref prop);
        }
        public RESULT getReverbProperties        (ref REVERB_CHANNELPROPERTIES prop)
        {
            return FMOD_Event_GetReverbProperties(eventraw, ref prop);
        }

        #region importfunctions

        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_Start                   (IntPtr _event);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_Stop                    (IntPtr _event, int immediate);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetInfo                 (IntPtr _event, ref int index, ref IntPtr name, ref EVENT_INFO info);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetState                (IntPtr _event, ref EVENT_STATE state);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetParentGroup          (IntPtr _event, ref IntPtr group);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetChannelGroup         (IntPtr _event, ref IntPtr channelgroup);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_SetCallback             (IntPtr _event, EVENT_CALLBACK callback, IntPtr userdata);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetParameter            (IntPtr _event, string name, ref IntPtr parameter);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetParameterByIndex     (IntPtr _event, int index, ref IntPtr parameter);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetNumParameters        (IntPtr _event, ref int numparameters);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetProperty             (IntPtr _event, string propertyname, IntPtr _value, int this_instance);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetPropertyByIndex      (IntPtr _event, int propertyindex, IntPtr _value, int this_instance);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_SetProperty             (IntPtr _event, string propertyname, IntPtr _value, int this_instance);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_SetPropertyByIndex      (IntPtr _event, int propertyindex, IntPtr _value, int this_instance);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetNumProperties        (IntPtr _event, ref int numproperties);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetCategory             (IntPtr _event, ref IntPtr category);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_SetVolume               (IntPtr _event, float volume);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetVolume               (IntPtr _event, ref float volume);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_SetPitch                (IntPtr _event, float pitch);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetPitch                (IntPtr _event, ref float pitch);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_SetPaused               (IntPtr _event, int paused);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetPaused               (IntPtr _event, ref int paused);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_SetMute                 (IntPtr _event, int mute);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetMute                 (IntPtr _event, ref int mute);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_Set3DAttributes         (IntPtr _event, ref VECTOR position, ref VECTOR velocity, IntPtr orientation);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_Get3DAttributes         (IntPtr _event, ref VECTOR position, ref VECTOR velocity, IntPtr orientation);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_Set3DAttributes         (IntPtr _event, ref VECTOR position, ref VECTOR velocity, ref VECTOR orientation);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_Get3DAttributes         (IntPtr _event, ref VECTOR position, ref VECTOR velocity, ref VECTOR orientation);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_Set3DOcclusion          (IntPtr _event, float directocclusion, float reverbocclusion);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_Get3DOcclusion          (IntPtr _event, ref float directocclusion, ref float reverbocclusion);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_SetReverbProperties     (IntPtr _event, ref REVERB_CHANNELPROPERTIES prop);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_Event_GetReverbProperties     (IntPtr _event, ref REVERB_CHANNELPROPERTIES prop);

        #endregion

        #region wrapperinternal

        private IntPtr eventraw;

        public void setRaw(IntPtr _event)
        {
            eventraw = new IntPtr();
            eventraw = _event;
        }

        public IntPtr getRaw()
        {
            return eventraw;
        }

        #endregion
    }


    /*
       'EventParameter' API
    */
    public class EventParameter
    {
        public RESULT getInfo                    (ref int index, ref IntPtr name)
        {
            return FMOD_EventParameter_GetInfo(eventparameterraw, ref index, ref name);
        }
        public RESULT getRange                   (ref float rangemin, ref float rangemax)
        {
            return FMOD_EventParameter_GetRange(eventparameterraw, ref rangemin, ref rangemax);
        }
        public RESULT setValue                   (float _value)
        {
            return FMOD_EventParameter_SetValue(eventparameterraw, _value);
        }
        public RESULT getValue                   (ref float _value)
        {
            return FMOD_EventParameter_GetValue(eventparameterraw, ref _value);
        }
        public RESULT setVelocity                (float _value)
        {
            return FMOD_EventParameter_SetVelocity(eventparameterraw, _value);
        }
        public RESULT getVelocity                (ref float _value)
        {
            return FMOD_EventParameter_GetVelocity(eventparameterraw, ref _value);
        }
        public RESULT setSeekSpeed               (float _value)
        {
            return FMOD_EventParameter_SetSeekSpeed(eventparameterraw, _value);
        }
        public RESULT getSeekSpeed               (ref float _value)
        {
            return FMOD_EventParameter_GetSeekSpeed(eventparameterraw, ref _value);
        }
        public RESULT keyOff                     ()
        {
            return FMOD_EventParameter_KeyOff(eventparameterraw);
        }

        #region importfunctions

        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventParameter_GetInfo        (IntPtr eventparameter , ref int index, ref IntPtr name);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventParameter_GetRange       (IntPtr eventparameter, ref float rangemin, ref float rangemax);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventParameter_SetValue       (IntPtr eventparameter, float _value);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventParameter_GetValue       (IntPtr eventparameter, ref float _value);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventParameter_SetVelocity    (IntPtr eventparameter, float _value);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventParameter_GetVelocity    (IntPtr eventparameter, ref float _value);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventParameter_SetSeekSpeed   (IntPtr eventparameter, float _value);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventParameter_GetSeekSpeed   (IntPtr eventparameter, ref float _value);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventParameter_KeyOff         (IntPtr eventparameter);

        #endregion

        #region wrapperinternal

        private IntPtr eventparameterraw;

        public void setRaw(IntPtr eventparameter)
        {
            eventparameterraw = new IntPtr();
            eventparameterraw = eventparameter;
        }

        public IntPtr getRaw()
        {
            return eventparameterraw;
        }

        #endregion
    }


    /*
       'EventReverb ' API
    */
    public class EventReverb
    {
        public RESULT release            ()
        {
            return FMOD_EventReverb_Release(eventreverbraw);
        }
        public RESULT set3DAttributes    (ref VECTOR position, float mindistance, float maxdistance)
        {
            return FMOD_EventReverb_Set3DAttributes(eventreverbraw, ref position, mindistance, maxdistance);
        }
        public RESULT get3DAttributes    (ref VECTOR position, ref float mindistance, ref float maxdistance)
        {
            return FMOD_EventReverb_Get3DAttributes(eventreverbraw, ref position, ref mindistance, ref maxdistance);
        }
        public RESULT setProperties      (ref REVERB_PROPERTIES props)
        {
            return FMOD_EventReverb_SetProperties(eventreverbraw, ref props);
        }
        public RESULT getProperties      (ref REVERB_PROPERTIES props)
        {
            return FMOD_EventReverb_GetProperties(eventreverbraw, ref props);
        }
        public RESULT setActive          (bool active)
        {
            return FMOD_EventReverb_SetActive(eventreverbraw, (active ? 1 : 0));
        }
        public RESULT getActive          (ref bool active)
        {
            RESULT result = RESULT.OK;
            int a = 0;

            result = FMOD_EventReverb_GetActive(eventreverbraw, ref a);

            active = (a != 0);

            return result;
        }

        #region importfunctions

        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventReverb_Release           (IntPtr eventreverb);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventReverb_Set3DAttributes   (IntPtr eventreverb, ref VECTOR position, float mindistance, float maxdistance);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventReverb_Get3DAttributes   (IntPtr eventreverb, ref VECTOR position, ref float mindistance, ref float maxdistance);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventReverb_SetProperties     (IntPtr eventreverb, ref REVERB_PROPERTIES props);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventReverb_GetProperties     (IntPtr eventreverb, ref REVERB_PROPERTIES props);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventReverb_SetActive         (IntPtr eventreverb, int active);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_EventReverb_GetActive         (IntPtr eventreverb, ref int active);

        #endregion

        #region wrapperinternal

        private IntPtr eventreverbraw;

        public void setRaw(IntPtr eventreverb)
        {
            eventreverbraw = new IntPtr();
            eventreverbraw = eventreverb;
        }

        public IntPtr getRaw()
        {
            return eventreverbraw;
        }

        #endregion
    }

    /*
        'MusicSystem' API 
    */
    public class MusicSystem
    {
        public RESULT reset              ()
        {
            return FMOD_MusicSystem_Reset(musicsystemraw);
        }
        public RESULT setVolume          (float volume)
        {
            return FMOD_MusicSystem_SetVolume(musicsystemraw, volume);
        }
        public RESULT getVolume          (ref float volume)
        {
            return FMOD_MusicSystem_GetVolume(musicsystemraw, ref volume);
        }
        public RESULT setPaused          (bool paused)
        {
            return FMOD_MusicSystem_SetPaused(musicsystemraw, paused ? 1 : 0);
        }
        public RESULT getPaused          (ref bool paused)
        {
            RESULT result;
            int p = 0;

            result = FMOD_MusicSystem_GetPaused(musicsystemraw, ref p);

            paused = (p != 0);

            return result;
        }
        public RESULT setMute            (bool mute)
        {
            return FMOD_MusicSystem_SetMute(musicsystemraw, mute ? 1 : 0);
        }
        public RESULT getMute            (ref bool mute)
        {
            RESULT result;
            int m = 0;

            result =  FMOD_MusicSystem_GetMute(musicsystemraw, ref m);

            mute = (m != 0);

            return result;
        }
        public RESULT promptCue          (uint music_cue_id)
        {
            return FMOD_MusicSystem_PromptCue(musicsystemraw, music_cue_id);
        }
        public RESULT prepareCue         (uint music_cue_id, ref MusicPrompt prompt)
        {
            RESULT result = RESULT.OK;
            IntPtr mpraw = new IntPtr();
            MusicPrompt mpnew = null;

            try
            {
                result = FMOD_MusicSystem_PrepareCue(musicsystemraw, music_cue_id, ref mpraw);
            }
            catch
            {
                result = RESULT.ERR_INVALID_PARAM;
            }
            if (result != RESULT.OK)
            {
                return result;
            }

            if (prompt == null)
            {
                mpnew = new MusicPrompt();
                mpnew.setRaw(mpraw);
                prompt = mpnew;
            }
            else
            {
                prompt.setRaw(mpraw);
            }

            return result;
        }
        public RESULT getParameter       (uint music_param_id, ref float parameter)
        {
            return FMOD_MusicSystem_GetParameter(musicsystemraw, music_param_id, ref parameter);
        }
        public RESULT setParameter       (uint music_param_id, float parameter)
        {
            return FMOD_MusicSystem_SetParameter(musicsystemraw, music_param_id, parameter);
        }
        public RESULT getCueID           (String name, ref uint music_cue_id)
        {
            return FMOD_MusicSystem_GetCueID(musicsystemraw, name, ref music_cue_id);
        }
        public RESULT getParameterID     (String name, ref uint music_param_id)
        {
            return FMOD_MusicSystem_GetParameterID(musicsystemraw, name, ref music_param_id);
        }

        #region importfunctions

        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_Reset             (IntPtr musicsystem);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_SetVolume         (IntPtr musicsystem, float volume);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_GetVolume         (IntPtr musicsystem, ref float volume);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_SetPaused         (IntPtr musicsystem, int paused);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_GetPaused         (IntPtr musicsystem, ref int paused);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_SetMute           (IntPtr musicsystem, int mute);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_GetMute           (IntPtr musicsystem, ref int mute);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_PromptCue         (IntPtr musicsystem, uint id);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_PrepareCue        (IntPtr musicsystem, uint id, ref IntPtr prompt);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_GetParameter      (IntPtr musicsystem, uint id, ref float parameter);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_SetParameter      (IntPtr musicsystem, uint id, float parameter);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_GetCueID          (IntPtr musicsystem, String name, ref uint id);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicSystem_GetParameterID    (IntPtr musicsystem, String name, ref uint id);

        #endregion

        #region wrapperinternal

        private IntPtr musicsystemraw;

        public void setRaw(IntPtr musicsystem)
        {
            musicsystemraw = new IntPtr();
            musicsystemraw = musicsystem;
        }

        public IntPtr getRaw()
        {
            return musicsystemraw;
        }

        #endregion
    }

    /*
        'MusicPrompt' API
    */
    public class MusicPrompt
    {
        public RESULT release            ()
        {
            return FMOD_MusicPrompt_Release(musicpromptraw);
        }
        public RESULT begin              ()
        {
            return FMOD_MusicPrompt_Begin(musicpromptraw);
        }
        public RESULT end                ()
        {
            return FMOD_MusicPrompt_End(musicpromptraw);
        }
        public RESULT isActive(ref bool active)
        {
            RESULT result;
            int a = 0;

            result = FMOD_MusicPrompt_IsActive(musicpromptraw, ref a);

            active = (a != 0);

            return result;
        }

        #region importfunctions

        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicPrompt_Release           (IntPtr musicprompt);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicPrompt_Begin             (IntPtr musicprompt);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicPrompt_End               (IntPtr musicprompt);
        [DllImport (EVENT_VERSION.dll)]
        private static extern RESULT FMOD_MusicPrompt_IsActive          (IntPtr musicprompt, ref int active);

        #endregion

        #region wrapperinternal

        private IntPtr musicpromptraw;

        public void setRaw(IntPtr musicprompt)
        {
            musicpromptraw = new IntPtr();
            musicpromptraw = musicprompt;
        }

        public IntPtr getRaw()
        {
            return musicpromptraw;
        }

        #endregion
    }
}