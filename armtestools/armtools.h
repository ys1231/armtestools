// ReSharper disable All
#ifndef MYSOCKET_H
#define MYSOCKET_H


#define port_server 9999
#define to_local 0
#define to_web 1
#include <cstdint>
#include"ctrlpower.h"
#include "logger.h"

// webcmdtype
enum cmd_type
{
    RQ_LOGIN = 1,
    // 登录
    RQ_HEART,
    // 心跳
    RQ_GET_PORT_STATUS,
    // 获取状态
    RQ_START,
    // 
    RQ_SHUTDOWN,
    RQ_ENFORCESHUTDOWN
};

// struct

struct recvtype
{
    int length;
    uint64_t id;
    int requesttype;
};

// 心跳响应
struct response
{
    int length;
    uint64_t id;
    int requesttype;
    int status; //(0:表示关机，1：表示开机)
    int result; //（结果 0:表示 失败 1：表示 成功）
};

struct request
{
    int length;
    uint64_t id;
    int requesttype; //获取状态命令（值为3）、开机命令（值为4、关机命令（ 值为5））    
    int port;
    int user_passwd_len; //用户名和密码总长度
    char* data; // 用户名和密码：分割符_  比如 user_passwd ; user_abc#123
};


class armtools
{
public:
    armtools();
    ~armtools();


private:
    int m_sock = 0;
    logger m_log;
    ctrlpower m_ctrlpower;
    static armtools* m_armtool;
    // 看门狗
    static void* callwatchdogproc(void*);
    static void* callrecvctrlproc(void* client_addr);
    static pthread_mutex_t mutex_x;
    // 数据转换
    void change_endian(bool is_change, int& date, uint64_t big = 0);
    // 用户名密码验证
    bool compare(char*);
};


#endif
