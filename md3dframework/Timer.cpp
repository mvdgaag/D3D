#include "Timer.h"
#include "Time.h"


Timer::Timer()
{
	mIsRunning = false;
}


Timer::~Timer()
{
}


void Timer::Start()
{
	GetTime();
	mIsRunning = true;
}


void Timer::Reset()
{
	mTime = 0.0;
	mStartTime = theTime.GetTime();
}


void Timer::Stop()
{
	GetTime();
	mIsRunning = false;
}


double Timer::GetTime()
{
	if (mIsRunning)
	{
		double now = theTime.GetTime();
		mTime += now - mStartTime;
		mStartTime = now;
	}
	return mTime;
}


bool Timer::IsRunning()
{
	return mIsRunning;
}