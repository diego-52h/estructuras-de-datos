#ifndef USER_H
#define USER_H

#include "Aliases.hxx"

const int BIASES_COUNT = 4;

enum Bias
{
	LEFT,
	RIGHT,
	CENTER,
	LIBERTARIAN,
};

struct User
{
	string name;
	
	int years;
	int tweets;
	
	int followerCount;
	int followeeCount;
	
	double bias[BIASES_COUNT];
	
	User();
};

bool compareByFollowers(const User& userA, const User& userB);
bool compareByFollowees(const User& userA, const User& userB);

#endif