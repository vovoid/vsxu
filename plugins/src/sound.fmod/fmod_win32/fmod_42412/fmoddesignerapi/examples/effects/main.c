/*===============================================================================================
 Effects Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 Demonstrates basic usage of event effects
===============================================================================================*/
#include "../../api/inc/fmod_event.h"
#include "../../api/inc/fmod_errors.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

#define UPDATE_INTERVAL 100.0f;

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
    FMOD_EVENTSYSTEM    *eventsystem;
    FMOD_EVENT          *event;
    FMOD_EVENTPARAMETER *param00;
    FMOD_RESULT          result;
    int                  key;
    float                param00_val, param00_min, param00_max, param00_increment;

    printf("======================================================================\n");
    printf("Effects Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("==============================-------=================================\n");
    printf("This demonstrates the use of FMOD event effects. It simply plays an\n");
    printf("event with effects applied and allows the user to adjust the parameters.\n");
    printf("======================================================================\n\n");

    ERRCHECK(result = FMOD_EventSystem_Create(&eventsystem));
    ERRCHECK(result = FMOD_EventSystem_Init(eventsystem, 64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL));
    ERRCHECK(result = FMOD_EventSystem_SetMediaPath(eventsystem, "..\\media\\"));
    ERRCHECK(result = FMOD_EventSystem_Load(eventsystem,"examples.fev", 0, 0));
    ERRCHECK(result = FMOD_EventSystem_GetEvent(eventsystem, "examples/FeatureDemonstration/Effects/PropertyAutomationEffectEnvelope", FMOD_EVENT_DEFAULT, &event));
    ERRCHECK(result = FMOD_Event_GetParameter(event, "param00", &param00));
    ERRCHECK(result = FMOD_EventParameter_GetRange(param00, &param00_min, &param00_max));
    ERRCHECK(result = FMOD_EventParameter_SetValue(param00, 0.0f));
    ERRCHECK(result = FMOD_Event_Start(event));
    
    printf("======================================================================\n");
    printf("Press '<' or ',' to decrease param00\n");
    printf("Press '>' or '.' to increase param00\n");
    printf("Press ESC        to quit\n");
    printf("======================================================================\n");
    printf("\n");

    param00_increment = (param00_max - param00_min) / UPDATE_INTERVAL;
    ERRCHECK(result = FMOD_EventParameter_GetValue(param00, &param00_val));


    key = 0;
    do
    {
        if (_kbhit())
        {
            key = _getch();

            if (key == '<' || key == ',')
            {
                param00_val -= param00_increment;
                if (param00_val < param00_min)
                {
                    param00_val = param00_min;
                }

                ERRCHECK(result = FMOD_EventParameter_SetValue(param00, param00_val));
            }
            else if (key == '>' || key == '.')
            {
                param00_val += param00_increment;
                if (param00_val > param00_max)
                {
                    param00_val = param00_max;
                }

                ERRCHECK(result = FMOD_EventParameter_SetValue(param00, param00_val));
            }
        }

        ERRCHECK(result = FMOD_EventSystem_Update(eventsystem));
        Sleep(15);

        printf("param00 = %.4f         \r", param00_val);

    } while (key != 27);

    ERRCHECK(result = FMOD_EventSystem_Release(eventsystem));
    return 0;
}
