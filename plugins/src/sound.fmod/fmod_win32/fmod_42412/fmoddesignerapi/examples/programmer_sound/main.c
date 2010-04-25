/*==============================================================================
 Programmer Sound Example
 Copyright (c), Firelight Technologies Pty, Ltd 2006-2009.

 Demonstrates how to use the "programmer sound" feature of the FMOD event system
==============================================================================*/
#include "../../api/inc/fmod_event.h"
#include "../../api/inc/fmod_errors.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

const char *MEDIA_PATH = "..\\media\\";
const char *FSB_NAME   = "..\\media\\tutorial_bank.fsb";

static int g_sound_index = 0;
#define SOUND_INDEX_MAX 4
static int g_index_map[SOUND_INDEX_MAX];

FMOD_SOUND  *fsb;
FMOD_SYSTEM *sys;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


void initIndexMap(FMOD_SOUND *fsb)
{
    FMOD_RESULT result;
    int numsubsounds, i;

    /* Sounds aren't in a logical order in the FSB, so we need to iterate
       through and assign the appropriate filenames to the index map
     */

    FMOD_OPENSTATE openstate;
    do
    {
        FMOD_Sound_GetOpenState(fsb, &openstate, 0, 0);
    } while (openstate != FMOD_OPENSTATE_READY);

	ERRCHECK(result = FMOD_Sound_GetNumSubSounds(fsb, &numsubsounds));

    for (i = 0; i < SOUND_INDEX_MAX; ++i)
    {
        g_index_map[i] = -1;
    }

    for (i = 0; i < numsubsounds; ++i)
    {
        FMOD_SOUND *sound;
        char name[100];

        ERRCHECK(result = FMOD_Sound_GetSubSound(fsb, i, &sound));

		do
		{
			FMOD_Sound_GetOpenState(fsb, &openstate, 0, 0);
		} while (openstate != FMOD_OPENSTATE_READY);

        ERRCHECK(result = FMOD_Sound_GetName(sound, name, 100));

        if(strncmp(name, "sequence-one.ogg", 100) == 0)
        {
            g_index_map[0] = i;
        }
        else if(strncmp(name, "sequence-two.ogg", 100) == 0)
        {
            g_index_map[1] = i;
        }
        else if(strncmp(name, "sequence-three.ogg", 100) == 0)
        {
            g_index_map[2] = i;
        }
        else if(strncmp(name, "sequence-four.ogg", 100) == 0)
        {
            g_index_map[3] = i;
        }
    }
}


FMOD_RESULT F_CALLBACK eventcallback(FMOD_EVENT *event, FMOD_EVENT_CALLBACKTYPE type, void *param1, void *param2, void *userdata)
{
    FMOD_RESULT result;

    switch (type)
    {
        case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_CREATE :
        {
            char *name      = (char *)param1;               // [in]  (char *) name of sound definition
            int entryindex  = *((int *)param2);             // [in]  (int) index of sound definition entry
            FMOD_SOUND **s = (FMOD_SOUND **)param2;       // [out] (FMOD::Sound *) a valid lower level API FMOD Sound handle
            char sound_name[128];

            ERRCHECK((g_index_map[g_sound_index] >= 0) ? FMOD_OK : FMOD_ERR_SUBSOUNDS);
            result = FMOD_Sound_GetSubSound(fsb, g_index_map[g_sound_index], s);
			if (result != FMOD_OK)
			{
				printf("FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_CREATE: %d: sound not ready, retry next frame\n", g_index_map[g_sound_index]);
				return result;
			}
            result = FMOD_Sound_GetName(*s, sound_name, 128);
			if (result != FMOD_OK)
			{
				printf("FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_CREATE: %d: sound not ready, retry next frame\n", g_index_map[g_sound_index]);
				return result;
			}
            printf("FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_CREATE '%s' (%d)\n", sound_name, g_index_map[g_sound_index]);
            break;
        }

        case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_RELEASE :
        {
            FMOD_SOUND *s = (FMOD_SOUND *)param2;         // [in]  (FMOD::Sound *) the FMOD sound handle that was previously created in FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_CREATE
            char sound_name[128];

            ERRCHECK(result = FMOD_Sound_GetName(s, sound_name, 128));
            printf("FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_RELEASE '%s'\n", sound_name);
            break;
        }
    }

    return FMOD_OK;
}


int main(int argc, char *argv[])
{
    FMOD_RESULT        result;
    FMOD_EVENTSYSTEM  *eventsystem;
    FMOD_EVENTGROUP   *eventgroup;
    FMOD_EVENT        *event;
    int                key;

    printf("======================================================================\n");
    printf("Programmer Sound. Copyright (c) Firelight Technologies 2006-2009.\n");
    printf("======================================================================\n");

    ERRCHECK(result = FMOD_EventSystem_Create(&eventsystem));
    ERRCHECK(result = FMOD_EventSystem_Init(eventsystem, 64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL));
    ERRCHECK(result = FMOD_EventSystem_SetMediaPath(eventsystem, (char *)MEDIA_PATH));
    ERRCHECK(result = FMOD_EventSystem_Load(eventsystem, "examples.fev", 0, 0));
    ERRCHECK(result = FMOD_EventSystem_GetGroup(eventsystem, "examples/FeatureDemonstration/SequencingAndStitching", FMOD_EVENT_DEFAULT, &eventgroup));

    ERRCHECK(result = FMOD_EventSystem_GetSystemObject(eventsystem, &sys));
    ERRCHECK(result = FMOD_System_CreateStream(sys, FSB_NAME, FMOD_2D | FMOD_NONBLOCKING | FMOD_SOFTWARE, 0, &fsb));

    initIndexMap(fsb);

    printf("======================================================================\n");
    printf("Press 'Space' to start the 'Programmer Sound' event\n");
    printf("Press '>'     to increase sound index\n");
    printf("Press '<'     to decrease sound index\n");
    printf("Press 'Esc'   to quit\n");
    printf("======================================================================\n");
    printf("Sound index = %d\n", g_sound_index + 1);

    key = 0;
    do
    {
        if (_kbhit())
        {
            key = _getch();

            switch(key)
            {
                case ' ' :
                    ERRCHECK(result = FMOD_EventGroup_GetEvent(eventgroup, "ProgrammerSounds", FMOD_EVENT_DEFAULT, &event));
                    ERRCHECK(result = FMOD_Event_SetCallback(event, eventcallback, 0));
                    ERRCHECK(result = FMOD_Event_Start(event));
                    break;

                case '>' : case '.':
                    ++g_sound_index;
                    g_sound_index = (g_sound_index >= SOUND_INDEX_MAX) ? SOUND_INDEX_MAX - 1 : g_sound_index;
                    printf("Sound index = %d\n", g_sound_index + 1);
                    break;

                case '<' : case ',':
                    --g_sound_index;
                    g_sound_index = (g_sound_index < 0) ? 0 : g_sound_index;
                    printf("Sound index = %d\n", g_sound_index + 1);
                    break;
            }
        }

        ERRCHECK(result = FMOD_EventSystem_Update(eventsystem));
        Sleep(10);

    } while (key != 27);

    ERRCHECK(result = FMOD_EventSystem_Unload(eventsystem));
    ERRCHECK(result = FMOD_Sound_Release(fsb));
    ERRCHECK(result = FMOD_EventSystem_Release(eventsystem));
    return 0;
}
