#ifndef VOICEDETECTION_H_INCLUDED
#define VOICEDETECTION_H_INCLUDED
#include "calcthreshold.h"
#include <map>
#include <vector>
using std::map;
using std::vector;
using std::pair;

#define MIN_VOICE_FREQUENCY 80.0
#define MAX_VOICE_FREQUENCY 500
#define MAX(x,y) ( (x)>(y)?(x):(y) )
#define MIN(x,y) ( (x)<(y)?(x):(y) )
#define AMP_MIN_MUL 30

struct SpeechSegment{
	int frequence;		//�����Ļ�Ƶ��Ȼ���������µķ�����δʹ���������
	unsigned int start;	//������ʼ�㣬��start������ʼ
	unsigned int end;	//������ֹ�㣬��end��������
	float segTime;		//��һ��������ʱ��
	SpeechSegment( int fre, int s, int e, float b ):
	frequence(fre), start(s), end(e), segTime(b)
	{
	}
};

class CVoiceDetection{
private:
    CVoiceDetection( const CVoiceDetection& ){}
    void operator=( const CVoiceDetection& ){}

	//�Ƿ�������
	bool IsVoice( double amp, int zcr );
	//�Ƿ��п���������
	bool IsMaybeVoice( double amp, int zcr );
	//���������
    void CalcZeroCrossRate();
	//��������
    void CalcAmplitude();
	//��������������
    void CalcAmpThreshold();
	/**
	* @brief : ������֡������
	*/
	void EnFrame( const float* dataIn, int sampleSize, int winSize, int hop );
	void EnFrame(const short* dataIn, int sampleSize, int winSize, int hop);
	
	/**
	* @brief : �����Ŀ�ʼ�����ֹ����
	*/
    void StartEndPointDetection();

	/**
	* @brief : ��������λ�Ƶ�����ȡ���ʼ����ֹ�㣨δʹ�ã�
	*/
	vector<SpeechSegment> FindSpeechSegment( const float* buffer, int sampleRate );
	/**
	* @brief : �����ʱƽ�����Ȳ���ڻ�Ƶ��⣨δʹ�ã�
	*/
	vector<float> AMDFCalc( const vector<float>& amdfData );
	/**
	* @brief : ����������Ƶ��δʹ�ã�
	*/
	int VoiceFrequenceCalc( const vector<float>& amdfResult, int sampleRate );

	/**
	* @brief : ��ȡ�����γ��ȡ���ʼ����ֹ�㡣
	*/
	vector<SpeechSegment> GetSpeechSegmentInfo( int sampleRate );


public:
    CVoiceDetection();
	~CVoiceDetection();
    vector<SpeechSegment> Detection( const float* buffer, int sampleCount, int sampleRate );
	vector<SpeechSegment> Detection(const short* buffer, int sampleCount, int sampleRate);


public:
    vector<int> m_zeroCrossRate;			//���ÿ���������Ĺ�����
    vector<double> m_amplitude;				//���ÿ��������������
	vector<SpeechSegment> m_speechSegment;	//��������ε���Ϣ
	map<int, int> m_startEndMap;
    int m_winSize;
	int m_hop;
	float m_ampThreshold;
	int m_zcrThreshold;

	int m_minSilence;
	int m_minVoice;
	int m_voiceCount;
	int m_silenceCount;

	int m_voiseStatus;

	int m_frameCount;
	float** m_frameData;
};


#endif // VOICEDETECTION_H_INCLUDED
