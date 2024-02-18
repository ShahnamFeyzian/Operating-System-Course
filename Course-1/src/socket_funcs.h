#ifndef SOCKET_FUNCS_H
#define SOCKET_FUNCS_H

int create_broadcast_socket(int port, struct sockaddr_in* bc_address);
int create_server_socket(int port);
int connet_to_server(int port);
int accept_client_connection(int server_fd);
int close_socket(int sock_fd);
int find_unused_port();

#endif