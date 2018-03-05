//
// Created by systemmatic on 11/1/17.
//

#ifndef TINYFTP_SERVER_H
#define TINYFTP_SERVER_H

#include "common.h"
#include "CommandHandle.h"

int RunServer(int argc , char** argv);

int HandleCommand(Command* current_command, Connection* current_connection);

// multithread function
void MultiThreadHandle(Connection* current_connection);

#endif //TINYFTP_SERVER_H
