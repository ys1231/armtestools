// ReSharper disable All
#include "armtools.h"
#include <string.h>
#include <netinet/in.h>
#include <cstdlib>
#include<sys/types.h>
#include <sys/socket.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include"timeout.h"
#include<sys/stat.h>
#include<fcntl.h>

pthread_mutex_t armtools::mutex_x = PTHREAD_MUTEX_INITIALIZER; //static init mutex
armtools* armtools::m_armtool = nullptr;

armtools::armtools()
{
    // ��ʼ��

    m_armtool = this;

    //1.�����׽���
    if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        m_log.logadd(CUTE_ERROR, "create socket error!");
        exit(1);
    }

    // 2. ���ð󶨵ĵ�ַ
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htonl(port_server);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //----------------------------------------------
    bind(m_sock, (sockaddr*)(&server_addr), sizeof(sockaddr));

    // 3. ����
    listen(m_sock, 5);

    // �������Ź��߳�
    pthread_t t;
    pthread_create(&t, nullptr, armtools::callwatchdogproc, nullptr);
    pthread_detach(t);

    // ��ѭ��
    int sin_size = sizeof(sockaddr);
    sockaddr_in client_addr;
    pthread_t t_pthread;
    // �ͻ���socket
    int client_sock = 0;
    while (1)
    {
        // �ȴ�����
        client_sock = accept(m_sock, (sockaddr*)(&client_addr), (socklen_t*)(&sin_size));
        // �����߳� 1. webͨѶ(��ʱ) 
        pthread_create(&t_pthread, nullptr, armtools::callrecvctrlproc, (void*)(&client_sock));
        pthread_detach(t_pthread);
    }
}

armtools::~armtools()
{
    close(m_sock);
}

void* armtools::callwatchdogproc(void*)
{
    m_armtool->m_log.logadd(CUTE_INFO, "start open /dev/watchdog");
    int watchdog_fd = open("/dev/watchdog", O_WRONLY);
    if (watchdog_fd < 0)
    {
        m_armtool->m_log.logadd(CUTE_ERROR, "open watchdog fd faield");
        exit(1);
    }
    char value = 0;
    int ret = 0;

    while (1)
    {
        ret = write(watchdog_fd, &value, 1);
        if (ret < 0)
        {
            m_armtool->m_log.logadd(CUTE_ERROR, "faield to feed watchdog");
        }
        sleep(10);

        // ���ڼ�������־�ļ���
        m_armtool->m_log.is_recreate();
    }
}

void* armtools::callrecvctrlproc(void* client_sock)
{
    // ������ʱ��
    timeout m_timeout;

    // ����web����

    int csock = *((int*)client_sock);
    int date_lenth = 0;
    int recv_lenth = 0;

    // �ظ��ı���
    int status = 0;
    int result = 0;
    int port = 0;
    int user_name_lenth = 0;
    char date[14] = {0};
    // ��һ�ν��������ж�����
    recvtype recv_type = {0};
    response res_ponse = {0};

    while (1)
    {
        // 1. �ж��Ƿ�ʱ
        if (m_timeout.is_timeout())
        {
            m_armtool->m_log.logadd(CUTE_ERROR, "time out auto exit");
            //exit(0);
        }

        recv_lenth = recv(csock, &recv_type, sizeof(recvtype), 0);
        if (recv_lenth != sizeof(recvtype))
        {
            m_armtool->m_log.logadd(CUTE_ERROR, "recv type lenth error normal=%d  recvlenth=%d", sizeof(recv_type),
                                    recv_lenth);
            result = 0;
            break;
        }
        m_armtool->change_endian(to_local, recv_type.requesttype);
        cout << "��������:" << res_ponse.requesttype << endl;

        switch (recv_type.requesttype)
        {
        case RQ_HEART: // ����
            {
                result = 1;
                break;
            }
        case RQ_LOGIN:
            {
                // 1.��ȡ�˿�
                recv_lenth = recv(csock, &port, sizeof(int), 0);
                if (recv_lenth != sizeof(int))
                {
                    m_armtool->m_log.logadd(CUTE_ERROR, "recv port lenth error normal=%d  recvlenth=%d", sizeof(int),
                                            recv_lenth);
                    result = 0;
                    break;
                }
                m_armtool->change_endian(to_local, port);
                cout << "�����Ķ˿�:" << port << endl;

                if (port > 24 || port < 1)
                {
                    result = 0;
                }

                // 2.��ȡ�û��������ܳ���
                recv_lenth = recv(csock, &user_name_lenth, sizeof(int), 0);
                if (recv_lenth != sizeof(int))
                {
                    m_armtool->m_log.logadd(CUTE_ERROR, "recv user_name_lenth error normal=%d  recvlenth=%d",
                                            sizeof(int), recv_lenth);
                    result = 0;
                    break;
                }
                m_armtool->change_endian(to_local, user_name_lenth);
                cout << "�û��������ܳ���:" << user_name_lenth << endl;

                // 3.��ȡ�û�������
                recv_lenth = recv(csock, &date, sizeof(date), 0);
                if (recv_lenth != sizeof(int))
                {
                    m_armtool->m_log.logadd(CUTE_ERROR, "recv user_passwd lenth error normal=%d  recvlenth=%d",
                                            sizeof(int), recv_lenth);
                    result = 0;
                    break;
                }
                cout << "���յ����û�������:" << date << endl;
                // �Ƚ��û�������
            }
        case RQ_GET_PORT_STATUS:
            {
           
            status= m_armtool->m_ctrlpower.power_get_staus(port);
                // ���ݶ˿ڲ�ѯ״̬
                cout << "��ѯ�˿�:" << port <<"---"<< status<<endl;
            }
        case RQ_START:
            {
                // ����
            m_armtool->m_ctrlpower.power_on(port);
                cout << "�����˿�:" << port << endl;
            }
        case RQ_SHUTDOWN:
            {
                // �ػ�
                cout << "�ػ��˿�:" << port << endl;
            }
        case RQ_ENFORCESHUTDOWN:
            {
                // ǿ�ƹػ�
                cout << "ǿ�ƹػ��˿�:" << port << endl;
            }
        default:
            status = 1;
            break;
        }

        // ƴ�ӻظ�����
        res_ponse.length = sizeof(response); //*
        res_ponse.id = recv_type.id;
        res_ponse.requesttype = recv_type.requesttype; //*
        res_ponse.status = status; //*
        res_ponse.result = result; //*
        // ת������
        m_armtool->change_endian(to_web, res_ponse.length);
        m_armtool->change_endian(to_web, res_ponse.requesttype);
        m_armtool->change_endian(to_web, res_ponse.status);
        m_armtool->change_endian(to_web, res_ponse.result);

        send(csock, &res_ponse, sizeof(response), 0);
        memset(&res_ponse, 0, sizeof(response));
        memset(&date, 0, sizeof(date));
    }


    return nullptr;
}

void armtools::change_endian(bool is_change, int& date, uint64_t big)
{
    int temp = 0;
    uint64_t temp64 = 0;

    if (big)
    {
        // idר��
        long high, low;
        low = (long)(big & 0x00000000FFFFFFFF);
        high = big >> 32;
        high = (long)(high & 0x00000000FFFFFFFF);
        low = ntohl(low);
        high = ntohl(high);
        temp64 = low;
        temp64 <<= 32;
        temp64 |= high;
        big = temp64;
    }
    else
    {
        if (!is_change) // ����ת����
            temp = ntohl(date);
        else // ����ת����
            temp = htonl(date);

        date = temp;
    }
}
