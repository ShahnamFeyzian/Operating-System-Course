#ifndef STD_MSG_H
#define STD_MSG_H

#define RESTAURANT 'r'
#define CUSTOMER 'c'
#define SUPPLIER 's'

void set_std_msg(char type, char* name, int port, int command, char* param, char*final_msg);
char* node_type_to_str(char type);
int get_command(char* msg);
void set_headers(char* type, char* name, int* port, int* command, char* param, char* msg);
void get_name(char* name, char* msg);


#endif