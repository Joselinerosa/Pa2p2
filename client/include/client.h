#ifndef CLIENT_H_
#define CLIENT_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <pthread.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DIGIT 10

// Queue structure, feel free to modify accoding to your requirements
struct QueueStruct {
    char chunk[1024];
};

// Shared data structure to keep track of the digit count
int digits[DIGIT] = {0};

// Mutex variables

// Condition variables

// Graceful exit


// REQUEST CODE
#define REGISTER 1
#define DIGITCOUNT 2
#define LATESTCOUNT 3
#define DEREGISTER 4

// RESPONSE CODE
#define RSP_OK 5
#define RSP_NOK 6

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
