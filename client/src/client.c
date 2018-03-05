
#include "client.h"


int RunClient() {
    struct sockaddr_in addr;
    char sentence[BUFFER_SIZE];
    int len;
    int p;
    int current_port = 6789;
    Command *current_command = malloc(sizeof(Command));
    Connection *current_connection = malloc(sizeof(Connection));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Error socket(): %s(%d)\n", strerror(errno), errno);
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(current_port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Error connect(): %s(%d)\n", strerror(errno), errno);
        return 1;
    }

    p = 0;
    while (1) {
        int n = read(sockfd, sentence + p, 8191 - p);
        if (n < 0) {
            printf("Error read(): %s(%d)\n", strerror(errno), errno);
            return 1;
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

    // handle the command
    while(1) {
        memset(sentence,'\0',BUFFER_SIZE);
        setbuf(stdin,NULL);
//        do {
//            fgets(sentence, 4096, stdin);
//        }while(!sentence[0]);
        //while(strlen( sentence) == 0) {
        fgets(sentence, 4096, stdin);
        //}

        len = strlen(sentence);

        // handle the message which was sent to the server
        current_connection->message_client_write = malloc(sizeof(char)*len);
        for(int i = 0 ; i < len ; i++){
            current_connection->message_client_write[i] = sentence[i];
        }

        //current_connection->message_client_write[len] = '\n';
        //current_connection->message_client_write[len + 1] = '\0';

        // write the command
        write_command_struct(sentence,current_command);
        // the socketfd
        current_connection->sockfd = sockfd;

        // handle the command
        int jump_index = HandleCommand(current_command,current_connection);

        // sometimes we need to handle the message specially
        if(jump_index == 1) {

            // write the message to the server
            p = 0;
            while (p < len) {
                int n = write(sockfd, sentence + p, len - p);
                if (n < 0) {
                    printf("Error write(): %s(%d)\n", strerror(errno), errno);
                    return 1;
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
                    return 1;
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

            len = strlen(sentence);
            current_connection->message_client_read = malloc(sizeof(char)*(len + 1));
            for(int i = 0 ; i < len ; i++){
                current_connection->message_client_read[i] = sentence[i];
            }
            current_connection->message_client_read[len] = '\0';

            // handle the command
            write_command_struct(sentence,current_command);
            current_connection->sockfd = sockfd;
            sprintf(current_connection->client_ip, "%d.%d.%d.%d",127,0,0,1);
            current_connection->server_addr.sin_family = AF_INET;
            current_connection->server_addr.sin_port = current_port;

            // handle the command that read
            HandleCommand(current_command,current_connection);
        }

    }

    close(sockfd);
    return 0;
}


// handle the command
int HandleCommand(Command* current_command, Connection* current_connection){
    switch (find_command_index(current_command->code)){
        case PORT:
            Command_PORT(current_command,current_connection);
            return 1;
        case PASV:
            Command_PASV(current_command,current_connection);
            return 1;
        case RETR:
            Command_RETR(current_command,current_connection);
            return 0;
        case STOR:
            Command_STOR(current_command,current_connection);
            return 0;
        case QUIT:
            Command_QUIT(current_command,current_connection);
            return 0;
        case ABOR:
            Command_ABOR(current_command,current_connection);
            return 0;
        case LIST:
            Command_LIST(current_command,current_connection);
            return 0;
        default:
            return 1;
    }
}

