#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 8888
#define SERVER_IP 127.0.0.1

void out_clientinfo(const struct sockaddr_in* outsockaddr)
{
    char ipstr[16];
    memset(ipstr, 0, sizeof(ipstr));
    // 将地址从网络字节序转换为点分十进制
    inet_ntop(AF_INET, &outsockaddr->sin_addr.s_addr, ipstr, sizeof(ipstr));
    
    printf("Connected by %s(%d), AF_INET = %d(ipv4=2)\n", ipstr, ntohs(outsockaddr->sin_port), outsockaddr->sin_family);
}

int main(int argc, char const *argv[])
{
    //1 创建socket
    
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET: ipv4地址
    // SOCK_STREAM: 数据传输方式
    // 0: 表示TCP协议，1: 表示UDP协议
    if (socketfd < 0)
    {
        perror("socket error");
        exit(1);
    }
    
    //2 connect
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; // 服务器的ip地址类型是ipv4
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器的ip地址
    serveraddr.sin_port = htons(SERVER_PORT);  // 服务端的端口
    // 服务器的IP地址默认是0.0.0.0
    // 客户端的端口由操作系统分配
    // 客户端的IP地址默认是本地ip地址
    
    out_clientinfo(&serveraddr);
    
    if(connect(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0 )
    {
        perror("connect error");
        exit(1);
    }
    
    //3 read write
    char buf[1024];
    //read是阻塞函数 如果服务器没有下发消息，会一直阻塞在这里，知道收到消息。
    if (read(socketfd, buf, sizeof(buf)) > 0)
    {
        printf("%s",buf);
    }
    
    
    //4 close
    close(socketfd);
    return 0;
}
