#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <OpenXLSX.hpp>

using namespace std;
using namespace OpenXLSX;

vector<pair<string, int>> readFile(string, vector<int>&);
vector<pair<string, int>> readCSVfile(string, vector<int>&);
vector<pair<string, int>> readXLSXfile(string, vector<int>&);
vector<pair<string, pair<int, float>>> hamiltonApportionment(const vector<pair<string, int>> &, const int &);
bool outputDataToFile(const string&, const vector<pair<string, pair<int, float>>>&);
void writeToCSVfile(string, vector<pair<string, pair<int, float>>>);
void writeToXLSXfile(string, vector<pair<string, pair<int, float>>>);
bool fileExists(const std::string& );
int getRep(const int);
string getSheetName(bool);

int main()
{
	string anotherFile = "Y";
	while (anotherFile == "Y" || anotherFile == "y") {
		vector<pair<string, int>> inputData;
		vector<int> errorLines;
		string input;
		string filePath;
		while (true) {
			try
			{
				cout << "Enter the CSV or XLSX file path: ";
				getline(cin, input);
				inputData = readFile(input, errorLines);
				filePath = input.substr(0, input.rfind('\\') + 1);
				break;
			}
			catch (exception& e)
			{
				cout << e.what() << endl;
				cout << "Would you like to try again? Type Y or y to proceed, any other key to terminate: ";
				getline(cin, input);
				if (input != "Y" && input != "y") {
					return 0;
				}
				cout << endl << endl;
			}
		}
		cout << endl << "File has been read." << endl;
		if (errorLines.size() > 0)
		{
			cout << "WARNING: States on line number ";
			for (int i = 0; i < errorLines.size(); i++)
			{
				if (i == errorLines.size() - 1)
					cout << errorLines[i];
				else
					cout << errorLines[i] << ", ";
			}
			cout << " have invalid data in state name and/or state population. These lines will be skipped.\n\n";
		}
		int totalPopulation = 0;
		for (auto itr = inputData.begin(); itr != inputData.end(); itr++) {
			totalPopulation += itr->second;
		}
		cout << "Total population is " << totalPopulation << endl;
		
		int repCount;
		try {
			repCount = getRep(totalPopulation);
		}
		catch (exception& e) {
			cout << e.what() << endl;
		}

		vector<pair<string, pair<int, float>>> outputData;
		try {
			outputData = hamiltonApportionment(inputData, repCount);
		}
		catch (exception& e) {
			cout << e.what() << endl;
			return 0;
		}
		bool dataWrittenSuccessfully = false;
		while(!dataWrittenSuccessfully)
			dataWrittenSuccessfully = outputDataToFile(filePath, outputData);

		cout << endl << "Would you like to run another file? Type Y or y to proceed, any other key to terminate:  ";
		getline(cin, anotherFile);

	}
	cout << "Thank you for using our program." << endl;
	return 0;
}

vector<pair<string, int>> readFile(string filePath, vector<int>& errorLines)
{
	if (filePath.size() > 4 && filePath.substr(filePath.size() - 4) == ".csv")
		return readCSVfile(filePath, errorLines);
	else if (filePath.size() > 5 && filePath.substr(filePath.size() - 5) == ".xlsx")
		return readXLSXfile(filePath, errorLines);
	throw invalid_argument("Invalid file type.\n");
	
}

vector<pair<string, int>> readCSVfile(string filePath, vector<int>& errorLines)
{
	ifstream myFile(filePath);
	if (myFile.fail())
		throw invalid_argument("File doesn't exists at this location.\n");
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
				if (name == "") {
					errorLines.push_back(++lineNumber);
				}
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
				if (stoi(number) <= 0) {
					errorLines.push_back(++lineNumber);
				}
				else {
					toReturn.push_back(make_pair(name, stoi(number)));
					++lineNumber;
					name = "";
					number = "";
				}
			}
		}
	}
	myFile.close();
	if (toReturn.size() == 0)
		throw invalid_argument("Your input file has no valid data.\n");
	return toReturn;
}

vector<pair<string, int>> readXLSXfile(const string filePath, vector<int>& errorLines)
{
	XLDocument document;
	try
	{
		document.open(filePath);
	}
	catch (...)
	{
		throw invalid_argument("File doesn't exists at this location.\n");
	}
	string name;
	int number;
	int lineNumber = 2;
	vector<pair<string, int>> toReturn;
	OpenXLSX::XLWorksheet worksheet;
	bool firstTime = true;
	while (true)
	{
		try
		{
			string fileName = getSheetName(firstTime);
			worksheet = document.workbook().worksheet(fileName);
			break;
		}
		catch (...)
		{
			firstTime = false;
		}
	}
	auto range = worksheet.range(XLCellReference("A2"), XLCellReference(worksheet.rowCount(), 2));

	bool isFirstColumn = true;
	int currentLine = lineNumber;
	for (auto& cell : range)
	{
		if (isFirstColumn)
		{
			isFirstColumn = false;
			name = cell.value().get<string>();
			for (int i = 0; i < name.size(); i++)
				if (!isalpha(name[i]) && name[i] != ' ')
				{
					errorLines.push_back(lineNumber++);
					break;
				}
		}
		else if ((!isFirstColumn) && currentLine == lineNumber)
		{
			isFirstColumn = true;
			if (cell.value().typeAsString() != "integer")
			{
				errorLines.push_back(lineNumber++);
				continue;
			}
			number = cell.value().get<int>();
			if (number < 1)
			{
				errorLines.push_back(lineNumber++);
				continue;
			}
			toReturn.push_back(make_pair(name, number));
			lineNumber++;
			name = "";
			number = 0;
		}
		else
			isFirstColumn = true;
		if (isFirstColumn)
			currentLine++;
	}

	if (toReturn.size() == 0)
		throw invalid_argument("Your input file has no valid data.\n");
	return toReturn;
}

vector<pair<string, pair<int, float>>> hamiltonApportionment(const vector<pair<string, int>> &data,const int &repCount) {
	//calculate the total population (sum of all states)
	int totalPopulation = 0;
	for (auto i = data.begin(); i != data.end(); i++) {
		totalPopulation += i->second;
	}
	
	if (totalPopulation < repCount || totalPopulation == 0) {
		throw logic_error("Total population can not be less than Representatives or 0.");
	}
	//calculate the average population per representative	
	float avgPopPerRep = totalPopulation / repCount;

	//calculate the 'divide' for each state, and seperating floor and remainder
	vector<pair<string, pair<int, float>>> Floor_Remain;
	int sumReps = 0;
	for (auto k = data.begin(); k != data.end(); k++) {
		float divide = k->second / avgPopPerRep;
		int floor = static_cast<int>(divide);
		float remain = divide - floor;
		Floor_Remain.push_back(make_pair(k->first, make_pair(floor, remain)));
		sumReps += floor;
	}
	//
	//distrubuting the extra representatives to the states with highest remainder
	int repsLeftOver = repCount - sumReps;
	if (repsLeftOver >= 1) {
		//sorting the vector by remainder
		sort(Floor_Remain.begin(), Floor_Remain.end(), [](auto& left, auto& right)
			{ return left.second.second > right.second.second; });
		//adding 1 rep to all states up to repsLeftover
		for (int i = 0; i < repsLeftOver; i++) {
			Floor_Remain[i].second.first++;
		}
	}
	
	//
	//sorting states alphabetically
	sort(Floor_Remain.begin(), Floor_Remain.end(), [](auto& left, auto& right)
		{ return left.first < right.first; });
	return Floor_Remain;
}

bool outputDataToFile(const string & filePath,const vector<pair<string, pair<int, float>>> & outputData) {
	string fileName;
	string input;
	try
	{
		cout << "Do you want to write to new file(enter N) or existing file(enter E)?" << endl;
		getline(cin, input);
		if (input == "E" || input == "e") {
			cout << "Enter output file complete path: ";
			getline(cin, input);
			if (!fileExists(input))
				cout << "Specififed File at the specified path does not exist. File will be created." << endl;
			fileName = input;
		}
		else {
			cout << endl << "New file is being created." << endl;
			cout << "Enter output file name without extension: ";
			getline(cin, input);
			int index = input.rfind('\\');
			if (index != -1) {
				input = input.substr(index);
			}
			input += ".csv";
			fileName = filePath + input;
			if (fileExists(fileName)) {
				cout << "There is an exisiting File with the same name. File name will be changed." << endl;
				fileName += "_outPutReps_1";
			}
		}
		writeToCSVfile(fileName, outputData);
		cout << endl << "Data written to file : " << fileName << endl;
		cout << endl << endl << "END OF PROGRAM" << endl;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		cout << "Data was not written Successfully. Please try again." << endl;
		return false;
	}
	return true;
}

// Writes the data in file 
void writeToCSVfile(string filePath, vector<pair<string, pair<int, float>>> data) {
	if (filePath.size() < 5 || filePath.substr(filePath.size() - 4) != ".csv")	//only write to csv files
		throw runtime_error("Invalid file type.\n");
	ofstream fileOutput;
	try
	{
		fileOutput.exceptions(ofstream::failbit | ofstream::badbit);
		fileOutput.open(filePath);
		fileOutput.exceptions(std::ofstream::goodbit);
		fileOutput << "State, Representatives\n";
		for (auto i = data.begin(); i != data.end(); i++)
			fileOutput << i->first << ',' << i->second.first << '\n';
	}
	catch (ofstream::failure const& ex)
	{
		throw runtime_error("File can not be open or connected to stream. Please try another valid file.");
	}

	fileOutput.close();
}

void writeToXLSXfile(string filePath, vector<pair<string, pair<int, float>>> data)
{
	XLDocument document;
	document.create(filePath);
	auto worksheet = document.workbook().worksheet("Sheet1");
	worksheet.cell("A1").value() = "State";
	worksheet.cell("B1").value() = "Representatives";
	for (int i = 2; i - 2 < data.size(); i++)
	{
		worksheet.cell("A" + to_string(i)).value() = data[i - 2].first;
		worksheet.cell("B" + to_string(i)).value() = data[i - 2].second.first;
	}
	document.save();
	document.close();
}

inline bool fileExists(const std::string& fileName) {
	struct stat buffer;
	if (stat(fileName.c_str(), &buffer) == 0)
		return true;
	else
		return false;
}

int getRep(const int totalPopulation) {
	string input;
	int repCount = 0;
	while (repCount == 0)
	{
		cout << "Enter number of representatives (default is 435): ";
		getline(cin, input);
		if (input == "")
			repCount = 435;
		else if (find_if(input.begin(), input.end(), [](char c) {return !isdigit(c); }) != input.end())
			cout << "Invalid number of representatives.\n";
		else
			repCount = stoi(input) >= 1 ? stoi(input) : 0;
		if (totalPopulation < repCount) {
			cout << "Total population can not be less than the number of representatives.\n";
			repCount = 0;
		}
	}

	cout << "Representative count is set to " << repCount << endl << endl;
	return repCount;
}

string getSheetName(bool firstTime = true)
{
	if (firstTime == false)
		cout << "Could not find sheet name.\n";
	cout << "Enter sheet name: ";
	string toReturn;
	getline(cin, toReturn);
	return toReturn;
}