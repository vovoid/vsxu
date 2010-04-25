/*===============================================================================================
 SimpleEvent Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 Demonstrates basic usage of FMOD's data-driven event library (fmod_event.dll)
===============================================================================================*/
#include "../../api/inc/fmod_event.h"
#include "../../api/inc/fmod_errors.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

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
    FMOD_EVENT        *event;
    FMOD_RESULT        result;
    int                key;

    printf("======================================================================\n");
    printf("Simple Event Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("======================================================================\n");
    printf("This example simply plays an event created with the FMOD Designer tool.\n");
    printf("======================================================================\n\n");

    ERRCHECK(result = FMOD_EventSystem_Create(&eventsystem));
    ERRCHECK(result = FMOD_EventSystem_Init(eventsystem, 64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL));
    ERRCHECK(result = FMOD_EventSystem_SetMediaPath(eventsystem, "..\\media\\"));
    ERRCHECK(result = FMOD_EventSystem_Load(eventsystem, "examples.fev", 0, 0));
    ERRCHECK(result = FMOD_EventSystem_GetEvent(eventsystem, "examples/FeatureDemonstration/Basics/SimpleEvent", FMOD_EVENT_DEFAULT, &event));

    printf("======================================================================\n");
    printf("Press SPACE to play the event.\n");
    printf("Press ESC   to quit\n");
    printf("======================================================================\n");

    key = 0;
    do
    {
        FMOD_EVENT_STATE state;

        if (_kbhit())
        {
            key = _getch();

            if (key == ' ')
            {
                ERRCHECK(result = FMOD_Event_Start(event));
            }
        }

        ERRCHECK(result = FMOD_EventSystem_Update(eventsystem));
        Sleep(15);

        ERRCHECK(result = FMOD_Event_GetState(event, &state));
        printf("Event is %s       \r", (state & FMOD_EVENT_STATE_PLAYING) ? "playing" : "stopped");

    } while (key != 27);

    ERRCHECK(result = FMOD_EventSystem_Release(eventsystem));
    return 0;
}
