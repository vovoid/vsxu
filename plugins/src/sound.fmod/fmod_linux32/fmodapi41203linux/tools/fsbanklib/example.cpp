/*
    This example builds the fsb's require for the "simple_event" example
*/

#include "fsbanklib.h"

#include <stdio.h>

#define NUMCARFILES   7
#define NUMOTHERFILES 9

// FMOD 3 Defines
#define FSOUND_HW3D          0x00001000  /* Attempts to make samples use 3d hardware acceleration. (if the card supports it) */
#define FSOUND_2D            0x00002000  /* Tells software (not hardware) based sample not to be included in 3d processing. */
#define FSOUND_HW2D          0x00080000  /* 2D hardware sounds.  allows hardware specific effects */

FSBANK_SAMPLE_DEFAULTS hw3d = {0,0,0,0,0,0,0,0,0, FSOUND_HW3D};
FSBANK_SAMPLE_DEFAULTS hw2d = {0,0,0,0,0,0,0,0,0, FSOUND_HW2D};
FSBANK_SAMPLE_DEFAULTS sw2d = {0,0,0,0,0,0,0,0,0, FSOUND_2D};

char *carfiles[NUMCARFILES] =
{
    "../../fmoddesignerapi/examples/media/car/offhigh.ogg",
    "../../fmoddesignerapi/examples/media/car/offlow.ogg",
    "../../fmoddesignerapi/examples/media/car/idle.ogg",
    "../../fmoddesignerapi/examples/media/car/offmid.ogg",
    "../../fmoddesignerapi/examples/media/car/onhigh.ogg",
    "../../fmoddesignerapi/examples/media/car/onlow.ogg",
    "../../fmoddesignerapi/examples/media/car/onmid.ogg"
};

FSBANK_SAMPLE_DEFAULTS *carsampledefaults[NUMCARFILES] =
{
    &hw3d, &hw3d, &hw3d, &hw3d, &hw3d, &hw3d, &hw3d
};

char *otherfiles[NUMOTHERFILES] =
{
    "../../fmoddesignerapi/examples/media/drumloop.wav",
    "../../fmoddesignerapi/examples/media/drumloop.wav",
    "../../fmoddesignerapi/examples/media/drumloop.wav",
    "../../fmoddesignerapi/examples/media/horse.ogg",
    "../../fmoddesignerapi/examples/media/horse.ogg",
    "../../fmoddesignerapi/examples/media/jaguar.ogg",
    "../../fmoddesignerapi/examples/media/jaguar.ogg",
    "../../fmoddesignerapi/examples/media/seal.ogg",
    "../../fmoddesignerapi/examples/media/seal.ogg"
};

FSBANK_SAMPLE_DEFAULTS *othersampledefaults[NUMOTHERFILES] =
{
    &hw2d, &hw3d, &sw2d, &hw2d, &sw2d, &hw2d, &sw2d, &hw2d, &sw2d
};

void Update(int index, int memused, void *userdata)
{
    printf("UPDATE : File %d, memory used %d kb\n", index, memused / 1024);
}

void Debug(const char *debugstring, void *userdata)
{
    printf("DEBUG  : %s\n", debugstring);
}

int main(void)
{
    FSBANK_RESULT result;

    result = FSBank_Init();
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return -1;
    }

    result = FSBank_SetUpdateCallback(Update, 0);
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return -1;
    }

    result = FSBank_SetDebugCallback(Debug, 0);
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return -1;
    }

    /*
        This version compiles the wavs into 1 fsb.
    */
    printf("Building car.fsb...\n");
    result = FSBank_Build(FSBANK_BUILDMODE_SINGLE, FSBANK_FORMAT_PCM, FSBANK_PLATFORM_PC, FSBANK_BUILD_XMAFILTERHIGHFREQ, 0, "car.fsb", NUMCARFILES, &carfiles[0], &carsampledefaults[0], 0, 0);
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return -1;
    }
    printf("done.\n");

    printf("Building other.fsb...\n");
    result = FSBank_Build(FSBANK_BUILDMODE_SINGLE, FSBANK_FORMAT_PCM, FSBANK_PLATFORM_PC, FSBANK_BUILD_XMAFILTERHIGHFREQ, 0, "other.fsb", NUMOTHERFILES, &otherfiles[0], &othersampledefaults[0], 0, 0);
    printf("done.\n");

    result = FSBank_Close();
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return -1;
    }

    return 0;
}
