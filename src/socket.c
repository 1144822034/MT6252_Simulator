#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define BUFFER_SIZE 8192
char buffer[BUFFER_SIZE];
char request[BUFFER_SIZE];

typedef struct socket_handle
{
    char ip[1024];
    char sendBuff[4096];
    char revBuff[4096];
    char port[5];
    char state;
    struct socket_handle *next;
    struct socket_handle *prev;
    char handle;
    WSADATA wsaData;
    SOCKET sock;
    UINT32 r;
    struct addrinfo hints;
    struct addrinfo *res;

} socketHandle;

socketHandle *socketHandleHead;

UINT32 get_host_ip_inner(char *host)
{
    WSADATA wsaData;
    SOCKET sock;
    UINT32 r;
    // 替换host
    if (!strcmp(host, "spd.skymobiapp.com") || !strcmp(host, "applist2.sky-mobi.net"))
    {
        host = "43.136.107.137";
    }
    // 解析主机名为 IP 地址
    struct addrinfo hints, *res;
    // 初始化 Winsock
    r = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (r != 0)
    {
        printf("WSAStartup failed: %d\n", r);
        return 1;
    }
    // socket测试
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("Socket creation failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // 流套接字
    hints.ai_protocol = IPPROTO_TCP; // TCP 协议                                     // 解析主机名为 IP 地址

    r = getaddrinfo(host, "80", &hints, &res);
    closesocket(sock);
    WSACleanup();
    freeaddrinfo(res); // 释放地址信息
    if (r != 0)
    {
        printf("getHost(%s)Failed\n", host);
        return 0;
    }
    struct in_addr *addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    printf("getHost(%s)(%s)\n", host, inet_ntoa(*addr));
    UINT32 ip = addr->S_un.S_addr; // 如果ip地址是1.2.3.4，那么这里存的的0x04030201
    return (ip << 24) | (((ip >> 8) & 0xff) << 16) | (((ip >> 16) & 0xff) << 8) | (ip >> 24);
}

// 连接到socket
void connect_socket_inner(socketHandle *handle)
{
    WSADATA wsaData = handle->wsaData;
    UINT32 r = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (r != 0)
    {
        printf("WSAStartup failed: %d\n", r);
        return 0;
    }
    handle->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (handle->sock == INVALID_SOCKET)
    {
        printf("Socket creation failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return 0;
    }

    memset(&handle->hints, 0, sizeof(handle->hints));
    handle->hints.ai_family = AF_INET;       // IPv4
    handle->hints.ai_socktype = SOCK_STREAM; // 流套接字
    handle->hints.ai_protocol = IPPROTO_TCP; // TCP 协议
                                             // 解析主机名为 IP 地址
    r = getaddrinfo(handle->ip, handle->port, &handle->hints, &handle->res);
    if (r != 0)
    {
        printf("getaddrinfo failed: %d\n", r);
        closesocket(handle->sock);
        WSACleanup();
        return 0;
    }
    // 释放地址信息
    freeaddrinfo(handle->res);
    // 连接到服务器
    r = connect(handle->sock, handle->res->ai_addr, (int)handle->res->ai_addrlen);
    if (r == SOCKET_ERROR)
    {
        printf("Connection failed: %ld\n", WSAGetLastError());
        closesocket(handle->sock);
        freeaddrinfo(handle->res);
        WSACleanup();
        return 0;
    }
    return 1; // 连接成功
}

// 重定向ip与port
void redirect_socket_inner(socketHandle *handle)
{
    closesocket(handle->sock);
    memset(&handle->hints, 0, sizeof(handle->hints));
    handle->hints.ai_family = AF_INET;       // IPv4
    handle->hints.ai_socktype = SOCK_STREAM; // 流套接字
    handle->hints.ai_protocol = IPPROTO_TCP; // TCP 协议
                                             // 解析主机名为 IP 地址
    UINT32 r = getaddrinfo(handle->ip, handle->port, &handle->hints, &handle->res);
    if (r != 0)
    {
        printf("getaddrinfo failed: %d\n", r);
        closesocket(handle->sock);
        WSACleanup();
        return 0;
    }
    // 连接到服务器
    r = connect(handle->sock, handle->res->ai_addr, (int)handle->res->ai_addrlen);
    if (r == SOCKET_ERROR)
    {
        printf("Connection failed: %ld\n", WSAGetLastError());
        closesocket(handle->sock);
        freeaddrinfo(handle->res);
        WSACleanup();
        return 0;
    }
    return 1;
}

/**
 * 返回已发送的字节数,-1表示失败
 */
int send_socket_inner(socketHandle *handle, int len)
{

    int r = send(handle->sock, &(handle->sendBuff), len, 0);
    if (r == SOCKET_ERROR)
    {
        printf("send failed: %ld\n", WSAGetLastError());
        closesocket(handle->sock);
        WSACleanup();
        return r;
    }
    return r;
}

int receive_socket_inner(socketHandle *handle, int len)
{
    return recv(handle->sock, &(handle->revBuff), len, 0);
}

void close_socket_inner(socketHandle *handle)
{
    // 关闭套接字
    closesocket(handle->sock);
    WSACleanup();
}