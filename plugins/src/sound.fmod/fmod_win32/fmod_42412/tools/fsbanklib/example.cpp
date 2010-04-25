#include "fsbanklib.h"

#include <stdio.h>

#define NUMFILES 2

char *files[NUMFILES] =
{
    "jbtennis.wav",
    "drumloop.wav"
};

void __stdcall Update(int index, int memused, void *userdata)
{
    printf("UPDATE : File %s, memory used %d kb\n", files[index], memused / 1024);
}

void __stdcall Debug(const char *debugstring, void *userdata)
{
    printf("DEBUG  : %s\n", debugstring);
}

void main()
{
    FSBANK_RESULT result;

    result = FSBank_Init();
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return;
    }

    result = FSBank_SetUpdateCallback(Update, 0);
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return;
    }

    result = FSBank_SetDebugCallback(Debug, 0);
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return;
    }

#if 1
    /*
        This version compiles the wavs into 1 fsb.
    */
    result = FSBank_Build(FSBANK_BUILDMODE_SINGLE, FSBANK_FORMAT_PCM, FSBANK_PLATFORM_PC, FSBANK_BUILD_DEFAULT, 0, "test.fsb", NUMFILES, &files[0], 0, 0, 0);
#else
    /*
        This version compiles the wavs into their own fsb.  1 each.
    */
    result = FSBank_Build(FSBANK_BUILDMODE_MULTI, FSBANK_FORMAT_PCM, FSBANK_PLATFORM_PC, FSBANK_BUILD_DEFAULT, 0, ".", NUMFILES, &files[0], 0, 0, 0);
#endif
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return;
    }

    result = FSBank_Close();
    if (result != FSBANK_OK)
    {
        printf("ERROR\n");
        return;
    }
}
