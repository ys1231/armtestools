#include "timeout.h"

timeout::timeout()
{
	// 初始化时间
	gettimeofday(&m_old_time, NULL);
}

timeout::~timeout()
{
}

bool timeout::is_timeout()
{
	gettimeofday(&m_new_time, 0);
	if (m_time_out < (m_new_time.tv_sec - m_old_time.tv_sec))
		return true;
	else
		return false;
}

void timeout::update_old_time(__time_t time_out)
{
	m_time_out = time_out;
	gettimeofday(&m_old_time, 0);
}
