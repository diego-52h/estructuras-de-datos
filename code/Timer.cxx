#include "Timer.hxx"

using namespace std::chrono;

Timer::Timer() : start(SELF.now())
{
}

double Timer::count() const
{
	Timer::instant finish = SELF.now();
	
	return duration_cast<microseconds>(finish - SELF.start).count() / 1e6;
}

void Timer::restart()
{
	SELF.start = SELF.now();
}

Timer::instant Timer::now() const
{
	return Timer::timer::now();
}