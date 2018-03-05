//
// Created by systemmatic on 11/1/17.
//

#ifndef TINYFTP_COMMANDHANDLE_H
#define TINYFTP_COMMANDHANDLE_H

#include "common.h"

// command USER
void Command_USER(Command* current_command, Connection* current_connection);

// command PASS
void Command_PASS(Command* current_command, Connection* current_connection);

// command PORT
void Command_PORT(Command* current_command, Connection* current_connection);

// command PASV
void Command_PASV(Command* current_command, Connection* current_connection);

// command RETR
void Command_RETR(Command* current_command, Connection* current_connection);

// command STOR
void Command_STOR(Command* current_command, Connection* current_connection);

// command SYST
void Command_SYST(Command* current_command, Connection* current_connection);

// command TYPE
void Command_TYPE(Command* current_command, Connection* current_connection);

// command QUIT
void Command_QUIT(Command* current_command, Connection* current_connection);

// command ABOR
void Command_ABOR(Command* current_command, Connection* current_connection);

// command MKD
void Command_MKD(Command* current_command, Connection* current_connection);

// command CWD
void Command_CWD(Command* current_command, Connection* current_connection);

// command LIST
void Command_LIST(Command* current_command, Connection* current_connection);

// command RMD()
void Command_RMD(Command* current_command, Connection* current_connection);

#endif //TINYFTP_COMMANDHANDLE_H
