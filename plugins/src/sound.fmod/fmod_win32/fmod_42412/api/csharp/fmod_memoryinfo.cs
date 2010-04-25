/* ======================================================================================== */
/* FMOD Ex - Memory info header file. Copyright (c), Firelight Technologies Pty, Ltd. 2008. */
/*                                                                                          */
/* Use this header if you are interested in getting detailed information on FMOD's memory   */
/* usage. See the documentation for more details.                                           */
/*                                                                                          */
/* ======================================================================================== */

namespace FMOD
{

/*
[ENUM]
[
    [DESCRIPTION]
    Description of "memoryused_array" fields returned by the getMemoryInfo function of every public FMOD class.

    [REMARKS]
    Every public FMOD class has a getMemoryInfo function which can be used to get detailed information on what memory resources are associated with the object in question. 
    The FMOD_MEMTYPE enumeration values can be used to address the "memoryused_array" returned by getMemoryInfo. See System::getMemoryInfo for an example.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii, Solaris

    [SEE_ALSO]
    System::getMemoryInfo
    EventSystem::getMemoryInfo
    FMOD_MEMBITS
    FMOD_EVENT_MEMBITS
]
*/
public enum MEMTYPE
{
    OTHER                 = 0,      /* Memory not accounted for by other types */
    STRING                = 1,      /* String data */

    SYSTEM                = 2,      /* System object and various internals */
    RECORDBUFFER          = 3,      /* Buffer used to store recorded data from microphone */
    PLUGINS               = 4,      /* Plugin objects and internals */
    OUTPUTMODULE          = 5,      /* Output module object and internals */
    SPEAKERLEVELSPOOL     = 6,      /* Pool of speaker levels structs */
    CHANNELI              = 7,      /* Channel object */
    CHANNELPOOL           = 8,      /* Channel pool data */
    CHANNELREAL           = 9,      /* Real channel objects */
    CHANNELGROUP          = 10,     /* ChannelGroup objects and internals */
    SOUNDI                = 11,     /* Sound objects and internals */
    SOUNDGROUP            = 12,     /* SoundGroup objects and internals */
    SAMPLE                = 13,     /* Sample objects */
    DSPCONNECTION         = 14,     /* DSPConnection objects and internals */
    DSPCONNECTIONPOOL     = 15,     /* DSPConnection pool data */
    DSPI                  = 16,     /* DSP implementation objects */
    DSPCODEC              = 17,     /* Realtime file format decoding DSP objects */
    DSPUNIT               = 18,     /* DSP unit objects */
    REVERBI               = 19,     /* Reverb implementation objects */
    REVERBCHANNELPROPS    = 20,     /* Reverb channel properties structs */
    GEOMETRY              = 21,     /* Geometry objects and internals */

    EVENTSYSTEM           = 22,     /* EventSystem and various internals */
    MUSICSYSTEM           = 23,     /* MusicSystem and various internals */
    FEV                   = 24,     /* Definition of objects contained in all loaded projects e.g. events, groups, categories */
    MEMORYFSB             = 25,     /* Data loaded with registerMemoryFSB */
    EVENTPROJECT          = 26,     /* EventProject objects and internals */
    EVENTGROUPI           = 27,     /* EventGroup objects and internals */
    SOUNDBANKCLASS        = 28,     /* Objects used to manage wave banks */
    SOUNDBANKLIST         = 29,     /* Data used to manage lists of wave bank usage */
    STREAMINSTANCE        = 20,     /* Stream objects and internals */
    SOUNDDEFCLASS         = 31,     /* Sound definition objects */
    SOUNDDEFDEFCLASS      = 32,     /* Sound definition static data objects */
    SOUNDDEFPOOL          = 33,     /* Sound definition pool data */
    REVERBDEF             = 34,     /* Reverb definition objects */
    EVENTREVERB           = 35,     /* Reverb objects */
    USERPROPERTY          = 36,     /* User property objects */
    EVENTINSTANCE         = 37,     /* Event instance base objects */
    EVENTINSTANCE_COMPLEX = 38,     /* Complex event instance objects */
    EVENTINSTANCE_SIMPLE  = 39,     /* Simple event instance objects */
    EVENTINSTANCE_LAYER   = 30,     /* Event layer instance objects */
    EVENTINSTANCE_SOUND   = 41,     /* Event sound instance objects */
    EVENTENVELOPE         = 42,     /* Event envelope objects */
    EVENTENVELOPEDEF      = 43,     /* Event envelope definition objects */
    EVENTPARAMETER        = 44,     /* Event parameter objects */
    EVENTCATEGORY         = 45,     /* Event category objects */
    EVENTENVELOPEPOINT    = 46,     /* Event envelope point objects */

    MAX,                            /* Number of "memoryused_array" fields. */
}


/*
[DEFINE]
[
    [NAME]
    FMOD_MEMBITS

    [DESCRIPTION]
    Bitfield used to request specific memory usage information from the getMemoryInfo function of every public FMOD Ex class.<br>
    Use with the "memorybits" parameter of getMemoryInfo to get information on FMOD Ex memory usage.

    [REMARKS]
    Every public FMOD class has a getMemoryInfo function which can be used to get detailed information on what memory resources are associated with the object in question. 
    The FMOD_MEMBITS defines can be OR'd together to specify precisely what memory usage you'd like to get information on. See System::getMemoryInfo for an example.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii, Solaris

    [SEE_ALSO]
    FMOD_EVENT_MEMBITS
    EventSystem::getMemoryInfo
]
*/
public class MEMBITS
{
    public const uint OTHER                     = (1 << MEMTYPE.OTHER);               /* Memory not accounted for by other types */
    public const uint STRING                    = (1 << MEMTYPE.STRING);              /* String data */

    public const uint SYSTEM                    = (1 << MEMTYPE.SYSTEM);              /* System object and various internals */
    public const uint RECORDBUFFER              = (1 << MEMTYPE.RECORDBUFFER);        /* Buffer used to store recorded data from microphone */
    public const uint PLUGINS                   = (1 << MEMTYPE.PLUGINS);             /* Plugin objects and internals */
    public const uint OUTPUTMODULE              = (1 << MEMTYPE.OUTPUTMODULE);        /* Output module object and internals */
    public const uint SPEAKERLEVELSPOOL         = (1 << MEMTYPE.SPEAKERLEVELSPOOL);   /* Pool of speaker levels structs */
    public const uint CHANNELI                  = (1 << MEMTYPE.CHANNELI);            /* Channel object */
    public const uint CHANNELPOOL               = (1 << MEMTYPE.CHANNELPOOL);         /* Channel pool data */
    public const uint CHANNELREAL               = (1 << MEMTYPE.CHANNELREAL);         /* Real channel objects */
    public const uint CHANNELGROUP              = (1 << MEMTYPE.CHANNELGROUP);        /* ChannelGroup objects and internals */
    public const uint SOUNDI                    = (1 << MEMTYPE.SOUNDI);              /* Sound objects and internals */
    public const uint SOUNDGROUP                = (1 << MEMTYPE.SOUNDGROUP);          /* SoundGroup objects and internals */
    public const uint SAMPLE                    = (1 << MEMTYPE.SAMPLE);              /* Sample objects */
    public const uint DSPCONNECTION             = (1 << MEMTYPE.DSPCONNECTION);       /* DSPConnection objects and internals */
    public const uint DSPCONNECTIONPOOL         = (1 << MEMTYPE.DSPCONNECTIONPOOL);   /* DSPConnection pool data */
    public const uint DSPI                      = (1 << MEMTYPE.DSPI);                /* DSP implementation objects */
    public const uint DSPCODEC                  = (1 << MEMTYPE.DSPCODEC);            /* Realtime file format decoding DSP objects */
    public const uint DSPUNIT                   = (1 << MEMTYPE.DSPUNIT);             /* DSP unit objects */
    public const uint REVERBI                   = (1 << MEMTYPE.REVERBI);             /* Reverb implementation objects */
    public const uint REVERBCHANNELPROPS        = (1 << MEMTYPE.REVERBCHANNELPROPS);  /* Reverb channel properties structs */
    public const uint GEOMETRY                  = (1 << MEMTYPE.GEOMETRY);            /* Geometry objects and internals */
    public const uint ALL                       = 0xffffffff;                 /* All memory used by FMOD Ex */
}

/*
[DEFINE]
[
    [NAME]
    FMOD_EVENT_MEMBITS

    [DESCRIPTION]   
    Bitfield used to request specific memory usage information from the getMemoryInfo function of every public FMOD Event System class.<br>
    Use with the "event_memorybits" parameter of getMemoryInfo to get information on FMOD Event System memory usage.

    [REMARKS]
    Every public FMOD Event System class has a getMemoryInfo function which can be used to get detailed information on what memory resources are associated with the object in question. 
    The FMOD_EVENT_MEMBITS defines can be OR'd together to specify precisely what memory usage you'd like to get information on. See EventSystem::getMemoryInfo for an example.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii, Solaris

    [SEE_ALSO]
    FMOD_MEMBITS
    System::getMemoryInfo
]
*/
public class EVENT_MEMBITS
{
    public const uint EVENTSYSTEM          = (1 << (MEMTYPE.EVENTSYSTEM           - MEMTYPE.EVENTSYSTEM));  /* EventSystem and various internals */
    public const uint MUSICSYSTEM          = (1 << (MEMTYPE.MUSICSYSTEM           - MEMTYPE.EVENTSYSTEM));  /* MusicSystem and various internals */
    public const uint FEV                  = (1 << (MEMTYPE.FEV                   - MEMTYPE.EVENTSYSTEM));  /* Definition of objects contained in all loaded projects e.g. events, groups, categories */
    public const uint MEMORYFSB            = (1 << (MEMTYPE.MEMORYFSB             - MEMTYPE.EVENTSYSTEM));  /* Data loaded with registerMemoryFSB */
    public const uint EVENTPROJECT         = (1 << (MEMTYPE.EVENTPROJECT          - MEMTYPE.EVENTSYSTEM));  /* EventProject objects and internals */
    public const uint EVENTGROUPI          = (1 << (MEMTYPE.EVENTGROUPI           - MEMTYPE.EVENTSYSTEM));  /* EventGroup objects and internals */
    public const uint SOUNDBANKCLASS       = (1 << (MEMTYPE.SOUNDBANKCLASS        - MEMTYPE.EVENTSYSTEM));  /* Objects used to manage wave banks */
    public const uint SOUNDBANKLIST        = (1 << (MEMTYPE.SOUNDBANKLIST         - MEMTYPE.EVENTSYSTEM));  /* Data used to manage lists of wave bank usage */
    public const uint STREAMINSTANCE       = (1 << (MEMTYPE.STREAMINSTANCE        - MEMTYPE.EVENTSYSTEM));  /* Stream objects and internals */
    public const uint SOUNDDEFCLASS        = (1 << (MEMTYPE.SOUNDDEFCLASS         - MEMTYPE.EVENTSYSTEM));  /* Sound definition objects */
    public const uint SOUNDDEFDEFCLASS     = (1 << (MEMTYPE.SOUNDDEFDEFCLASS      - MEMTYPE.EVENTSYSTEM));  /* Sound definition static data objects */
    public const uint SOUNDDEFPOOL         = (1 << (MEMTYPE.SOUNDDEFPOOL          - MEMTYPE.EVENTSYSTEM));  /* Sound definition pool data */
    public const uint REVERBDEF            = (1 << (MEMTYPE.REVERBDEF             - MEMTYPE.EVENTSYSTEM));  /* Reverb definition objects */
    public const uint EVENTREVERB          = (1 << (MEMTYPE.EVENTREVERB           - MEMTYPE.EVENTSYSTEM));  /* Reverb objects */
    public const uint USERPROPERTY         = (1 << (MEMTYPE.USERPROPERTY          - MEMTYPE.EVENTSYSTEM));  /* User property objects */
    public const uint EVENTINSTANCE        = (1 << (MEMTYPE.EVENTINSTANCE         - MEMTYPE.EVENTSYSTEM));  /* Event instance base objects */
    public const uint EVENTINSTANCE_COMPLEX= (1 << (MEMTYPE.EVENTINSTANCE_COMPLEX - MEMTYPE.EVENTSYSTEM));  /* Complex event instance objects */
    public const uint EVENTINSTANCE_SIMPLE = (1 << (MEMTYPE.EVENTINSTANCE_SIMPLE  - MEMTYPE.EVENTSYSTEM));  /* Simple event instance objects */
    public const uint EVENTINSTANCE_LAYER  = (1 << (MEMTYPE.EVENTINSTANCE_LAYER   - MEMTYPE.EVENTSYSTEM));  /* Event layer instance objects */
    public const uint EVENTINSTANCE_SOUND  = (1 << (MEMTYPE.EVENTINSTANCE_SOUND   - MEMTYPE.EVENTSYSTEM));  /* Event sound instance objects */
    public const uint EVENTENVELOPE        = (1 << (MEMTYPE.EVENTENVELOPE         - MEMTYPE.EVENTSYSTEM));  /* Event envelope objects */
    public const uint EVENTENVELOPEDEF     = (1 << (MEMTYPE.EVENTENVELOPEDEF      - MEMTYPE.EVENTSYSTEM));  /* Event envelope definition objects */
    public const uint EVENTPARAMETER       = (1 << (MEMTYPE.EVENTPARAMETER        - MEMTYPE.EVENTSYSTEM));  /* Event parameter objects */
    public const uint EVENTCATEGORY        = (1 << (MEMTYPE.EVENTCATEGORY         - MEMTYPE.EVENTSYSTEM));  /* Event category objects */
    public const uint EVENTENVELOPEPOINT   = (1 << (MEMTYPE.EVENTENVELOPEPOINT    - MEMTYPE.EVENTSYSTEM));  /* Event envelope point objects */
    public const uint ALL                  = 0xffffffff;                                                              /* All memory used by FMOD Event System */

    /* All event instance memory */
    public const uint EVENTINSTANCE_GROUP  = (EVENTINSTANCE         |
                                              EVENTINSTANCE_COMPLEX |
                                              EVENTINSTANCE_SIMPLE  |
                                              EVENTINSTANCE_LAYER   |
                                              EVENTINSTANCE_SOUND);

    /* All sound definition memory */
    public const uint SOUNDDEF_GROUP       = (SOUNDDEFCLASS    |
                                              SOUNDDEFDEFCLASS |
                                              SOUNDDEFPOOL);
}

}