#include "Time.h"


void Time::Init()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	fFreq = (double)freq.QuadPart;
	QueryPerformanceCounter(&startTime);
}


void Time::CleanUp()
{
}