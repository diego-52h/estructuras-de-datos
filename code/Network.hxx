#ifndef NETWORK_H
#define NETWORK_H

#include "Aliases.hxx"
#include "User.hxx"

class Logger;

class Network
{
	public:
		Network(pointer<Logger> logger = NULLPTR);
		Network(const vector<User>& users, pointer<Logger> logger = NULLPTR);
		
		void insertUser(const User& user);
		void connectUsers(const string& followee, const string& follower);
		
		void computeUsersBias(const vector<string>& presetUsers);
		
		const vector<User>& getUsers();
		
		vector<vector<reference<User>>> getSCComponents();
	
	private:
		vector<User> users;
		
		vector<set<int>> followers;
		vector<set<int>> followees;
		
		unordered_map<string, int> nameToID;
		
		stack<int> reversePostorder;
		
		pointer<Logger> logger;
		
		stack<int> getReversePostorder();
		
		void computeBias(int user);
		void computeBiasDFS(int user, set<int>& processed);
		
		void removeCycles();
		void removeCycles(int source, set<int>& seen, set<int>& inTrace, vector<int>& trace);
		
		void orderDFS(int source, set<int>& seen);
		void getSCComponent(int source, set<int>& seen, vector<reference<User>>& component);
		
		void reportBiasComputationS(const int user);
		void reportBiasComputationF(const int user);
		void reportConnectionRemoval(const int followee, const int follower);
};

#endif