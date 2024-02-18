#ifndef SYSTEM_ENVIRONMENT_H
#define SYSTEM_ENVIRONMENT_H

#include "recipe.h"

#define CHECK_NAME 1
#define OPEN 2 
#define CLOSE 3
#define SHOW_RESTAURANTS 4
#define GIVE_INFO 5
#define SHOW_SUPPLIER 6
#define REQUEST_INGREDIENT 7
#define BUSY 8
#define ACCEPT 9
#define DENI 10
#define ORDER_FOOD 11

#define OPEN_COMMAD "start working"
#define CLOSE_COMMAD "break"
#define SHOW_RESTAURANT_COMMAND "show restaurants"
#define SHOW_MENU_COMMAND "show menu"
#define SHOW_RECIPES_COMMAND "show recipes"
#define SHOW_INGREDIENTS_COMMAD "show ingredients"
#define SHOW_SUPPLIER_COMMAND "show suppliers"
#define REQUEST_INGREDIENT_COMMAND "request ingredient"
#define ANSWER_REQUEST_COMMAND "answer request"
#define ORDER_FOOD_COMMAND "order food"
#define SHOW_ACTIVE_ORDERS_COMMAND "show requests list"
#define SHOW_SALES_HISTORY_COMMAND "show sales history"

#define FAIL_CONN_MSG "connection failed\n"
#define NEW_ORDERR_MSG "new order\n"
#define INVALID_COMMAND_MSG "Invalid command!!!\n"

enum RequestStatus{wake, accepted, denied, time_out};
#define WAKE_STAT_STR "wake"
#define ACCEPTED_STAT_STR "accepted"
#define DENIED_STAT_STR "denied"
#define TIMEOUT_STAT_STR "time out"

struct SupRequest
{
    int client_port;
    char client_name[BUF_SIZE];
    int socket;
    char ing_name[BUF_SIZE];
    int ing_amount;
    enum RequestStatus status;
};

struct ResRequest
{
    int cus_port;
    char cus_name[BUF_SIZE];
    int socket;
    char food_name[BUF_SIZE];
    enum RequestStatus status;
    struct ResRequest* next;
};

struct RestaurantInfo
{
    int is_open;
    struct ResRequest* orders;
};

struct MyInfo
{
    char name[BUF_SIZE];
    char type;
    int broad_port;
    int server_port;
    int broad_sock;
    int server_sock;
    struct sockaddr_in* broad_addr;
    struct RestaurantInfo* res_info;
    struct Food* food_list;
    struct Supply* sup_list;
    struct SupRequest* sup_requests;
};

struct OthInfo
{
    char name[BUF_SIZE];
    int port;
};

void run(struct MyInfo* info);
void enter_to_system(struct MyInfo* info);
void set_headers(char* type, char* name, int* port, int* command, char* param, char* msg);
void checking_name_equality(struct MyInfo* info, const char* name_in, int sender_port);
int max(int a, int b);
void cli_handler(char* input_str, struct MyInfo* info);
void broadcast_handler(char* msg, struct MyInfo* info);
void request_handler(char* msg, struct MyInfo* info, int client_sock);
void get_and_show_addresses(struct MyInfo* info);
void show_menu_for_cus(struct Food* food_head);
void show_menu_for_res(struct Food* food_head);
void show_supply(struct Supply* sup_head);
void send_info(int server_port, struct MyInfo* info);
void request_supply(struct MyInfo* info);
void order_food(struct MyInfo* info);
void add_to_order_list(struct ResRequest* new, struct ResRequest* head);
struct ResRequest* find_order_by_sock(int sock, struct ResRequest* list);
int is_there_any_active_order(struct ResRequest* head);
void show_active_orders(struct ResRequest* list);
void answer_order(struct MyInfo* info);
struct ResRequest* find_order_by_port(int port, struct ResRequest* list);
void show_sales_history(struct ResRequest* list);
char* status_to_str(enum RequestStatus status);
int is_supply_enough(struct Food* order, struct Supply* list);
void reduce_supplies(struct Food* order, struct Supply* list);

void time_out_func();
void do_nothing();
void invalid_command();


#endif