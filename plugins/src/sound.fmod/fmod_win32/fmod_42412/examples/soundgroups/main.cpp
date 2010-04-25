/*===============================================================================================
 soundgroups Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2009.

 This example shows how to use the sound group behavior modes
===============================================================================================*/
#include <conio.h>
#include <stdio.h>
#include <climits>
#include <windows.h>

#include "../../api/inc/fmod.hpp"
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
    FMOD::System        *system;
    FMOD::Sound         *sound1, *sound2, *sound3;
    FMOD::SoundGroup    *soundgroup;
    FMOD::Channel       *channel[3];
    FMOD_RESULT          result;
    int                  key=0;
    int                  mode=1;
    unsigned int         version;

    printf("======================================================================\n");
    printf("soundgroups Example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("======================================================================\n");
    printf("This example plays 3 sounds in a sound group, demonstrating the effect\n");
    printf("of the three different sound group behavior modes\n");
    printf("======================================================================\n\n");

    /*
        Create a System object and initialize.
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);    

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return 0;
    }

    result = system->init(100, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);
    	
	/*
	    Load some sound files from the hard disk.
	*/
	result = system->createSound("../media/drumloop.wav", FMOD_HARDWARE |  FMOD_LOOP_NORMAL, 0, &sound1);
	ERRCHECK(result);

	result = system->createSound("../media/jaguar.wav", FMOD_HARDWARE | FMOD_LOOP_NORMAL, 0, &sound2);
	ERRCHECK(result);

	result = system->createSound("../media/swish.wav", FMOD_HARDWARE | FMOD_LOOP_NORMAL, 0, &sound3);
	ERRCHECK(result);
    
    /* 
        Create the sound group with the following attributes:
          Name       = MyGroup
          MaxAudible = 1
          Behavior   = Mute 
    */
	result = system->createSoundGroup("MyGroup",&soundgroup);
	ERRCHECK(result);

    result = soundgroup->setMaxAudible(1);
    ERRCHECK(result);

    result = soundgroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_MUTE);
    ERRCHECK(result);

    result = soundgroup->setMuteFadeSpeed(2);
    ERRCHECK(result);

    /*
        Put the sounds in the sound group
    */
	result = sound1->setSoundGroup(soundgroup);
	ERRCHECK(result);

	result = sound2->setSoundGroup(soundgroup);
	ERRCHECK(result);

	result = sound3->setSoundGroup(soundgroup);
	ERRCHECK(result);
	
    
	/*
        Play the sounds (two will be muted because of the behavior mode)
    */
	result = system->playSound(FMOD_CHANNEL_FREE, sound1,false,&channel[0]);
	ERRCHECK(result);	

	result = system->playSound(FMOD_CHANNEL_FREE, sound2,false,&channel[1]);
	ERRCHECK(result);

	result = system->playSound(FMOD_CHANNEL_FREE, sound3,false,&channel[2]);
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

		if( _kbhit())
		{
			key = _getch();

			if( key=='1' )
			{
                result = soundgroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_FAIL);
                ERRCHECK(result);
			}

            if( key=='2' )
			{
                result = soundgroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_MUTE);
                ERRCHECK(result);
			}

            if( key=='3' )
			{
                result = soundgroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_STEALLOWEST);
                ERRCHECK(result);
			}

            if( key=='q' )
			{
                channel[0]->getIndex(&index);
                if (!index)
                {
                    result = system->playSound(FMOD_CHANNEL_FREE, sound1,false,&channel[0]);
	                if (result!=FMOD_ERR_MAXAUDIBLE)
                    {
                        ERRCHECK(result);
                    }
                }
                else
                {
                    result = channel[0]->stop();
                    ERRCHECK(result);
                }
			}

            if( key=='w' )
			{
                channel[1]->getIndex(&index);
                if (!index)
                {
                    result = system->playSound(FMOD_CHANNEL_FREE, sound2,false,&channel[1]);
                    if (result!=FMOD_ERR_MAXAUDIBLE)
                    {
	                    ERRCHECK(result);
                    }
                }
                else
                {
                    result = channel[1]->stop();
                    ERRCHECK(result);
                }
			}

            if( key=='e' )
			{
                channel[2]->getIndex(&index);
                if (!index)
                {
                    result = system->playSound(FMOD_CHANNEL_FREE, sound3,false,&channel[2]);
                    if (result!=FMOD_ERR_MAXAUDIBLE)
                    {
	                    ERRCHECK(result);
                    }
                }
                else
                {
                    result = channel[2]->stop();
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
            soundgroup->getMaxAudibleBehavior(&behavior);
            
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
                channel[i]->getAudibility(&audibility);

                if (!audibility)
                {
                    channel[i]->getIndex(&index);
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

		result = system->update();
		ERRCHECK(result);
        Sleep(10);
	}while (key!=27);

	return 0;
}


