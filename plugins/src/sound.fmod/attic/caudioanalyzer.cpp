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


#include <windows.h>
#include <Mmsystem.h>
#include <math.h>
#include "caudioanalyzer.h"
//*************************************************************
//*************************************************************
//*************************************************************
static char errText[1000];


CAudioAnalyzer::CAudioAnalyzer() {

  ext_pos = 0;
  
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM; 
	waveFormat.nChannels = CHANNELS; 
	waveFormat.nSamplesPerSec = FREQUENZY; 
	waveFormat.nAvgBytesPerSec = FREQUENZY * BYTES_PER_SAMPLE;
	waveFormat.nBlockAlign = BYTES_PER_SAMPLE; 
	waveFormat.wBitsPerSample = BITS_PER_SAMPLE; 
	waveFormat.cbSize = 0;

	MM_ERR(
		waveInOpen(
			&hWaveIn, 
			WAVE_MAPPER, 
			&waveFormat, 
			(DWORD)(LPVOID)waveInProc, 
			(DWORD)(LPVOID)this, 
			CALLBACK_FUNCTION | WAVE_FORMAT_DIRECT);
	)

	numBuffers = NUM_BUFFERS;
	bufferSize = BUFFER_SIZE;
	nChannels = CHANNELS;

	pCurrentWaveHdr = NULL;

	lstWaveHdrs = new std::list<LPWAVEHDR>;

	for(int i = 0; i < numBuffers; i++) {
		LPWAVEHDR pHdr = new WAVEHDR;
		pHdr->dwBufferLength = bufferSize * nChannels * sizeof(SAMPLE_DATA_TYPE);
		pHdr->lpData = (LPSTR)new SAMPLE_DATA_TYPE[bufferSize * nChannels];
		pHdr->dwFlags = 0;
		lstWaveHdrs->push_back(pHdr);
		prepareAddBuffer(pHdr);
	}

	pFFTObject = new FFTReal(bufferSize);
	pFFTObject512 = new FFTReal(bufferSize*2);

	MM_ERR(
		waveInStart(hWaveIn);
	)

}


//*************************************************************
//*************************************************************
//*************************************************************


CAudioAnalyzer::~CAudioAnalyzer() {
  //printf("a");

  //waveInStop(hWaveIn);
	/*MM_ERR(
		waveInReset(hWaveIn);
	)*/
	//printf("b");
	//MM_ERR(
//		waveInClose(hWaveIn);
//	)
	//printf("c");

	//delete pFFTObject;

	//printf("e");
	/*while(!lstWaveHdrs->empty()) {
		LPWAVEHDR pHdr = lstWaveHdrs->back();

		lstWaveHdrs->pop_back();
		delete pHdr->lpData;
		delete pHdr;
	}*/
}


//*************************************************************
//*************************************************************
//*************************************************************


void CAudioAnalyzer::prepareAddBuffer(LPWAVEHDR pHdr) {
	MM_ERR(
		waveInPrepareHeader(hWaveIn, pHdr, sizeof(WAVEHDR));
	)
	MM_ERR(
		waveInAddBuffer(hWaveIn, pHdr, sizeof(WAVEHDR));
	)
}


//*************************************************************
//*************************************************************
//*************************************************************


SAudioData* CAudioAnalyzer::getCurrentData(float inputLevel,int quality) {

	LPWAVEHDR pHdr = NULL;

	if(pCurrentWaveHdr != NULL) {
		pHdr = pCurrentWaveHdr;
		pCurrentWaveHdr = NULL;
	}
	else {
		pHdr = NULL;
	}

	if(pHdr != NULL) {
		SAMPLE_DATA_TYPE* sampleData = (SAMPLE_DATA_TYPE*)(LPVOID)pHdr->lpData;

		static int ch, i;
		static bool vu_run;
		vu_run = false;

		static float scale = inputLevel / (float)(1 << (BITS_PER_SAMPLE - 1));

		for(ch = 0; ch < nChannels; ch++) {
			for(i = 0; i < bufferSize; i++) {
				audioData.wave[ch][i] = (float)sampleData[i * 2 + ch] * scale;
			}
		}
    
    if (quality & 1) {
  		for(ch = 0; ch < nChannels; ch++) {
  			pFFTObject->do_fft(
  				(float*)(void*)audioData.complexSpectrum[ch], 
  				(float*)(void*)audioData.wave[ch]);
  			float re, im;
  			for(i = 0; i < bufferSize / 2; i++) {
  				re = audioData.complexSpectrum[ch][i];
  				im = audioData.complexSpectrum[ch][i + (bufferSize >> 1)];
  				audioData.spectrum[ch][i] = (float)sqrt(re * re + im * im) / (bufferSize / 2);
  			}
  		}
  
  		for(ch = 0; ch < nChannels; ch++) {
  			audioData.vu[ch] = 0;
  			for(i = 0; i < OCTAVE_SPECTRUM_SIZE; i++) {
  				audioData.octaveSpectrum[ch][i] = 0.0f;
  				for(int j = (1 << i); j < (1 << (i + 1)); j++) {
  					audioData.octaveSpectrum[ch][i] += audioData.spectrum[ch][j];
  				}
  				audioData.vu[ch] += audioData.octaveSpectrum[ch][i];
  			}
  		}
  		vu_run = true;
    }
    if (quality & 2) {
      int ofs = ext_pos*BUFFER_SIZE; // 0 or 512
      for(ch = 0; ch < nChannels; ch++) {
  			for(i = 0; i < bufferSize; i++) {
  				audioData.wave_1024[ch][ofs+i] = (float)sampleData[i * 2 + ch] * scale;
  			}
  		}
  		for(ch = 0; ch < nChannels; ch++) {
  			pFFTObject512->do_fft(
  				(float*)(void*)audioData.complexSpectrum_1024[ch], 
  				(float*)(void*)audioData.wave_1024[ch]);
  
  			float re, im;
  			for(i = 0; i < bufferSize; i++) {
  				re = audioData.complexSpectrum_1024[ch][i];
  				im = audioData.complexSpectrum_1024[ch][i + (BUFFER_SIZE*2 >> 1)];
  				audioData.spectrum_512[ch][i] = (float)sqrt(re * re + im * im) / (bufferSize);
  			}
  		} 
      if (!vu_run) // no need to do this task again :)
  		for(ch = 0; ch < nChannels; ch++) {
  			audioData.vu[ch] = 0;
  			for(i = 0; i < OCTAVE_SPECTRUM_SIZE; i++) {
  				audioData.octaveSpectrum[ch][i] = 0.0f;
  				for(int j = (1 << i); j < (1 << (i + 1)); j++) {
  					audioData.octaveSpectrum[ch][i] += audioData.spectrum_512[ch][j*2];
  				}
  				audioData.vu[ch] += audioData.octaveSpectrum[ch][i];
  			}
  		}
  		if (ofs == 1) ofs = 0; else ofs = 1;

    }
	
	}
	return &audioData;
}


//*************************************************************
//*************************************************************
//*************************************************************


void CALLBACK CAudioAnalyzer::waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {

	LPWAVEHDR pHdr; //, currHdr;
	CAudioAnalyzer* aa = (CAudioAnalyzer*)(LPVOID)dwInstance;

	switch(uMsg) {
		case WIM_DATA:
			pHdr = (LPWAVEHDR)(LPVOID)dwParam1;
			MM_ERR(
				waveInUnprepareHeader(aa->hWaveIn, pHdr, sizeof(WAVEHDR));
			)
			aa->pCurrentWaveHdr = pHdr;
			aa->prepareAddBuffer(pHdr);
			break;

		default:
			break;
	}
}
