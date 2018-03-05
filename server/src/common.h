//
// Created by systemmatic on 11/1/17.
//

#ifndef TINYFTP_COMMON_H
#define TINYFTP_COMMON_H

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
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>


#ifndef BUFFER_SIZE
#define BUFFER_SIZE 8192
#endif

/* Valid usernames for anonymous ftp */
// a user table
// Commands enumeration
typedef enum command_list
{
    USER,PASS,PORT,PASV,RETR,STOR,SYST,TYPE,QUIT,ABOR,MKD,CWD,LIST,RMD
} command_list;

//server connection
typedef struct Connection
{
    /* Connection mode: NORMAL:0, PORT:1, PASV:2*/
    int mode;

    // listen fd
    int listenfd;

    // new fd
    int newfd;

    // conn fd
    int connfd;

    /* Is user loggd in? */
    int logged_in;

    /* Is this username allowed? */
    int username_check;
    char *username;

    /* Response message to client e.g. 220 Welcome */
    char *message;

    /* Socket for passive connection (must be accepted later) */
    int sock_pasv;

    /* Transfer process id */
    int transfer_process_id;

    // current client ip address
    char client_ip[20];

    // current server ip address
    char server_ip[20];

    //client addr
    struct sockaddr_in client_addr;

    // server addr
    struct sockaddr_in server_addr;

} Connection;


/* Command struct */
typedef struct Command
{
    char command[5];
    char arg[BUFFER_SIZE];
} Command;


//find command in list
int find_command_index(char *current_cmd);

int find_string_index(char* target_string,const char **array, int number);

// write to client function
void write_to_client(Connection* current_connection);

// transfer string to a command
void write_command_struct(char *command_string, Command *current_string);

// get a random port from 20000 to 65535
int get_random_port();

// get ip address from input
void get_ip_from_input(char* arg,int *ip);

// get port from input
int get_port_from_input(char* arg);

//get home ip address
void get_home_ip(char* ip);

#endif //TINYFTP_COMMON_H
