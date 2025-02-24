#include <math.h>

#include "User.hxx"

User::User()
{
	SELF.years = 0;
	SELF.tweets = 0;
	
	SELF.followerCount = 0;
	SELF.followeeCount = 0;
	
	for(int i = 0; i < BIASES_COUNT; i++)
		SELF.bias[i] = 1.0 / BIASES_COUNT;
}

double User::getImpact() const
{
	double baseline = 1;
	double prestige = std::log2(1 + SELF.years);
	
	return baseline + (SELF.tweets * prestige);
}

bool compareByFollowers(const User& userA, const User& userB)
{
	return userA.followerCount < userB.followerCount;
}

bool compareByFollowees(const User& userA, const User& userB)
{
	return userA.followeeCount < userB.followeeCount;
}