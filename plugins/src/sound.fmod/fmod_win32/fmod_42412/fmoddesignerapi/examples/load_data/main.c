/*===============================================================================================
 Load Event Data Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 Demonstrates basic loading and unloading of event data per event
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
    FMOD_EVENTSYSTEM     *eventsystem;
    FMOD_EVENTGROUP      *eventgroup;
    FMOD_EVENT           *event = 0;
    FMOD_RESULT           result;
    int                   key;
    int memory_changed = 1;

    printf("======================================================================\n");
    printf("Load Event Data Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("==============================-------=================================\n");
    printf("This demonstrates loading and unloading of event data per event and\n");
    printf("per group.\n");
    printf("======================================================================\n\n");

    ERRCHECK(result = FMOD_EventSystem_Create(&eventsystem));
    ERRCHECK(result = FMOD_EventSystem_Init(eventsystem, 64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL));
    ERRCHECK(result = FMOD_EventSystem_SetMediaPath(eventsystem, "..\\media\\"));
    ERRCHECK(result = FMOD_EventSystem_Load(eventsystem, "examples.fev", 0, 0));
    ERRCHECK(result = FMOD_EventSystem_GetGroup(eventsystem, "examples/FeatureDemonstration/Basics", 1, &eventgroup));

    printf("======================================================================\n");
    printf("Press 'e'        to load event data\n");
    printf("Press 'E'        to unload event data\n");
    printf("Press 'g'        to load group data\n");
    printf("Press 'G'        to unload group data\n");
    printf("Press ESC        to quit\n");
    printf("======================================================================\n");

    key = 0;
    do
    {
        if (_kbhit())
        {

            key = _getch();

            if (key == 'e')
            {
                ERRCHECK(result = FMOD_EventGroup_GetEvent(eventgroup, "SimpleEvent", FMOD_EVENT_DEFAULT, &event));
                printf("Event data loaded\n");
                memory_changed = 1;
            }
            else if (key == 'E')
            {
                if (event)
                {
                    result = FMOD_EventGroup_FreeEventData(eventgroup, event, 1);
                    if (result != FMOD_ERR_INVALID_HANDLE)
                    {
                        ERRCHECK(result);
                        printf("Event data unloaded\n");
                        memory_changed = 1;
                    }
                    event = 0;
                }
            }
            else if (key == 'g')
            {
                ERRCHECK(result = FMOD_EventGroup_LoadEventData(eventgroup, FMOD_EVENT_RESOURCE_STREAMS_AND_SAMPLES, 0));
                printf("Event group data loaded\n");
                memory_changed = 1;
            }
            else if (key == 'G')
            {
                ERRCHECK(result = FMOD_EventGroup_FreeEventData(eventgroup, 0, 1));
                printf("Event group data unloaded\n");
                memory_changed = 1;
            }

        }

        if (memory_changed)
        {
            int memory_current, memory_max;
            ERRCHECK(result = FMOD_Memory_GetStats(&memory_current, &memory_max));

            printf("Memory usage: current = %10d, max = %10d\n", memory_current, memory_max);
            memory_changed = 0;
        }

        ERRCHECK(result = FMOD_EventSystem_Update(eventsystem));
        Sleep(15);

    } while (key != 27);

    ERRCHECK(result = FMOD_EventGroup_FreeEventData(eventgroup, 0, 1));
    ERRCHECK(result = FMOD_EventSystem_Release(eventsystem));
    return 0;
}
