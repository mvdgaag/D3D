#pragma once
#include <Windows.h>


#define theTime Time::GetInstance()


class Time
{
public:
	static Time& GetInstance()
	{
		static Time instance;
		return instance;
	}
	
	double GetTime() const 
	{
		LARGE_INTEGER endTime;
		QueryPerformanceCounter(&endTime);
		return (endTime.QuadPart - startTime.QuadPart) / fFreq;
	}

private:
	Time() 
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		fFreq = (double)freq.QuadPart;
		QueryPerformanceCounter(&startTime);
	}
	~Time() {};
	Time(Time const&) = delete;
	void operator=(Time const&) = delete;

	LARGE_INTEGER startTime;
	double fFreq;

};

