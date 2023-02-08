#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;


int main(int argc, char* argv) {
	//read info from file
	pair<vector<string>, vector<int>> fileInput = readFile("C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Winter 2023\\Software Engineering\\Sample Input Files(1)\\Sample Input Files\\census2020.csv");
	vector<string> stateNames;
	vector<int> statePops;

	//moving the pair of vectors from file into 2 seperate vectors for easier organization
	auto i = fileInput.first.begin();
	auto j = fileInput.second.begin();
	for (; i != fileInput.first.end(), j != fileInput.second.end(); i++, j++) {
		stateNames.push_back(*i);
		statePops.push_back(*j);
	}
	//
	//calculate the total population (sum of all states)
	int totalPopulation = 0;
	for (auto i = fileInput.second.begin(); i != fileInput.second.end(); i++) {
		totalPopulation += *i;
	}
	//
	//calculate the average population per representative	
	int totalReps = 435; //default reps
	float avgPopPerRep = totalPopulation / totalReps;
	//
	//calculate the 'divide' for each state, and seperating floor and remainder
	float divide;
	vector<pair<string, pair<int, float>>> Floor_Remain;
	int sumReps = 0;
	auto k = stateNames.begin();
	auto l = statePops.begin();
	for (; k != stateNames.end(), l != statePops.end(); k++, l++) {
		int floor;
		float remain;
		divide = *l / avgPopPerRep;
		floor = static_cast<int>(divide);
		remain = divide - floor;
		Floor_Remain.push_back(make_pair(*k, make_pair(floor, remain)));
		sumReps += floor;
	}
	//
	//printing unsorted states for testing
	for (auto i : Floor_Remain) {
		cout << i.first << "," << i.second.first << "," << i.second.second << endl;

	}
	//
	//distrubuting the extra representatives to the states with highest remainder
	int repsLeftOver = totalReps - sumReps;
	cout << endl << endl << endl;
	sort(Floor_Remain.begin(), Floor_Remain.end(), [](auto& left, auto& right)
		{ return left.second.second > right.second.second; });
	for (int i = 0; i < repsLeftOver; i++) {
		Floor_Remain[i].second.first++;
		cout << Floor_Remain[i].first << "," << Floor_Remain[i].second.first << ","
			<< Floor_Remain[i].second.second << endl;
	}
	//
	return 0;
}