#include "slave.h"

using namespace std;

class zipUtil // used for zip extraction later on
{
public:
	const char *name = "stocks_db.zip";
	set<string> extract_zip();
};
bool option1_done_flag = false;

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main(void)
{
	// removing the pipes if they exist
	system("rm -f /data/pipe");

	system("chmod 755 get_stocks_data.sh"); // first thing, enabling the option to execute the script

	FILE *fd_log = fopen("/data/log.txt", "w+"); // opening the log file for testing purposes

	if (mkfifo("/data/pipe", 0777) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		return 1;
	}

	// int slave_id = getpid(); // TODO

	// closing unused pipe ends
	// close(slave_to_master[READ]);
	// close(master_to_slave[WRITE]);

	int buffSize = 8000;
	char slave_buffer[buffSize];

	// reading from parent process

	while (true)
	{
		int fd_master_to_slave = open("/data/pipe", O_RDONLY);
		read(fd_master_to_slave, slave_buffer, buffSize);
		close(fd_master_to_slave);

		// seperating the stock name and the user choice
		string temp(slave_buffer);
		string to_pipe_buffer = "";

		int user_choice = temp[0] - '0';

		temp = temp.substr(1, temp.length());

		// getting the output of each function, and doing the operations
		switch (user_choice)
		{
		case 1:
			to_pipe_buffer += option_1or3(1, option1_done_flag, temp);
			option1_done_flag = true;
			break;
		case 2:
			to_pipe_buffer += option_2();
			break;
		case 3:
			to_pipe_buffer += option_1or3(3, option1_done_flag, temp);
			option1_done_flag = true;
			break;
		case 4:
			to_pipe_buffer += option_4();
			// plumb_pipes_and_remove_files();	//TODO
			exit(1);
			break;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		fprintf(fd_log, "Got option %d.\n", user_choice);
		fflush(fd_log);

		fprintf(fd_log, "Sending to parent: %s\n", to_pipe_buffer.c_str());
		fflush(fd_log);

		string out = "";
		string stockName = "GOOG";
		string commandScript = "./get_stocks_data.sh ";
		commandScript = commandScript + stockName;
		int exitStat = system(commandScript.c_str());
		out += exec(commandScript.c_str());

		fprintf(fd_log, "Got output: %s\n", out.c_str());

		struct dirent *d;
		DIR *dr;
		dr = opendir(".");
		if (dr != NULL)
		{
			for (d = readdir(dr); d != NULL; d = readdir(dr))
			{
				fprintf(fd_log, "%s\n", d->d_name);
			}
			fprintf(fd_log, "\n\n\n\n\n");
			closedir(dr);
		}
		//////////////////////////////////////////////////////////////////////////////////////////////

		// writing back to pipe
		int fd_slave_to_master = open("/data/pipe", O_WRONLY);
		write(fd_slave_to_master, to_pipe_buffer.c_str(), buffSize);
		close(fd_slave_to_master);
	}
}

string option_4()
{
	vector<string> names = fetch_stock_names_in_dir();
	filesEndingWithStockToCSV(names); // all data- into csv format
	remove("stocks_db.zip");
	string buffer = "";

	int errorp;
	zip_t *zipper;
	zip_source_t *zip_source;
	ifstream fs;		  // used to open files, READ them into buffer, into new files inside the zip
	string name_with_csv; // used to add ".csv"

	for (long int i = 0; i < names.size(); i++) // each csv file, is being opened, READ, and putted inside the zip
	{
		zipper = zip_open("stocks_db.zip", ZIP_CREATE, &errorp);
		name_with_csv = names[i] + ".csv";

		fs.open(name_with_csv, std::ios::binary);
		std::string content((std::istreambuf_iterator<char>(fs)), (std::istreambuf_iterator<char>()));
		fs.close();

		zip_source = zip_source_buffer(zipper, content.c_str(), content.length(), 0);
		zip_file_add(zipper, name_with_csv.c_str(), zip_source, ZIP_FL_OVERWRITE);
		zip_close(zipper);

		remove(name_with_csv.c_str());
	}
	string tmp1;

	char tmp[2048];
	getcwd(tmp, 2048);
	tmp1 = tmp;
	buffer += "Data saved in " + tmp1 + "/stocks_db.zip\n";
	return buffer;
}

string option_2()
{
	string buffer = "";
	vector<string> names = fetch_stock_names_in_dir();
	buffer += "Available stocks: ";
	for (unsigned int i = 0; i < names.size(); i++)
		buffer += names[i] + ' ';
	if (names.size() == 0)
		buffer += "No stocks available";
	buffer += '\n';

	return buffer;
}

string option_1or3(int option, bool &option1_done_flag, string stock_name)
{
	string buffer = "";
	string curr_year = "2022";

	if (option == 1)
	{
		try
		{
			fetchStock(stock_name);
			option1_done_flag = true;
			buffer = "Fetch " + stock_name + " DONE\n";
		}
		catch (const exception &e)
		{
			cout << e.what() << '\n';
			return nullptr;
		}
	}
	else if (option == 3)
		buffer += printStockData(stock_name, curr_year);
	else
		throw "invalid command";

	return buffer;
}

set<string> zipUtil::extract_zip()
{
	set<string> stockNameSet;
	int err = 0;
	zip *zip = zip_open(name, 0, &err);
	struct zip_stat st;

	for (int i = 0; i < zip_get_num_entries(zip, 0); i++)
	{
		zip_stat_index(zip, i, 0, &st);
		char *inside = new char[st.size];
		zip_file *file = zip_fopen(zip, st.name, 0);
		zip_fread(file, inside, st.size);
		zip_fclose(file);

		ofstream outfile(st.name);
		outfile << inside;
		outfile.flush();
		outfile.close();

		delete[] inside;
		string s(st.name);
		stockNameSet.insert(s.substr(0, s.find(".")));
	}
	zip_close(zip);
	return stockNameSet;
}

void errorMsg(string msg, bool &flag)
{
	flag = false;
	cout << msg << endl;
}

void fetchStock(string stockName)
{
	string commandScript = "./get_stocks_data.sh ";
	commandScript = commandScript + stockName;
	int exitStat = system(commandScript.c_str());

	if (exitStat == 1)
		throw "cant fetch stock";
}

string printStockData(string stockName, string year)
{
	string buffer = "";
	int curr_year = stoi(year);
	for (int i = 0; i < 3; i++)
	{
		curr_year -= i;
		year = to_string(curr_year);

		stockDataNode stockData;
		stockData.name = stockName;
		buffer += sumOneYearData(year, stockData);
	}
	return buffer;
}

string sumOneYearData(string year, stockDataNode &stockData)
{
	string buffer = "";
	ifstream fStockData, fStockEps;
	ofstream fileA("stocksData.csv");

	fStockData.open(stockData.name + ".stock");
	fStockEps.open(stockData.name + ".eps");
	vector<stockDataNode> data;

	string delimiter = "-", line;
	size_t posYear;
	int currMonth = 0, prevMonth = 0;
	bool foundYear = false, isFirstIter = true;
	double yearlyEps = stockData.eps = get1YearEps(stockData.name, year);
	buffer += "stock " + stockData.name + " year " + year + " data: " + '\n';
	do
	{
		getline(fStockData, line);
		posYear = line.find(delimiter);

		string token = line.substr(0, posYear);
		if (year.compare(token) != 0 && prevMonth == 0)
			continue;

		else
		{ // correct year
			string monthToken;
			size_t posMonth = posYear;
			line.erase(0, posMonth + delimiter.length());
			posYear = line.find(delimiter);	 // advence to month (next mm-dd)
			token = line.substr(0, posYear); // tokenYear gets month

			if (stoi(token) != currMonth && !isFirstIter)
			{
				buffer += printOneMonth(stockData, year, currMonth);
				saveToFile(stockData, year, currMonth, fileA);

				stockData = stockDataNode();
				stockData.eps = yearlyEps;
			}
			currMonth = stoi(token);

			if (!isFirstIter)
			{
				if (currMonth == 12 && prevMonth == 1) // last month
					break;							   // endloop
			}
			else
				isFirstIter = false;

			prevMonth = currMonth;
			readLine(line, stockData);
		}
	} while ((posYear = line.find(delimiter)) != string::npos && foundYear == false && !fStockData.eof());

	fStockData.close();
	return buffer;
}

// format: date Y\M\D 2022-03-18 open 206.7000 high 216.8000 low 206.0000 close 216.4900  valume 51235040
void readLine(string line, stockDataNode &stockData)
{
	string delimiter = " ";
	size_t pos = 0;
	pos = line.find(delimiter);
	line.erase(0, pos + delimiter.length()); // delete date

	pos = line.find(delimiter);
	stockData.open += getOneParam(delimiter, line, pos);
	stockData.high += getOneParam(delimiter, line, pos);
	stockData.low += getOneParam(delimiter, line, pos);
	stockData.close += getOneParam(delimiter, line, pos);
	stockData.volume += getOneParam(delimiter, line, pos);
}

double getOneParam(string delimiter, string &line, size_t &pos)
{
	string token = line.substr(0, pos);
	line.erase(0, pos + delimiter.length());
	pos = line.find(delimiter);
	return stod(token);
}

vector<string> fetch_stock_names_in_dir()
{
	vector<string> stock_names;

	DIR *dir;
	struct dirent *pointToName;

	string temp;

	dir = opendir("."); // open all directory
	if (dir)
	{
		while ((pointToName = readdir(dir)) != NULL)
		{
			temp = string(pointToName->d_name);
			if (has_suffix(temp, ".stock"))							  // if sstring file ends with .stock
				stock_names.push_back(temp.erase(temp.length() - 6)); // it's a valid stock, insert to names
		}
		closedir(dir); // close all directory
	}
	return stock_names;
}

bool has_suffix(const string &str, const string &suffix)
{
	return str.size() >= suffix.size() &&
		   str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

string percision(double num)
{
	int whole_part = int(num);

	string after_dec_point;
	after_dec_point = to_string(num - whole_part);

	return to_string(whole_part) + '.' + after_dec_point.substr(2, 4);
}

string printOneMonth(stockDataNode stockData, string year, int month)
{
	string buffer = "";
	buffer += " ";
	if (month < 10)
		buffer += '0';
	buffer += to_string(month) + ": { 1. open: " + percision(stockData.open) +
			  ", 2. high: " + percision(stockData.high) +
			  ", 3. low: " + percision(stockData.low) +
			  ", 4. close: " + percision(stockData.close) +
			  ", 5. volume: " + percision(stockData.volume) +
			  ", 6. reportedEPS: " + percision(stockData.eps) + " }" + '\n';
	return buffer;
}
void saveToFile(stockDataNode stockData, string year, int month, ofstream &fileA)
{
	fileA << " ";
	if (month < 10)
		fileA << '0';
	fileA << month << ", " << fixed << std::setprecision(4) << stockData.open << ", " << fixed << std::setprecision(4) << stockData.high << ", " << fixed << std::setprecision(4) << stockData.low << ", " << fixed << std::setprecision(4) << stockData.close << ", " << fixed << std::setprecision(0) << stockData.volume << ", " << fixed << std::setprecision(4) << stockData.eps << endl;
}
