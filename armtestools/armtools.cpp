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
    // 初始化

    m_armtool = this;

    //1.创建套接字
    if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        m_log.logadd(CUTE_ERROR, "create socket error!");
        exit(1);
    }

    // 2. 设置绑定的地址
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htonl(port_server);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //----------------------------------------------
    bind(m_sock, (sockaddr*)(&server_addr), sizeof(sockaddr));

    // 3. 监听
    listen(m_sock, 5);

    // 创建看门狗线程
    pthread_t t;
    pthread_create(&t, nullptr, armtools::callwatchdogproc, nullptr);
    pthread_detach(t);

    // 主循环
    int sin_size = sizeof(sockaddr);
    sockaddr_in client_addr;
    pthread_t t_pthread;
    // 客户端socket
    int client_sock = 0;
    while (1)
    {
        // 等待连接
        client_sock = accept(m_sock, (sockaddr*)(&client_addr), (socklen_t*)(&sin_size));
        // 创建线程 1. web通讯(超时) 
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

        // 用于检测更新日志文件名
        m_armtool->m_log.is_recreate();
    }
}

void* armtools::callrecvctrlproc(void* client_sock)
{
    // 创建定时器
    timeout m_timeout;

    // 处理web命令

    int csock = *((int*)client_sock);
    int date_lenth = 0;
    int recv_lenth = 0;

    // 回复的变量
    int status = 0;
    int result = 0;
    int port = 0;
    int user_name_lenth = 0;
    char date[14] = {0};
    // 第一次接收用于判断命令
    recvtype recv_type = {0};
    response res_ponse = {0};

    while (1)
    {
        // 1. 判断是否超时
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
        cout << "命令类型:" << res_ponse.requesttype << endl;

        switch (recv_type.requesttype)
        {
        case RQ_HEART: // 心跳
            {
                result = 1;
                break;
            }
        case RQ_LOGIN:
            {
                // 1.获取端口
                recv_lenth = recv(csock, &port, sizeof(int), 0);
                if (recv_lenth != sizeof(int))
                {
                    m_armtool->m_log.logadd(CUTE_ERROR, "recv port lenth error normal=%d  recvlenth=%d", sizeof(int),
                                            recv_lenth);
                    result = 0;
                    break;
                }
                m_armtool->change_endian(to_local, port);
                cout << "操作的端口:" << port << endl;

                if (port > 24 || port < 1)
                {
                    result = 0;
                }

                // 2.获取用户名密码总长度
                recv_lenth = recv(csock, &user_name_lenth, sizeof(int), 0);
                if (recv_lenth != sizeof(int))
                {
                    m_armtool->m_log.logadd(CUTE_ERROR, "recv user_name_lenth error normal=%d  recvlenth=%d",
                                            sizeof(int), recv_lenth);
                    result = 0;
                    break;
                }
                m_armtool->change_endian(to_local, user_name_lenth);
                cout << "用户名密码总长度:" << user_name_lenth << endl;

                // 3.获取用户名密码
                recv_lenth = recv(csock, &date, sizeof(date), 0);
                if (recv_lenth != sizeof(int))
                {
                    m_armtool->m_log.logadd(CUTE_ERROR, "recv user_passwd lenth error normal=%d  recvlenth=%d",
                                            sizeof(int), recv_lenth);
                    result = 0;
                    break;
                }
                cout << "接收到的用户名密码:" << date << endl;
                // 比较用户名密码
            }
        case RQ_GET_PORT_STATUS:
            {
           
            status= m_armtool->m_ctrlpower.power_get_staus(port);
                // 根据端口查询状态
                cout << "查询端口:" << port <<"---"<< status<<endl;
            }
        case RQ_START:
            {
                // 开机
            m_armtool->m_ctrlpower.power_on(port);
                cout << "开机端口:" << port << endl;
            }
        case RQ_SHUTDOWN:
            {
                // 关机
                cout << "关机端口:" << port << endl;
            }
        case RQ_ENFORCESHUTDOWN:
            {
                // 强制关机
                cout << "强制关机端口:" << port << endl;
            }
        default:
            status = 1;
            break;
        }

        // 拼接回复数据
        res_ponse.length = sizeof(response); //*
        res_ponse.id = recv_type.id;
        res_ponse.requesttype = recv_type.requesttype; //*
        res_ponse.status = status; //*
        res_ponse.result = result; //*
        // 转换数据
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
        // id专用
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
        if (!is_change) // 网络转本地
            temp = ntohl(date);
        else // 本地转网络
            temp = htonl(date);

        date = temp;
    }
}
