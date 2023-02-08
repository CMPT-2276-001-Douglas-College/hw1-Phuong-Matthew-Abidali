#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <stdexcept>

using namespace std;

pair<vector<string>, vector<int>> readFile(string fileName)
{
	if (fileName.substr(fileName.size() - 4) != ".csv")
		throw runtime_error("Invalid file type.\n");
	ifstream myFile(fileName);
	string name;
	int number;
	getline(myFile, name, '\n');
	pair<vector<string>, vector<int>> toReturn;
	while (getline(myFile, name, ','))
	{
		myFile >> number;
		toReturn.first.push_back(name);
		toReturn.second.push_back(number);
		myFile.ignore(1, '\n');
	}
	return toReturn;
}

int main()
{
	cout << "File to read: ";
	string inputFileName;
	getline(cin, inputFileName);
	pair<vector<string>, vector<int>> inputData;
	try
	{
		inputData = readFile(inputFileName);
	}
	catch (exception& e)
	{
		cout << e.what();
		return 0;
	}
	//for (int i = 0; i < inputData.first.size(); i++)
	//	cout << inputData.first[i] << ' ' << inputData.second[i] << '\n';
}