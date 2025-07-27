#include "include/client.h"

// For phase 2, you may either process the given file using multiple threads or directly. If you are going to directly parse the file and store the digit count, feel free to remove processChunks
// Phase 1 code will not influence phase 2 grades

// Function executed by each thread
struct queue_t {
    char **chunks; // Array of strings to hold chunks
    int front; // Index of the front of the queue
    int rear; // Index of the rear of the queue
    int size; // Current size of the queue
    int capacity; // Maximum capacity of the queue
    pthread_mutex_t mutex; // Mutex for thread safety
    pthread_cond_t not_empty; // Condition variable for not empty
    pthread_cond_t not_full; // Condition variable for not full
    int digit_count[10]; // Count of digits 0-9
};
void *processChunks(void *args) {
    
    while(1){
        // wait for main to add chunks (condvar)
        

        // done condition (a variable and condvar)
        
        // pick chunk from queue and signal main about available free spot

        // store the per digit count 

        // Critical sections which are independent should be handled independently
    }
    return NULL;
}

// main thread
int main(int argc, char *argv[]) {
    // Argument checking
    if (argc != 6) {
        printf("ERROR: Usage: ./client <filepath> <qsize> <nthreads> <clientid> <serverport>\n");
        exit(EXIT_FAILURE);
    }

    char *filepath = argv[1];
    int qsize = atoi(argv[2]);
    int nthreads = atoi(argv[3]);
    int clientid = atoi(argv[4]);
    int serverport = atoi(argv[5]);
    // Create queue
    queue_t *queue = createQueue(qsize);
    if (queue == NULL) {
        perror("Queue creation failed");
        exit(EXIT_FAILURE);
    }   

    // Create threads
    pthread_t threads[nthreads];
    for (int i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, processChunks, (void *)queue) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // Partition file and push to queue only if there is space in the queue
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        perror("File opening failed");
        exit(EXIT_FAILURE);
    }   
    if (queue < size ){
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            // Process the line and push to queue
            // Ensure to handle the queue size limit
            if (enqueue(queue, buffer) == -1) {
                fprintf(stderr, "Queue is full, waiting for space...\n");
                // Wait for a signal that space is available
                pthread_cond_wait(&queue->not_full, &queue->mutex);
            }
        }
        fclose(file);
    } else {
        fprintf(stderr, "Queue is full, cannot process file.\n");
        exit(EXIT_FAILURE);
    }
    
    // Graceful exit condition for threads?
    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }   
    
    // Join threads
    pthread_join(threads[i], NULL);

    // print per digit count
    for (int i = 0; i < 10; i++) {
        fprintf(stdout, "Digit %d: %d\n", i, queue->digit_count[i]);
    }   
    
    // establish TCP connection with server
    
    // register

    // digit
    
    // latestcount
    // print latest per digit count

    // De-register

    return 0;
}
