#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include "socket_funcs.h"
#include "define_values.h"

int create_broadcast_socket(int port, struct sockaddr_in* bc_address)
{
    int sock_fd, broadcast=1, opt=1;
    
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == ERROR) return ERROR;

    if (setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == ERROR) return ERROR;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == ERROR) return ERROR;

    bc_address->sin_family = AF_INET;
    bc_address->sin_port = htons(port);
    bc_address->sin_addr.s_addr = inet_addr("255.255.255.255");
    // bc_address->sin_addr.s_addr = inet_addr("192.168.1.255");
    // bc_address->sin_addr.s_addr = INADDR_ANY;

    if (bind(sock_fd, (struct sockaddr*)bc_address, sizeof(*bc_address)) == ERROR) {printf("AAAA\n"); return ERROR;}

    return sock_fd;
}

int create_server_socket(int port)
{
    int sock_fd, opt = 1;
    struct sockaddr_in server_address;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == ERROR) return ERROR;

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == ERROR) return ERROR;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) return ERROR;

    if (listen(sock_fd, SERVER_SOCKET_QUEUE_SIZE) == ERROR) return ERROR;

    return sock_fd;
}

int connect_to_server (int port)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;

    if (sock_fd == ERROR) return ERROR;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == ERROR) return ERROR;

    return sock_fd;
}

int accept_client_connection(int server_fd)
{
    int client_fd;
    struct sockaddr_in client_address;
    int addr_len = sizeof(client_address);

    client_fd = accept(server_fd, (struct sockaddr*)&client_address, (socklen_t*)&addr_len);
    if (client_fd == ERROR) return ERROR;

    return client_fd; 
}

int close_socket(int sock_fd)
{
    if (close(sock_fd) == ERROR) return ERROR;
    else return 0; 
}

int find_unused_port() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // Bind to localhost
    addr.sin_port = 0;  // Let the system assign an available port
    
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        //perror("Socket binding failed");
        close(sockfd);
        return -1;
    }
    
    socklen_t addrLen = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *)&addr, &addrLen) < 0) {
        //perror("getsockname failed");
        close(sockfd);
        return -1;
    }
    
    int port = ntohs(addr.sin_port);
    close(sockfd);
    
    return port;
}