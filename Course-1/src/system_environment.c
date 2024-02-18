#include <signal.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include "system_environment.h"
#include "std_msg.h"
#include "loger.h"
#include "recipe.h"
#include "socket_funcs.h"

void run(struct MyInfo* info)
{
    char buf[BUF_SIZE];

    fd_set master_set, working_set;

    FD_ZERO(&master_set);
    FD_SET(STDIN_FD, &master_set);
    FD_SET(info->broad_sock, &master_set);
    FD_SET(info->server_sock, &master_set);
    int max_sd = max(info->broad_sock, info->server_sock) + 1;

    enter_to_system(info);

    while(1)
    {
        working_set = master_set;
        select(max_sd+1, &working_set,  NULL, NULL, NULL);

        for(int i=0; i<=max_sd; i++)
        {
            if(FD_ISSET(i, &working_set))
            {
                if(i == STDIN_FD)
                {
                    int n = read(STDIN_FD, buf, BUF_SIZE-1);
                    buf[n-1] = '\0';
                    cli_handler(buf, info);
                }
                else if(i == info->broad_sock)
                {
                    int n = recv(info->broad_sock, buf, BUF_SIZE-1, 0);
                    buf[n] = '\0';
                    broadcast_handler(buf, info);
                }
                else if(i == info->server_sock)
                {
                    int new_sock = accept_client_connection(info->server_sock);
                    max_sd = (new_sock > max_sd) ? new_sock : max_sd;
                    FD_SET(new_sock, &master_set);
                }
                else
                {
                    int n = recv(i, buf, BUF_SIZE-1, 0);
                    buf[n] = '\0';
                    if(n == 0)
                    {
                        close_socket(i);
                        FD_CLR(i, &master_set);
                    }
                    request_handler(buf, info, i);
                }
            }
        }
    }
}

void enter_to_system(struct MyInfo* info)
{
    char name[BUF_SIZE];
    char msg[BUF_SIZE]; 
    write(STDOUT_FD, "Please enter your name : ", strlen("Please enter your name : "));
    int n = read(STDIN_FD, name, BUF_SIZE);
    name[n-1] = '\0';
    set_std_msg(info->type, name, info->server_port, CHECK_NAME, name, msg);
    sendto(info->broad_sock, msg, strlen(msg), 0, (struct sockaddr*)info->broad_addr, sizeof(*(info->broad_addr)));
    write(STDOUT_FD, "Waiting for checking your name...\n", strlen("Waiting for checking your name...\n"));

    signal(SIGALRM, do_nothing);
    siginterrupt(SIGALRM, 1);
    alarm(2);
    n = accept_client_connection(info->server_sock);
    alarm(0);

    if(n == -1) 
    {
        strcpy(info->name, name);
        strcpy(name, node_type_to_str(info->type));
        sprintf(msg, "Wellcome %s as %s!!\nyour port is %d\n", info->name, name, info->server_port);
        write(STDOUT_FD, msg, strlen(msg));
        creating_log_file(info->name);
        log_msg(info->name, "Log in");
        if(info->type == RESTAURANT)
        {
            info->res_info->is_open = TRUE;
            char msg[BUF_SIZE];
            set_std_msg(info->type, info->name, info->server_port, OPEN, info->name, msg);
            sendto(info->broad_sock, msg, strlen(msg), 0, (struct sockaddr*)info->broad_addr, sizeof(*(info->broad_addr)));
            log_msg(info->name, "open");
        }
    }
    else
    {
        char c;
        int sender_port, command;
        recv(n, msg, BUF_SIZE, 0);
        set_headers(&c, name, &sender_port, &command, name, msg);
        sprintf(msg, "The port %d node is already registered with the name %s, try again\n", sender_port, name);
        write(STDOUT_FD, msg, strlen(msg));
        close_socket(n);
        enter_to_system(info);
    }
}

void checking_name_equality(struct MyInfo* info, const char* name_in, int sender_port)
{
    if(strcmp(info->name, name_in) == 0) 
    {
        char msg[BUF_SIZE];
        set_std_msg(info->type, info->name, info->server_port, CHECK_NAME, info->name, msg);
        
        signal(SIGALRM, do_nothing);
        siginterrupt(SIGALRM, 1);
        alarm(2);
        int new_sock = connect_to_server(sender_port);
        alarm(0);
        if(new_sock == ERROR) return;
        
        send(new_sock, msg, strlen(msg), 0);
        close_socket(new_sock);
    }
}

int max(int a, int b)
{
    if(a > b) return a;
    else return b;
}

void cli_handler(char* input_str, struct MyInfo* info)
{
    char msg[BUF_SIZE];
    if(info->type == RESTAURANT)
    {
        if(info->res_info->is_open == FALSE && strcmp(input_str, OPEN_COMMAD) != 0)
        {
            write(STDOUT_FD, "The restaurant is closed!!\n", strlen("The restaurant is closed!!\n"));
        }
        else if(info->res_info->is_open == TRUE && strcmp(input_str, OPEN_COMMAD) == 0)
        {
            write(STDOUT_FD, "The restaurant is already open!!\n", strlen("The restaurant is already open!!\n"));
        }
        else if(strcmp(input_str, CLOSE_COMMAD) == 0)
        {
            if(is_there_any_active_order(info->res_info->orders) == TRUE)
            {
                write(STDOUT_FD, "There are some active order(s)!!\n", strlen("There are some active order(s)!!\n"));
                return;
            }

            info->res_info->is_open = FALSE;
            set_std_msg(info->type, info->name, info->server_port, CLOSE, info->name, msg);
            sendto(info->broad_sock, msg, strlen(msg), 0, (struct sockaddr*)info->broad_addr, sizeof(*(info->broad_addr)));
            log_msg(info->name, "close");
        }
        else if(strcmp(input_str, OPEN_COMMAD) == 0)
        {
            info->res_info->is_open = TRUE;
            set_std_msg(info->type, info->name, info->server_port, OPEN, info->name, msg);
            sendto(info->broad_sock, msg, strlen(msg), 0, (struct sockaddr*)info->broad_addr, sizeof(*(info->broad_addr)));
            log_msg(info->name, "open");
        }
        else if(strcmp(input_str, SHOW_RECIPES_COMMAND) == 0)
        {
            show_menu_for_res(info->food_list);
        }
        else if(strcmp(input_str, SHOW_INGREDIENTS_COMMAD) == 0)
        {
            show_supply(info->sup_list);
        }
        else if(strcmp(input_str, SHOW_SUPPLIER_COMMAND) == 0)
        {
            set_std_msg(info->type, info->name, info->server_port, SHOW_SUPPLIER, info->name, msg);
            sendto(info->broad_sock, msg, strlen(msg), 0, (struct sockaddr*)info->broad_addr, sizeof(*(info->broad_addr)));
            get_and_show_addresses(info);
        }
        else if(strcmp(input_str, REQUEST_INGREDIENT_COMMAND) == 0)
        {
            request_supply(info);
        }
        else if(strcmp(input_str, SHOW_ACTIVE_ORDERS_COMMAND) == 0)
        {
            show_active_orders(info->res_info->orders);
        }
        else if(strcmp(input_str, ANSWER_REQUEST_COMMAND) == 0)
        {
            answer_order(info);
        }
        else if(strcmp(input_str, SHOW_SALES_HISTORY_COMMAND) == 0)
        {
            show_sales_history(info->res_info->orders);
        }
        else
        {
            invalid_command();
        }
    }
    else if(info->type == CUSTOMER)
    {
        if(strcmp(input_str, SHOW_RESTAURANT_COMMAND) == 0)
        {
            set_std_msg(info->type, info->name, info->server_port, SHOW_RESTAURANTS, "", msg);
            sendto(info->broad_sock, msg, strlen(msg), 0, (struct sockaddr*)info->broad_addr, sizeof(*(info->broad_addr)));
            get_and_show_addresses(info);
        }
        else if(strcmp(input_str, SHOW_MENU_COMMAND) == 0)
        {
            show_menu_for_cus(info->food_list);
        }
        else if(strcmp(input_str, ORDER_FOOD_COMMAND) == 0)
        {
            order_food(info);
        }
        else
        {
            invalid_command();
        }
    }
    else if(info->type == SUPPLIER)
    {
        if(strcmp(input_str, ANSWER_REQUEST_COMMAND) == 0)
        {
            if(info->sup_requests->status != wake)
            {
                write(STDOUT_FD, "There is no request!!\n", strlen("There is no request!!\n"));
                return;
            }
            char buf[BUF_SIZE];
            sprintf(buf, "The restaurant %s wants (%s : %d), do you agree (yes/no)? ", info->sup_requests->client_name, info->sup_requests->ing_name, info->sup_requests->ing_amount);
            write(STDOUT_FD, buf, strlen(buf));
            int n = read(STDIN_FD, buf, BUF_SIZE);
            buf[n-1] = '\0';
            if(strcmp(buf, "yes") == 0)
            {
                set_std_msg(info->type, info->name, info->server_port, ACCEPT, "", buf);
                send(info->sup_requests->socket, buf, strlen(buf), 0);
                info->sup_requests->status = accepted;
                log_msg(info->name, "accept request");
            }
            else
            {
                set_std_msg(info->type, info->name, info->server_port, DENI, "", buf);
                send(info->sup_requests->socket, buf, strlen(buf), 0);
                info->sup_requests->status = denied;
                log_msg(info->name, "deni request");
            }
        }
        else
        {
            invalid_command();
        }
    }
}

void broadcast_handler(char* msg, struct MyInfo* info)
{
    int sender_port, command;
    char sender_type;
    char param[BUF_SIZE];
    char sender_name[BUF_SIZE];
    set_headers(&sender_type, sender_name, &sender_port, &command, param,  msg);
    if(info->server_port == sender_port) return;

    if(command == CHECK_NAME)
    {
        checking_name_equality(info, param, sender_port);
    }
    else if(command == OPEN)
    {
        if(info->type == CUSTOMER) 
        {
            char buf[BUF_SIZE];
            sprintf(buf, "%s %s opened!\n", param, node_type_to_str(RESTAURANT));
            write(STDOUT_FD, buf, strlen(buf));
        }
    }
    else if(command == CLOSE)
    {
        if(info->type == CUSTOMER) 
        {
            char buf[BUF_SIZE];
            sprintf(buf, "%s %s closed!\n", param, node_type_to_str(RESTAURANT));
            write(STDOUT_FD, buf, strlen(buf));
        }
    }
    else if(command == SHOW_RESTAURANTS)
    {
        if(info->type == RESTAURANT) send_info(sender_port, info);
    }
    else if(command == SHOW_SUPPLIER)
    {
        if(info->type == SUPPLIER) send_info(sender_port, info);
    }
}

void request_handler(char* msg, struct MyInfo* info, int client_sock)
{
    if(strlen(msg) == 0)
    {
        if(info->type == SUPPLIER)
        {
            info->sup_requests->status = time_out;
            return;
        }
        else if(info->type == RESTAURANT)
        {
            struct ResRequest* order = find_order_by_sock(client_sock, info->res_info->orders);
            if(order != NULL) order->status = time_out;
            return;
        }
        else if(info->type == CUSTOMER)
        {
            return;
        }
    }


    int sender_port, command;
    char sender_type;
    char param[BUF_SIZE];
    char sender_name[BUF_SIZE];
    set_headers(&sender_type, sender_name, &sender_port, &command, param,  msg);

    if(info->type == SUPPLIER)
    {
        if(command == REQUEST_INGREDIENT)
        {
            char buf[BUF_SIZE];
            
            if(info->sup_requests->status == wake)
            {
                set_std_msg(info->type, info->name, info->server_port, BUSY, "", buf);
                send(client_sock, buf, strlen(buf), 0);
                return;
            }
            
            info->sup_requests->client_port = sender_port;
            info->sup_requests->socket = client_sock;
            info->sup_requests->status = wake;
            strcpy(info->sup_requests->client_name, sender_name);
            
            int i;
            for(i=0; param[i] != ' '; i++)
            {
                buf[i] = param[i];
            }
            buf[i++] = '\0';
            strcpy(info->sup_requests->ing_name, buf);

            int j;
            for(j=0; param[i] != '\0'; j++)
            {
                buf[j] = param[i++];
            }
            buf[j] = '\0';
            info->sup_requests->ing_amount = atoi(buf);

            write(STDOUT_FD, NEW_ORDERR_MSG, strlen(NEW_ORDERR_MSG));
            log_msg(info->name, "new request");
        }
    }
    else if(info->type == RESTAURANT)
    {
        if(command == ORDER_FOOD)
        {
            if(info->res_info->is_open == FALSE)
            {
                char buf[BUF_SIZE];
                set_std_msg(info->type, info->name, info->server_port, BUSY, "", buf);
                send(client_sock, buf, strlen(buf), 0);
                return;
            }
            write(STDOUT_FD, NEW_ORDERR_MSG, strlen(NEW_ORDERR_MSG));
            struct ResRequest* new_order = (struct ResRequest*)malloc(sizeof(struct ResRequest));
            strcpy(new_order->cus_name, sender_name);
            new_order->cus_port = sender_port;
            strcpy(new_order->food_name, param);
            new_order->socket = client_sock;
            new_order->status = wake;
            new_order->next = NULL;
            add_to_order_list(new_order, info->res_info->orders);
            log_msg(info->name, "new order");
        }
    }
}

void get_and_show_addresses(struct MyInfo* info)
{
    char buf[BUF_SIZE][BUF_SIZE];
    char sender_type;
    int command, i=0;

    signal(SIGALRM, do_nothing);
    siginterrupt(SIGALRM, 1);
    alarm(2);
    
    write(STDOUT_FD, "\nGetting informations, please waite...\n", strlen("\nGetting informations, please waite...\n"));
    while(1)
    {
        int new_sock = accept_client_connection(info->server_sock);
        if(new_sock == ERROR) break;

        int n = recv(new_sock, buf[i], BUF_SIZE, 0);
        if(n == ERROR) 
        {
            close_socket(new_sock); 
            break;
        }

        i++;
        close_socket(new_sock);
    }
    
    write(STDOUT_FD, "--------------------------------\n", strlen("--------------------------------\n"));
    write(STDOUT_FD, "Name   /   Port\n\n", strlen("Name   /   Port\n\n"));
    for(int j=0; j<i; j++)
    {
        char msg[BUF_SIZE];
        char param[BUF_SIZE];
        char name[BUF_SIZE];
        int sender_port;
        set_headers(&sender_type, name, &sender_port, &command, param,  buf[j]);
        sprintf(msg, "%s / %d\n", param, sender_port);
        write(STDOUT_FD, msg, strlen(msg));
        memset(param, '\0', BUF_SIZE);
    }
    write(STDOUT_FD, "--------------------------------\n\n", strlen("--------------------------------\n\n"));
}

void show_menu_for_cus(struct Food* food_head)
{
    struct Food* iterator = food_head->next;

    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));
    for(int i=1; iterator != NULL; i++)
    {
        char buf[BUF_SIZE];
        sprintf(buf, "%d- %s\n", i, iterator->name);
        write(STDOUT_FD, buf, strlen(buf));
        iterator = iterator->next;
    }
    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));

}

void show_menu_for_res(struct Food* food_head)
{
    struct Food* iterator = food_head->next;
    struct Supply* iterator1;

    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));
    for(int i=1; iterator != NULL; i++)
    {
        char temp[BUF_SIZE];
        iterator1 = iterator->supply_list->next;

        sprintf(temp, "%d- %s:\n", i, iterator->name);
        write(STDOUT_FD, temp, strlen(temp));
        
        while(iterator1 != NULL)
        {
            sprintf(temp, "    %s : %d\n", iterator1->name, iterator1->amount);
            write(STDOUT_FD, temp, strlen(temp));
            iterator1 = iterator1->next;
        }
        write(STDOUT_FD, "\n", 1);
        iterator = iterator->next;
    }
    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));
}

void show_supply(struct Supply* sup_head)
{
    struct Supply* iterator = sup_head;

    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));
    int count = 0;
    for(int i=1; iterator->next != NULL; i++)
    {
        char buf[BUF_SIZE];
        iterator = iterator->next;
        if (iterator->amount == 0) continue;
        count++;
        sprintf(buf, "%s : %d\n", iterator->name, iterator->amount);
        write(STDOUT_FD, buf, strlen(buf));
    }
    if(count == 0) write(STDOUT_FD, "Empty!!!\n", strlen("Empty!!!\n"));   
    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));

}

void send_info(int server_port, struct MyInfo* info)
{
    signal(SIGALRM, do_nothing);
    siginterrupt(SIGALRM, 1);
    alarm(2);
    int new_sock = connect_to_server(server_port);
    alarm(0);
    if(new_sock == ERROR) return;

    char buf[BUF_SIZE];
    set_std_msg(info->type, info->name, info->server_port, GIVE_INFO, info->name, buf);
    send(new_sock, buf, strlen(buf), 0);
    close_socket(new_sock);   
}

void request_supply(struct MyInfo* info)
{
    char buf[BUF_SIZE];
    char sup_name[BUF_SIZE];
    char msg[BUF_SIZE];
    char params[BUF_SIZE];
    int sup_port, sup_num;
    
    write(STDOUT_FD, "port of supplier : ", strlen("port of supplier : "));
    read(STDIN_FD, buf, BUF_SIZE);
    sup_port = atoi(buf);

    write(STDOUT_FD, "name of ingredient : ", strlen("name of ingredient : "));
    int n = read(STDIN_FD, sup_name, BUF_SIZE);
    sup_name[n-1] = '\0';

    write(STDOUT_FD, "number of ingredient : ", strlen("number of ingredient : "));
    read(STDIN_FD, buf, BUF_SIZE);
    sup_num = atoi(buf);

    if(is_supply_exist(sup_name, info->sup_list) == FALSE)
    {
        write(STDOUT_FD, "This ingredient dosn't exist!!\n", strlen("This ingredient dosn't exist!!\n"));
        return;
    }

    signal(SIGALRM, do_nothing);
    siginterrupt(SIGALRM, 1);
    alarm(2);
    int new_sock = connect_to_server(sup_port);
    alarm(0);
    if(new_sock == ERROR) 
    {
        write(STDOUT_FD, FAIL_CONN_MSG, strlen(FAIL_CONN_MSG));
        return;
    }

    sprintf(params, "%s %d", sup_name, sup_num);
    set_std_msg(info->type, info->name, info->server_port, REQUEST_INGREDIENT, params, msg);
    send(new_sock, msg, strlen(msg), 0);
    log_msg(info->name, "request ingredient");

    write(STDOUT_FD, "Waiting for supplier's response...\n", strlen("Waiting for supplier's response...\n"));
    
    signal(SIGALRM, time_out_func);
    siginterrupt(SIGALRM, 1);
    alarm(90);
    n = recv(new_sock, buf, BUF_SIZE, 0);
    alarm(0);
    
    if(n != ERROR)
    {
        char name[BUF_SIZE];
        int command = get_command(buf);
        get_name(name, buf);
        
        if(command == BUSY)
        {
            sprintf(buf, "The supplier %s is busy right now try later!\n", name);
            log_msg(info->name, "supplier was busy");
        }
        else if(command == DENI)
        {
            sprintf(buf, "The supplier %s denied your request\n", name);
            log_msg(info->name, "request was denied");
        }
        else if(command == ACCEPT)
        {
            sprintf(buf, "The supplier %s accepted your request\n", name);
            find_supply(sup_name, info->sup_list)->amount += sup_num;
            log_msg(info->name, "request was accepted");
        }

        write(STDOUT_FD, buf, strlen(buf));
    }
    else log_msg(info->name, "request has expired");
    close_socket(new_sock);
} 

void order_food(struct MyInfo* info)
{
    char buf[BUF_SIZE];
    char food_name[BUF_SIZE];
    int res_port;
    
    write(STDOUT_FD, "Name of food : ", strlen("Name of food : "));
    int n = read(STDIN_FD, food_name, BUF_SIZE);
    food_name[n-1] = '\0';

    write(STDOUT_FD, "Port of restaurant : ", strlen("Port of restaurant : "));
    read(STDIN_FD, buf, BUF_SIZE);
    res_port = atoi(buf);

    if(is_food_exist(food_name, info->food_list) == FALSE)
    {
        write(STDOUT_FD, "This food dosn't exist!!\n", strlen("This food dosn't exist!!\n"));
        return;
    }

    signal(SIGALRM, do_nothing);
    siginterrupt(SIGALRM, 1);
    alarm(2);
    int new_sock = connect_to_server(res_port);
    alarm(0);
    if(new_sock == ERROR) 
    {
        write(STDOUT_FD, FAIL_CONN_MSG, strlen(FAIL_CONN_MSG));
        return;
    }

    set_std_msg(info->type, info->name, info->server_port, ORDER_FOOD, food_name, buf);
    send(new_sock, buf, strlen(buf), 0);

    write(STDOUT_FD, "Waiting for restaurant's response...\n", strlen("Waiting for restaurants's response...\n"));
    log_msg(info->name, "order some food");


    signal(SIGALRM, time_out_func);
    siginterrupt(SIGALRM, 1);
    alarm(120);
    n = recv(new_sock, buf, BUF_SIZE, 0);
    alarm(0);

    if(n != ERROR)
    {
        char name[BUF_SIZE];
        int command = get_command(buf);
        get_name(name, buf);
        
        if(command == DENI)
        {
            sprintf(buf, "The restaurant %s denied your request\n", name);
            log_msg(info->name, "order was denied");
        }
        else if(command == ACCEPT)
        {
            sprintf(buf, "The restaurant %s accepted your request\n", name);
            log_msg(info->name, "order was accepted");
        }
        else if(command == BUSY)
        {
            sprintf(buf, "The restaurant %s is close right now, try later\n", name);
            log_msg(info->name, "restaurant was closed");
        }

        write(STDOUT_FD, buf, strlen(buf));
    }
    else log_msg(info->name, "order has expired");

    close_socket(new_sock);
}

void add_to_order_list(struct ResRequest* new, struct ResRequest* head)
{
    struct ResRequest* iterator = head;

    while(iterator->next != NULL) iterator = iterator->next;

    iterator->next = new;
}

struct ResRequest* find_order_by_sock(int sock, struct ResRequest* list)
{
    struct ResRequest* iterator = list->next;

    while(iterator != NULL)
    {
        if(iterator->socket == sock && iterator->status == wake) break;
        iterator = iterator->next;
    }

    return iterator;
}

struct ResRequest* find_order_by_port(int port, struct ResRequest* list)
{
    struct ResRequest* iterator = list->next;

    while(iterator != NULL)
    {
        if(iterator->cus_port == port && iterator->status == wake) break;
        iterator = iterator->next;
    }

    return iterator;
}


int is_there_any_active_order(struct ResRequest* head)
{
    struct ResRequest* iterator = head->next;

    while(iterator != NULL)
    {
        if(iterator->status == wake) return TRUE;
        iterator = iterator->next;
    }

    return FALSE;
}

void show_active_orders(struct ResRequest* list)
{
    struct ResRequest* iterator = list->next;

    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));
    write(STDOUT_FD, "username / port / order\n", strlen("username / port / order\n"));
    while(iterator != NULL)
    {
        if(iterator->status != wake)
        {
            iterator = iterator->next;
            continue;
        }

        char buf[BUF_SIZE];
        sprintf(buf, "%s / %d / %s\n", iterator->cus_name, iterator->cus_port, iterator->food_name);
        write(STDOUT_FD, buf, strlen(buf));
        iterator = iterator->next;
    }
    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));
}

void answer_order(struct MyInfo* info)
{
    char buf[BUF_SIZE];
    write(STDOUT_FD, "Port of request: ", strlen("port of request: "));
    read(STDIN_FD, buf, BUF_SIZE);
    int req_port = atoi(buf);

    write(STDOUT_FD, "Your answer (yes/no): ", strlen("Your answer (yes/no): "));
    int n = read(STDIN_FD, buf, BUF_SIZE);
    buf[n-1] = '\0';

    struct ResRequest* order = find_order_by_port(req_port, info->res_info->orders);
    if(order == NULL)
    {
        write(STDOUT_FD, "There is no active order with this port!!\n", strlen("There is no active order with this port!!\n"));
        return;
    }

    if(strcmp(buf, "yes") == 0)
    {
        if(is_supply_enough(find_food(order->food_name, info->food_list), info->sup_list) == FALSE)
        {
            write(STDOUT_FD, "There is not enough supply!!!\n", strlen("There is not enough supply!!!\n"));
            return;
        }
        else 
        {
            reduce_supplies(find_food(order->food_name, info->food_list), info->sup_list);
        }
        
        set_std_msg(info->type, info->name, info->server_port, ACCEPT, "", buf);
        order->status = accepted;
        log_msg(info->name, "accept order");
    }
    else
    {
        set_std_msg(info->type, info->name, info->server_port, DENI, "", buf);
        order->status = denied;
        log_msg(info->name, "denie order");
    }

    send(order->socket, buf, strlen(buf), 0);
}

void show_sales_history(struct ResRequest* list)
{
    struct ResRequest* iterator = list->next;

    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));
    write(STDOUT_FD, "username / order / result\n", strlen("username / order / result\n"));   
    while(iterator != NULL)
    {
        if(iterator->status == wake)
        {
            iterator = iterator->next;
            continue;
        }

        char buf[BUF_SIZE];
        sprintf(buf, "%s / %s / %s\n", iterator->cus_name, iterator->food_name, status_to_str(iterator->status));
        write(STDOUT_FD, buf, strlen(buf));
        iterator = iterator->next;
    }
    write(STDOUT_FD, "-------------------------------\n", strlen("-------------------------------\n"));
}

int is_supply_enough(struct Food* order, struct Supply* list)
{
    if(order == NULL || list == NULL || order->supply_list == NULL) return TRUE;
    
    struct Supply* food_sup = order->supply_list->next;

    while(food_sup != NULL)
    {
        struct Supply* s = find_supply(food_sup->name, list);
        if(s == NULL) continue;
        if(food_sup->amount > s->amount) return FALSE;
        food_sup = food_sup->next;
    }  

    return TRUE;
}

void reduce_supplies(struct Food* order, struct Supply* list)
{
    if(order == NULL || list == NULL || order->supply_list == NULL) return;

    struct Supply* food_sup = order->supply_list->next;

    while(food_sup != NULL)
    {
        struct Supply* s = find_supply(food_sup->name, list);
        if(s == NULL) continue;
        s->amount -= food_sup->amount;
        food_sup = food_sup->next;
    }  
}

void do_nothing() {}

void time_out_func()
{
    write(STDOUT_FD, "Time out!!!\n", strlen("Time out!!!\n"));
}

void invalid_command()
{
    write(STDOUT_FD, INVALID_COMMAND_MSG, strlen(INVALID_COMMAND_MSG));
}

char* status_to_str(enum RequestStatus status)
{
    char* status_str[BUF_SIZE];

    switch (status)
    {
        case wake : strcpy(status_str, WAKE_STAT_STR); break;
        case accepted : strcpy(status_str, ACCEPTED_STAT_STR); break;
        case denied : strcpy(status_str, DENIED_STAT_STR); break;
        case time_out : strcpy(status_str, TIMEOUT_STAT_STR); break;
    }
}