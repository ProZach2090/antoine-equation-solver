#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <iomanip>

using namespace std;

void findConstants(string);
void output(char, string, int);
void addEntry();

string element;

int main()
{
	double min, max, a, b, c;
	char input;

	while (true)
	{
		// Get input from user
		cout << "Please enter the element name of the vapor pressure. Or enter 'terminate' to exit the program\nInput: ";
		getline(cin, element);
		for (int i = 0; i < element.length(); i++) element[i] = tolower(element[i]);
		// Terminate the program when the user types in "terminate"
		if (element.compare("terminate") == 0) { break; }
		findConstants(element);
		cin.ignore();// Ignoring any extra input
	}
	system("pause");
    return 0;
}

// Get the a, b, and c constants
void findConstants(string name)
{
	ifstream dataAEC;
	char entrySTR[1000], *value, *entry, input;
	int iterations = 1, matches = 0;
	double min, max, a, b, c;
	string matchentries;

	dataAEC.open("Constants.txt");
	if (dataAEC.fail())// If file doesn't exist, make a new one
	{
		ofstream newDataAEC;
		newDataAEC.open("Constants.txt");
		newDataAEC.close();
		cout << "Constants.txt was not found in directory. Creating a new one... Press any key to continue." << endl;
	}
	else
	{
		// finding all possible entries related to user input
		while (dataAEC >> entrySTR)
		{
			value = strtok(entrySTR, ",");
			entry = value;
			if (!name.compare(entry)) 
			{
				matches++;
				while (value != NULL)
				{
					switch (iterations)
					{						
						case 2: min = atof(value);
						case 3: max = atof(value);
						case 4: a = atof(value);
						case 5: b = atof(value);
						case 6: c = atof(value);
					}
					value = strtok(NULL, ",");
					iterations++;
				}
				iterations = 1;
				matchentries += to_string(min) + "," + to_string(max) + "," + to_string(a) + "," + to_string(b) + "," + to_string(c) + "\n";
			}
		}
		// If there was at least one entry found
		if (matches > 0)
		{
			cout << "'" + element + "' entry found.\nSelect the option of the unknown variable: 1. Temperature 2. Vapor Pressure\nInput: ";
			while (!(cin >> input) || (input != '1' && input != '2')) { cout << "Invalid option. Please try again.\nInput: "; }
			output(input, matchentries, matches);
		}
		// If not entries are found
		else
		{
			cout << "'" + element + "' entry could not be found. Add this entry to the database? (Y or N): ";
			while (!(cin >> input) || (input != 'Y' && input != 'y' && input != 'N' && input != 'n')) { cout << "Invalid option. Please try again.\nInput: "; }
			input = toupper(input);
			if (input == 'Y') addEntry();
		}
	}
	dataAEC.close();
}

// Calculating the requested output
void output(char option, string matches, int numOfMatches)
{
	int precision;
	double **entries, min, max, a, b, c;
	int const COLUMNS = 5;
	char *line = "", *value;
	char *matchesCSTR = new char[matches.length() + 1];
	strcpy(matchesCSTR, matches.c_str());

	// Hand-picking all values from the "matches" string and organizing them into an array
	entries = new double*[numOfMatches];
	for (int row = 0; row < numOfMatches; row++) entries[row] = new double[COLUMNS];
	value = strtok(matchesCSTR, ",");
	int rows = 0, cols = 0;
	while (value != NULL)
	{
		if (cols >= COLUMNS) { rows++; cols = 0; }
		entries[rows][cols] = atof(value);
		cols++;
		value = strtok(NULL, ",\n");
	}
	delete[] matchesCSTR;

	// Asking for precision
	cout << "Number of decimal places.\nInput: ";
	while (!(cin >> precision) || precision < 0)
	{
		cin.clear();
		cin.ignore();
		cout << "Invalid option. Please try again.\nInput: ";
	}
	switch (option)
	{
		double temperature, pressure;
		case '1':// If determining temperature
			cout << "Please enter the vapor pressure of " + element + " (in mm Hg): ";
			while (!(cin >> pressure) || pressure < 0)// Asking for vapour pressure
			{
				cin.clear();
				cin.ignore();
				cout << "Invalid option. Please try again.\nInput: ";
			}
			for (int i = 0; i < numOfMatches; i++)// Finding most appropriate entry
			{
				a = entries[i][2];
				b = entries[i][3];
				c = entries[i][4];
				min = entries[i][0];
				max = entries[i][1];
				int tempTemperature = -(b / (log10(pressure) - a)) - c;
				if (tempTemperature >= min && tempTemperature <= max) { temperature = tempTemperature; break; }// Output
				else max = INT_MIN;
			}
			if (max <= min) cout << "ERROR. Calculated temperature is not applicable to any stored temperature ranges, please check if the given pressure is correct." << endl;// Error check
			else  cout << "The temperature of " + element + " at " << pressure << " mm Hg is " << setprecision(precision) << fixed << temperature << char(167) << "C." << endl;
			break;
		case '2':// If finding vapour pressure
			cout << "Please enter the temperature of " + element + " (in " << char(167) << "C)" << endl;
			cout << "The following list is the acceptable ranges of " + element + ":" << endl;
			for (int i = 0; i < numOfMatches; i++) cout << entries[i][0] << char(167) << "C to " << entries[i][1] << char(167) << "C" << endl;
			cout << "Input: ";
			while (!(cin >> temperature))// Asking for temperature
			{
				cin.clear();
				cin.ignore();
				cout << "Invalid option. Please try again.\nInput: ";
			}
			for (int i = 0; i < numOfMatches; i++)// Finding most appropriate entry 
			{
				min = entries[i][0]; 
				max = entries[i][1];
				if (temperature >= min && temperature <= max)
				{
					a = entries[i][2];
					b = entries[i][3];
					c = entries[i][4];
					break;
				}
				else max = INT_MIN;
			}
			cout << a << " " << b << " " << c << endl;
			if (max <= min)// Error checking
			{
				char input;
				cout << "Input does not fall between range. Would you like to add another entry under " << element << " that will have a range suited for your input? (Y or N): ";
				while (!(cin >> input) || (input != 'Y' && input != 'y' && input != 'N' && input != 'n')) { cout << "Invalid option. Please try again.\nInput: "; }
				input = toupper(input);
				if (input == 'Y') addEntry();
			}
			else// Output
			{
				pressure = pow(10, a - (b / (temperature + c)));
				cout << "The vapor pressure of " + element + " at " << temperature << char(167) << "C is " << setprecision(precision) << fixed << pressure << " mm Hg." << endl;
			}
	}
}

// Adding new entries to the text file
void addEntry() 
{
	ifstream dataAEC;
	ofstream dataAEC_UPDATED;
	double min, max, a, b, c;
	string entry;
	string dataStr = "";

	// Taking existing info from old text file
	dataAEC.open("Constants.txt");
	while (dataAEC >> entry) { dataStr += entry; }
	dataAEC.close();

	// Asking information about new entity
	dataStr += "\n" + element + ",";
	cout << "Please enter the temperature range for the element (enter the minimum and maximum in the respected order): ";
	while (!(cin >> min >> max) || min > max)
	{
		cin.clear();
		cin.ignore();
		cout << "Invalid option. Please try again. (Make sure minimum isn't bigger than maximum)\nInput: ";
	}
	cout << "\nPlease enter the constants of the element (enter a, b, and c in the the respected order): ";
	while (!(cin >> a >> b >> c) || a <= 0 || b <= 0 || c <= 0)
	{
		cin.clear();
		cin.ignore();
		cout << "Invalid option. Please try again. (Make sure all constants are bigger than 0)\nInput: ";
	}
	// Overwriting the file with new information
	dataStr += to_string(min) + "," + to_string(max) + "," + to_string(a) + "," + to_string(b) + "," + to_string(c);
	dataAEC_UPDATED.open("Constants.txt");
	dataAEC_UPDATED << dataStr;
	dataAEC_UPDATED.close();
	cout << "\n'" + element + "' has been added to the database.\n";
}