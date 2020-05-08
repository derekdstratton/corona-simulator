//
// Created by WIll on 5/7/2020.
//

#include "Timer.h"

Timer::Timer()
{
	initialized = false;
}

Timer::Timer(bool startNow)
{
	initialized = false;
	if(startNow)
		startTimer();
}

void Timer::startTimer()
{
	if( !initialized )
	{
		gettimeofday( &start, NULL );
		initialized = true;
	}
}

double Timer::getElapsedTime()
{
	struct timeval end;
	gettimeofday( &end, NULL );
	return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}
