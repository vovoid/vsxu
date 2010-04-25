/*===============================================================================================
 Music System Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 Demonstrates basic usage of FMOD's data-driven event library (fmod_event.dll)
===============================================================================================*/
#include "../../api/inc/fmod_event.hpp"
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
    FMOD::EventSystem    *eventsystem;
    FMOD::MusicSystem    *musicsystem;
    FMOD::MusicPrompt    *country;
    FMOD::MusicPrompt    *stealth;
    FMOD::MusicPrompt    *retrorock;
    FMOD_RESULT           result;
    int                   key;

    printf("======================================================================\n");
    printf("Music System Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("======================================================================\n");
    printf("Press '1'       to activate/deactivate country cue.\n");
    printf("Press '2'       to activate/deactivate stealth cue.\n");
    printf("Press '3'       to activate/deactivate retro rock cue.\n");
    printf("Press '<' / '>' to decrease/increase retro rock intensity parameter.\n");
    printf("======================================================================\n");

    ERRCHECK(result = FMOD::EventSystem_Create(&eventsystem));
    ERRCHECK(result = eventsystem->init(64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL));
    ERRCHECK(result = eventsystem->setMediaPath("..\\media\\"));
    ERRCHECK(result = eventsystem->load("examples.fev", 0, 0));

    ERRCHECK(result = eventsystem->getMusicSystem(&musicsystem));

    ERRCHECK(result = musicsystem->prepareCue(MUSICCUE_EXAMPLES_COUNTRY_ARENA,    &country));
    ERRCHECK(result = musicsystem->prepareCue(MUSICCUE_EXAMPLES_STEALTH_ARENA,    &stealth));
    ERRCHECK(result = musicsystem->prepareCue(MUSICCUE_EXAMPLES_RETRO_ROCK_ARENA, &retrorock));

    /*
        Start off with country cue active
    */
    result = country->begin();
    ERRCHECK(result);

    key = 0;
    do
    {
        if (_kbhit())
        {
            key = _getch();

            bool active = false;

            if (key == '1')
            {
                ERRCHECK(result = country->isActive(&active));
                if (!active)
                {
                    ERRCHECK(result = country->begin());
                    printf("Country cue begin             \n");
                }
                else
                {
                    ERRCHECK(result = country->end());
                    printf("Country cue end               \n");
                }
            }
            else if (key == '2')
            {
                ERRCHECK(result = stealth->isActive(&active));
                if (!active)
                {
                    ERRCHECK(result = stealth->begin());
                    printf("Stealth cue begin             \n");
                }
                else
                {
                    ERRCHECK(result = stealth->end());
                    printf("Stealth cue end               \n");
                }
            }
            else if (key == '3')
            {
                ERRCHECK(result = retrorock->isActive(&active));
                if (!active)
                {
                    ERRCHECK(result = retrorock->begin());
                    printf("Retro rock cue begin          \n");
                }
                else
                {
                    ERRCHECK(result = retrorock->end());
                    printf("Retro rock cue end            \n");
                }
            }

            if (key == '<' || key == ',')
            {
                float intensity;

                ERRCHECK(result = musicsystem->getParameterValue(MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, &intensity));

                intensity -= 3.3f;
                if (intensity < 0.0f)
                {
                    intensity = 0.0f;
                }

                ERRCHECK(result = musicsystem->setParameterValue(MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, intensity));
            }
            if (key == '>' || key == '.')
            {
                float intensity;

                ERRCHECK(result = musicsystem->getParameterValue(MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, &intensity));

                intensity += 3.3f;
                if (intensity > 9.9f)
                {
                    intensity = 9.9f;
                }

                ERRCHECK(result = musicsystem->setParameterValue(MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, intensity));
            }
        }

        ERRCHECK(result = eventsystem->update());
        Sleep(15);

        {
            float intensity = 0.0f;

            musicsystem->getParameterValue(MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, &intensity);

            printf("retro rock intensity : %.02f\r", intensity);
        }

    } while (key != 27);

    ERRCHECK(result = eventsystem->release());
    return 0;
}
