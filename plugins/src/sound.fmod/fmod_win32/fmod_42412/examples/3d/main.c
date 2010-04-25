/*===============================================================================================
 3d Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 This example shows how to basic 3d positioning
===============================================================================================*/
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

#include "../../api/inc/fmod.h"
#include "../../api/inc/fmod_errors.h"

#define INTERFACE_UPDATETIME 50      // 50ms update for interface
#define DISTANCEFACTOR       1.0f    // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


int main(int argc, char *argv[])
{
    FMOD_SYSTEM     *system;
    FMOD_SOUND      *sound1, *sound2, *sound3;
    FMOD_CHANNEL    *channel1 = 0, *channel2 = 0, *channel3 = 0;
    FMOD_RESULT      result;
    int              key, numdrivers;
    int              listenerflag = TRUE;
    FMOD_VECTOR      listenerpos  = { 0.0f, 0.0f, -1.0f * DISTANCEFACTOR }; 
    unsigned int     version;
    FMOD_SPEAKERMODE speakermode;
    FMOD_CAPS        caps;
    char             name[256];

    printf("===============================================================\n");
    printf("3d Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("===============================================================\n");
    printf("This example plays 2 3D sounds in hardware.  Optionally you can\n");
    printf("play a 2D hardware sound as well.\n");
    printf("===============================================================\n\n");

    /*
        Create a System object and initialize.
    */
    result = FMOD_System_Create(&system);
    ERRCHECK(result);
    
    result = FMOD_System_GetVersion(system, &version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return 0;
    }

    result = FMOD_System_GetNumDrivers(system, &numdrivers);
    ERRCHECK(result);

    if (numdrivers == 0)
    {
        result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_NOSOUND);
        ERRCHECK(result);
    }
    else
    {
        result = FMOD_System_GetDriverCaps(system, 0, &caps, 0, 0, &speakermode);
        ERRCHECK(result);

        result = FMOD_System_SetSpeakerMode(system, speakermode);       /* Set the user selected speaker mode. */
        ERRCHECK(result);

        if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
        {                                                   /* You might want to warn the user about this. */
            result = FMOD_System_SetDSPBufferSize(system, 1024, 10);
            ERRCHECK(result);
        }

        result = FMOD_System_GetDriverInfo(system, 0, name, 256, 0);
        ERRCHECK(result);

        if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
        {
            result = FMOD_System_SetSoftwareFormat(system, 48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
            ERRCHECK(result);
        }
    }

    result = FMOD_System_Init(system, 100, FMOD_INIT_NORMAL, 0);
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
    {
        result = FMOD_System_SetSpeakerMode(system, FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);
                
        result = FMOD_System_Init(system, 100, FMOD_INIT_NORMAL, 0);/* ... and re-init. */
        ERRCHECK(result);
    }

    /*
        Set the distance units. (meters/feet etc).
    */
    result = FMOD_System_Set3DSettings(system, 1.0, DISTANCEFACTOR, 1.0f);
    ERRCHECK(result);

    /*
        Load some sounds
    */
    result = FMOD_System_CreateSound(system, "../media/drumloop.wav", FMOD_HARDWARE | FMOD_3D, 0, &sound1);
    ERRCHECK(result);
    result = FMOD_Sound_Set3DMinMaxDistance(sound1, 2.0f * DISTANCEFACTOR, 10000.0f * DISTANCEFACTOR);
    ERRCHECK(result);
    result = FMOD_Sound_SetMode(sound1, FMOD_LOOP_NORMAL);
    ERRCHECK(result);

    result = FMOD_System_CreateSound(system, "../media/jaguar.wav", FMOD_HARDWARE | FMOD_3D, 0, &sound2);
    ERRCHECK(result);
    result = FMOD_Sound_Set3DMinMaxDistance(sound2, 2.0f * DISTANCEFACTOR, 10000.0f * DISTANCEFACTOR);
    ERRCHECK(result);
    result = FMOD_Sound_SetMode(sound2, FMOD_LOOP_NORMAL);
    ERRCHECK(result);

    result = FMOD_System_CreateSound(system, "../media/swish.wav", FMOD_HARDWARE | FMOD_2D, 0, &sound3);
    ERRCHECK(result);

    /*
        Play sounds at certain positions
    */
    {
        FMOD_VECTOR pos = { -10.0f * DISTANCEFACTOR, 0.0f, 0.0f };
        FMOD_VECTOR vel = {  0.0f, 0.0f, 0.0f };

        result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, TRUE, &channel1);
        ERRCHECK(result);
        result = FMOD_Channel_Set3DAttributes(channel1, &pos, &vel);
        ERRCHECK(result);
        result = FMOD_Channel_SetPaused(channel1, FALSE);
        ERRCHECK(result);
    }

    {
        FMOD_VECTOR pos = { 15.0f * DISTANCEFACTOR, 0.0f, 0.0f };
        FMOD_VECTOR vel = {  0.0f,  0.0f,  0.0f };

        result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound2, TRUE, &channel2);
        ERRCHECK(result);
        result = FMOD_Channel_Set3DAttributes(channel2, &pos, &vel);
        ERRCHECK(result);
        result = FMOD_Channel_SetPaused(channel2, FALSE);
        ERRCHECK(result);
    }

    /*
        Display help
    */
    {
        int num3d = 0, num2d = 0;

        result = FMOD_System_GetHardwareChannels(system, &num2d, &num3d, 0);
        ERRCHECK(result);
    
        printf("Hardware 2D channels : %d\n", num2d);
        printf("Hardware 3D channels : %d\n", num3d);
    }

    printf("=========================================================================\n");
    printf("Press 1        Pause/Unpause 16bit 3D sound at any time\n");
    printf("      2        Pause/Unpause 8bit 3D sound at any time\n");
    printf("      3        Play 16bit STEREO 2D sound at any time\n");
    printf("      <        Move listener left (in still mode)\n");
    printf("      >        Move listener right (in still mode)\n");
    printf("      SPACE    Stop/Start listener automatic movement\n");
    printf("      ESC      Quit\n");
    printf("=========================================================================\n");

    /*
        Main loop
    */
    do
    {
        if (kbhit())
        {
            key = getch();

            if (key == '1') 
            {
                int paused;
                FMOD_Channel_GetPaused(channel1, &paused);
                FMOD_Channel_SetPaused(channel1, !paused);
            }

            if (key == '2') 
            {
                int paused;
                FMOD_Channel_GetPaused(channel2, &paused);
                FMOD_Channel_SetPaused(channel2, !paused);
            }

            if (key == '3') 
            {
                result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound3, FALSE, &channel3);
                ERRCHECK(result);
            }

            if (key == ' ')
            {
                listenerflag = !listenerflag;
            }

            if (!listenerflag)
            {
                if (key == '<') 
                {
                    listenerpos.x -= 1.0f * DISTANCEFACTOR;
                    if (listenerpos.x < -35 * DISTANCEFACTOR)
                    {
                        listenerpos.x = -35 * DISTANCEFACTOR;
                    }
                }
                if (key == '>') 
                {
                    listenerpos.x += 1.0f * DISTANCEFACTOR;
                    if (listenerpos.x > 36 * DISTANCEFACTOR)
                    {
                        listenerpos.x = 36 * DISTANCEFACTOR;
                    }
                }
            }
        }

        // ==========================================================================================
        // UPDATE THE LISTENER
        // ==========================================================================================
        {
            static float t = 0;
            static FMOD_VECTOR lastpos = { 0.0f, 0.0f, 0.0f };
            FMOD_VECTOR forward        = { 0.0f, 0.0f, 1.0f };
            FMOD_VECTOR up             = { 0.0f, 1.0f, 0.0f };
            FMOD_VECTOR vel;

            if (listenerflag)
            {
                listenerpos.x = (float)sin(t * 0.05f) * 33.0f * DISTANCEFACTOR; // left right pingpong
            }

            // ********* NOTE ******* READ NEXT COMMENT!!!!!
            // vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
            vel.x = (listenerpos.x - lastpos.x) * (1000 / INTERFACE_UPDATETIME);
            vel.y = (listenerpos.y - lastpos.y) * (1000 / INTERFACE_UPDATETIME);
            vel.z = (listenerpos.z - lastpos.z) * (1000 / INTERFACE_UPDATETIME);

            // store pos for next time
            lastpos = listenerpos;

            result = FMOD_System_Set3DListenerAttributes(system, 0, &listenerpos, &vel, &forward, &up);
            ERRCHECK(result);

            t += (30 * (1.0f / (float)INTERFACE_UPDATETIME));    // t is just a time value .. it increments in 30m/s steps in this example

            // print out a small visual display
            {
                char s[80];

                sprintf(s, "|.......................<1>......................<2>....................|");
                
                s[(int)(listenerpos.x / DISTANCEFACTOR) + 35] = 'L';                
                printf("%s\r", s);
            }            
        }

        FMOD_System_Update(system);

        Sleep(INTERFACE_UPDATETIME - 1);

    } while (key != 27);

    printf("\n");

    /*
        Shut down
    */
    result = FMOD_Sound_Release(sound1);
    ERRCHECK(result);
    result = FMOD_Sound_Release(sound2);
    ERRCHECK(result);
    result = FMOD_Sound_Release(sound3);
    ERRCHECK(result);

    result = FMOD_System_Close(system);
    ERRCHECK(result);
    result = FMOD_System_Release(system);
    ERRCHECK(result);

    return 0;
}
