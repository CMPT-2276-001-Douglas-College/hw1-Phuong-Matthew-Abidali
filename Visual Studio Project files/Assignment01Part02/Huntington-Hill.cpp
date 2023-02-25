#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

//Huntington-Hill algorithm
vector<pair<string, pair<int, float>>> huntingtonHillApportionment(const vector<pair<string, int>>& data, const int& repCount) {
	vector<pair<string, pair<int, float>>> nameRepsPriorities;
	//initially give each state 1 rep, and calculate the priorty based on that
	float priority;
	for (int i = 0; i < data.size(); i++) {
		//formula is simplified since 'n' in the given formula is 1
		priority = data[i].second / sqrt(2);
		nameRepsPriorities.push_back(make_pair(data[i].first, make_pair(1, priority)));
	}
	//distribute representatives based on priority
	int repsLeft = repCount - nameRepsPriorities.size();
	while (repsLeft > 0) {
		//find the state with the highest priority
		float max = nameRepsPriorities[0].second.second;	//first element
		int maxIndex = 0;	//keep track of the index where highest priority is found
		for (int i = 0; i < nameRepsPriorities.size(); i++) {
			if (nameRepsPriorities[i].second.second > max) {
				max = nameRepsPriorities[i].second.second;
				maxIndex = i;
			}
		}
		//assign an extra rep to the state with highest priority
		nameRepsPriorities[maxIndex].second.first++;
		//, then recalcualte its priority
		nameRepsPriorities[maxIndex].second.second = data[maxIndex].second /
			sqrt(nameRepsPriorities[maxIndex].second.first *
				(nameRepsPriorities[maxIndex].second.first + 1));
		repsLeft--;
	}
	//sorting states alphabetically
	sort(nameRepsPriorities.begin(), nameRepsPriorities.end(), [](auto& left, auto& right)
		{ return left.first < right.first; });
	return nameRepsPriorities;
}