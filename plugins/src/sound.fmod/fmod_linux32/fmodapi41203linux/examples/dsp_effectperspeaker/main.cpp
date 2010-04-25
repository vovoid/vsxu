/*===============================================================================================
 DSP Effect per speaker Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2005.

 This example shows how to manipulate a DSP network and as an example, creates 2 dsp effects,
 and splits a single sound into 2 audio paths, which it then filters seperately.
 To only have each audio path come out of one speaker each, DSP::setInputLevels is used just
 before the 2 branches merge back together again.
===============================================================================================*/
#include "../../api/inc/fmod.hpp"
#include "../../api/inc/fmod_errors.h"
#include "../common/wincompat.h"

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
    FMOD::System       *system;
    FMOD::Sound        *sound;
    FMOD::Channel      *channel;
    FMOD::DSP          *dspreverb, *dspchorus, *dsphead, *dspchannelmixer;
    FMOD_RESULT         result;
    int                 key;
    unsigned int        version;
    float               pan = 0;

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

    result = system->init(32, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);

    result = system->createSound("../media/drumloop.wav", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sound);
    ERRCHECK(result);

    printf("================================================================================\n");
    printf("DSP Effect per speaker example.  Copyright (c) Firelight Technologies 2004-2005.\n");
    printf("================================================================================\n");
    printf("Press 'L' to toggle reverb on/off on left speaker only\n");
    printf("Press 'R' to toggle chorus on/off on right speaker only\n");
    printf("Press '[' to pan sound left\n");
    printf("Press ']' to pan sound right\n");
    printf("Press 'Esc' to quit\n");
    printf("\n");

    result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
    ERRCHECK(result);

    /*
        Create the DSP effects.
    */  
    result = system->createDSPByType(FMOD_DSP_TYPE_REVERB, &dspreverb);
    ERRCHECK(result);

    result = system->createDSPByType(FMOD_DSP_TYPE_CHORUS, &dspchorus);
    ERRCHECK(result);

    /*
        Connect up the DSP network
    */

    /*
        When a sound is played, a subnetwork is set up in the DSP network which looks like this.
        Wavetable is the drumloop sound, and it feeds its data from right to left.

        [DSPHEAD]<------------[DSPCHANNELMIXER]
    */  
    result = system->getDSPHead(&dsphead);
    ERRCHECK(result);

    result = dsphead->getInput(0, &dspchannelmixer);
    ERRCHECK(result);

    /*
        Now disconnect channeldsp head from wavetable to look like this.

        [DSPHEAD]             [DSPCHANNELMIXER]
    */
    result = dsphead->disconnectFrom(dspchannelmixer);
    ERRCHECK(result);

    /*
        Now connect the 2 effects to channeldsp head.

                  [DSPREVERB]
                 /           
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \           
                  [DSPCHORUS]
    */
    result = dsphead->addInput(dspreverb);
    ERRCHECK(result);
    result = dsphead->addInput(dspchorus);
    ERRCHECK(result);
    
    /*
        Now connect the wavetable to the 2 effects

                  [DSPREVERB]
                 /           \
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \           /
                  [DSPCHORUS]
    */
    result = dspreverb->addInput(dspchannelmixer);
    ERRCHECK(result);
    result = dspchorus->addInput(dspchannelmixer);
    ERRCHECK(result);

    /*
        Now the drumloop will be twice as loud, because it is being split into 2, then recombined at the end.
        What we really want is to only feed the dspchannelmixer->dspreverb through the left speaker, and 
        dspchannelmixer->dspchorus to the right speaker.
        We can do that simply by setting the pan, or speaker levels of the connections.

                  [DSPREVERB]
                 /1,0        \
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \0,1        /
                  [DSPCHORUS]
    */
    {
        float leftinputon[2]  = { 1.0f, 0.0f };
        float rightinputon[2] = { 0.0f, 1.0f };
        float inputsoff[2]    = { 0.0f, 0.0f };

        result = dsphead->setInputLevels(0, FMOD_SPEAKER_FRONT_LEFT, leftinputon, 2);
        ERRCHECK(result);
        result = dsphead->setInputLevels(0, FMOD_SPEAKER_FRONT_RIGHT, inputsoff, 2);
        ERRCHECK(result);

        result = dsphead->setInputLevels(1, FMOD_SPEAKER_FRONT_LEFT, inputsoff, 2);
        ERRCHECK(result);
        result = dsphead->setInputLevels(1, FMOD_SPEAKER_FRONT_RIGHT, rightinputon, 2);
        ERRCHECK(result);
    }

    result = dspreverb->setBypass(true);
    result = dspchorus->setBypass(true);

    result = dspreverb->setActive(true);
    result = dspchorus->setActive(true);

    /*
        Main loop.
    */
    do
    {
        if (kbhit())
        {
            key = getch();

            switch (key)
            {
                case 'l' : 
                case 'L' : 
                {
                    static bool reverb = false;

                    dspreverb->setBypass(reverb);

                    reverb = !reverb;
                    break;
                }
                case 'r' : 
                case 'R' : 
                {
                    static bool chorus = false;

                    dspchorus->setBypass(chorus);

                    chorus = !chorus;
                    break;
                }
                case '[' :
                {
                    channel->getPan(&pan);
                    pan -= 0.1f;
                    if (pan < -1)
                    {
                        pan = -1;
                    }
                    channel->setPan(pan);
                    break;
                }
                case ']' :
                {
                    channel->getPan(&pan);
                    pan += 0.1f;
                    if (pan > 1)
                    {
                        pan = 1;
                    }
                    channel->setPan(pan);
                    break;
                }
            }
        }

        system->update();

        {
            int  channelsplaying = 0;

            system->getChannelsPlaying(&channelsplaying);

            printf("Channels Playing %2d : Pan = %.02f\r", channelsplaying, pan);
        }

        Sleep(10);

    } while (key != 27);

    printf("\n");

    /*
        Shut down
    */
    result = sound->release();
    ERRCHECK(result);

    result = dspreverb->release();
    ERRCHECK(result);
    result = dspchorus->release();
    ERRCHECK(result);

    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    return 0;
}


