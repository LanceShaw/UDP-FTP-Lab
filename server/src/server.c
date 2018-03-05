//
// Created by systemmatic on 11/1/17.
//

#include "server.h"


int RunServer(int argc , char** argv) {

    int listenfd;
    char root[200] = "/tmp";
    //char sentence[BUFFER_SIZE];
    //int p;
    int current_port = 21;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-root") == 0)
            strcpy(root, argv[i+1]);
        else if(strcmp(argv[i], "-port") == 0)
            current_port = atoi(argv[i+1]);
    }

    chdir(root);

    Connection *current_connection = malloc(sizeof(Connection));
    current_connection->username = NULL;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Error socket(): %s(%d)\n", strerror(errno), errno);
        return 1;
    }

    memset(&current_connection->client_addr, 0, sizeof(current_connection->client_addr));
    current_connection->client_addr.sin_family = AF_INET;
    current_connection->client_addr.sin_port = htons(current_port);
    current_connection->client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&current_connection->client_addr, sizeof(current_connection->client_addr)) == -1) {
        printf("Error bind(): %s(%d)\n", strerror(errno), errno);
        return 1;
    }

    if (listen(listenfd, 10) == -1) {
        printf("Error listen(): %s(%d)\n", strerror(errno), errno);
        return 1;
    }



    while (1) {
        current_connection = malloc(sizeof(Connection));
        current_connection->username = NULL;
        current_connection->client_addr.sin_family = AF_INET;
        current_connection->client_addr.sin_port = htons(current_port);
        current_connection->client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if ((current_connection->connfd = accept(listenfd, NULL, NULL)) == -1) {
            printf("Error accept(): %s(%d)\n", strerror(errno), errno);
            continue;
        }

        //current_connection->connfd = connfd;

        char *welcome_message = "220 Anonymous FTP server ready.\r\n";
        // send welcome message after finishing connecting
        int n = write(current_connection->connfd,welcome_message, strlen(welcome_message));
        if (n < 0) {
            printf("Error write(): %s(%d)\n", strerror(errno), errno);
            return 1;
        }

//        while(1) {
//            Command *current_command = malloc(sizeof(Command));
//            p = 0;
//            while (1) {
//                int n = read(connfd, sentence + p, 8191 - p);
//                if (n < 0) {
//                    printf("Error read(): %s(%d)\n", strerror(errno), errno);
//                    close(connfd);
//                    continue;
//                } else if (n == 0) {
//                    break;
//                } else {
//                    p += n;
//                    // sentence[p-1] = '\0'
//                    if (sentence[p - 1] == '\n') {
//                        break;
//                    }
//                }
//            }
//
//            sentence[p] = '\0';
//            //len = p;
//
//            write_command_struct(sentence,current_command);
//            current_connection->connfd = connfd;
//            sprintf(current_connection->client_ip,"%d.%d.%d.%d",127,0,0,1);
//
//            // handle the command
//            HandleCommand(current_command,current_connection);
//        }

        pthread_t id;
        int ret;
        //ret = service(connection);
        ret = pthread_create(&id, NULL, (void *)MultiThreadHandle, current_connection);
        if(ret != 0)
            printf("thread create failed");

        //close(connfd);
    }

    close(listenfd);
}

int HandleCommand(Command* current_command, Connection* current_connection){

    switch (find_command_index(current_command->command)){
        case USER:
            Command_USER(current_command,current_connection);
            break;
        case PASS:
            Command_PASS(current_command,current_connection);
            break;
        case PORT:
            Command_PORT(current_command,current_connection);
            break;
        case PASV:
            Command_PASV(current_command,current_connection);
            break;
        case RETR:
            Command_RETR(current_command,current_connection);
            break;
        case STOR:
            Command_STOR(current_command,current_connection);
            break;
        case SYST:
            Command_SYST(current_command,current_connection);
            break;
        case TYPE:
            Command_TYPE(current_command,current_connection);
            break;
        case QUIT:
            Command_QUIT(current_command,current_connection);
            break;
        case ABOR:
            Command_ABOR(current_command,current_connection);
            break;
        case MKD:
            Command_MKD(current_command,current_connection);
            break;
        case CWD:
            Command_CWD(current_command,current_connection);
            break;
        case LIST:
            Command_LIST(current_command,current_connection);
            break;
        case RMD:
            Command_RMD(current_command,current_connection);
            break;
        default:
            current_connection->message = "500 Unknown command\n";
            write_to_client(current_connection);
            break;
    }
    return 0;
}

// multithread function
void MultiThreadHandle(Connection* current_connection){
    while(1) {
        if(current_connection->connfd == -1)
            break;
        Command *current_command = malloc(sizeof(Command));
        int p = 0;
        char sentence[BUFFER_SIZE];
        while (1) {
            int n = read(current_connection->connfd, sentence + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                close(current_connection->connfd);
                continue;
            } else if (n == 0) {
                break;
            } else {
                p += n;
                // sentence[p-1] = '\0'
                if (sentence[p - 1] == '\n') {
                    break;
                }
            }
        }

        sentence[p] = '\0';
        //len = p;

        write_command_struct(sentence,current_command);
        sprintf(current_connection->client_ip,"%d.%d.%d.%d",127,0,0,1);

        // handle the command
        HandleCommand(current_command,current_connection);
    }
}
