/*===============================================================================================
 Info-only Event Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 Demonstrates usage and functionality of info-only events
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
    int state_changed = 1;

    printf("======================================================================\n");
    printf("Info-only Event Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("==============================-------=================================\n");
    printf("This demonstrates usage and functionality of info-only events.\n");
    printf("======================================================================\n\n");

    ERRCHECK(result = FMOD_EventSystem_Create(&eventsystem));
    ERRCHECK(result = FMOD_EventSystem_Init(eventsystem, 64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL));
    ERRCHECK(result = FMOD_EventSystem_SetMediaPath(eventsystem, "..\\media\\"));
    ERRCHECK(result = FMOD_EventSystem_Load(eventsystem, "examples.fev", 0, 0));
    ERRCHECK(result = FMOD_EventSystem_GetGroup(eventsystem, "examples/FeatureDemonstration/Basics", 0, &eventgroup));

    printf("======================================================================\n");
    printf("Press 'i'        to get an info-only event\n");
    printf("Press 'e'        to get an event instance\n");
    printf("Press 'u'        to unload all event data\n");
    printf("Press '1' - '0'  to set event volume between 10%% - 100%%\n");
    printf("Press Space      to start/stop the current event\n");
    printf("Press ESC        to quit\n");
    printf("======================================================================\n");


    key = 0;
    do
    {
        if (_kbhit())
        {

            key = _getch();

            if (key == 'i')
            {
                /* Get an info-only event.
                   Notes:
                   - This is the parent from which all instances of the
                     specified event are derived.
                   - This call will not allocate memory for event instances or
                     load sample data.
                   - The resulting event cannot be played, it simply allows
                     querying and setting of event properties.
                   - Any properties which are set on the info-only event will be
                     copied to all instances which are retrieved from then on.
                 */
                ERRCHECK(result = FMOD_EventGroup_GetEvent(eventgroup, "SimpleEventWithLooping", FMOD_EVENT_INFOONLY, &event));
                printf("getEvent(FMOD_EVENT_INFOONLY) succeeded\n");
                state_changed = 1;
            }
            else if (key == 'e')
            {
                /* Get an event instance.
                   Notes:
                   - This call will allocate memory for event instances and
                     load sample data if required.
                   - The resulting event can be played.
                   - Any properties which have been set on the info-only event
                     will be copied to this instance.
                 */
                ERRCHECK(result = FMOD_EventGroup_GetEvent(eventgroup, "SimpleEventWithLooping", FMOD_EVENT_DEFAULT, &event));
                printf("getEvent(FMOD_EVENT_DEFAULT) succeeded\n");
                state_changed = 1;
            }
            else if (key == ' ')
            {
                if (event)
                {
                    FMOD_EVENT_STATE state;

                    result = FMOD_Event_GetState(event, &state);
                    if (result != FMOD_ERR_INVALID_HANDLE)
                    {
                        ERRCHECK(result);
                    }

                    if (state & FMOD_EVENT_STATE_PLAYING)
                    {
                        /* Attempt to stop the event.
                           - This will fail if the event is info-only.
                         */
                        result = FMOD_Event_Stop(event, 0);
                        if(result == FMOD_OK)
                        {
                            printf("event->stop() succeeded\n");
                            state_changed = 1;
                        }
                        else
                        {
                            printf("event->stop() returned an error:\n\t(%d) %s\n", result, FMOD_ErrorString(result));
                        }
                    }
                    else
                    {
                        /* Attempt to start the event.
                           - This will fail if the event is info-only.
                         */
                        result = FMOD_Event_Start(event);
                        if (result == FMOD_OK)
                        {
                            printf("event->start() succeeded\n");
                            state_changed = 1;
                        }
                        else
                        {
                            printf("event->start() returned an error:\n\t(%d) %s\n", result, FMOD_ErrorString(result));
                        }
                    }
                }
            }
            else if (key >= '0' && key <= '9')
            {
                /* Attempt to set the event's volume.
                   - This will succeed on both info-only events and instances.
                   - Volume set on the info-only event will be copied to
                     all instances retrieved from now on.
                   - Volume set on an instance will only apply to that instance.
                 */
                float volume = (float)(key - '0');

                if (volume == 0)
                {
                    volume = 10.0f;
                }

                volume /= 10.0f;

                result = FMOD_Event_SetVolume(event, volume);
                if (result == FMOD_OK)
                {
                    printf("event->setVolume() succeeded\n");
                    state_changed = 1;
                }
                else
                {
                    printf("event->setVolume() returned an error:\n\t(%d) %s\n", result, FMOD_ErrorString(result));
                }
            }
            else if (key == 'u')
            {
                ERRCHECK(result = FMOD_EventGroup_FreeEventData(eventgroup, 0, 1));
                printf("Event data unloaded\n");
                state_changed = 1;
            }

        }

        if (state_changed)
        {
            int memory_current, memory_max;
            ERRCHECK(result = FMOD_Memory_GetStats(&memory_current, &memory_max));

            printf("Memory usage: current = %10d, max = %10d\n", memory_current, memory_max);

            if (event)
            {
                /* Attempt to get the event's volume.
                   - This will succeed on both info-only events and instances.
                 */
                float volume;

                result = FMOD_Event_GetVolume(event, &volume);
                if (result != FMOD_ERR_INVALID_HANDLE)
                {
                    ERRCHECK(result);
                    printf("Volume: %.2f\n", volume);
                }
            }

            state_changed = 0;
        }

        ERRCHECK(result = FMOD_EventSystem_Update(eventsystem));
        Sleep(15);

    } while (key != 27);

    ERRCHECK(result = FMOD_EventGroup_FreeEventData(eventgroup, 0, 1));
    ERRCHECK(result = FMOD_EventSystem_Release(eventsystem));
    return 0;
}
