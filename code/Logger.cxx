#include <cassert>

#include "Logger.hxx"

Logger::Logger(const string& route) : stream(route)
{
	assert(SELF.stream.is_open());
}

Logger::~Logger()
{
	SELF.stream.close();
}

void Logger::print(const string& message)
{
	SELF.stream << message;
	SELF.stream << '\n';
	
	SELF.stream.flush();
}