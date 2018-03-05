//
// Created by systemmatic on 11/1/17.
//

#include "common.h"

static const char *command_list_str[] =
        {
                "USER","PASS","PORT","PASV","RETR","STOR","SYST","TYPE","QUIT","ABOR","MKD","CWD","LIST","RMD"
        };

int find_command_index(char *current_cmd){
    const int total_number_command = sizeof(command_list_str)/sizeof(char *);
    return find_string_index(current_cmd,command_list_str,total_number_command);
}

int find_string_index(char* target_string,const char** array, int number){
    for(int i = 0; i < number; i++){
        if(strcmp(target_string,array[i])==0){
            return i;
        }
    }
    return -1;
}

void write_to_client(Connection* current_connection){
    write(current_connection->connfd, current_connection->message, strlen(current_connection->message));
}

// transfer string to a command
void write_command_struct(char *command_string, Command *current_string){
    sscanf(command_string,"%s %s",current_string->command,current_string->arg);
}

// get a random port from 20000 to 65535
int get_random_port(){
    srand((unsigned)time(NULL));
    int port = rand() % (65535 - 20000) + 20000;
    return port;
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

//get home ip address
void get_home_ip(char* ip){
//    system("ifconfig | grep inet[^6] | awk \'{if(NR == 1){print $2}}\' > a.txt ");
//    FILE *p1 = fopen("a.txt","r");
//    fgets(ip,200,p1);
//    fclose(p1);
//    remove("a.txt");
    sprintf(ip,"addr:%d.%d.%d.%d\n",127,0,0,1);
}