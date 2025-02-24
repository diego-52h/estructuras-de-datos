#ifndef TIMER_H
#define TIMER_H

#include <chrono>

#include "Aliases.hxx"

class Timer
{
	public:
		Timer();
		
		double count() const;
		
		void restart();
	
	private:
		using timer = std::chrono::steady_clock;
		using instant = std::chrono::time_point<timer>;
		
		instant start;
		
		instant now() const;
};

#endif