#include "system_environment.h"
#include "std_msg.h"
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>

int main(int argc, char const* argv[])
{
    if (argc != 2) exit(ERROR);
    struct MyInfo info;
    info.broad_port = atoi(argv[1]);
    info.server_port = find_unused_port();
    
    struct sockaddr_in broad_addr;
    info.broad_addr = &broad_addr;
    info.broad_sock = create_broadcast_socket(info.broad_port, &broad_addr);
    info.server_sock = create_server_socket(info.server_port);
    info.type = SUPPLIER;
    info.res_info = NULL;
    info.food_list = NULL;
    info.sup_list = generate_supply_list();

    info.sup_requests = (struct SupRequet*)malloc(sizeof(struct SupRequest));
    info.sup_requests->status = denied;

    run(&info);
}