#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <stdexcept>
#include "HamiltonAlgorithm.h"
#include "writeToFile.h"

using namespace std;

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

int main()
{
	cout << "File to read: ";
	string inputFilePath;
	getline(cin, inputFilePath);
	vector<pair<string, int>> inputData;
	vector<int> errorLines;
	try
	{
		inputData = readFile(inputFilePath, errorLines);
	}
	catch (exception& e)
	{
		cout << e.what();
		return 0;
	}
	int repCount = 0;
	while (repCount == 0)
	{
		cout << "Enter number of representatives (any input other than a number defaults to 435): ";
		cin >> repCount;
		if (cin.fail())
		{
			repCount = 435;
			break;
		}
		if (repCount < 1)
		{
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Invalid number of representatives.\n";
		}
	}

	hamiltonApportionment(inputData, repCount);

}