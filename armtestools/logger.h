// ReSharper disable All
#ifndef LOGGER_H
#define LOGGER_H
#include<time.h>
#include<fstream>
#include <iostream>
using namespace std;

#define CUTE_INFO 0
#define CUTE_DEBUG 1
#define CUTE_ERROR 2

class logger
{
public:
	logger();
	~logger();
	string getdate();

	void logadd(int level, const char*str,...);
	void is_recreate();
private:
	fstream m_filelog;
	
	string m_old_time = {0};
	
};

#endif
