#ifndef SERVER_H_
#define SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// REQUEST CODE
#define REGISTER 1
#define DIGITCOUNT 2
#define LATESTCOUNT 3
#define DEREGISTER 4

// RESPONSE CODE
#define RSP_OK 5
#define RSP_NOK 6

// digit count
#define DIGIT 10
int digits[DIGIT] = {0};

// Mutex variables

// listen backlog
#define BACKLOG 5

// Threads
#define MAXTHREADS 100

// Request Message structure
typedef struct requestMessage {
    int reqcode;
    int clientid;
    int data[DIGIT];
} req_message_t;

// Response Message structure
typedef struct responseMessage {
    int reqcode;
    int respcode;
    int data[DIGIT];
} resp_message_t;

#endif
