/*===============================================================================================
 Music System Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 Demonstrates basic usage of FMOD's data-driven event library (fmod_event.dll)
===============================================================================================*/
#include "../../api/inc/fmod_event.h"
#include "../../api/inc/fmod_errors.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "examples.h"

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
    FMOD_EVENTSYSTEM  *eventsystem;
    FMOD_MUSICSYSTEM  *musicsystem;
    FMOD_MUSICPROMPT  *country;
    FMOD_MUSICPROMPT  *stealth;
    FMOD_MUSICPROMPT  *retrorock;
    FMOD_RESULT        result;
    int                key;

    printf("======================================================================\n");
    printf("Music System Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("======================================================================\n");
    printf("Press '1'       to activate/deactivate country cue.\n");
    printf("Press '2'       to activate/deactivate stealth cue.\n");
    printf("Press '3'       to activate/deactivate retro rock cue.\n");
    printf("Press '<' / '>' to decrease/increase retro rock intensity parameter.\n");
    printf("======================================================================\n");

    ERRCHECK(result = FMOD_EventSystem_Create(&eventsystem));
    ERRCHECK(result = FMOD_EventSystem_Init(eventsystem, 64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL));
    ERRCHECK(result = FMOD_EventSystem_SetMediaPath(eventsystem, "..\\media\\"));
    ERRCHECK(result = FMOD_EventSystem_Load(eventsystem, "examples.fev", 0, 0));

    ERRCHECK(result = FMOD_EventSystem_GetMusicSystem(eventsystem, &musicsystem));

    ERRCHECK(result = FMOD_MusicSystem_PrepareCue(musicsystem, MUSICCUE_EXAMPLES_COUNTRY_ARENA,    &country));
    ERRCHECK(result = FMOD_MusicSystem_PrepareCue(musicsystem, MUSICCUE_EXAMPLES_STEALTH_ARENA,    &stealth));
    ERRCHECK(result = FMOD_MusicSystem_PrepareCue(musicsystem, MUSICCUE_EXAMPLES_RETRO_ROCK_ARENA, &retrorock));

    /*
        Start off with country cue active
    */
    result = FMOD_MusicPrompt_Begin(country);
    ERRCHECK(result);

    key = 0;
    do
    {
        if (_kbhit())
        {
            int active = 0;

            key = _getch();

            if (key == '1')
            {
                ERRCHECK(result = FMOD_MusicPrompt_IsActive(country, &active));
                if (!active)
                {
                    ERRCHECK(result = FMOD_MusicPrompt_Begin(country));
                    printf("Country cue begin\n");
                }
                else
                {
                    ERRCHECK(result = FMOD_MusicPrompt_End(country));
                    printf("Country cue end\n");
                }
            }
            else if (key == '2')
            {
                ERRCHECK(result = FMOD_MusicPrompt_IsActive(stealth, &active));
                if (!active)
                {
                    ERRCHECK(result = FMOD_MusicPrompt_Begin(stealth));
                    printf("Stealth cue begin\n");
                }
                else
                {
                    ERRCHECK(result = FMOD_MusicPrompt_End(stealth));
                    printf("Stealth cue end\n");
                }
            }
            else if (key == '3')
            {
                ERRCHECK(result = FMOD_MusicPrompt_IsActive(retrorock, &active));
                if (!active)
                {
                    ERRCHECK(result = FMOD_MusicPrompt_Begin(retrorock));
                    printf("Retro rock cue begin\n");
                }
                else
                {
                    ERRCHECK(result = FMOD_MusicPrompt_End(retrorock));
                    printf("Retro rock cue end\n");
                }
            }

            if (key == '<' || key == ',')
            {
                float intensity;

                ERRCHECK(result = FMOD_MusicSystem_GetParameterValue(musicsystem, MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, &intensity));

                intensity -= 3.3f;
                if (intensity < 0.0f)
                {
                    intensity = 0.0f;
                }

                ERRCHECK(result = FMOD_MusicSystem_SetParameterValue(musicsystem, MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, intensity));
            }
            if (key == '>' || key == '.')
            {
                float intensity;

                ERRCHECK(result = FMOD_MusicSystem_GetParameterValue(musicsystem, MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, &intensity));

                intensity += 3.3f;
                if (intensity > 9.9f)
                {
                    intensity = 9.9f;
                }

                ERRCHECK(result = FMOD_MusicSystem_SetParameterValue(musicsystem, MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, intensity));
            }
        }

        ERRCHECK(result = FMOD_EventSystem_Update(eventsystem));
        Sleep(15);

        {
            float intensity = 0.0f;

            FMOD_MusicSystem_GetParameterValue(musicsystem, MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, &intensity);

            printf("retro rock intensity : %.02f\r", intensity);
        }

    } while (key != 27);

    ERRCHECK(result = FMOD_EventSystem_Release(eventsystem));
    return 0;
}
