#pragma once
#include <iostream>
#include <filesystem>
#include <istream>
#include <fstream> 
#include <sstream> 
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdio.h>
#include <iomanip> 

using namespace std;

using std::ifstream;
using std::ios;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::ofstream;


double get1YearEps(string stockName, string year);
bool isValidYear(string year);
int linesInFile(string stockName, int& firstYear);
int currYear();

