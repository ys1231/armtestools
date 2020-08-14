// ReSharper disable All
#include "logger.h"

#include <string.h>
#include <stdarg.h>
#include<stdio.h>

logger::logger()
{
	
	time_t t = time(0);
	char tmp[64] = {};
	strftime(tmp, sizeof(tmp), "arm_%Y-%m-%d", localtime(&t));
	string fileName;
	fileName = tmp;
	fileName += ".log";

	m_filelog.open(fileName.c_str(), ios::app);
	m_filelog << getdate().c_str()<< "info:"<<"start log"<< endl;

	m_old_time = getdate();
}

logger::~logger()
{
	m_filelog.close();
}

string logger::getdate()
{
	time_t t = time(0);
	char tmp[64] = {};
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %X:", localtime(&t));
	return tmp;
}

void logger::logadd(int level,const char * str,...)
{
	char buffer[512];   // ·ÖÅä512×Ö½ÚµÄ×Ö·û´®»º´æ
	memset(buffer, 0, 512);
	va_list aptr;
	va_start(aptr, str);
	vsnprintf(buffer, sizeof(buffer), str, aptr);
	va_end(aptr);
	char  levelstr[3][10] = { "info: ","debug: ","error: " };
	m_filelog << getdate()<< levelstr[level] << buffer << endl;
}


void logger::is_recreate()
{
	if(getdate().compare(m_old_time))
	{
		m_filelog.close();
		time_t t = time(0);
		char tmp[64] = {};
		strftime(tmp, sizeof(tmp), "arm_%Y-%m-%d", localtime(&t));
		string fileName;
		fileName = tmp;
		fileName += ".log";

		m_filelog.open(fileName.c_str(), ios::app);
		m_filelog << getdate().c_str() << "info:" << "start log" << endl;

		m_old_time = getdate();
		
	}
}
