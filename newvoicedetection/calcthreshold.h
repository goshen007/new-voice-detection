#pragma once
#include <vector>
using std::vector;

class CThreshodCalculator {
public:
	CThreshodCalculator(const std::vector<double>& amp,
		const std::vector<int>&zcr);

	double GetThreshold()const;
	double GetMaxAmp()const;
	double GetAveAmp()const;
	double GetNoiseAmp()const;
private:
	vector<double> m_amp;	//����
	vector<int> m_zcr;		//������

	static const double kZcrDown;
	static const double kZcrUp;
private:
	bool IsNoise(int max, double zcr)const {
		return zcr < double(max) / kZcrUp && zcr > double(max) / kZcrDown;
	}

	CThreshodCalculator(const CThreshodCalculator& t) {}
	CThreshodCalculator& operator=(const CThreshodCalculator& t) {}
};
