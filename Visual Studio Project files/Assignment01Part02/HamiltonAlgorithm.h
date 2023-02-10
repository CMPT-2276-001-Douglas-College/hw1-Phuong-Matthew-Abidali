
#ifndef HAMILTONALGORITHM_H
#define HAMILTONALGORITHM_H


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void hamiltonApportionment(vector<pair<string, int>> data, int repCount) {
	//calculate the total population (sum of all states)
	int totalPopulation = 0;
	for (auto i = data.begin(); i != data.end(); i++) {
		totalPopulation += i->second;
	}

	//calculate the average population per representative	
	float avgPopPerRep = totalPopulation / repCount;

	//calculate the 'divide' for each state, and seperating floor and remainder
	float divide;
	vector<pair<string, pair<int, float>>> Floor_Remain;
	int sumReps = 0;
	for (auto k = data.begin(); k != data.end(); k++) {
		int floor;
		float remain;
		divide = k->second / avgPopPerRep;
		floor = static_cast<int>(divide);
		remain = divide - floor;
		Floor_Remain.push_back(make_pair(k->first, make_pair(floor, remain)));
		sumReps += floor;
	}

	//remianing representatives
	int repsLeftOver = repCount - sumReps;

	//sorting by highest remiander value
	sort(Floor_Remain.begin(), Floor_Remain.end(), [](auto& left, auto& right)
	{ 
		return left.second.second > right.second.second; 
	});
		
	//adding 1 rep to all states up to repsLeftover
	for (int i = 0; i < repsLeftOver; i++) {
		Floor_Remain[i].second.first++;
	}
	
	//sorting states alphabetically
	sort(Floor_Remain.begin(), Floor_Remain.end(), [](auto& left, auto& right)
	{ 
		return left.first < right.first; 
	});
}
#endif