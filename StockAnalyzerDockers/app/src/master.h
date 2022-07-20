#pragma once
using namespace std;
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
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
#include <iostream>
#include <string>
#include <vector>
#include <set>

static void display_menu();
int get_input();
void sigint_handler(int signum);
