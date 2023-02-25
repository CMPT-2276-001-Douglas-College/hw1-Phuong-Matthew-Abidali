#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <OpenXLSX.hpp>

using namespace std;
using namespace OpenXLSX;


vector<pair<string, int>> readFile(string, vector<int>&);
vector<pair<string, int>> readCSVfile(string, vector<int>&);
vector<pair<string, int>> readXLSXfile(string, vector<int>&);
vector<pair<string, pair<int, float>>> hamiltonApportionment(const vector<pair<string, int>> &, const int &);
vector<pair<string, pair<int, float>>> huntingtonHillApportionment(const vector<pair<string, int>>& data, const int& repCount);
bool outputDataToFile(const string&, const vector<pair<string, pair<int, float>>>&);
void writeToCSVfile(string, vector<pair<string, pair<int, float>>>);
void writeToXLSXfile(string, vector<pair<string, pair<int, float>>>);
bool fileExists(const std::string& );
int getRep(const int);
string getSheetName(bool);





int main()
{
	string anotherFile = "Y";
	//loop through whole program 
	while (anotherFile == "Y" || anotherFile == "y") {
		
		vector<pair<string, int>> inputData; //Data read from file
		vector<int> errorLines; //lines with error
		string input; //input from keyboard
		string filePath; //path for input file stored for writing to same folder
		vector<pair<string, pair<int, float>>> outputData; // outputData sorted and ready to write to file

		//Take file Name input and read Data
		std::cout << "_____INPUT DATA __________________________________________________________________" << endl << endl;
		try
		{
			std::cout << "Enter the CSV or XLSX file path: ";
			getline(cin, input);
			inputData = readFile(input, errorLines);
			filePath = input.substr(0, input.rfind('\\') + 1);
		}
		catch (exception& e)
		{
			std::cout << e.what() << endl;
			std::cout << endl << "Would you like to run another file? Type Y or y to proceed, any other key to terminate:  ";
			getline(cin, anotherFile);
			continue;
		}
		std::cout << endl << "File has been read!" << endl;
		
		//print out error warnings and skipped line
		if (errorLines.size() > 0)
		{
			std::cout << "WARNING: States on line number ";
			for (int i = 0; i < errorLines.size(); i++)
			{
				if (i == errorLines.size() - 1)
					std::cout << errorLines[i];
				else
					std::cout << errorLines[i] << ", ";
			}
			std::cout << " have invalid data in state name and/or state population. These lines will be skipped.\n\n";
		}

		//calculating total population 
		int totalPopulation = 0;
		for (auto itr = inputData.begin(); itr != inputData.end(); itr++) {
			totalPopulation += itr->second;
		}
		
		int repCount;
		try {
			repCount = getRep(totalPopulation);
		}
		catch (exception& e) {
			std::cout << e.what() << endl;
		}
		
		std::cout << "__________________________________________________________________________________" << endl << endl;

		std::cout << "_____METHOD__________________________________________________________________" << endl << endl;

		std::cout << endl << "To use Hamilton method enter 'H' to use Huntington-Hill method enter 'HH' :";
		getline(cin, input);
		std::transform(input.begin(),input.end(),input.begin(), [](unsigned char c) { return std::tolower(c); });
		if (input == "h") {
			try {
				cout << endl << "Using Hamilton Algorithm!" << endl << endl;
				outputData = hamiltonApportionment(inputData, repCount);
			}
			catch (exception& e) {
				std::cout << e.what() << endl;
				std::cout << endl << "Something went wrong in Hamilton Method. Aborting this file." << endl;
				std::cout << endl << "Would you like to run another file? Type Y or y to proceed, any other key to terminate:  ";
				getline(cin, anotherFile);
				continue;
			}
		}
		else {
			if (input != "hh")
				cout << "Unexpected input! HuntingtonHill Algorithm will be used." << endl;
			try {
				cout << endl << "Using HuntingtonHill Algorithm!" << endl << endl;
				outputData = huntingtonHillApportionment(inputData, repCount);
			}
			catch (exception& e) {
				std::cout << e.what() << endl;
				std::cout << endl << "Something went wrong in Hamilton Method. Aborting this file." << endl;
				std::cout << endl << "Would you like to run another file? Type Y or y to proceed, any other key to terminate:  ";
				getline(cin, anotherFile);
				continue;
			}
		}
		
		std::cout << "__________________________________________________________________________________" << endl << endl;

		std::cout << "_____WRITING TO FILE______________________________________________________________" << endl << endl;

		bool dataWrittenSuccessfully = false;
		while(!dataWrittenSuccessfully)
			dataWrittenSuccessfully = outputDataToFile(filePath, outputData);

		std::cout << endl << "Would you like to run another file? Type Y or y to proceed, any other key to terminate:  ";
		getline(cin, anotherFile);
		std::cout << endl << endl;
		for (int i = 0; i < 25; i++)
			cout << "=";
		std::cout << endl << endl;
	}
	
	std::cout << endl << "Thank you for using our program!" << endl << endl;
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

/// <summary>
/// Calculates number of representative for each input state using repcount 
/// </summary>
/// <param name="data"> input data containing valid state names and population</param>
/// <param name="repCount">Total representative to be distributed </param>
/// <returns>
/// A vector containing State name, Representative Number and additional floating point remainders from calculation
/// </returns>
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



/// <summary>
/// HuntingtonHill algorithm distributes representative number from repCount for each state in data depending on population in data
/// </summary>
/// <param name="data">a vector pair contating STATE NAME and POPULATION</param>
/// <param name="repCount">NUmber of representatives</param>
/// <returns> Alphabetically ordered State name, Representative NUmber and additional remainder used during calculation</returns>
vector<pair<string, pair<int, float>>> huntingtonHillApportionment(const vector<pair<string, int>>& data, const int& repCount) {
	double squareRoot2 = sqrt(2);
	vector<pair<string, pair<int, float>>> nameRepsPriorities;
	//initially give each state 1 rep, and calculate the priorty based on that
	float priority;
	for (int i = 0; i < data.size(); i++) {
		//formula is simplified since 'n' in the given formula is 1
		priority = data[i].second / squareRoot2;
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

/// <summary>
/// Taking information on output file type, name and path and writing to file
/// Takes information from keyboard through prompts
/// </summary>
/// <param name="filePath"> The filepath is path to directory of input file</param>
/// <param name="outputData"> Data to be written to file</param>
/// <returns>
/// True if succeds or false
/// </returns>
bool outputDataToFile(const string & filePath,const vector<pair<string, pair<int, float>>> & outputData) {
	string fileName;
	string input;
	bool fileType = false;

	try
	{
		std::cout << "Do you want to write to new file(enter N) or existing file(enter E)? " ;
		getline(cin, input);
		if (input == "E" || input == "e") {
			std::cout << "Enter output file complete path: ";
			getline(cin, input);
			
			if (!fileExists(input)) {
				std::cout << "Specififed File at the specified path does not exist. File will be created." << endl;	
			}

			if (input.ends_with(".csv"))
				fileType = false;
			else if (input.ends_with(".xlsx")) {
				fileType = true;
			}
			else {
				std::cout << std::endl << "Extension is unrecognizable. So csv file will be created." << endl;
			}
			int index = input.rfind('.');
			if (index != -1) {
				input = input.substr(0, index);
				input = input + ".csv";
			}	
			fileName = input;
		}
		else {
			std::cout << endl << "New file is being created." << endl;
			std::cout << "To create a csv file press C and for xlsx press X:";
			getline(cin, input);
			if (input == "C" || input == "c")
				fileType = false;
			else if (input == "X" || input == "x")
				fileType = true;
			else {
				std::cout << std::endl << "button other than C or X was entered. So csv file will be created."<<endl; 
			}

			std::cout << "Enter output file name without extension: ";
			getline(cin, input);
			int index = input.rfind('\\');
			if (index != -1) {
				input = input.substr(index);
			}
			string type = fileType == 0 ? ".csv" : ".xlsx";
			fileName = filePath + input + type;
			if (fileExists(fileName)) {
				std::cout << "There is an exisiting File with the same name. File name will be changed." << endl;
				fileName += "_outPutReps_1";
			}
		}

		if (fileType == false)
			writeToCSVfile(fileName, outputData);
		else
			writeToXLSXfile(fileName, outputData);

		std::cout << endl << "Data written to file : " << fileName << endl;
		std::cout << endl << endl << "END OF PROGRAM" << endl;
		
	}
	catch (exception& e)
	{
		std::cout << e.what() << endl;
		std::cout << "Data was not written Successfully. Please try again." << endl;
		return false;
	}
	return true;
}


/// <summary>
/// Writes data to file in filepath of type csv
/// </summary>
/// <param name="filePath">a csv file path</param>
/// <param name="data">data to write</param>
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


/// <summary>
/// Writes to Xlsx file
/// </summary>
/// <param name="filePath">filepath is xlsx file </param>
/// <param name="data">data to write</param>
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

/// <summary>
/// Checks if the file exists
/// </summary>
/// <param name="fileName">path of file including name and type</param>
/// <returns></returns>
inline bool fileExists(const std::string& fileName) {
	struct stat buffer;
	if (stat(fileName.c_str(), &buffer) == 0)
		return true;
	else
		return false;
}


/// <summary>
/// Get representative number input from keyboard
/// Loop continues unless valid data is entered or skipped by pressing enter key
/// Representative number can not be more than total population and can not be zero
/// </summary>
/// <param name="totalPopulation"></param>
/// <returns>
/// NUmber of representatives
/// </returns>
int getRep(const int totalPopulation) {
	string input;
	int repCount = 0;
	while (repCount == 0)
	{
		std::cout << "Enter number of representatives (default is 435): ";
		getline(cin, input);
		if (input == "")
			repCount = 435;
		else if (find_if(input.begin(), input.end(), [](char c) {return !isdigit(c); }) != input.end())
			std::cout << "Invalid number of representatives.\n";
		else
			repCount = stoi(input) >= 1 ? stoi(input) : 0;
		if (totalPopulation < repCount) {
			std::cout << "Total population can not be less than the number of representatives.\n";
			repCount = 0;
		}
	}

	std::cout << "Representative count is set to " << repCount << endl << endl;
	return repCount;
}


/// <summary>
/// Gets sheet name for xlsx file
/// </summary>
/// <param name="firstTime"></param>
/// <returns></returns>
string getSheetName(bool firstTime = true)
{
	if (firstTime == false)
		std::cout << "Could not find sheet name.\n";
	std::cout << "Enter sheet name: ";
	string toReturn;
	getline(cin, toReturn);
	return toReturn;
}