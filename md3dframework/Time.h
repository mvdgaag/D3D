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

	void Init();
	void CleanUp();

private:
	Time() {}
	~Time() {}

	Time(Time const&) = delete;
	void operator=(Time const&) = delete;

	LARGE_INTEGER startTime;
	double fFreq;
};

