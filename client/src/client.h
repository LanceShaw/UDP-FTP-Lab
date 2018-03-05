//
// Created by systemmatic on 11/2/17.
//

#ifndef TINYFTP_CLIENT_CLIENT_H
#define TINYFTP_CLIENT_CLIENT_H

#include "common.h"
#include "CommandHandle.h"


// command to run the command
int RunClient();

//handle the command
int HandleCommand(Command* current_command, Connection* current_connection);

#endif //TINYFTP_CLIENT_CLIENT_H
