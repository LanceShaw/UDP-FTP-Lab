//
// Created by systemmatic on 11/2/17.
//

#ifndef TINYFTP_CLIENT_COMMON_H
#define TINYFTP_CLIENT_COMMON_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 8192
#endif

int sockfd;

// Commands enumeration
typedef enum command_list
{
    PORT,PASV,RETR,STOR,QUIT,ABOR,LIST
} command_list;

// String mappings for cmdlist
static const char *command_list_str[] =
        {
                "200","227","RETR","STOR","QUIT","ABOR","LIST"
        };


//client connection
typedef struct Connection
{
    /* Connection mode: NORMAL:0, PORT:1, PASV:2*/
    int mode;

    // listen fd
    int sockfd;

    // new fd
    int newfd;

    // current client ip address
    char client_ip[20];
    int client_port;

    // current server ip address
    char server_ip[20];
    int server_port;

    //client addr
    struct sockaddr_in client_addr;

    // server addr
    struct sockaddr_in server_addr;

    // message that client read
    char* message_client_read;

    // message that get
    char* message_client_write;

} Connection;

/* Command struct */
typedef struct Command
{
    char code[5];
    char arg[BUFFER_SIZE];
} Command;

// transfer string to a command
void write_command_struct(char *command_string, Command *current_string);

// get ip address from input
void get_ip_from_input(char* arg,int *ip);

// get port from input
int get_port_from_input(char* arg);

//find command in list
int find_command_index(char *current_cmd);


#endif //TINYFTP_CLIENT_COMMON_H
