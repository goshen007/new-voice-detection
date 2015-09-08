#pragma once
#include "RtAudio.h"
#include "pcmaudio.h"
#include "scopeguard.h"


template <typename T>
int outputCallback(void *outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
	double, RtAudioStreamStatus, void* data)
{
	PcmData<T> *oData = (PcmData<T>*) data;
	unsigned int frames = nBufferFrames;
	if (oData->frameCounter + nBufferFrames > oData->totalFrames) {
		frames = oData->totalFrames - oData->frameCounter;
		oData->bufferBytes = frames * oData->channels * sizeof(T);
	}
	//����Ŀǰ���ŵ�ʱ���ᣨƫ������
	unsigned long offset = oData->frameCounter * oData->channels;
	//�����ݿ���������������в���
	memcpy(outputBuffer, oData->buffer + offset, oData->bufferBytes);
	oData->frameCounter += frames;
	//return 2 ��ʾ����
	if (oData->frameCounter >= oData->totalFrames)
		return 2;
	return 0;
}

template <typename T>
class CPcmPlayer
{
public:
	explicit CPcmPlayer( T* buffer, unsigned long totalBytes, const PcmPlayParameters & params);
	~CPcmPlayer() {}

	void Start();
private:
	void InitOutputParams(RtAudio::StreamParameters& params, RtAudio& adc);
	void PrintPlayMessage()const;
	void WaitForFinish(const RtAudio& adc )const;
private:
	PcmData<T> m_data;
	PcmPlayParameters m_params;

	CPcmPlayer(){}
	CPcmPlayer(const CPcmPlayer& p) {}
	CPcmPlayer& operator=(const CPcmPlayer& p){}
};


template<typename T>
inline CPcmPlayer<T>::CPcmPlayer(T * buffer, unsigned long totalBytes, const PcmPlayParameters & params)
{
	m_params = params;
	m_data.buffer = buffer;
	m_data.channels = params.nChannels;
	m_data.bufferBytes = params.framesPerBuffer * params.nChannels * sizeof(T);
	m_data.totalFrames = totalBytes / params.nChannels / sizeof(T);
	m_data.frameCounter = 0;
}

template<typename T>
inline void CPcmPlayer<T>::Start()
{

	RtAudio adc;
	assert(adc.getDeviceCount() >= 1);
	//��ʼ��������
	RtAudio::StreamParameters oParams;
	InitOutputParams(oParams, adc);
	//����ʱ����
	m_data.frameCounter = 0;
	//����Ƶ����ͬʱ����ȫ�Ĳ����쳣����
	TRY_CATCH_RTAUDIOERROR(adc.openStream(&oParams, NULL, m_params.audioFormat, 
		m_params.sampleRate,&m_params.framesPerBuffer, &outputCallback<T>, (void*)&m_data));
	//��ʼ���ţ�ͬʱ����ȫ�Ĳ����쳣����
	TRY_CATCH_RTAUDIOERROR(adc.startStream());
	//��ӡ������Ϣ
	PrintPlayMessage();
	//�ȴ����Ž���
	WaitForFinish(adc);
	if (adc.isStreamOpen()) adc.closeStream();
}

template<typename T>
inline void CPcmPlayer<T>::InitOutputParams(RtAudio::StreamParameters & params, RtAudio & adc)
{
	//�������ʹ��Ĭ������豸
	params.deviceId = adc.getDefaultOutputDevice();
	params.nChannels = m_data.channels;
	params.firstChannel = 0;
}

template<typename T>
inline void CPcmPlayer<T>::PrintPlayMessage()const
{
	cout << "\nPlay for " << m_data.totalFrames << " Frames.(buffer frames = " << m_params.framesPerBuffer << endl;
	cout << "\nChannnels:" << m_params.nChannels << "\nSample Rate:" << m_params.sampleRate << endl;
}

template<typename T>
inline void CPcmPlayer<T>::WaitForFinish(const RtAudio & adc) const
{
	while (adc.isStreamRunning())
	{
		SLEEP(100);
	}
}
