#include "test.h"
#include <stdarg.h>
#include <string.h>
#include<stdio.h>


void test::logadd(int level, const char * str, ...)
{
	char buffer[512];   // ·ÖÅä512×Ö½ÚµÄ×Ö·û´®»º´æ
	memset(buffer, 0, 512);
	va_list aptr;
	va_start(aptr, str);
	vsnprintf(buffer, sizeof(buffer), str, aptr);
	va_end(aptr);
	puts(buffer);
	cout << buffer << endl;
	//char  levelstr[3][10] = { "info: ","debug: ","error: " };
	
}