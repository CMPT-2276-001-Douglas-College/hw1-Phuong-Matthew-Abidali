#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

vector<pair<string, int>> readFile(string, vector<int>&);
vector<pair<string, pair<int, float>>> hamiltonApportionment(vector<pair<string, int>>, int);
void writeToFile(string, vector<pair<string, pair<int, float>>>);

int main()
{
	vector<pair<string, int>> inputData;
	vector<int> errorLines;
	string input;
	try
	{
		cout << "File path: ";
		getline(cin, input);
		inputData = readFile(input, errorLines);
	}
	catch (exception& e)
	{
		cout << e.what();
		return 0;
	}
	int repCount = 0;
	while (repCount == 0)
	{
		cout << "Enter number of representatives (default is 435): ";
		getline(cin, input);
		if (input == "")
			repCount = 435;
		else if (stoi(input) >= 1)
			repCount = stoi(input);
		else
			cout << "Invalid number of representatives.\n";
	}

	vector<pair<string, pair<int, float>>> outputData = hamiltonApportionment(inputData, repCount);
	
	cout << "There are problems in line number ";
	for (int i = 0; i < errorLines.size(); i++)
	{
		if (i == errorLines.size() - 1)
			cout << errorLines[i];
		else
			cout << errorLines[i] << ", ";
	}
	cout << ". Would you like to proceed (type Y to proceed, any other key to terminate)? ";
	getline(cin, input);
	if (input == "Y")
	{
		while (true)
		{
			try
			{
				cout << "Enter a new or existing file path: ";
				getline(cin, input);
				writeToFile(input, outputData);
				return 0;
			}
			catch (exception& e)
			{
				cout << e.what();
			}
		}
	}
}

vector<pair<string, int>> readFile(string filePath, vector<int>& errorLines)
{
	if (filePath.size() < 5 || filePath.substr(filePath.size() - 4) != ".csv")
		throw invalid_argument("Invalid file type.\n");
	ifstream myFile(filePath);
	if (myFile.fail())
		throw invalid_argument("File doesn't exist at this location.\n");
	string name;
	string number;
	string line;
	int lineNumber = 1;
	getline(myFile, line);
	vector<pair<string, int>> toReturn;

	while (getline(myFile, line))
	{
		int i = 0;
		int errorCount = errorLines.size();
		for (; i < line.size(); i++)
		{
			if (isalpha(line[i]) || line[i] == ' ')
				name += line[i];
			else if (line[i] == ',')
			{
				i++;
				if (name == "")
					errorLines.push_back(++lineNumber);
				break;
			}
			else
			{
				errorLines.push_back(++lineNumber);
				name = "";
				break;
			}
		}
		if (errorLines.size() == errorCount)
		{
			for (; i <= line.size(); i++)
			{
				if (isdigit(line[i]))
					number += line[i];
				else if (line[i] == ',' || line[i] == '\0')
				{
					if (number == "")
					{
						errorLines.push_back(++lineNumber);
						name = "";
					}
					break;
				}
				else
				{
					errorLines.push_back(++lineNumber);
					name = "";
					number = "";
					break;
				}
			}
			if (errorLines.size() == errorCount)
			{
				toReturn.push_back(make_pair(name, stoi(number)));
				name = "";
				number = "";
			}
		}
	}
	myFile.close();
	if (toReturn.size() == 0)
		throw invalid_argument("File has no data.\n");
	return toReturn;
}

vector<pair<string, pair<int, float>>> hamiltonApportionment(vector<pair<string, int>> data, int repCount) {
	//calculate the total population (sum of all states)
	int totalPopulation = 0;
	for (auto i = data.begin(); i != data.end(); i++) {
		totalPopulation += i->second;
	}
	//
	//calculate the average population per representative	
	float avgPopPerRep = totalPopulation / repCount;
	//
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
	//
	//distrubuting the extra representatives to the states with highest remainder
	int repsLeftOver = repCount - sumReps;
	//sorting the vector by remainder
	sort(Floor_Remain.begin(), Floor_Remain.end(), [](auto& left, auto& right)
		{ return left.second.second > right.second.second; });
	//adding 1 rep to all states up to repsLeftover
	for (int i = 0; i < repsLeftOver; i++) {
		Floor_Remain[i].second.first++;
	}
	//
	//sorting states alphabetically
	sort(Floor_Remain.begin(), Floor_Remain.end(), [](auto& left, auto& right)
		{ return left.first < right.first; });
	return Floor_Remain;
}

void writeToFile(string filePath, vector<pair<string, pair<int, float>>> data) {
	if (filePath.size() < 5 || filePath.substr(filePath.size() - 4) != ".csv")	//only write to csv files
		throw runtime_error("Invalid file type.\n");
	ofstream fileOutput(filePath);
	if (fileOutput.fail())
		throw runtime_error("Invalid file path.\n");
	fileOutput << "State, Representatives\n";
	for (auto i = data.begin(); i != data.end(); i++)
		fileOutput << i->first << ',' << i->second.first << '\n';
	fileOutput.close();
}