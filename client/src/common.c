//
// Created by systemmatic on 11/2/17.
//

#include "common.h"

// transfer string to a command
void write_command_struct(char *command_string, Command *current_string){
    sscanf(command_string,"%s %s",current_string->code,current_string->arg);
}

// get ip address from input
void get_ip_from_input(char* arg,int *ip){
    int temp_p1,temp_p2;
    sscanf(arg,"%d,%d,%d,%d,%d,%d",&ip[0],&ip[1],&ip[2],&ip[3],&temp_p1,&temp_p2);
}

// get port from input
int get_port_from_input(char* arg){
    int temp_ip1,temp_ip2,temp_ip3,temp_ip4;
    int p1,p2;
    sscanf(arg,"%d,%d,%d,%d,%d,%d",&temp_ip1,&temp_ip2,&temp_ip3,&temp_ip4,&p1,&p2);
    return p1 * 256 + p2;
}

int find_command_index(char *current_cmd){
    int total_number_command = sizeof(command_list_str)/sizeof(char *);
    for(int i=0; i < total_number_command; i++){
        if(strcmp(current_cmd,command_list_str[i]) == 0){
            return i;
        }
    }
    return -1;
}