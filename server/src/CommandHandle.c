//
// Created by systemmatic on 11/1/17.
//

#include "CommandHandle.h"

static const char *user_name_list[] =
        {
                "anonymous"
        };

// command USER
void Command_USER(Command* current_command, Connection* current_connection) {

    const int total_number_usernames = sizeof(user_name_list)/sizeof(char *);
    if(find_string_index(current_command->arg,user_name_list,total_number_usernames)>=0){
        current_connection->username = malloc(20);
        memset(current_connection->username,0,20);
        strcpy(current_connection->username,current_command->arg);
        current_connection->username_check = 1;
        current_connection->message = "331 User name accepted, you need to input password!\r\n";
    }else{
        current_connection->message = "530 Wrong username!\r\n";
    }
    write_to_client(current_connection);
    return;
}

// command PASS
void Command_PASS(Command* current_command, Connection* current_connection){
    if (current_connection->username_check == 1) {
        if (strlen(current_command->arg) > 0) {
            current_connection->logged_in = 1;
            current_connection->message = "230 Successful login in!\r\n";
        }
    } else {
        current_connection->message = "Wrong password or username!\r\n";
    }
    write_to_client(current_connection);
    return;
}

// command PORT
void Command_PORT(Command* current_command, Connection* current_connection){
    if(current_connection->logged_in == 1){
        int ip[4];
        int port;
        get_ip_from_input(current_command->arg,ip);
        port = get_port_from_input(current_command->arg);
        current_connection->message = "200 PORT command successful.\r\n";
        sprintf(current_connection->client_ip,"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
        
        current_connection->client_addr.sin_port = htons(port);
        current_connection->client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        current_connection->client_addr.sin_family = AF_INET;

        // set the mode
        current_connection->mode = 1;

        // create a socket
        if ((current_connection->newfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            printf("Error socket(): %s(%d)\n", strerror(errno), errno);
            current_connection->mode = 0;
            current_connection->message = "PORT command failed!\r\n";
            write_to_client(current_connection);
            return;
        }

        if (inet_pton(AF_INET,current_connection->client_ip, &current_connection->client_addr.sin_addr) <= 0) {
            printf("Error inet_pton(): %s(%d)\n", strerror(errno), errno);
            current_connection->mode = 0;
            current_connection->message = "PORT command failed!\r\n";
            write_to_client(current_connection);
            return;
        }

    }
    else{
        current_connection->message = "Please connect the server with proper username and password!\r\n";
    }
    write_to_client(current_connection);
    return;
}

// command PASV
void Command_PASV(Command* current_command, Connection* current_connection){
    if(current_connection->logged_in == 1){
        int home_ip[4];
        int port = get_random_port();
        int port_p1 = port / 256;
        int port_p2 = port % 256;
        char ip_string[200];
        char temp_string[200];
        get_home_ip(ip_string);
        sscanf(ip_string,"addr:%d.%d.%d.%d\n%s",&home_ip[0],&home_ip[1],&home_ip[2],&home_ip[3],temp_string);
        sprintf(current_connection->server_ip ,"%d.%d.%d.%d",home_ip[0],home_ip[1],home_ip[2],home_ip[3]);

        // write the message to the client
        sprintf(temp_string,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n",home_ip[0],home_ip[1],home_ip[2],home_ip[3],port_p1,port_p2);
        current_connection->message = temp_string;

        // create the socket
        if ((current_connection->listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            printf("Error socket(): %s(%d)\n", strerror(errno), errno);
            return;
        }

        memset(&current_connection->client_addr, 0, sizeof(current_connection->client_addr));
        current_connection->client_addr.sin_family = AF_INET;
        current_connection->client_addr.sin_port = htons( port);
        current_connection->client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        // bind the port to the socket
        if (bind(current_connection->listenfd, (struct sockaddr*)&current_connection->client_addr, sizeof(current_connection->client_addr)) == -1) {
            printf("Error bind(): %s(%d)\n", strerror(errno), errno);
            return;
        }

        if (listen(current_connection->listenfd, 10) == -1) {
            printf("Error listen(): %s(%d)\n", strerror(errno), errno);
            return ;
        }

        // change the mode tag
        current_connection->mode = 2;

        }
        else{
            current_connection->message = "Please connect the server with proper username and password!\r\n";
        }
        write_to_client(current_connection);
        return;
    }

// command RETR
void Command_RETR(Command* current_command, Connection* current_connection){
        //file name
    char* filename = current_command->arg;

    if(current_connection->logged_in  == 1){
        if(current_connection->mode > 0){
            if(current_connection->mode == 1){
                // port mode
                //  the server connect the client
                printf("2222222\n");
                if (connect(current_connection->newfd, (struct sockaddr*)&current_connection->client_addr, sizeof(current_connection->client_addr)) < 0) {
                    printf("Error connect(): %s(%d)\n", strerror(errno), errno);
                    current_connection->message = "426 The TCP RETR connection was established but then broken by the client or by network failure!\r\n";
                    write_to_client(current_connection);
                    return;
                }
            }
            else if(current_connection->mode == 2){
                // pasv mode : the client connect the server
                if ((current_connection->newfd = accept(current_connection->listenfd, NULL, NULL)) == -1) {
                    printf("Error accept(): %s(%d)\n", strerror(errno), errno);
                    close(current_connection->newfd);
                    return;
                }
            }

            // open the file
            int fp;
            fp = open(current_command->arg, O_RDONLY , S_IRWXG | S_IRWXO | S_IRWXU);

            // calculate the file size
            struct stat buf;
            stat(filename, &buf);

            // send the 150 code message
            char temp_string[BUFFER_SIZE];
            sprintf(temp_string,"150 Opening BINARY mode data connection for %s (%ld bytes).\r\n",filename,buf.st_size);

            current_connection->message = temp_string;
            write_to_client(current_connection);

            // read the file
            char sentence[BUFFER_SIZE];
            int len;
            while((len = read(fp, sentence, 8191)) > 0){
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
            close(current_connection->newfd);

            // write the message
            current_connection->message = "226 Transfer complete.\r\n";
            if(current_connection->mode > 0){
                sleep(1);
            }
            write_to_client(current_connection);
            return;

        }
        else{
            current_connection->message = "425 No TCP connection was established!\r\n";
            write_to_client(current_connection);
            return;
        }
    }
    else{
        current_connection->message = "Please connect the server with proper username and password!\r\n";
        write_to_client(current_connection);
        return;
    }

    return;
}

// command STOR
void Command_STOR(Command* current_command, Connection* current_connection){

    char* filename = current_command->arg;
    if(current_connection->logged_in  == 1){
        if(current_connection->mode > 0){
            if(current_connection->mode == 1){

                // port mode
                printf("2222222\n");
                if (connect(current_connection->newfd, (struct sockaddr*)&current_connection->client_addr, sizeof(current_connection->client_addr)) < 0) {
                    printf("Error connect(): %s(%d)\n", strerror(errno), errno);
                    current_connection->message = "426 The TCP STOR connection was established but then broken by the client or by network failure!\r\n";
                    write_to_client(current_connection);
                    return;
                }

                // 150 command
                // send the 150 code message
                char temp_string[BUFFER_SIZE];
                sprintf(temp_string,"150 Opening BINARY mode data connection for %s (%d bytes).\r\n",filename,20);
                current_connection->message = temp_string;
                write_to_client(current_connection);


                // create the file
                int fp;
                fp = open(filename, O_WRONLY | O_CREAT , S_IRWXG | S_IRWXO | S_IRWXU);

                // prepare to read the info from the server
                int p;
                p = 0;
                char sentence[BUFFER_SIZE];
                //int len;
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

                    // write the info to the file
                    int file_state = write(fp,sentence,n);
                    if(file_state == 0){
                        printf("Error when writing the file!");
                        break;
                    }
                }


                // close the file and port
                close(fp);

                // 226 command
                // write the message
                current_connection->message = "226 Transfer complete.\r\n";
                if(current_connection->mode > 0){
                    sleep(1);
                }
                write_to_client(current_connection);


                return;

            }
            else if(current_connection->mode == 2){

                // pasv mode : the client connect the server
                if ((current_connection->newfd = accept(current_connection->listenfd, NULL, NULL)) == -1) {
                    printf("Error accept(): %s(%d)\n", strerror(errno), errno);
                    return;
                }

                // 150 command
                // send the 150 code message
                char temp_string[BUFFER_SIZE];
                sprintf(temp_string,"150 Opening BINARY mode data connection for %s (%d bytes).\r\n",filename,20);
                current_connection->message = temp_string;
                write_to_client(current_connection);


                // create the file
                int fp;
                fp = open(filename, O_WRONLY | O_CREAT , S_IRWXG | S_IRWXO | S_IRWXU);

                // prepare to read the info from the server
                int p;
                p = 0;
                char sentence[BUFFER_SIZE];
                //int len;
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

                    // write the info to the file
                    int file_state = write(fp,sentence,n);
                    if(file_state == 0){
                        printf("Error when writing the file!");
                        break;
                    }
                }


                // close the file and port
                close(fp);

                //226 command
               // write the message
                current_connection->message = "226 Transfer complete.\r\n";
                if(current_connection->mode > 0){
                    sleep(1);
                }
                write_to_client(current_connection);

                return;

            }
            else{

            }
        }
        else{
            current_connection->message = "425 No TCP connection was established!\r\n";
        }
    }
    else{
        current_connection->message = "Please connect the server with proper username and password!\r\n";
    }

    write_to_client(current_connection);
    return;
}

// command SYST
void Command_SYST(Command* current_command, Connection* current_connection){
    if(current_connection->logged_in == 1){
        current_connection->message = "215 UNIX Type: L8\r\n";
    }
    else{
        current_connection->message = "Please connect the server with proper username and password!\r\n";
    }
    write_to_client(current_connection);
    return;
}

// command TYPE
void Command_TYPE(Command* current_command, Connection* current_connection){
    if(current_connection->logged_in == 1){
        char* tempString = "I";
        if(strcmp(current_command->arg,tempString) == 0){
            current_connection->message = "200 Type set to I.\r\n";
        }
        else{
            current_connection->message = "Wrong TYPE Command!\r\n";
        }
    }
    else{
        current_connection->message = "Please connect the server with proper username and password!\r\n";
    }
    write_to_client(current_connection);
    return;
}

// command QUIT
void Command_QUIT(Command* current_command, Connection* current_connection){
    printf("this login : %d\n", current_connection->logged_in);

    if(current_connection->logged_in == 1){
        current_connection->message = "221 Thank you for using the FTP service on ftp.ssast.org.Goodbye.\r\n";
        write_to_client(current_connection);
        close(current_connection->connfd);
        current_connection->connfd = -1;
        return;
    }
    else{
        current_connection->message = "221 Thank you for using the FTP service on ftp.ssast.org.Goodbye.\r\n";
        write_to_client(current_connection);
        close(current_connection->connfd);
        current_connection->connfd = -1;
        return;
    }
    //write_to_client(current_connection);
    //return;
}

// command ABOR
void Command_ABOR(Command* current_command, Connection* current_connection){
    printf("this login : %d\n", current_connection->logged_in);
    if(current_connection->logged_in == 1){
        current_connection->message = "221 Thank you for using the FTP service on ftp.ssast.org.Goodbye.\r\n";
        write_to_client(current_connection);
        close(current_connection->connfd);
        current_connection->connfd = -1;
        return;
    }
    else{
        current_connection->message = "221 Thank you for using the FTP service on ftp.ssast.org.Goodbye.\r\n";
        write_to_client(current_connection);
        close(current_connection->connfd);
        current_connection->connfd = -1;
        return;
        //current_connection->message = "221 Please connect the server with proper username and password!\r\n";
    }
    //write_to_client(current_connection);
    //return;
}

// command MKD
void Command_MKD(Command* current_command, Connection* current_connection){
    // make the directory
    if(current_connection->logged_in == 1){
        int status = mkdir(current_command->arg, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(status != 0){
            current_connection->message = "MKD command fail!\r\n";
            write_to_client(current_connection);
            return;
        }
        current_connection->message = "MKD command successful!\r\n";
        write_to_client(current_connection);
        return;
    }
    else{
        current_connection->message = "Please connect the server with proper username and password!\r\n";
    }
    write_to_client(current_connection);
    return;
}

// command CWD
void Command_CWD(Command* current_command, Connection* current_connection){
    // make the directory
    if(current_connection->logged_in == 1){
        int status = chdir(current_command->arg);
        //if fail
        if(status != 0){
            current_connection->message = "CWD command fail!\r\n";
            write_to_client(current_connection);
            return;
        }
        current_connection->message = "CWD command successful!\r\n";
        write_to_client(current_connection);
        return;
    }
    else{
        current_connection->message = "Please connect the server with proper username and password!\r\n";
    }
    write_to_client(current_connection);
    return;
}

// command RMD()
void Command_RMD(Command* current_command, Connection* current_connection){

    // make the directory
    if(current_connection->logged_in == 1){
        char system_command[100];
        sprintf(system_command,"rm -rf %s",current_command->arg);
        int status = system(system_command);
        if(status != 0){
            current_connection->message = "RMD command fail!\r\n";
            write_to_client(current_connection);
            return;
        }
        current_connection->message = "RMD command successful!\r\n";
        write_to_client(current_connection);
        return;

    }
    else{
        current_connection->message = "Please connect the server with proper username and password!\r\n";
    }
    write_to_client(current_connection);
    return;

}

// command LIST
void Command_LIST(Command* current_command, Connection* current_connection){
    // MUST be used in PASV or PORT command

    if(current_connection->logged_in  == 1){
        if(current_connection->mode > 0){
            if(current_connection->mode == 1){
                // port mode
                //  the server connect the client
                if (connect(current_connection->newfd, (struct sockaddr*)&current_connection->client_addr, sizeof(current_connection->client_addr)) < 0) {
                    printf("Error connect(): %s(%d)\n", strerror(errno), errno);
                    current_connection->message = "426 The TCP LIST connection was established but then broken by the client or by network failure!\r\n";
                    write_to_client(current_connection);
                    return;
                }
            }
            else if(current_connection->mode == 2){
                // pasv mode : the client connect the server
                if ((current_connection->newfd = accept(current_connection->listenfd, NULL, NULL)) == -1) {
                    printf("Error accept(): %s(%d)\n", strerror(errno), errno);
                    close(current_connection->newfd);
                    return;
                }
            }

            char* temp_command = "ls -l";
            //char line[BUFFER_SIZE];
            FILE* stream;
            stream = popen(temp_command,"r");

            // read the file
            char sentence[BUFFER_SIZE];
            int len;
            while( fgets(sentence, 8191,stream) ){
                // write the file
                int p = 0;
                len = strlen(sentence);
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
            close(current_connection->newfd);
            // write the message
            return;

        }
        else{
            current_connection->message = "425 No TCP connection was established!\r\n";
            write_to_client(current_connection);
            return;
        }
    }
    else{
        current_connection->message = "Please connect the server with proper username and password!\r\n";
        write_to_client(current_connection);
        return;
    }

    return;
}

