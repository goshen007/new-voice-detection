#pragma  once



#include <iostream> 
#include <fstream> 
#include <string.h> 
#include<math.h> 
#include<cmath> 
#include<stdlib.h> 
#include <bitset> 
#include <iomanip> 


struct wav_struct 
{ 
	const char* filename;
	unsigned long file_size;        //�ļ���С     
	unsigned short channel;         //ͨ����     
	unsigned long frequency;        //����Ƶ��     
	unsigned long Bps;              //Byte��     
	unsigned short sample_num_bit;  //һ��������λ��     
	unsigned long data_size;        //���ݴ�С     
	unsigned char *data;            //��Ƶ���� ,����Ҫ����ʲô�Ϳ�����λ���ˣ�������ֻ�ǵ����ĸ������� 
};


class CWavread
{
private:
	CWavread(const CWavread& cp){}
	CWavread& operator = (const CWavread& cp){}

	int hex_char_value(char c);
	int hex_to_decimal(char *szHex);
public:
	CWavread();
	~CWavread();
	wav_struct ReadHead(const char* filename);
	//��ȡ��������Ƶ����
	float* ReadMonoData(wav_struct wav);
	//��ȡ˫������Ƶ����
	float* ReadStereoData(wav_struct wav);
};