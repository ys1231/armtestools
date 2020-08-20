#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <sys/time.h>
#include <unistd.h>
class timeout
{
	// ��ʱ���
	__time_t m_time_out = 30;
	timeval m_old_time = { 0 };
	timeval m_new_time = { 0 };
public:
	timeout();
	~timeout();

	
	// ��ʼ��ʱ��
	bool is_timeout();
	void update_old_time(__time_t time_out = 30);
};

#endif