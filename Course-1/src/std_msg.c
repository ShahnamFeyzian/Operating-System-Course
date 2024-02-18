#include <stdio.h>
#include <string.h>
#include "std_msg.h"
#include "define_values.h"

void set_std_msg(char type, char* name, int port, int command, char* param, char*final_msg)
{ 
    sprintf(final_msg, "%c %s %d %d %s", type, name, port, command, param);
}

int get_command(char* msg)
{
    int i=0, j=0;
    char buf[BUF_SIZE];
    while (j != 3)
    {
        if(msg[i] == ' ') j++;
        i++;
    }
    j=0;
    while(msg[i] != ' ')
    {
        buf[j] = msg[i];
        i++;
        j++;
    }
    buf[j] = '\0';

    return atoi(buf);
}

void get_name(char* name, char* msg)
{
    int i;
    for(i=2; msg[i] != ' '; i++)
    {
        name[i-2] = msg[i];
    }
    name[i-2] = '\0';
}

void set_headers(char* type, char* name, int* port, int* command, char* param, char* msg)
{
    char port_str[BUF_SIZE];
    char command_str[BUF_SIZE];
    *type = msg[0];
    int i=2, j=0;
    while(msg[i] != ' ')
    {
        name[j] = msg[i];
        i++;
        j++;
    }
    name[j] = '\0';
    j=0;
    i++;
    while(msg[i] != ' ')
    {
        port_str[j] = msg[i];
        i++;
        j++;
    }
    port_str[j] = '\0';
    j=0;
    i++;
    *port = atoi(port_str);
    while(msg[i] != ' ')
    {
        command_str[j] = msg[i];
        i++;
        j++;
    }
    command_str[j] = '\0';
    *command = atoi(command_str);
    j=0;
    i++;
    while(i != strlen(msg))
    {
        param[j] = msg[i];
        j++;
        i++;
    }
    param[j] = '\0';
}

char* node_type_to_str(char type)
{
    switch (type)
    {
        case RESTAURANT : return "Restaurant";
        case CUSTOMER : return "Customer";
        case SUPPLIER : return "Supplier";
        default : return NULL;
    }
}