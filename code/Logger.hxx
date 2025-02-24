#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

#include "Aliases.hxx"

class Logger
{
	public:
		Logger(const string& route);
		~Logger();
		
		void print(const string& message);
	
	private:
		std::ofstream stream;
};

#endif