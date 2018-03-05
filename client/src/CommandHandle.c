//
// Created by systemmatic on 11/2/17.
//

#include "CommandHandle.h"
int listenfd;

// command PORT
void Command_PORT(Command* current_command, Connection* current_connection){

    int ip[4];
    int port_p1,port_p2,port;
    char temp_command[100];
    char temp_ip_string[100];
    sscanf(current_connection->message_client_write,"%s %s",temp_command,temp_ip_string);
    sscanf(temp_ip_string,"%d,%d,%d,%d,%d,%d",&ip[0],&ip[1],&ip[2],&ip[3],&port_p1,&port_p2);
    port = 256 * port_p1 + port_p2;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Error socket(): %s(%d)\n", strerror(errno), errno);
        return;
    }

    memset(&current_connection->client_addr, 0, sizeof(current_connection->client_addr));
    current_connection->client_addr.sin_family = AF_INET;
    current_connection->client_addr.sin_port =htons(port);
    current_connection->client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&current_connection->client_addr, sizeof(current_connection->client_addr)) == -1) {
        printf("Error bind(): %s(%d)\n", strerror(errno), errno);
        return;
    }

    if (listen(listenfd, 10) == -1) {
        printf("Error listen(): %s(%d)\n", strerror(errno), errno);
        return;
    }

    current_connection->mode = 1;

}

// command PASV
void Command_PASV(Command* current_command, Connection* current_connection){

    int ip[4];
    int port_p1,port_p2,port;
    char temp_command[100];
    char temp_ip_string[100];

    sscanf(current_connection->message_client_read, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip[0],&ip[1],&ip[2],&ip[3],&port_p1,&port_p2);
    sprintf(current_connection->server_ip,"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
    port = 256 * port_p1 + port_p2;

    current_connection->server_port = port;
    current_connection->server_addr.sin_port = htons(current_connection->server_port);
    current_connection->server_addr.sin_family = AF_INET;

    // create a new socket
    current_connection->newfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    inet_pton(AF_INET, current_connection->server_ip, &current_connection->server_addr.sin_addr);

    current_connection->mode = 2;

}

// command PORT
void Command_RETR(Command* current_command, Connection* current_connection){

    if(current_connection->mode == 1){

        // port mode:wirte the command to the server
        int p;
        int len = strlen(current_connection->message_client_write);
        p = 0;
        while (p < len) {
            int n = write(current_connection->sockfd, current_connection->message_client_write + p, len- p);
            if (n < 0) {
                printf("Error write(): %s(%d)\n", strerror(errno), errno);
                return;
            } else {
                p += n;
            }
        }

        // read the command 150 from the server
        p = 0;
        while (1) {
            int n = read(sockfd, current_connection->message_client_read + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                return ;
            } else if (n == 0) {
                break;
            } else {
                p += n;
                if (current_connection->message_client_read[p - 1] == '\n') {
                    break;
                }
            }
        }
        current_connection->message_client_read[p] = '\0';
        printf("FROM SERVER: %s", current_connection->message_client_read);
        // when meet the error
        if(current_connection->message_client_read[0] == '4' || current_connection->message_client_read[0] == 'P' ){
            return;
        }


        char* filename = current_command->arg;
        while (1) {
            int connfd;

            // accept the connect
            if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
                printf("Error accept(): %s(%d)\n", strerror(errno), errno);
                continue;
            }

            // create the file
            int fp;
            fp = open(filename, O_WRONLY | O_CREAT , S_IRWXG | S_IRWXO | S_IRWXU);

            // prepare to read the info from the server
            int p;
            p = 0;
            char sentence[BUFFER_SIZE];
            int len;

            int n = 8191;
            // read the info
            while (1) {
                n = read(connfd, sentence + p, 8191 - p);
                if (n < 0) {
                    printf("Error read(): %s(%d)\n", strerror(errno), errno);
                    close(connfd);
                    break;
                } else if (n == 0) {
                    break;
                }
                // write the file
                int file_state = write(fp,sentence,n);
                if(file_state == 0){
                    printf("Error when writing the file!");
                    break;
                }
            }


            // close the file
            close(fp);

            //close the connfd
            close(connfd);
            break;

        }

        // read the command 226 from the server
        p = 0;
        while (1) {
            int n = read(sockfd, current_connection->message_client_read + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                return ;
            } else if (n == 0) {
                break;
            } else {
                p += n;
                if (current_connection->message_client_read[p - 1] == '\n') {
                    break;
                }
            }
        }
        current_connection->message_client_read[p] = '\0';
        printf("FROM SERVER: %s", current_connection->message_client_read);

        close(listenfd);

    }

    else if(current_connection->mode == 2){
        // write the command to the server
        int p;
        int len = strlen(current_connection->message_client_write);
        p = 0;
        while (p < len) {
            int n = write(current_connection->sockfd, current_connection->message_client_write + p, len - p);
            if (n < 0) {
                printf("Error write(): %s(%d)\n", strerror(errno), errno);
                return;
            } else {
                p += n;
            }
        }

        // pasv mode : the client connect the server
        if (connect(current_connection->newfd, (struct sockaddr*)&current_connection->server_addr, sizeof(current_connection->server_addr)) < 0) {
            printf("Error connect(): %s(%d)\n", strerror(errno), errno);
            printf("426 The TCP connection was established but then broken by the client or by network failure!\r\n");
            return;
        }


        // read the command 150 from the server
        p = 0;
        while (1) {
            int n = read(sockfd, current_connection->message_client_read + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                return ;
            } else if (n == 0) {
                break;
            } else {
                p += n;
                if (current_connection->message_client_read[p - 1] == '\n') {
                    break;
                }
            }
        }
        current_connection->message_client_read[p] = '\0';
        printf("FROM SERVER: %s", current_connection->message_client_read);
        // when meet the error
        if(current_connection->message_client_read[0] == '4' || current_connection->message_client_read[0] == 'P' ){
            return;
        }

        char* filename = current_command->arg;
        // create the file
        int fp;
        fp = open(filename, O_WRONLY | O_CREAT , S_IRWXG | S_IRWXO | S_IRWXU);

        // prepare to read the info from the server
        p = 0;
        char sentence[BUFFER_SIZE];

        int n = 8191;
        // read the info
        while (1) {
            n = read(current_connection->newfd, sentence + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                close(current_connection->newfd);
                break;
            } else if (n == 0) {
                break;
            }
            // write the file
            int file_state = write(fp,sentence,n);
            if(file_state == 0){
                printf("Error when writing the file!");
                break;
            }
        }

        // close the file
        close(fp);
        close(current_connection->newfd);

        // read the 226 command
        p = 0;
        while (1) {
            int n = read(current_connection->sockfd, current_connection->message_client_read + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                return ;
            } else if (n == 0) {
                break;
            } else {
                p += n;
                if (current_connection->message_client_read[p - 1] == '\n') {
                    break;
                }
            }
        }

        current_connection->message_client_read[p] = '\0';
        printf("FROM SERVER: %s",current_connection->message_client_read);

    }
    else{

    }


    return;
}

// command PASV
void Command_STOR(Command* current_command, Connection* current_connection){

    if(current_connection->mode == 1){

        // port mode:wirte the command to the server
        int p;
        int len = strlen(current_connection->message_client_write);
        p = 0;
        while (p < len) {
            int n = write(current_connection->sockfd, current_connection->message_client_write + p, len - p);
            if (n < 0) {
                printf("Error write(): %s(%d)\n", strerror(errno), errno);
                return;
            } else {
                p += n;
            }
        }

        // open the file
        char* filename = current_command->arg;
        int fp;
        fp = open(filename, O_RDONLY);

        // read the command 150 from the server
        p = 0;
        while (1) {
            int n = read(sockfd, current_connection->message_client_read + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                return ;
            } else if (n == 0) {
                break;
            } else {
                p += n;
                if (current_connection->message_client_read[p - 1] == '\n') {
                    break;
                }
            }
        }
        current_connection->message_client_read[p] = '\0';
        printf("FROM SERVER: %s", current_connection->message_client_read);
        // when meet the error
        if(current_connection->message_client_read[0] == '4' || current_connection->message_client_read[0] == 'P' ){
            return;
        }



        while (1) {
            int connfd;

            // accept the connect
            if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
                printf("Error accept(): %s(%d)\n", strerror(errno), errno);
                continue;
            }

            // write the file info
            // read the file
            char sentence[BUFFER_SIZE];
            int len;
            while((len = read(fp, sentence, 8191)) > 0){
                // write the file
                int p = 0;
                while (p < len) {
                    int n = write(connfd, sentence + p, len - p);
                    if (n < 0) {
                        printf("Error write(): %s(%d)\n", strerror(errno), errno);
                        return;
                    } else {
                        p += n;
                    }
                }
            }

            // close the file
            close(fp);
            //close the connfd
            close(connfd);
            break;

        }


        // read the command 226 from the server
        p = 0;
        while (1) {
            int n = read(sockfd, current_connection->message_client_read + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                return ;
            } else if (n == 0) {
                break;
            } else {
                p += n;
                if (current_connection->message_client_read[p - 1] == '\n') {
                    break;
                }
            }
        }
        current_connection->message_client_read[p] = '\0';
        printf("FROM SERVER: %s", current_connection->message_client_read);

        close(listenfd);

    }

    else if(current_connection->mode == 2){

        // port mode:wirte the command to the server
        int p;
        int len = strlen(current_connection->message_client_write);
        p = 0;
        while (p < len) {
            int n = write(current_connection->sockfd, current_connection->message_client_write + p, len - p);
            if (n < 0) {
                printf("Error write(): %s(%d)\n", strerror(errno), errno);
                return;
            } else {
                p += n;
            }
        }

        // pasv mode : the client connect the server
        if (connect(current_connection->newfd, (struct sockaddr*)&current_connection->server_addr, sizeof(current_connection->server_addr)) < 0) {
            printf("Error connect(): %s(%d)\n", strerror(errno), errno);
            printf("426 The TCP connection was established but then broken by the client or by network failure!\r\n");
            return;
        }

        // open the file
        char* filename = current_command->arg;
        int fp;
        fp = open(filename, O_RDONLY);

        // read the command 150 from the server
        p = 0;
        while (1) {
            int n = read(sockfd, current_connection->message_client_read + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                return ;
            } else if (n == 0) {
                break;
            } else {
                p += n;
                if (current_connection->message_client_read[p - 1] == '\n') {
                    break;
                }
            }
        }
        current_connection->message_client_read[p] = '\0';
        printf("FROM SERVER: %s", current_connection->message_client_read);
        // when meet the error
        if(current_connection->message_client_read[0] == '4' || current_connection->message_client_read[0] == 'P' ){
            return;
        }

        // read the file info and write
        char sentence[BUFFER_SIZE];
        while ((len = read(fp, sentence, 8191)) > 0) {
            // write the file
            int p = 0;
            while (p < len) {
                int n = write(current_connection->newfd, sentence + p, len - p);
                if (n < 0) {
                    printf("Error write(): %s(%d)\n", strerror(errno), errno);
                    return;
                } else {
                    p += n;
                }
            }
        }

        // close the file
        close(fp);
        //close the connfd
        close(current_connection->newfd);

        // read the command 226
        //printf("226 Transfer complete.\n");
        // read the 226 command
        p = 0;
        while (1) {
            int n = read(current_connection->sockfd, current_connection->message_client_read + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                return ;
            } else if (n == 0) {
                break;
            } else {
                p += n;
                if (current_connection->message_client_read[p - 1] == '\n') {
                    break;
                }
            }
        }

        current_connection->message_client_read[p] = '\0';
        printf("FROM SERVER: %s",current_connection->message_client_read);


        //close(listenfd);

    }
    else{

    }
    return;
}

// command QUIT
void Command_QUIT(Command* current_command, Connection* current_connection){

    char sentence[BUFFER_SIZE];
    sprintf(sentence,"%s",current_command->code);
    int len = strlen(sentence);
    sentence[len] = '\n';
    sentence[len+1] = '\0';
    len++;

    // write the message to the server
    int p = 0;
    while (p < len) {
        int n = write(sockfd, sentence + p, len - p);
        if (n < 0) {
            printf("Error write(): %s(%d)\n", strerror(errno), errno);
            return ;
        } else {
            p += n;
        }
    }

    // read the message from the server
    // handle the command from the server
    p = 0;
    while (1) {
        int n = read(sockfd, sentence + p, 8191 - p);
        if (n < 0) {
            printf("Error read(): %s(%d)\n", strerror(errno), errno);
            return ;
        } else if (n == 0) {
            break;
        } else {
            p += n;
            if (sentence[p - 1] == '\n') {
                break;
            }
        }
    }

    sentence[p] = '\0';
    printf("FROM SERVER: %s", sentence);

    exit(0);


}

// command ABOR
void Command_ABOR(Command* current_command, Connection* current_connection){
    // ABOR
    char sentence[BUFFER_SIZE];
    sprintf(sentence,"%s",current_command->code);
    int len = strlen(sentence);
    sentence[len] = '\n';
    sentence[len+1] = '\0';
    len++;

    // write the message to the server
    int p = 0;
    while (p < len) {
        int n = write(sockfd, sentence + p, len - p);
        if (n < 0) {
            printf("Error write(): %s(%d)\n", strerror(errno), errno);
            return ;
        } else {
            p += n;
        }
    }

    // read the message from the server
    // handle the command from the server
    p = 0;
    while (1) {
        int n = read(sockfd, sentence + p, 8191 - p);
        if (n < 0) {
            printf("Error read(): %s(%d)\n", strerror(errno), errno);
            return ;
        } else if (n == 0) {
            break;
        } else {
            p += n;
            if (sentence[p - 1] == '\n') {
                break;
            }
        }
    }

    sentence[p] = '\0';
    printf("FROM SERVER: %s", sentence);

    exit(0);
}

// command LIST
void Command_LIST(Command* current_command, Connection* current_connection){
    if(current_connection->mode == 1){

        char sentence[BUFFER_SIZE];
        sprintf(sentence,"%s",current_command->code);
        int len = strlen(sentence);
        sentence[len] = '\n';
        sentence[len+1] = '\0';
        len++;

        // port mode:wirte the command to the server
        int p;
        p = 0;
        while (p < len) {
            int n = write(current_connection->sockfd, sentence + p, len- p);
            if (n < 0) {
                printf("Error write(): %s(%d)\n", strerror(errno), errno);
                return;
            } else {
                p += n;
            }
        }

        int connfd;
        // accept the connect
        if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
            printf("Error accept(): %s(%d)\n", strerror(errno), errno);
            return;
        }

        // prepare to read the info from the server
        p = 0;
        int n = 8191;
        // read the info
        while (1) {
            n = read(connfd, sentence + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                close(connfd);
                break;
            } else if (n == 0) {
                break;
            }
            // print the command
            printf(sentence);

        }

        //close the connfd
        close(connfd);
        close(listenfd);

    }

    else if(current_connection->mode == 2){

        char sentence[BUFFER_SIZE];
        sprintf(sentence,"%s",current_command->code);
        int len = strlen(sentence);
        sentence[len] = '\n';
        sentence[len+1] = '\0';
        len++;

        // write the command to the server
        int p;
        p = 0;
        while (p < len) {
            int n = write(current_connection->sockfd, sentence + p, len - p);
            if (n < 0) {
                printf("Error write(): %s(%d)\n", strerror(errno), errno);
                return;
            } else {
                p += n;
            }
        }

        // pasv mode : the client connect the server
        if (connect(current_connection->newfd, (struct sockaddr*)&current_connection->server_addr, sizeof(current_connection->server_addr)) < 0) {
            printf("Error connect(): %s(%d)\n", strerror(errno), errno);
            printf("426 The TCP connection was established but then broken by the client or by network failure!\r\n");
            return;
        }

        // prepare to read the info from the server
        p = 0;

        int n = 8191;
        // read the info
        while (1) {
            n = read(current_connection->newfd, sentence + p, 8191 - p);
            if (n < 0) {
                printf("Error read(): %s(%d)\n", strerror(errno), errno);
                close(current_connection->newfd);
                break;
            } else if (n == 0) {
                break;
            }
            // print the command
            printf(sentence);
        }

        // close the file
        close(current_connection->newfd);

    }
    else{
        printf("LIST must be in PASV mode or PORT mode!\n");
    }
    return;
}