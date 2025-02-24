#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <sys/resource.h>

#include "Aliases.hxx"
#include "Logger.hxx"
#include "Network.hxx"
#include "Timer.hxx"
#include "User.hxx"

using stream = std::fstream;

const string LOG_FILE = "log.txt";
const string OUT_FILE_1 = "biases.txt";
const string OUT_FILE_2 = "components.txt";

unordered_map<string, array<double, BIASES_COUNT>> presets = {
	{"Iz", {1, 0, 0, 0}},
	{"De", {0, 1, 0, 0}},
	{"Ce", {0, 0, 1, 0}},
	{"Li", {0, 0, 0, 1}},
	
	{"latercera", {0, 0, 1, 0}},
	{"elmostrador", {0, 0, 0, 1}},
	{"Cooperativa", {1, 0, 0, 0}},
	{"soyvaldiviacl", {0, 1, 0, 0}},
};

/*
	* https://mrswolf.github.io/memory-usage-cpp
	* https://www.youtube.com/watch?v=Os5cK0H8EOA
*/

long get_memory_usage()
{
	struct rusage usage;
	
	getrusage(RUSAGE_SELF, &usage);
	
	return usage.ru_maxrss;
}

void parse_args(int argc, char* args[], stream& usersData, stream& connectionsData)
{
	try
	{
		for(int i = 1; i < (argc - 1); i++)
		{
			string arg = args[i];
			
			if(arg == "--users")
				usersData = std::fstream(args[++i]);
			
			if(arg == "--connections")
				connectionsData = std::fstream(args[++i]);
		}
		
		if(!usersData.is_open() || !connectionsData.is_open())
			throw -1;
	}
	
	catch(...)
	{
		throw "Wrong command line arguments";
	}
}

Network retrieve_data(stream& usersData, stream& connectionsData)
{
	const int INF = 10000;
	const int CURRENT_YEAR = 2025;
	
	Network network(createNew<Logger>(LOG_FILE));
	
	usersData.ignore(INF, '\n');
	connectionsData.ignore(INF, '\n');
	
	while(!usersData.eof())
	{
		User user;
		
		try
		{
			string name;
			
			string years;
			string tweets;
			
			usersData.ignore(INF, ';');
			
			std::getline(usersData, name, ';');
			std::getline(usersData, tweets, ';');
			
			usersData.ignore(INF, ';');
			usersData.ignore(INF, '+');
			usersData.ignore(INF, ' ');
			usersData.ignore(2, '\0');
			
			std::getline(usersData, years, '"');
			
			usersData.ignore(INF, '\n');
			
			user.name = name;
			
			user.years = CURRENT_YEAR - std::stoi(years);
			user.tweets = std::stoi(tweets);
		}
		
		catch(...)
		{
			throw "Error while reading users file";
		}
		
		if(presets.contains(user.name))
		{
			for(int i = 0; i < BIASES_COUNT; i++)
				user.bias[i] = presets.at(user.name)[i];
		}
		
		network.insertUser(user);
	}
	
	while(!connectionsData.eof())
	{
		string followee;
		string follower;
		
		try
		{
			std::getline(connectionsData, followee, ';');
			std::getline(connectionsData, follower, '\n');
			
			network.connectUsers(followee, follower);
		}
		
		catch(...)
		{
			throw "Error while reading connections file";
		}
	}
	
	return network;
}

int main(int argc, char* args[])
{
	stream usersData;
	stream connectionsData;
	
	stream out1(OUT_FILE_1, std::ios::out);
	stream out2(OUT_FILE_2, std::ios::out);
	
	std::cout << std::fixed;
	std::cout << std::setprecision(8);
	
	long baseline = get_memory_usage();
	
	try
	{
		parse_args(argc, args, usersData, connectionsData);
		
		Timer timer;
		Network network = retrieve_data(usersData, connectionsData);
		
		std::cout << "Resident memory used: " << get_memory_usage() - baseline << " [KB]" << '\n';
		std::cout << '\n';
		
		/* A */
		{
			vector<string> presetUsers;
			
			for(const auto& [name, bias] : presets)
				presetUsers.push_back(name);
			
			timer.restart();
			
			network.computeUsersBias(presetUsers);
			
			std::cout << "Bias computed in " << timer.count() << " [sec]" << '\n';
			
			out1 << std::left;
			out1 << std::fixed;
			out1 << std::setprecision(10);
			
			out1 << "| " << std::setw(20) << "NAME" << " | " << std::setw(12) << "LEFT" << " | " << std::setw(12) << "RIGHT" << " | " << std::setw(12) << "CENTER" << " | " << std::setw(12) << "LIBERTARIAN" << " |";
			out1 << '\n';
			
			for(const User& user : network.getUsers())
			{
				out1 << "| ";
				out1 << std::setw(20) << user.name << " |";
				
				for(int i = 0; i < BIASES_COUNT; i++)
					out1 << " " << std::setw(12) << user.bias[i] << " |";
				
				out1 << '\n';
			}
		}
		
		/* B */
		{
			timer.restart();
			
			vector<vector<reference<User>>> SCComponents = network.getSCComponents();
			
			std::cout << "SCCs computed in " << timer.count() << " [sec]" << '\n';
			
			out2 << "SCCs: " << SCComponents.size() << '\n';
			out2 << '\n';
			
			int count = 0;
			
			for(const vector<reference<User>>& component : SCComponents)
			{
				if(component.size() == 1)
					continue;
				
				count++;
				
				out2 << "[" << count << "]" << '\n';
				
				for(const User& user : component)
					out2 << " * " << user.name << '\n';
				
				out2 << '\n';
			}
		}
		
		/* C */
		{
			vector<User> topA = network.getUsers();
			vector<User> topB = network.getUsers();
			
			timer.restart();
			
			std::sort(topA.begin(), topA.end(), compareByFollowers);
			std::sort(topB.begin(), topB.end(), compareByFollowees);
			
			std::cout << "Top computed in " << timer.count() << " [sec]" << '\n';
			
			int count = std::min(10, (int) topA.size());
			
			std::cout << '\n';
			std::cout << "TOP INFLUENTIAL:" << '\n';
			std::cout << '\n';
			
			for(int i = 0; i < count; i++)
			{
				User& user = topA[(topA.size() - 1) - i];
				
				std::cout << "[" << (i + 1) << "] " << user.name << ": " << user.followerCount << '\n';
			}
			
			std::cout << '\n';
			std::cout << "TOP INFLUENCEABLE:" << '\n';
			std::cout << '\n';
			
			for(int i = 0; i < count; i++)
			{
				User& user = topB[(topB.size() - 1) - i];
				
				std::cout << "[" << (i + 1) << "] " << user.name << ": " << user.followeeCount << '\n';
			}
		}
		
		out1.flush();
		out2.flush();
	}
	
	catch(const char* error)
	{
		std::cerr << error << '\n';
		
		return -1;
	}
	
	out1.close();
	out2.close();
}