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
typedef struct queue_t {
    char **chunks; // Array of strings to hold chunks
    int done; // Flag to indicate if processing is done
    int front; // Index of the front of the queue
    int rear; // Index of the rear of the queue
    int size; // Current size of the queue
    int capacity; // Maximum capacity of the queue
    pthread_mutex_t mutex; // Mutex for thread safety
    pthread_cond_t not_empty; // Condition variable for not empty
    pthread_cond_t not_full; // Condition variable for not full
    int digit_count[10]; // Count of digits 0-9
} queue_t;
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

