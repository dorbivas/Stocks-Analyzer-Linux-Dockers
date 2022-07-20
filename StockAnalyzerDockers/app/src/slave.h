#pragma once
#include <iostream>
#include <filesystem>
#include <istream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/syscall.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <zip.h>
#include <sys/resource.h>
#include <limits.h>
#include <string>
#include <vector>
#include <set> 

#include "utils.h"
#include "csvExport.h"

using namespace std;
using std::ifstream;
using std::ios;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::ofstream;

typedef struct stockDataNode {
	double open = 0;
	double high = 0;
	double low = 0;
	double close = 0;
	double volume = 0;
	string name;
	double eps = 0;
} stockDataNode;


void errorMsg(string msg, bool& flag);
void fetchStock(string stockName);
string printStockData(string stockName, string year);
string sumOneYearData(string year, stockDataNode& stockData);

string option_1or3(int option, bool& option1_done_flag, string stock_name);
void readLine(string line, stockDataNode& stockData);
double getOneParam(string delimiter, string& line, size_t& pos);
string printOneMonth(stockDataNode stockData, string year, int month);
void saveToFile(stockDataNode stockData, string year, int month, ofstream& fileA);

string option_2();
vector<string> fetch_stock_names_in_dir();
string extract_stock_name(string fileName);
bool has_suffix(const string &str, const string &suffix);
string percision(double num);


string option_4();
