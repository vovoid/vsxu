/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef C_AUDIO_ANALYZER
#define C_AUDIO_ANALYZER

#include "FFTReal\FFTReal.h"

#include <list>
//using namespace std;

typedef struct {
	float wave[2][512];
	float complexSpectrum[2][512];
	float spectrum[2][256];
	float octaveSpectrum[2][8];
	float vu[2];
	
	float wave_1024[2][1024];
	float complexSpectrum_1024[2][1024];
	float spectrum_512[2][512];
} SAudioData;

#define FREQUENZY 44100
#define CHANNELS 2
#define BITS_PER_SAMPLE 16
#define SAMPLE_DATA_TYPE short

#define BUFFER_SIZE 512
#define OCTAVE_SPECTRUM_SIZE 8
#define NUM_BUFFERS 8

#define BYTES_PER_SAMPLE (CHANNELS * BITS_PER_SAMPLE / 8)
#define BUFFER_BYTE_SIZE (BUFFER_SIZE * BYTES_PER_SAMPLE)

class CAudioAnalyzer{
	SAudioData audioData;
	HWAVEIN hWaveIn;

	int numBuffers;
	int bufferSize;
	int nChannels;
	int ext_pos;

	std::list<LPWAVEHDR> *lstWaveHdrs;
	LPWAVEHDR pCurrentWaveHdr;

	FFTReal* pFFTObject;
	FFTReal* pFFTObject512;

	void prepareAddBuffer(LPWAVEHDR);
	static void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

public:
	CAudioAnalyzer();

  // 1 = only normal quality (86fps)
  // 2 = only high quality
  // 1+2 = both
	SAudioData* getCurrentData(float inputLevel = 1.0f, int quality = 0);

	~CAudioAnalyzer();
};





//#define MM_ERR(_MY_CODE) _MY_CODE

#define MM_ERR(_MY_CODE) {																		\
	MMRESULT err = _MY_CODE																			\
	if(err != MMSYSERR_NOERROR) {																\
		waveInGetErrorText(err, errText, 1000);										\
		MessageBox(NULL, errText, "Error", MB_OK | MB_ICONERROR);	\
	}																														\
}																															\

#endif
