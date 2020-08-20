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
    // ��¼
    RQ_HEART,
    // ����
    RQ_GET_PORT_STATUS,
    // ��ȡ״̬
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

// ������Ӧ
struct response
{
    int length;
    uint64_t id;
    int requesttype;
    int status; //(0:��ʾ�ػ���1����ʾ����)
    int result; //����� 0:��ʾ ʧ�� 1����ʾ �ɹ���
};

struct request
{
    int length;
    uint64_t id;
    int requesttype; //��ȡ״̬���ֵΪ3�����������ֵΪ4���ػ���� ֵΪ5����    
    int port;
    int user_passwd_len; //�û����������ܳ���
    char* data; // �û��������룺�ָ��_  ���� user_passwd ; user_abc#123
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
    // ���Ź�
    static void* callwatchdogproc(void*);
    static void* callrecvctrlproc(void* client_addr);
    static pthread_mutex_t mutex_x;
    // ����ת��
    void change_endian(bool is_change, int& date, uint64_t big = 0);
    // �û���������֤
    bool compare(char*);
};


#endif
