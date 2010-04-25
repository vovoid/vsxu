/*===============================================================================================
 Parameters Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 Demonstrates basic usage of event parameters
===============================================================================================*/
#include "../../api/inc/fmod_event.h"
#include "../../api/inc/fmod_errors.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define UPDATE_INTERVAL 100.0;


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
    FMOD_EVENTCATEGORY   *mastercategory;
    FMOD_EVENT           *car;
    FMOD_EVENTPARAMETER  *rpm;
    FMOD_EVENTPARAMETER  *load;
    FMOD_RESULT           result;
    int                   key;
    float                 rpm_val, rpm_min, rpm_max, rpm_increment, load_val, load_min, load_max, load_increment;

    printf("======================================================================\n");
    printf("Parameters Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("==============================-------=================================\n");
    printf("This demonstrates the use of FMOD event parameters. It simply plays an\n");
    printf("event, retrieves the parameters and allows the user to adjust them.\n");
    printf("======================================================================\n\n");

    ERRCHECK(result = FMOD_EventSystem_Create(&eventsystem));
    ERRCHECK(result = FMOD_EventSystem_Init(eventsystem, 64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL));
    ERRCHECK(result = FMOD_EventSystem_SetMediaPath(eventsystem, "..\\media\\"));
    ERRCHECK(result = FMOD_EventSystem_Load(eventsystem, "examples.fev", 0, 0));
    ERRCHECK(result = FMOD_EventSystem_GetGroup(eventsystem, "examples/AdvancedTechniques", FMOD_EVENT_DEFAULT, &eventgroup));
    ERRCHECK(result = FMOD_EventGroup_GetEvent(eventgroup, "car", FMOD_EVENT_DEFAULT, &car));

    ERRCHECK(result = FMOD_EventSystem_GetCategory(eventsystem, "master", &mastercategory));

    ERRCHECK(result = FMOD_Event_GetParameter(car, "load", &load));
    ERRCHECK(result = FMOD_EventParameter_GetRange(load, &load_min, &load_max));
    ERRCHECK(result = FMOD_EventParameter_SetValue(load, load_max));

    ERRCHECK(result = FMOD_Event_GetParameterByIndex(car, 0, &rpm));
    ERRCHECK(result = FMOD_EventParameter_GetRange(rpm, &rpm_min, &rpm_max));
    ERRCHECK(result = FMOD_EventParameter_SetValue(rpm, 1000.0f));

    ERRCHECK(result = FMOD_Event_Start(car));

    printf("======================================================================\n");
    printf("Press '<' or ',' to decrease RPM\n");
    printf("Press '>' or '.' to increase RPM\n");
    printf("Press '-' or '_' to decrease load\n");
    printf("Press '+' or '=' to increase load\n");
    printf("Press ESC        to quit\n");
    printf("======================================================================\n");

    rpm_increment   = (rpm_max - rpm_min) / (float)UPDATE_INTERVAL;
    ERRCHECK(result = FMOD_EventParameter_GetValue(rpm, &rpm_val));
    load_increment  = (load_max - load_min) / (float)UPDATE_INTERVAL;
    ERRCHECK(result = FMOD_EventParameter_GetValue(load, &load_val));

    key = 0;
    do
    {
        if (_kbhit())
        {
            key = _getch();

            if (key == '<' || key == ',')
            {
                rpm_val -= rpm_increment;
                if (rpm_val < rpm_min)
                {
                    rpm_val = rpm_min;
                }

                ERRCHECK(result = FMOD_EventParameter_SetValue(rpm, rpm_val));
            }
            else if (key == '>' || key == '.')
            {
                rpm_val += rpm_increment;
                if (rpm_val > rpm_max)
                {
                    rpm_val = rpm_max;
                }

                ERRCHECK(result = FMOD_EventParameter_SetValue(rpm, rpm_val));
            }
            if (key == '-' || key == '_')
            {
                load_val -= load_increment;
                if (load_val < load_min)
                {
                    load_val = load_min;
                }

                ERRCHECK(result = FMOD_EventParameter_SetValue(load, load_val));
            }
            else if (key == '+' || key == '=')
            {
                load_val += load_increment;
                if (load_val > load_max)
                {
                    load_val = load_max;
                }

                ERRCHECK(result = FMOD_EventParameter_SetValue(load, load_val));
            }
            else if (key == ' ')
            {
                int paused;

                ERRCHECK(result = FMOD_EventCategory_GetPaused(mastercategory, &paused));
                paused = !paused;
                ERRCHECK(result = FMOD_EventCategory_SetPaused(mastercategory, paused));
            }

        }

        ERRCHECK(result = FMOD_EventSystem_Update(eventsystem));
        Sleep(15);

        printf("RPM = %.4f, load = %.4f        \r", rpm_val, load_val);

    } while (key != 27);

    ERRCHECK(result = FMOD_EventGroup_FreeEventData(eventgroup, 0, 1));
    ERRCHECK(result = FMOD_EventSystem_Release(eventsystem));
    return 0;
}
