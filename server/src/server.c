#include "include/server.h"

void *clientHandler(void *args) {

    while(1) {
        // receive request from the client

        // Depending on the type of request take action
            // register

            // digitcount

            // latestcount

            // deregister
    }

    // close the connection
    return NULL;
}

int main(int argc, char *argv[]) {

    int port = atoi(argv[1]);

    // create TCP socket

    // bind server information to the TCP socket

    // start listening on the socket with given backlog

    while(1) {
        // accept a connection from a client

        // create a thread that calls clientHandler with argument as socket returned by accept to handle the client request

        // Detach the thread
    }

    // close the socket
    return 0;
}
