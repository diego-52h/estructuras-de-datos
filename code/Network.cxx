#include <assert.h>

#include <sstream>

#include "Logger.hxx"
#include "Network.hxx"

Network::Network(pointer<Logger> logger) : logger(std::move(logger))
{
}

Network::Network(const vector<User>& users, pointer<Logger> logger) : users(users), followers(users.size()), followees(users.size()), logger(std::move(logger))
{
}

void Network::insertUser(const User& user)
{
	SELF.users.push_back(user);
	
	SELF.followers.push_back({});
	SELF.followees.push_back({});
	
	SELF.nameToID.insert({user.name, SELF.users.size() - 1});
}

void Network::connectUsers(const string& followee, const string& follower)
{
	int ID1 = SELF.nameToID.at(followee);
	int ID2 = SELF.nameToID.at(follower);
	
	SELF.users[ID1].followerCount++;
	SELF.users[ID2].followeeCount++;
	
	SELF.followers[ID1].insert(ID2);
	SELF.followees[ID2].insert(ID1);
}

const vector<User>& Network::getUsers()
{
	return SELF.users;
}

void Network::computeUsersBias(const vector<string>& presetUsers)
{
	set<int> processed;
	
	for(string name : presetUsers)
	{
		if(!SELF.nameToID.contains(name))
			continue;
		
		int ID = SELF.nameToID.at(name);
		
		processed.insert(ID);
	}
	
	vector<set<int>> auxFollowers = SELF.followers;
	vector<set<int>> auxFollowees = SELF.followees;
	
	SELF.removeCycles();
	
	for(int i = 0; i < SELF.users.size(); i++)
		SELF.computeBias(i, processed);
	
	SELF.followers = auxFollowers;
	SELF.followees = auxFollowees;
}

vector<vector<reference<User>>> Network::getSCComponents()
{
	set<int> seen;
	stack<int> order = SELF.getReversePostorder();
	
	vector<vector<reference<User>>> components;
	
	while(!order.empty())
	{
		int user = order.top(); order.pop();
		
		if(seen.contains(user))
			continue;
		
		vector<reference<User>> component; SELF.getSCComponent(user, seen, component);
		
		components.push_back(component);
	}
	
	return components;
}

stack<int> Network::getReversePostorder()
{
	if(SELF.reversePostorder.size() == SELF.users.size())
		return SELF.reversePostorder;
	
	set<int> seen;
	
	for(int userID = 0; userID < SELF.users.size(); userID++)
		SELF.orderDFS(userID, seen);
	
	return SELF.reversePostorder;
}


void Network::removeCycles()
{
	set<int> seen;
	
	for(int user = 0; user < SELF.users.size(); user++)
	{
		if(seen.contains(user))
			continue;
		
		set<int> inTrace;
		vector<int> trace;
		
		SELF.removeCycles(user, seen, inTrace, trace);
	}
}

void Network::removeCycles(int source, set<int>& seen, set<int>& inTrace, vector<int>& trace)
{
	static const int INF = 1e9;
	
	if(inTrace.contains(source))
	{
		int followee = -1;
		int follower = -1;
		
		int minImpact = INF;
		
		int prev = source;
		int curr = -1;
		
		int index = trace.size() - 1;
		
		while(curr != source)
		{
			curr = trace[index];
			
			double impact = SELF.users[prev].getImpact();
			
			if(impact < minImpact)
			{
				followee = prev;
				follower = curr;
				
				minImpact = impact;
			}
			
			prev = curr;
			index -= 1;
		}
		
		assert((followee != -1) && (follower != -1));
		
		SELF.followers[followee].erase(follower);
		SELF.followees[follower].erase(followee);
		
		SELF.reportConnectionRemoval(followee, follower);
	}
	
	if(seen.contains(source))
		return;
	
	seen.insert(source);
	
	trace.push_back(source);
	inTrace.insert(source);
	
	set<int> followees = SELF.followees[source];
	
	for(int next : followees)
		SELF.removeCycles(next, seen, inTrace, trace);
	
	trace.pop_back();
	inTrace.erase(source);
}

void Network::computeBias(int user, set<int>& processed)
{
	SELF.reportBiasComputationS(user);
	
	if(processed.contains(user))
		return;
	
	processed.insert(user);
	
	double total = 0;
	double exposition[BIASES_COUNT];
	
	for(int i = 0; i < BIASES_COUNT; i++)
		exposition[i] = 0;
	
	for(int followee : SELF.followees[user])
	{
		computeBias(followee, processed);
		
		double impact = SELF.users[followee].getImpact();
		
		for(int i = 0; i < BIASES_COUNT; i++)
			exposition[i] += impact * SELF.users[followee].bias[i];
		
		total += impact;
	}
	
	SELF.reportBiasComputationF(user);
	
	if(total == 0)
		return;
	
	for(int i = 0; i < BIASES_COUNT; i++)
		SELF.users[user].bias[i] = exposition[i] / total;
	
	double sum = 0;
	
	for(int i = 0; i < BIASES_COUNT; i++)
		sum += SELF.users[user].bias[i];
	
	assert(abs(1 - sum) <= 1e-10);
}

void Network::orderDFS(int source, set<int>& seen)
{
	if(seen.contains(source))
		return;
	
	seen.insert(source);
	
	for(int next : SELF.followers[source])
		SELF.orderDFS(next, seen);
	
	SELF.reversePostorder.push(source);
}

void Network::getSCComponent(int source, set<int>& seen, vector<reference<User>>& component)
{
	if(seen.contains(source))
		return;
	
	seen.insert(source);
	component.push_back(SELF.users[source]);
	
	for(int next : SELF.followees[source])
		SELF.getSCComponent(next, seen, component);
}

void Network::reportBiasComputationS(const int user)
{
	if(SELF.logger == NULLPTR)
		return;
	
	std::stringstream message;
	
	message << "=> ";
	message << SELF.users[user].name;
	
	SELF.logger -> print(message.str());
}

void Network::reportBiasComputationF(const int user)
{
	if(SELF.logger == NULLPTR)
		return;
	
	std::stringstream message;
	
	message << "<= ";
	message << SELF.users[user].name;
	
	SELF.logger -> print(message.str());
}

void Network::reportConnectionRemoval(const int followee, const int follower)
{
	if(SELF.logger == NULLPTR)
		return;
	
	std::stringstream message;
	
	message << "Removing connection: ";
	message << SELF.users[follower].name;
	message << " -> ";
	message << SELF.users[followee].name;
	
	SELF.logger -> print(message.str());
}