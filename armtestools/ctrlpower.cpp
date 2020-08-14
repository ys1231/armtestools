// ReSharper disable All
#include "ctrlpower.h"
#include <string.h>
#include<termios.h> 
#include <fcntl.h>
#include <unistd.h>

ctrlpower::ctrlpower(logger* log): m_log(log)
{
	// 1. 打开串口
	m_serial = open("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NONBLOCK);
	//O_NONBLOCK设置为非阻塞模式，在read时不会阻塞住，在读的时候将read放在while循环中，下一节篇文档将详细讲解阻塞和非阻塞
	if (m_serial == -1)
	{
		m_log->logadd(CUTE_ERROR, " Open Serial failed");
		exit(1);
	}
	// 2. 初始化串口
	set_opt();
	
	
}

ctrlpower::~ctrlpower()
{
	m_log = nullptr;
	close(m_serial);
}

int ctrlpower::power_on(int port)
{
	int cmd[6] = { 0x5A,0x0,0x03,0x02 };
	cmd[4] = port;
	cmd[5] = 0x0;
	int result=excute_cmd(cmd);
	usleep(200);
	cmd[5] = 0x1;
	result=excute_cmd(cmd);
	usleep(200);
	cmd[5] = 0x0;
	result=excute_cmd(cmd);

	return result;
}

int ctrlpower::power_off(int port)
{
	int cmd[6] = { 0x5A,0x0,0x03,0x02 };
	cmd[4] = port;
	cmd[5] = 0x0;
	int result = excute_cmd(cmd);
	usleep(200);
	cmd[5] = 0x1;
	result = excute_cmd(cmd);
	usleep(200);
	cmd[5] = 0x0;
	result = excute_cmd(cmd);

	return result;
}

int ctrlpower::power_frcoff(int port)
{
	int cmd[6] = { 0x5A,0x0,0x03,0x02 };
	cmd[4] = port;
	cmd[5] = 0x0;
	int result = excute_cmd(cmd);
	usleep(200);
	cmd[5] = 0x1;
	result = excute_cmd(cmd);
	usleep(5000);
	cmd[5] = 0x0;
	result = excute_cmd(cmd);

	return result;
}

int ctrlpower::power_get_staus(int port)
{
	int cmd[6] = { 0x5A,0x0,0x01,0x01 };
	cmd[4] = port;

	int ret_cmd[5] = { 0 };
	int result = excute_cmd(cmd,ret_cmd);

	return ret_cmd[4];
}

int ctrlpower::set_opt(int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio, oldtio;
	/*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
	if (tcgetattr(m_serial, &oldtio) != 0)
	{
		perror("SetupSerial 1");
		printf("tcgetattr( fd,&oldtio) -> %d\n", tcgetattr(m_serial, &oldtio));
		return -1;
	}
	memset(&newtio, 0,sizeof(newtio));
	/*步骤一，设置字符大小*/
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;
	/*设置停止位*/
	switch (nBits)
	{
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}
	/*设置奇偶校验位*/
	switch (nEvent)
	{
	case 'o':
	case 'O': //奇数 
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'e':
	case 'E': //偶数 
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'n':
	case 'N': //无奇偶校验位 
		newtio.c_cflag &= ~PARENB;
		break;
	default:
		break;
	}
	/*设置波特率*/
	switch (nSpeed)
	{
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	case 460800:
		cfsetispeed(&newtio, B460800);
		cfsetospeed(&newtio, B460800);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}
	/*设置停止位*/
	if (nStop == 1)
		newtio.c_cflag &= ~CSTOPB;
	else if (nStop == 2)
		newtio.c_cflag |= CSTOPB;
	/*设置等待时间和最小接收字符*/
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;
	/*处理未接收字符*/
	tcflush(m_serial, TCIFLUSH);
	/*激活新配置*/
	if ((tcsetattr(m_serial, TCSANOW, &newtio)) != 0)
	{
		m_log->logadd(CUTE_ERROR,"serial set error");
		return -1;
	}
	m_log->logadd(CUTE_INFO, "set done!");
	return 0;
}

int ctrlpower::excute_cmd(int * cmd,int *type)
{
	int result = write(m_serial, cmd, sizeof(cmd));
	if (result == -1)
	{
		result = write(m_serial, cmd, sizeof(cmd));
	}

	//////////////////////////
	printf("写入%d字节:",result);
	for(int i=0;i<sizeof(cmd);i++)
	{
		cout << cmd[i];
	}
	cout << "" << endl;

	
	usleep(10);
	int result_cmd[5] = { 0 };
	result = read(m_serial, result_cmd, sizeof(result_cmd));
	if (result == -1)
		return 0;

	////////////////////////////
	printf("读取%d字节\n", result);
	for (int i = 0; i < sizeof(result_cmd); i++)
	{
		cout << result_cmd[i];
	}
	cout << "" << endl;
	
	// 判断命令是否执行成功
	if (result_cmd[2] != (cmd[2] | 0x80))
		return 0;
	else
	{
		if (type == nullptr)
			return 1;
		else
			type[4] = result_cmd[4];
	}
		
}
