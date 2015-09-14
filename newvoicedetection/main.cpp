#include <iostream >
#include <vector>
#include "detection.h"
#include "voicedetection.h"
#include "CAudioTimeSandPitchS.h"
#include "CWavread.h"
#include "pcm2wav.h"
#include "CPitchShift.h"
#include <cmath>
#include <fstream>
#include <memory>
#include <map>
using namespace std;
#define MY_INT16 2



const char* url = "D:/iii.wav";
vector<Solmization> getMelody();
void PrintSgm( const vector<SpeechSegment>& sgms );
int main()
{
	CWavread wavReader;
	CAudioTimeSandPitchS pitch;
	vector<float> wavbuffer;
	wav_struct wavhead = wavReader.ReadHead( url );
	float* buffer = wavReader.ReadMonoData( wavhead );
	int sampleSize = wavhead.data_size/MY_INT16;
	int sampleRate = wavhead.frequency;

	CVoiceDetection detection;
	auto result( detection.Detection( buffer, sampleSize, sampleRate ) );
	float dst_freq = 260;
	float dst_time = 0.0;
	int melody[] = { C3,D3,E3,C3,C3,D3,E3,C3,E3,F3,G3,E3,F3,G3,G3,A3, G3, F3, E3,C3,G3, A3, G3, F3, E3,C3,D3,G2,C3 };
	float thythm[]={ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2,0.5,0.5,0.5,0.5,1, 1, 0.5,0.5,0.5,0.5,1, 1, 1, 1, 2  };
	int melodySize=sizeof(melody)/sizeof(melody[0]);
	PrintSgm( result );
	for (int i=0; i<melodySize;)
	{
		for( auto it = result.begin(); it != result.end(); ++it )
		{
			if( i >= melodySize )
				break;
			dst_freq=melody[i];
			dst_time = it->segTime / thythm[i] * 3;

			int distance = it->start * detection.m_hop;
			int length = (it->end - it->start) * sizeof(float) * detection.m_hop;

			auto datain = pitch.WavReadBuffer( buffer+distance, length,1);
			auto dataResult = pitch.TimeScalingAndPitchShiftingRobot(dst_freq, dst_time, datain, 1024);

			for (int k = 0;k < pitch.GetSize() / sizeof(float);k++)
			{
				wavbuffer.push_back(dataResult[k]);
			}
			i++;
		}

	}


	float* testout = new float[wavbuffer.size()];
	memset(testout, 0, wavbuffer.size());
	for (int i = 0;i < wavbuffer.size();i++)
	{
		testout[i] = wavbuffer[i];
	}
		
	CPcm2Wav convert(testout, wavbuffer.size()*sizeof(float), "D:/333.wav");
	Pcm2WavParameter params;
	params.channels = 1;
	params.formatTag = 3;
	params.sampleBits = 32;
	params.sampleRate = 44100;
	convert.Pcm2Wav(params);

	delete[] testout;
}




//int main()
//{
//	CWavread wavReader;
//	CAudioTimeSandPitchS pitch;
//	vector<float> wavbuffer;
//
//	wav_struct wavhead = wavReader.ReadHead(url);
//	short* buffer = wavReader.ReadMonoDataShort(wavhead);
//
//	int sampleSize = wavhead.data_size / MY_INT16;
//	int sampleRate = wavhead.frequency;
//
//	CVoiceDetection detection;
//	auto result(detection.Detection(buffer, sampleSize, sampleRate));
//
//	float dst_freq = 260;
//	float dst_time = 0.0;
//
//	int melody[] = { C3,D3,E3,C3,C3,D3,E3,C3,E3,F3,G3,E3,F3,G3,G3,A3, G3, F3, E3,C3,G3, A3, G3, F3, E3,C3,D3,G2,C3 };
//	float thythm[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2,0.5,0.5,0.5,0.5,1, 1, 0.5,0.5,0.5,0.5,1, 1, 1, 1, 2 };
//	int melodySize = sizeof(melody) / sizeof(melody[0]);
//	PrintSgm(result);
//	for (int i = 0; i<melodySize;)
//	{
//		for (auto it = result.begin(); it != result.end(); ++it)
//		{
//			if (i >= melodySize)
//				break;
//			dst_freq = melody[i];
//			dst_time = it->segTime / thythm[i] * 3;
//
//			int distance = it->start * detection.m_hop;
//			int length = (it->end - it->start) * sizeof(float) * detection.m_hop;
//
//			auto datain = pitch.WavReadBuffer(buffer + distance, length);
//
//			auto dataResult = pitch.TimeScalingAndPitchShiftingRobot(dst_freq, dst_time, datain, 1024);
//
//			for (int k = 0;k < pitch.GetSize() / sizeof(float);k++)
//			{
//				wavbuffer.push_back(dataResult[k]);
//			}
//
//			i++;
//		}
//
//	}
//
//
//	float* testout = new float[wavbuffer.size()];
//	memset(testout, 0, wavbuffer.size());
//	for (int i = 0;i < wavbuffer.size();i++)
//	{
//		testout[i] = wavbuffer[i];
//	}
//
//	CPcm2Wav convert(testout, wavbuffer.size()*sizeof(float), "D:/444.wav");
//	Pcm2WavParameter params;
//	params.channels = 1;
//	params.formatTag = 3;
//	params.sampleBits = 32;
//	params.sampleRate = 44100;
//	convert.Pcm2Wav(params);
//
//	delete[] testout;
//
//}


void WriteResultToFile( const float* buffer, const vector<SpeechSegment>& sgm, int hop )
{

	FILE* file = fopen( "result.raw", "wb" );
	for( auto it = sgm.begin(); it != sgm.end(); ++it )
	{
		int distance = it->start * hop;
		fwrite( buffer + distance, 1,(it->end - it->start)*hop*sizeof(float) , file );
	}
	fclose( file );
}

void PrintSgm( const vector<SpeechSegment>& sgms )
{
	for( auto it = sgms.begin(); it != sgms.end(); ++it  )
	{
		cout << it->start << " " << it->end << " freq:" << it->frequence << " beat:" << it->segTime << endl;
	}
}