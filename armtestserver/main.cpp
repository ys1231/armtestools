#include <cstdlib>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h> 
#define portnum 3333

int main()
{
	int sockfd;
	int new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char buffer[128];
	int nByte;
	int sin_size;

	//1.�����׽���
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("create socket error!\n");
		exit(1);
	}

	//2.1����Ҫ�󶨵ĵ�ַ
	bzero(&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portnum); //�ֽ���(��С��)
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//2.�󶨵�ַ
	bind(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr));

	//3.�����˿�
	listen(sockfd, 5);

	while (1)
	{
		//4.�ȴ�����
		sin_size = sizeof(struct sockaddr);
		new_fd = accept(sockfd, (struct sockaddr*)(&client_addr), &sin_si ze);//ע��:��3������Ϊsocklen_t*�����ε�ָ�룩
		printf("server get connection from %s\n", inet_ntoa(client_addr.sin_addr)); //�������͵�IP��ַת��Ϊ�ַ���(192.168.1.1)

		//5.��������
		nByte = recv(new_fd, buffer, 128, 0);
		buffer[nByte] = '\0';
		printf("server reciivd : %s\n", buffer);

		//6.��������
		close(new_fd);
	}

	close(sockfd);

	return 0;

}

//
//
// #include<sys/types.h>
// #include<sys/socket.h>
// #include<netinet/in.h>
// #include<arpa/inet.h>
// #include<unistd.h>
// #include<stdlib.h>
// #include<stdio.h>
// #include<string.h>
// #include<netdb.h>
// #include<errno.h>
// #define PORT 2345
// #define MAXSIZE 1024
//
// int main(int argc, char *argv[])
// {
//     int sockfd, newsockfd;
//     //���������׽ӿ����ݽṹ
//     struct sockaddr_in server_addr;
//     struct sockaddr_in client_addr;
//     int sin_zise, portnumber;
//     //�������ݻ�����
//     char buf[MAXSIZE];
//     //����ͻ����׽ӿ����ݽṹ
//     int addr_len = sizeof(struct sockaddr_in);
//     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//     {
//         fprintf(stderr, "create socket failed\n");
//         exit(EXIT_FAILURE);
//     }
//     puts("create socket success");
//     printf("sockfd is %d\n", sockfd);
//     //��ձ�ʾ��ַ�Ľṹ�����
//     bzero(&server_addr, sizeof(struct sockaddr_in));
//     //����addr�ĳ�Ա������Ϣ
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(PORT);
//     //����ipΪ����IP
//     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     if (bind(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0)
//     {
//         fprintf(stderr, "bind failed \n");
//         exit(EXIT_FAILURE);
//     }
//     puts("bind success\n");
//     if (listen(sockfd, 10) < 0)
//     {
//         perror("listen fail\n");
//         exit(EXIT_FAILURE);
//     }
//     puts("listen success\n");
//     int sin_size = sizeof(struct sockaddr_in);
//     printf("sin_size is %d\n", sin_size);
//     if ((newsockfd = accept(sockfd, (struct sockaddr *)(&client_addr), &sin_size)) < 0)
//     {
//         perror("accept error");
//         exit(EXIT_FAILURE);
//     }
//     printf("accepted a new connetction\n");
//     printf("new socket id is %d\n", newsockfd);
//     printf("Accept clent ip is %s\n", inet_ntoa(client_addr.sin_addr));
//     printf("Connect successful please input message\n");
//     char sendbuf[1024];
//     char mybuf[1024];
//     while (1)
//     {
//         int len = recv(newsockfd, buf, sizeof(buf), 0);
//         if (strcmp(buf, "exit\n") == 0)
//             break;
//         fputs(buf, stdout);
//         send(newsockfd, buf, len, 0);
//         memset(sendbuf, 0, sizeof(sendbuf));
//         memset(buf, 0, sizeof(buf));
//     }
//     close(newsockfd);
//     close(sockfd);
//     puts("exit success");
//     exit(EXIT_SUCCESS);
//     return 0;
// }

//
// #include <iostream>
// #include<pthread.h>
// #include<stdlib.h>
// #include<string.h>
// #include<unistd.h>
//
// using namespace std;
//
// int ticket_sum = 20;
// pthread_mutex_t mutex_x = PTHREAD_MUTEX_INITIALIZER;//static init mutex
//
// void *sell_ticket(void *arg)
// {
// 	for (int i = 0; i < 20; i++)
// 	{
// 		pthread_mutex_lock(&mutex_x);//atomic opreation through mutex lock
// 		if (ticket_sum > 0)
// 		{
// 			sleep(1);
// 			cout << "sell the " << 20 - ticket_sum + 1 << "th" << endl;
// 			ticket_sum--;
// 		}
// 		pthread_mutex_unlock(&mutex_x);
// 	}
// 	return 0;
// }
//
// int main()
// {
// 	int flag;
// 	pthread_t tids[4];
//
// 	for (int i = 0; i < 4; i++)
// 	{
// 		flag = pthread_create(&tids[i], NULL, &sell_ticket, NULL);
// 		if (flag)
// 		{
// 			cout << "pthread create error ,flag=" << flag << endl;
// 			return flag;
// 		}
// 	}
//
// 	sleep(20);
// 	void *ans;
// 	for (int i = 0; i < 4; i++)
// 	{
// 		flag = pthread_join(tids[i], &ans);
// 		if (flag)
// 		{
// 			cout << "tid=" << tids[i] << "join erro flag=" << flag << endl;
// 			return flag;
// 		}
// 		cout << "ans=" << ans << endl;
// 	}
// 	return 0;
// }

// #include <iostream>
// #include<string>
// #include<unistd.h>
// using namespace std;
//
// #include <time.h>
//
//
// int main() {
// 	time_t t = time(0);
// 	char tmp[64] = {};
//
// 	while (1)
// 	{
// 		strftime(tmp, sizeof(tmp), "%Y-%m-%d %X - %S", localtime(&t));
// 		sleep(1);
// 		cout << tmp << endl;
// 	}
//
//
// }
#include <iostream>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

using namespace std;

int ticket_sum = 20;
pthread_mutex_t mutex_x = PTHREAD_MUTEX_INITIALIZER;//static init mutex

void *sell_ticket(void *arg)
{
	for (int i = 0; i < 20; i++)
	{
		pthread_mutex_lock(&mutex_x);//atomic opreation through mutex lock
		if (ticket_sum > 0)
		{
			sleep(1);
			cout << "sell the " << 20 - ticket_sum + 1 << "th" << endl;
			ticket_sum--;
		}
		pthread_mutex_unlock(&mutex_x);
	}
	return 0;
}

int main()
{
	int flag;
	pthread_t tids[4];

	for (int i = 0; i < 4; i++)
	{
		flag = pthread_create(&tids[i], NULL, &sell_ticket, NULL);
		if (flag)
		{
			cout << "pthread create error ,flag=" << flag << endl;
			return flag;
		}
	}

	sleep(20);
	void *ans;
	for (int i = 0; i < 4; i++)
	{
		flag = pthread_join(tids[i], &ans);
		if (flag)
		{
			cout << "tid=" << tids[i] << "join erro flag=" << flag << endl;
			return flag;
		}
		cout << "ans=" << ans << endl;
	}
	return 0;
}