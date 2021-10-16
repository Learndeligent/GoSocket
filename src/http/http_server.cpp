#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define LISTEN_QUEUE_SISE 10

int socketfd;

void signal_handler(int signo)
{
    printf("this serveice close\n");
    close(socketfd);
    exit(1);
}

void out_clientinfo(const struct sockaddr_in* outsockaddr)
{
    char ipstr[16];
    memset(ipstr, 0, sizeof(ipstr));
    // 将地址从网络字节序转换为点分十进制
    inet_ntop(AF_INET, &outsockaddr->sin_addr.s_addr, ipstr, sizeof(ipstr));
    
    printf("Connected by %s(%d), AF_INET = %d(ipv4=2)\n", ipstr, ntohs(outsockaddr->sin_port), outsockaddr->sin_family);
}

void dosomething(int fd)
{
    //获取系统当前时间
    long t = time(0);
    char* times = ctime(&t);
    size_t size = strlen(times)*sizeof(char);
    //将时间写回到客户端
    if(write(fd, times, size) != size)  // write这里要用到返回的客户端的socket
    {
        perror("write to client error");
    }
}

void send_html(int fd, char *filename)
{
	FILE *pfile = fopen(filename,"r");
	if (pfile == NULL)
	{
		printf("not found html locally.\n");
		return;
	}
	char temp[1024] = "";
	do
	{
		fgets(temp, 1024, pfile);
		send(fd, temp, strlen(temp), 0);
	} while (!feof(pfile));
}

int main(int argc, char const *argv[])
{
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        perror("signal error");
        exit(1);
    }
    
    // 1 sokect
    // AF_INET ipv4
    // SOCK_STREAM tcp
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        exit(1);
    }
    
    // 2 bind 绑定本地地址和端口
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;//ipv4
    serveraddr.sin_port = htons(SERVER_PORT); //端口
//    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//响应任意网卡的请求
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 设置ip地址
    // 这里的端口和ip只是服务器端需要监听的ip地址和端口
    // 因为一台计算机会存在多个ip地址以及多个网口
    if(bind(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind error");
        exit(1);
    }
    
    // out_clientinfo(&serveraddr); // server的ip地址为0.0.0.0，端口为8888
    // 0.0.0.0 表示本机上任何ip地址
    // 3 listen 启动监听 通知系统接受来自客户端的连接 准备好连接队列，也称为请求队列
    if(listen(socketfd, LISTEN_QUEUE_SISE) < 0)
    {
        perror("listen error");
        exit(1);
    }
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    while(1)
    {
        // 4 accept 从队列拿出第一个
        // clientaddr获取客户端的地址信息，是传出参数
        int clientfd = accept(socketfd, (struct sockaddr*)&clientaddr, &clientaddr_len);
        // 如果请求队列为空，那么accept则会阻塞
        if (clientfd  < 0)
        {
            perror("accept error");
            continue;
        }
        // 5 read/write
		char revdata[1024] = "";
		recv(clientfd, revdata , 1024, 0);
		printf("recv from client is: \n %s \n", revdata);

		char filename[] = "/Users/feihong/Workspace/git_repo/GoSocket/doc/index.html";
		// send_html(clientfd, filename);

        // out_clientinfo(&clientaddr); // 本地输出客户端的ip地址和端口
        dosomething(clientfd); // 发送给client字符串信息
        
        // printf("clientfd = %d, serverfd = %d\n", clientfd, socketfd);
        
        // 6 close
        close(clientfd);
    }
    
    // 6 close
    return 0;
}
