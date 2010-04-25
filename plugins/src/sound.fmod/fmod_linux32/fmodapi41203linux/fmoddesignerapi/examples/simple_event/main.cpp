/*===============================================================================================
 SimpleEvent Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2007.

 Demonstrates basic usage of FMOD's data-driven event library (fmod_event.dll)
===============================================================================================*/
#include "../../api/inc/fmod_event.hpp"
#include "../../../examples/common/wincompat.h"

#include <stdio.h>
#include <stdlib.h>

const float UPDATE_INTERVAL = 100.0f;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d)\n", result);
        exit(-1);
    }
}


int main(int argc, char *argv[])
{
    FMOD::EventSystem    *eventsystem;
    FMOD::EventGroup     *eventgroup;
    FMOD::Event          *car;
    FMOD::EventParameter *rpm;
    FMOD::EventParameter *load;
    FMOD_RESULT           result;
    int                   key;
    float                 val, rangemin, rangemax, updatespeed;

    printf("======================================================================\n");
    printf("Simple Event Example.  Copyright (c) Firelight Technologies 2004-2005.\n");
    printf("==============================-------=================================\n");
    printf("This example plays an event created with the FMOD Designer sound \n");
    printf("designer tool.  It simply plays an event, retrieves the parameters\n");
    printf("and allows the user to adjust them.\n");
    printf("======================================================================\n\n");

    result = FMOD::EventSystem_Create(&eventsystem);
    ERRCHECK(result);
    result = eventsystem->init(64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL);
    ERRCHECK(result);
    result = eventsystem->setMediaPath("../media/");
    ERRCHECK(result);

    result = eventsystem->load("examples.fev", 0, 0);
    ERRCHECK(result);

    result = eventsystem->getGroup("examples/examples/car", FMOD_EVENT_DEFAULT, &eventgroup);
    ERRCHECK(result);
    result = eventgroup->getEvent("car", FMOD_EVENT_DEFAULT, &car);
    ERRCHECK(result);

    result = car->getParameter("load", &load);
    ERRCHECK(result);
    result = load->getRange(&rangemin, &rangemax);
    ERRCHECK(result);
    result = load->setValue(rangemax);
    ERRCHECK(result);

    result = car->getParameterByIndex(0, &rpm);
    ERRCHECK(result);
    result = rpm->getRange(&rangemin, &rangemax);
    ERRCHECK(result);
    result = rpm->setValue(1000.0f);
    ERRCHECK(result);

    result = car->start();
    ERRCHECK(result);

    printf("======================================================================\n");
    printf("Press '<'   to decrease RPM\n");
    printf("Press '>'   to increase RPM\n");
    printf("Press ESC   to quit\n");
    printf("======================================================================\n");

    updatespeed = (rangemax - rangemin) / UPDATE_INTERVAL;
    rpm->getValue(&val);
    key         = 0;
    do
    {
        if (kbhit())
        {
            key = getch();

            if (key == '<')
            {
                val -= updatespeed;
                if (val < rangemin)
                {
                    val = rangemin;
                }

                result = rpm->setValue(val);
                ERRCHECK(result);
            }
            else if (key == '>')
            {
                val += updatespeed;
                if (val > rangemax)
                {
                    val = rangemax;
                }

                result = rpm->setValue(val);
                ERRCHECK(result);
            }

        }

        eventsystem->update();
        Sleep(15);

        printf("Car RPM = %.4f       \r", val);
        fflush(stdout);

    } while (key != 27);

    result = eventgroup->freeEventData();
    ERRCHECK(result);

    result = eventsystem->release();
    ERRCHECK(result);

    return 0;
}
