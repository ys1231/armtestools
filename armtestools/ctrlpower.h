// ReSharper disable All
#ifndef CTRLPOWER_H
#define CTRLPOWER_H

#include"logger.h"

class ctrlpower
{
public:

    ctrlpower(logger* m_log);
    ~ctrlpower();

	int power_on(int port);
	int power_off(int port);
	int power_frcoff(int port);
	int power_get_staus(int port);

	
private:
	int m_serial = 0;
	logger* m_log = nullptr;
	int set_opt(int nSpeed=115200, int nBits=8, char nEvent='n', int nStop=1);
	int excute_cmd(int* cmd,int *type= nullptr);
};

#endif
