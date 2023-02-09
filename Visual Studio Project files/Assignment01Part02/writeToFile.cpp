#include <fstream>
#include <string>
#include <vector>

using namespace std;

void writeToFile(string fileName, vector<pair<string, pair<int, float>>> data) {
	if (fileName.substr(fileName.size() - 4) != ".csv")	//only write to csv files
		throw runtime_error("Invalid file type.\n");
	ofstream fileOutput(fileName.c_str());
	fileOutput << "State, #Representatives\n";
	for (auto i = data.begin(); i != data.end(); i++) {
		fileOutput << i->first << ", " << i->second.first << endl;
	}
	fileOutput << "test";
	fileOutput.close();
}