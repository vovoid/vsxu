/*===============================================================================================
 soundgroups Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 This example shows how to use the sound group behavior modes
===============================================================================================*/
#include <conio.h>
#include <stdio.h>
#include <climits>
#include <windows.h>

#include "../../api/inc/fmod.h"
#include "../../api/inc/fmod_errors.h"


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

int main(int argc, char * argv[])
{
    FMOD_SYSTEM        *system;
    FMOD_SOUND         *sound1, *sound2, *sound3;
    FMOD_SOUNDGROUP    *soundgroup;
    FMOD_CHANNEL       *channel[3];
    FMOD_RESULT         result;
    int                 key=0;
    int                 mode=1;
    unsigned int        version;

    printf("======================================================================\n");
    printf("soundgroups Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("======================================================================\n");
    printf("This example plays 3 sounds in a sound group, demonstrating the effect\n");
    printf("of the three different sound group behavior modes\n");
    printf("======================================================================\n\n");

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

    result = FMOD_System_Init(system, 100, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);
    	
	/*
	    Load some sound files from the hard disk.
	*/
	result = FMOD_System_CreateSound(system, "../media/drumloop.wav", FMOD_HARDWARE |  FMOD_LOOP_NORMAL, 0, &sound1);
	ERRCHECK(result);

	result = FMOD_System_CreateSound(system, "../media/jaguar.wav", FMOD_HARDWARE | FMOD_LOOP_NORMAL, 0, &sound2);
	ERRCHECK(result);

	result = FMOD_System_CreateSound(system, "../media/swish.wav", FMOD_HARDWARE | FMOD_LOOP_NORMAL, 0, &sound3);
	ERRCHECK(result);
    
    /* 
        Create the sound group with the following attributes:
          Name       = MyGroup
          MaxAudible = 1
          Behavior   = Mute 
    */
	result = FMOD_System_CreateSoundGroup(system, "MyGroup", &soundgroup);
	ERRCHECK(result);

    result = FMOD_SoundGroup_SetMaxAudible(soundgroup, 1);
    ERRCHECK(result);

    result = FMOD_SoundGroup_SetMaxAudibleBehavior(soundgroup, FMOD_SOUNDGROUP_BEHAVIOR_MUTE);
    ERRCHECK(result);

    result = FMOD_SoundGroup_SetMuteFadeSpeed(soundgroup, 2);
    ERRCHECK(result);

    /*
        Put the sounds in the sound group
    */
	result = FMOD_Sound_SetSoundGroup(sound1, soundgroup);
	ERRCHECK(result);

	result = FMOD_Sound_SetSoundGroup(sound2, soundgroup);
	ERRCHECK(result);

	result = FMOD_Sound_SetSoundGroup(sound3, soundgroup);
	ERRCHECK(result);
	
	/*
        Play the sounds (two will be muted because of the behavior mode)
    */
	result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, FALSE, &channel[0]);
	ERRCHECK(result);	

	result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound2, FALSE, &channel[1]);
	ERRCHECK(result);

	result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound3, FALSE, &channel[2]);
	ERRCHECK(result);
    
    /*
        Display help
    */
    printf("=========================================================================\n");
    printf("Press 1        BEAVIOR_FAIL \n");
    printf("      2        BEAVIOR_MUTE \n");
    printf("      3        BEAVIOR_STEALLOWEST\n");
    printf("      Q        Play/stop drumloop sound\n");
    printf("      W        Play/stop Jaguar sound\n");
    printf("      E        Play/stop shwish sound\n");
    printf("      ESC      Quit\n");
    printf("=========================================================================\n");
    
	do
	{
        float audibility;
        int index;

		if( kbhit())
		{
			key = getch();

			if( key=='1' )
			{
                result = FMOD_SoundGroup_SetMaxAudibleBehavior(soundgroup, FMOD_SOUNDGROUP_BEHAVIOR_FAIL);
                ERRCHECK(result);
			}

            if( key=='2' )
			{
                result = FMOD_SoundGroup_SetMaxAudibleBehavior(soundgroup, FMOD_SOUNDGROUP_BEHAVIOR_MUTE);
                ERRCHECK(result);
			}

            if( key=='3' )
			{
                result = FMOD_SoundGroup_SetMaxAudibleBehavior(soundgroup, FMOD_SOUNDGROUP_BEHAVIOR_STEALLOWEST);
                ERRCHECK(result);
			}

            if( key=='q' )
			{
                result = FMOD_Channel_GetIndex(channel[0],&index);

                if (!index)
                {
                    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound1, FALSE, &channel[0]);
	                if (result!=FMOD_ERR_MAXAUDIBLE)
                    {
                        ERRCHECK(result);
                    }
                }
                else
                {
                    result = FMOD_Channel_Stop(channel[0]);
                    ERRCHECK(result);
                }
			}

            if( key=='w' )
			{
                result = FMOD_Channel_GetIndex(channel[1],&index);

                if (!index)
                {
                    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound2, FALSE,&channel[1]);
                    if (result!=FMOD_ERR_MAXAUDIBLE)
                    {
	                    ERRCHECK(result);
                    }
                }
                else
                {
                    result = FMOD_Channel_Stop(channel[1]);
                    ERRCHECK(result);
                }
			}

            if( key=='e' )
			{
                result = FMOD_Channel_GetIndex(channel[2],&index);

                if (!index)
                {
                    result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound3, FALSE,&channel[2]);
                    if (result!=FMOD_ERR_MAXAUDIBLE)
                    {
	                    ERRCHECK(result);
                    }
                }
                else
                {
                    result = FMOD_Channel_Stop(channel[2]);
                    ERRCHECK(result);
                }
			}
        }

        // print out a small visual display
        {
            char s[80];
            char s1[6];
            char s2[3][6];  
            int i;
            FMOD_SOUNDGROUP_BEHAVIOR behavior;
            FMOD_SoundGroup_GetMaxAudibleBehavior(soundgroup, &behavior);
            
            switch (behavior)
            {
                case FMOD_SOUNDGROUP_BEHAVIOR_FAIL : 
                {
                    sprintf(s1,"FAIL");
                    break;
                }

                case FMOD_SOUNDGROUP_BEHAVIOR_MUTE : 
                {
                    sprintf(s1,"MUTE");
                    break;
                }

                case FMOD_SOUNDGROUP_BEHAVIOR_STEALLOWEST : 
                {
                    sprintf(s1,"STEAL");
                    break;
                }
            };
            
            for (i=0; i<3; i++)
            {
                FMOD_Channel_GetAudibility(channel[i], &audibility);
                
                if (!audibility)
                {
                    result = FMOD_Channel_GetIndex(channel[i], &index);
                    
                    if (!index)
                    {
                        sprintf(s2[i], "STOP");
                    }
                    else
                    {
                        sprintf(s2[i], "MUTE");
                    }
                }
                else
                {
                    sprintf(s2[i], "PLAY");
                }
            }   
            
            sprintf(s, "MODE:%6s      | SOUND1: %s | SOUND2: %s | SOUND3: %s |", s1, s2[0], s2[1], s2[2]);
            printf("%s\r", s);
        }

		result = FMOD_System_Update(system);
		ERRCHECK(result);
        Sleep(10);
	}while (key!=27);

	return 0;
}


