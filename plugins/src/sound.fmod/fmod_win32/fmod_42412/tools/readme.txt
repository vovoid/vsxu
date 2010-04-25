
                        --------------------------------
                        FMOD Ex Sample Bank/Stream Compiler
                                      v1.63
                   Copyright Firelight Technologies 2002-2008.
                        --------------------------------

Index.
------

1. Introduction.
2. Using it to generate 1 FSB file, or multiple single FSB files!
3. Hit 'Build' and notice the RAM usage bar graph.
4. Error dialog box
5. Using FSB files in your program.
    5.1 As a static sample bank.
    5.2 For streaming.
6. Using list files
7. Command line options
8. Support


1. Introduction.
----------------
This tool takes wav files, mp3 files, ogg files, aiff files etc, and converts them
to the .FSB format (FMOD Sample Bank).
It will compress them in the native format of the machine.  


2. Using it to generate 1 FSB file, or multiple single FSB files!
--------------------------------------------------------------
If you look to the top of the dialog, you will see a menu item called 'Build Mode'.
In this you can tell FSBank to write out 1 packed FSB file with all your sounds in it,
or alternatively write out 1 FSB for each sound you provide.  This is useful for streams.

- The default mode is 'Generate Single FSB File'

This means if you provide a source directory to compile from, it will compile every sound
in that directory structure recursively, into a single .FSB file.
This is mainly used for sample banks, or packing streams into a single file for indexed streaming.

- The second mode type is 'Generate Multiple FSB Files'

This mode means that for every sound in your specified source directory, it will generate
an equivalent FSB file in the specified destination directory.  This would only realistically be 
used for streams, as a sort of 'batch converter'.
It will build all files into the directory you specify by default.  If you want to recreate
the directory structure from the source directory, click 'Rebuild source directory tree to
destination'

- The third mode is 'Generate Single Interleaved FSB file'

This mode means it will take all input files, and interleave them into one FSB file.
For example, a normal stereo stream is interleaved as LRLRLRLR etc.
This is what it would look like if you provided 2 mono files or 1 stereo file as the source.
If you specify more, for example 4 mono files, and a stereo file, the resulting FSB looks like
this : 123456123456123456.  
What this means is multiple audio tracks can be played back as one stream, and seeking is eliminated.
Each channel within this multitrack stream can have its attributes modified (ie volume/pan) so you
can use this feature for interactive music, or mood based ambiences.  Many other uses could be found.




3. Hit 'Build' and notice the RAM usage bar graph.
-----------------------------------------------
Press the big 'Build' button and it will start compiling.

Notice the RAM usage bar graph below.  If you intend to load the FSB into ram for sample 
playback (not streamed), then you will have to make sure the graph does not go red.  This
means you will have overflowed sound ram and you will not fit all of your sounds in.

On PlayStation 2, SPU2 ram has 1,790kb available for sound if you are using full reverb.
If you uncheck 'Reverb Core0 enabled' or  'Reverb Core1 enabled' you gain 128k to give 1,918kb free.
If you uncheck 'Reverb Core1 enabled' and 'Reverb Core0 enabled' you gain 256k to give 2,046kb free.
Note disabling reverb has to be coupled in the code by using FMOD_INIT_PS2_DISABLECORE0REVERB and/or
FMOD_INIT_PS2_DISABLECORE1REVERB in the call to System::init.

If you intend to use the FSB file(s) for streaming purposes, ignore the ram usage warnings as
streams can be of unlimited size.




4. Error dialog box
--------------------

When the build has completed, a list of warnings will appear.


'Memory for this platform has been exceeded'

If your usage is intended for static loading into memory for sample playback.  You have overrun
the hardware memory.
If you intend to use the file(s) for streaming then ignore this message.  On PlayStation 2, Streams 
use 4k of SPU2 memory if targeted for SPU2 (and not IOP).




5. Using FSB files in your program (Using FMOD Ex).
---------------------------------------------------

5.1 As a static sample bank.
----------------------------
To load a bank use System::createSound.

To get access to the individual sounds use Sound::getSubSound.

The C header contains #defines to reference the sound from your code with Sound::getSubSound.

You can even preview a .FSB file from your program by playing the parent sound!  
It will simply play through all sounds in the bank sequentially (and gaplessly) and then stop.

5.2 For streaming.
------------------

To open an FSB file as a stream use System::createSound with FMOD_CREATESTREAM flag, or System::createStream.

call System::playSound to play the first stream in the FSB file.

call Sound::getSubSound to get access to the relevant file.
use the #defines generated by FSBank in this function.

Sound::setSubSoundSentence can be used to 'stitch', or 'sentence' multiple substreams together.



6. Using list files
------------------------

You can specify a text file for the build source rather than a directory. This text file must contain 
a list of the files you want to build into an FSB. Each file must start on a new line. You can also 
specify sample defaults for each file in the list by appending them to the filename in a comma-seperated
list. Here's an example list file : 

jungle.wav
goonie.wav, deffreq=22050, defvol=128, defpan=128, defpri= 128, varfreq=2000, varvol=128
freddo.wav, deffreq=44100, varfreq=4000, mindistance=500.0
blarg.wav,  fsound_2d, fsound_loop_normal


The defaults that you can specify are :

deffreq                 Frequency in hz                     e.g. deffreq = 44100
defvol                  Volume (0 - 255)                    e.g. defvol = 255
defpan                  Pan (0 - 255)                       e.g. defpan = 128
defpri                  Priority (0 - 255)                  e.g. defpri = 128
varfreq                 Frequency variation in hz           e.g. varfreq = 2000
varvol                  Volume variation (0 - 255)          e.g. varvol = 128
varpan                  Pan variation (0 - 255)             e.g. varpan = 64
fsound_2d               See FSOUND_2D mode bit              e.g. fsound_2d
fsound_hw2d             See FSOUND_HW2D mode bit            e.g. fsound_hw2d
fsound_hw3d             See FSOUND_HW3D mode bit            e.g. fsound_hw3d
fsound_loop_normal      See FSOUND_LOOP_NORMAL mode bit     e.g. fsound_loop_normal
fsound_loop_bidi        See FSOUND_LOOP_BIDI mode bit       e.g. fsound_loop_bidi
mindistance             Minimum volume distance in "units" (See Sound::set3DMinMaxDistance)    e.g. mindistance = 1.0
maxdistance             Maximum volume distance in "units" (See Sound::set3DMinMaxDistance)    e.g. maxdistance = 10000.0




7. Command Line Options
------------------------

Usage: fsbankex <-o dest> <source> [-p ps2|gc|xbox|cross|psp] [-f vag|gcadpcm|xadpcm|pcm|source] [-m s|m|i] [-d] [-r 0|1] [-h] [-b] [-?] [-a]

  -o dest                                         Destination FSB file/directory
  source                                          Source directory or list file
  -p ps2|gc|xbox|xbox360|pc|psp|ps3               Target platform (default = pc)
  -f vag|gcadpcm|xadpcm|xma|pcm|source|adpcm|mp3  Target format (default = pcm)
  -m s|m|i                                        Build mode :
                                                  s = Single FSB (default)
                                                  m = Multiple FSBs
                                                  i = single Interleaved FSB
  -d                                              Rebuild dir structure (Multiple FSB only) (default = off)
  -q [0...100]                                    Compression quality (XMA/MP2/MP3 only) (default = 75)
  -O [0...90]                                     Optimize sample rate (and further reduce rate by given percentage) (default = off)
  -x                                              XMA disable high frequency filtering (Xbox360 only) (default = on)
  -s                                              XMA disable seeking to save memory (Xbox360 only) (default = on)
  -r 0|1                                          Disable reverbcore 0 or 1 (ps2 only) (default = both enabled)
  -h                                              Hidden mode - don't display the FSBank window at all (default = off)
  -b                                              Use small headers (default = off)
  -i                                              Ignore errors in compilation such as "file not found" (default = off)
  -n                                              No C headers. Don't generate C header (.h) files (default = off)
  -l                                              Disable the automatic behaviour for mp2/mp3 encoding, which is to encode the file as if it was looping.
  -?                                              Display command line help
  -z                                              Command line quiet mode (disable log output).
  -e [encryptionkey]                              Encryption key
  -t                                              For stereo and above VAG/GCADPCM , don't interleave the data as LRLR.. instead use LLRR type encoding.
                                                
OutputDebugString (debugger window output) is used to display errors and progress while in hidden mode.

8. Support
----------
Write to support@fmod.org for questions and help.
