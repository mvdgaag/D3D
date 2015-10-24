#pragma once


class Timer
{
public:
	Timer();
	~Timer();
	void Start();
	void Reset();
	void Stop();
	double GetTime();
	bool IsRunning();
	
private:
	bool mIsRunning;
	double mTime;
	double mStartTime;
};

