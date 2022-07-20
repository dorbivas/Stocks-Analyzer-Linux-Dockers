#include "master.h"

/***************globals***************/
int slave_id = 0;
int master_id = 0;
int master_to_slave[2]; // Used to store two ends of first pipe
int slave_to_master[2]; // Used to store two ends of second pipe

#define READ 0
#define WRITE 1
/*************************************/

class zipUtil // used for zip extraction later on
{
public:
    const char *name = "stocks_db.zip";
    set<string> extract_zip();
};

int main(void)
{
    // removing the pipes if they exist
	//system("rm -f pipe");

    //zipUtil ext;
    //ext.extract_zip(); // extracting previous db

    int buffSize = 8000;
    char slave_buffer[buffSize];
    string stock_for_option_1_or_3;  

    while (true)
    {

        //master_id = getpid(); // updating the global variable

        cout << endl
             << endl
             << "================================" << endl;

        signal(SIGINT, sigint_handler); // for catching ^C

        // menu 1-4 display
        display_menu();

        // recieving input from the user
        int choice = get_input();

        string temp = to_string(choice); // later on, we will concatenate the chosen option to the input stock name, in order to write together to the pipe.
                                         //  for example: 1GOOG
        switch (choice)                  // specifically, in option 1 and 3 we need to get the stock name
        {
        case 1:
            cout << "Type stock name: ";
            cin >> stock_for_option_1_or_3;
            temp += stock_for_option_1_or_3;
            break;
        case 3:
            cout << "Type stock name: ";
            cin >> stock_for_option_1_or_3;
            temp += stock_for_option_1_or_3;
            break;
        }

        char input_str[100];
        strcpy(input_str, &temp[0]);

        int fd_master_to_slave = open("/data/pipe", O_WRONLY);
        write(fd_master_to_slave, input_str, strlen(input_str) + 1); // writing to child
        close(fd_master_to_slave);

        if (choice == 4)
        {
            cout << "Exiting..." << endl;
            break;
        }

        int fd_slave_to_master = open("/data/pipe", O_RDONLY);
        read(fd_slave_to_master, slave_buffer, buffSize); // read from child
        close(fd_slave_to_master);

        cout << slave_buffer; // display buffer;
    }
}
static void display_menu()
{
    cout << "1 - Fetch stock data      " << endl
         << "2 - List fetched sotcks   " << endl
         << "3 - Print stock data      " << endl
         << "4 - Save all stocks data  " << endl
         << "Make your choice(1/2/3/4): ";
}

void sigint_handler(int signum) // catch CTRL C
{
    if (getpid() == slave_id || getpid() == master_id) /// child process will do option 4 and exit
    {
        cout << endl
             << "Crushing gently :-)..." << endl;
    }

    exit(1);
}

int get_input()
{
    int input = 0, inputChecker = 0;
    bool inputFlag = false;
    cin >> input;

    if (inputChecker != input)
        inputFlag = false;

    if (input < 1 || input > 4)
    {
        if (!inputFlag) // 1-4 are the only valid options
        {
            cout << "Must be options 1-4 " << endl;
            inputFlag = true;
        }
    }
    return input;
}

