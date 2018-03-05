//
// Created by systemmatic on 11/2/17.
//

#ifndef TINYFTP_CLIENT_COMMANDHANDLE_H
#define TINYFTP_CLIENT_COMMANDHANDLE_H

#include "common.h"

// command PORT
void Command_PORT(Command* current_command, Connection* current_connection);

// command PASV
void Command_PASV(Command* current_command, Connection* current_connection);

// command PORT
void Command_RETR(Command* current_command, Connection* current_connection);

// command PASV
void Command_STOR(Command* current_command, Connection* current_connection);

// command QUIT
void Command_QUIT(Command* current_command, Connection* current_connection);

// command ABOR
void Command_ABOR(Command* current_command, Connection* current_connection);

// command LIST
void Command_LIST(Command* current_command, Connection* current_connection);

#endif //TINYFTP_CLIENT_COMMANDHANDLE_H
