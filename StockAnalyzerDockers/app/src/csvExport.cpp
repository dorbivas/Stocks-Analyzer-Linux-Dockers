#include "csvExport.h"
using namespace std;
int main1()
{
    editFileIntoCSVFornat();

    return 0;
}

void editFileIntoCSVFornat()
{
    ifstream fileA("stockData.csv");
    ifstream fileB("epsSorted.csv");
    ofstream finalFile("data.csv");

    string line;

    while (getline(fileA, line))
        finalFile << line << endl;

    finalFile << endl;

    while (getline(fileB, line))
        finalFile << line << endl;

    fileA.close();
    fileB.close();
    finalFile.close();

    remove("stockData.csv");
    remove("epsSorted.csv");
    cout << "done." << endl;
}

void filesEndingWithStockToCSV(vector<string> files_names)
{
    for (unsigned int i = 0; i < files_names.size(); i++)
    {
        ifstream in(files_names[i] + ".stock", ios::in | ios::binary);
        ofstream out(files_names[i] + ".csv", ios::out | ios::binary);

        for (char c; in.get(c); out.put(c))
            if (c == ' ')
                c = ',';
    }
}
