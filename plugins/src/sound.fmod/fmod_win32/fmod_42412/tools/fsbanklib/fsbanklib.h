#ifndef _FSBANKLIB_H
#define _FSBANKLIB_H

#ifndef F_STDCALL
    #if defined(WIN32) || defined(__WIN32__) || defined(_XBOX)
        #define F_STDCALL _stdcall
    #else
	    #define F_STDCALL
    #endif
#endif

/*
[ENUM]
[
    [DESCRIPTION]   
    Errorcode returned by all FSBank commands
    
    [SEE_ALSO]      
]
*/
typedef enum FSBANK_RESULT
{
    FSBANK_OK,
    FSBANK_ERR_INIT,                // Failed to initialize 
    FSBANK_ERR_UNINITIALIZED,       // FSBank_Init hasnt been called yet.
    FSBANK_ERR_FILE_DIRNOTFILE,     // The target is an existing file.  The specified build mode requires a destination directory, not a file.    
    FSBANK_ERR_FILE_DESTFILE,       // Cannot create destination file.  File may be in use or read only
    FSBANK_ERR_FILE_WORKING,        // Cannot create working file.  File may be in use or read only
    FSBANK_ERR_FILE_HEADER,         // Cannot create destination c header file.  File may be in use or read only
    FSBANK_ERR_FILE_EOF,            // End of file was encountered unexpectedly.
    FSBANK_ERR_FILE_OS,             // An operating system based file error was encountered (target file in use?).  Could cause corruption or failure of FSB to be created.
    FSBANK_ERR_INVALID_PARAM,       // An invalid parameter was passed to this function
    FSBANK_ERR_INVALID_FORMAT,      // A dll was missing for this format or the environment wasnt set up properly.
    FSBANK_ERR_CANCELLED,           // The build process was cancelled during compilation by the user.
    FSBANK_ERR_COMPILATION_ABORTED, // Compilation aborted due to error
    FSBANK_ERR_MEMORY               // Out of memory.
} FSBANK_RESULT;


/*
[ENUM]
[
    [DESCRIPTION]   
    Describes the target build type or method of creating the FSB file(s).
    
    [SEE_ALSO]     
    FSBank_Build 
]
*/
typedef enum FSBANK_BUILDMODE
{
    FSBANK_BUILDMODE_SINGLE = 0,    // This creates a single FSB file with multiple sounds in it, or a standard sound bank.
    FSBANK_BUILDMODE_MULTI,         // This creates multiple FSB files with 1 sound in each.   The destfile_or_dir parameter of FSBank_Build is then interpreted as a directory and not a file.
    FSBANK_BUILDMODE_INTERLEAVED,   // This creates a single FSB file with a single sound in it, but with all the source files interleaved/multiplexed into it so that when it is played, all files play at once, and are given a channel each.
} FSBANK_BUILDMODE;


/*
[ENUM]
[
    [DESCRIPTION]
    Describes the target platform.
    
    [SEE_ALSO]      
    FSBank_Build
]
*/
typedef enum FSBANK_PLATFORM
{
    FSBANK_PLATFORM_PS2 = 0,        // Sony PlayStation 2
    FSBANK_PLATFORM_GC,             // Nintendo GameCube / Wii
    FSBANK_PLATFORM_XBOX,           // Microsoft Xbox
    FSBANK_PLATFORM_XBOX360,        // Microsoft Xbox360
    FSBANK_PLATFORM_PC,             // Windows / Mac / Linux
    FSBANK_PLATFORM_PSP,            // Sony PlayStation Portable
    FSBANK_PLATFORM_PS3,            // Sony PlayStation 3
    FSBANK_PLATFORM_MAX
} FSBANK_PLATFORM;


/*
[ENUM]
[
    [DESCRIPTION]   
    Describes the target format.
    
    [SEE_ALSO]      
    FSBank_Build
]
*/
typedef enum FSBANK_FORMAT
{
    FSBANK_FORMAT_VAG = 0,          // VAG (SPU2) (3.5:1) PlayStation 2 Only. Hardware decompression, no cpu hit.  Requires ENCVAG.DLL.
    FSBANK_FORMAT_GCADPCM,          // GCADPCM (3.5:1) GameCube Only. Hardware decompression, no cpu hit.
    FSBANK_FORMAT_XADPCM,           // XADPCM (3.5:1) Xbox only. Hardware decompression, no cpu hit.
    FSBANK_FORMAT_XMA,              // XMA (VBR) Xbox360 only. Hardware decompression, no cpu hit.
    FSBANK_FORMAT_PCM,              // PCM (1:1) All Platforms. 
    FSBANK_FORMAT_IMAADPCM,         // IMA ADPCM (3.5:1) All platforms except PlayStation 2.  
    FSBANK_FORMAT_MP3,              // MPEG Layer 3.  Requires lame_enc.dll.
    FSBANK_FORMAT_MP2,              // MPEG Layer 2.  Requires toolame.dll.
    FSBANK_FORMAT_MAX
} FSBANK_FORMAT;



/*
[DEFINE]
[
    [NAME]
    FSBANK_SOUNDMODES

    [DESCRIPTION]   
    Build flags.  Use them with FSBank_Build in the buildflags parameter to change build behaviour.

    [REMARKS]
    These defines are from FMOD 3.  The flags are not really neccessary as you can override them at runtime.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FSBANK_SAMPLE_DEFAULTS
]
*/
#define FSOUND_LOOP_NORMAL           0x00000002  /* For forward looping samples. */
#define FSOUND_LOOP_BIDI             0x00000004  /* For bidirectional looping samples.  (no effect if in hardware). */
#define FSOUND_CHANNELMODE_ALLMONO   0x00000400  /* Sample is a collection of mono channels. */
#define FSOUND_CHANNELMODE_ALLSTEREO 0x00000800  /* Sample is a collection of stereo channel pairs */
#define FSOUND_CHANNELMODE_PROTOOLS  0x00010000  /* Sample is 6ch and uses L C R LS RS LFE standard. */
#define FSOUND_HW3D                  0x00001000  /* Attempts to make samples use 3d hardware acceleration. (if the card supports it) */
#define FSOUND_2D                    0x00002000  /* Tells software (not hardware) based sample not to be included in 3d processing. */
/* [DEFINE_END] */


typedef unsigned int FSBANK_BUILDFLAGS;

/*
[DEFINE]
[
    [NAME]
    FSBANK_BUILDFLAGS

    [DESCRIPTION]   
    Build flags.  Use them with FSBank_Build in the buildflags parameter to change build behaviour.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3

    [SEE_ALSO]
    FSBank_Build
]
*/
#define FSBANK_BUILD_DEFAULT                    0x00000000 /* Build normally */
#define FSBANK_BUILD_IGNOREERRORS               0x00000001 /* If set, compilation will continue even if errors occur. */
#define FSBANK_BUILD_BASICHEADERS               0x00000002 /* Generate FSBs with small sample header data.  They only contain basic information such as sample length, and everything else has its attributes inherited from the first sample (for example the default frequency). */
#define FSBANK_BUILD_CREATEINCLUDES             0x00000004 /* Create C header files with #defines defining indicies for each member of the FSB. */
#define FSBANK_BUILD_DUPDIRSTRUCTURE            0x00000008 /* For format type FSBANK_BUILDMODE_MULTI only.  This will create the same source directory structure to the destination target directory. */
#define FSBANK_BUILD_COMPRESSIONCACHING         0x00000010 /* If set, fsbanklib caches compressed FSB data, so that next time the same source data is referenced, it simply copies the compressed data instead of re-compressing it.  If not set, standard no cache method.  FSBANK_FORMAT_PCM is of course ignored. */
#define FSBANK_BUILD_XMAFILTERHIGHFREQ          0x00000020 /* Xbox360 only - Enable high frequency filtering when encoding into XMA format. */
#define FSBANK_BUILD_DISABLESEEKING             0x00000040 /* Xbox360 only - Disable seek tables to save memory when encoding into XMA format. */
#define FSBANK_BUILD_DISABLESYNCPOINTS          0x00000080 /* Disable sample syncpoints to save memory. */
#define FSBANK_BUILD_DONTINTERLEAVE             0x00000100 /* For stereo and above sounds, don't interleave the data as LRLRLRLR.. instead use LLLLRRRR type encoding.  PS2 VAG, PSP VAG, GCADPCM only. Speeds up loading for stereo and above samples (not streams, streams must remained interleaved), allows FMOD_OPENMEMORY_POINT with these formats. */
#define FSBANK_BUILD_OVERRIDE_XMAFILTERHIGHFREQ 0x00010000 /* Xbox360 only - Per-waveform flag to override FSBANK_BUILD_XMAFILTERHIGHFREQ. */
#define FSBANK_BUILD_DONTLOOPMEG                0x00020000 /* MP2 / MP3 only.  Disable perfect looping encoding.  Stops chirps at start of oneshot sounds. */

/* [DEFINE_END] */

/*
[STRUCTURE] 
[
    [DESCRIPTION]
    Structure containing default values for various sample attributes.

    [SEE_ALSO]
    FSBank_Build
]
*/
typedef struct _FSBANK_SAMPLE_DEFAULTS
{                                   
	float        mindistance;            // Minimum volume distance in "units".  Default = 1.0.
	float        maxdistance;            // Maximum volume distance in "units".  Default = 10000.0.
    int          deffreq;                // Sample default speed in hz.  ie 44100.  Default is the source sound rate.
	int          defvol;                 // Sample default volume.  0 = silent.  255 = full volume.
	int          defpan;                 // Sample default pan.     0 = full left.  255 = full right.
	int          defpri;                 // Sample priority.        0 = low priority, 255 = high priority
    int          varfreq;                // Frequency variation in hz
    int          varvol;                 // Volume variation
    int          varpan;                 // Pan variation
    unsigned int mode;                   // FSOUND_MODES bits. Bits allowed are FSOUND_LOOP_NORMAL, FSOUND_LOOP_BIDI, FSOUND_2D, FSOUND_HW2D, FSOUND_HW3D, FSOUND_CHANNELMODE_ALLMONO, FSOUND_CHANNELMODE_ALLSTEREO and FSOUND_CHANNELMODE_PROTOOLS.
    int          quality;                // Compression quality. XMA / MP2 / MP3 only - Value between 1 - 100. 1 being the highest compression but lowest quality and 100 being the lowest compression but highest quality.  With MP3 bitrate = quality * 3.2, minimum 16kbps, maximum 320kbps.   -1 = use "quality" parameter passed to FSBank_Build function.
    int          optimizesamplerate;     // Ouput sample rate optimization flag.  0 = leave sample alone.  1 = attempt to optimize samplerate down.
    float        optimizedratereduction; // If 'optimizesamplerate' is true, multiply the optimized rate by this fraction. (in case you want more reduction, or less).  100.0 = value optimizer chose, 50.0 = half the rate the optimizer chose, 1000.0 = 10x the rate the optimizer chose.
    FSBANK_BUILDFLAGS buildflags;        // Per-waveform build flags (FSBANK_BUILD_OVERRIDE_x bit must be set for FSBANK_BUILD_x option to take effect)
} FSBANK_SAMPLE_DEFAULTS;


typedef void (F_STDCALL *FSBANK_UPDATECALLBACK)(int index, int memused, void *userdata);
typedef void (F_STDCALL *FSBANK_DEBUGCALLBACK)(const char *debugstring, void *userdata);


#ifdef __cplusplus
extern "C" {
#endif

FSBANK_RESULT FSBank_Init();
FSBANK_RESULT FSBank_Close();
FSBANK_RESULT FSBank_IsFormatAllowed(FSBANK_FORMAT format);
FSBANK_RESULT FSBank_Build(FSBANK_BUILDMODE buildmode,
                           FSBANK_FORMAT format,
                           FSBANK_PLATFORM platform,
                           FSBANK_BUILDFLAGS buildflags,
                           char *encryptkey,
                           const char *destfile_or_dir, 
                           int numsrcfiles, 
                           char **srcfile, 
                           FSBANK_SAMPLE_DEFAULTS **defaults,
                           const char *srcdir,
                           int quality);
FSBANK_RESULT FSBank_SetUpdateCallback(FSBANK_UPDATECALLBACK callback, void *userdata);
FSBANK_RESULT FSBank_SetDebugCallback(FSBANK_DEBUGCALLBACK callback, void *userdata);
FSBANK_RESULT FSBank_SetBuildCancel(int cancel);
FSBANK_RESULT FSBank_GetBuildCancel(int *cancel);

#ifdef __cplusplus
}
#endif

#endif
