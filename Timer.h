//
// Created by WIll on 5/7/2020.
//

#ifndef CORONA_TIMER_H
#define CORONA_TIMER_H

#include <time.h>
#include <sys/time.h>

class Timer
{
	public:
		Timer();
		explicit Timer(bool StartNow);

		void startTimer();
		double getElapsedTime();

	private:
		bool initialized;
		struct timeval start;

};


#endif //CORONA_TIMER_H
