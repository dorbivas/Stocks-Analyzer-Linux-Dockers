#include "utils.h"

int linesInFile(string stockName, int& firstYear)
{
	fstream fStockEps;
	string line;
	int res = 0;

	fStockEps.open(stockName + ".eps");
	getline(fStockEps, line);
	firstYear = stoi(line.substr(0, 4));

	res++;

	while (getline(fStockEps, line))
		res++;

	fStockEps.close();
	return res;
}

int currYear()
{
	time_t now = time(0);
	tm* ltm = localtime(&now);

	return 1900 + ltm->tm_year;
}

bool isValidYear(string year) //at most we want last 20 years
{
	return stoi(year) >= 2002 && stoi(year) <= currYear();
}
double get1YearEps(string stockName, string year)
{
	//format example: 2021 7.97

	fstream fStockEps;
	fStockEps.open(stockName + ".eps");
	string line;
	double eps;
	bool yearFound = false;
	do
	{
		getline(fStockEps, line);
		if (line.substr(0, 4) == year)
		{
			line.erase(0, 5);
			eps = stod(line);
			yearFound = true;
		}
	} while (!yearFound && !fStockEps.eof());
	return eps;
}

