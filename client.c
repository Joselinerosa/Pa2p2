#include "include/client.h"

// For phase 2, you may either process the given file using multiple threads or directly. If you are going to directly parse the file and store the digit count, feel free to remove processChunks
// Phase 1 code will not influence phase 2 grades

// Function executed by each thread
struct queue_t *createQueue(int capacity){
    queue_t *queue = malloc(sizeof(queue_t));
    if (!queue) return NULL;
    
    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    queue->done = 0; // Initialize done condition
    queue->chunks = malloc(capacity * sizeof(char *));
    if (!queue->chunks) {
        free(queue);
        return NULL;
    }
    
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    
    for (int i = 0; i < 10; i++) {
        queue->digit_count[i] = 0;
    }
    
    return queue;
}
    

int enqueue(queue_t *queue, char *chunk) {
    pthread_mutex_lock(&queue->mutex);
    
    while (queue->size == queue->capacity) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }
    
    queue->chunks[queue->rear] = chunk;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;
    
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    
    return 0;
}
char *dequeue(queue_t *queue) {
    char *chunk = queue->chunks[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    pthread_cond_signal(&queue->not_full);
    return chunk;
}


void *processChunks(void *args) {
    queue_t *queue = (queue_t *)args;
    
    while(1){
        // wait for main to add chunks (condvar)
        pthread_mutex_lock(&queue->mutex);
        while (queue->size == 0 && !queue->done) {
            pthread_cond_wait(&queue->not_empty, &queue->mutex);
        }   

        

        // done condition (a variable and condvar)
        // Check for a condition to exit the loop, e.g., a special chunk or a flag
        if (queue->size == 0 && queue->done) { 
            pthread_mutex_unlock(&queue->mutex);
            break; // Exit the loop if done condition is met
        }
        
        // pick chunk from queue and signal main about available free spot
        char *chunk = dequeue(queue);
        pthread_mutex_unlock(&queue->mutex);        

        // store the per digit count 
        for (int i = 0; chunk[i] != '\0'; i++) {
            if (chunk[i] >= '0' && chunk[i] <= '9') {
                int digit = chunk[i] - '0';
                pthread_mutex_lock(&queue->mutex);
                queue->digit_count[digit]++;
                pthread_mutex_unlock(&queue->mutex);
            }
        }
free(chunk); // Free the chunk after processing
    }
    return NULL;
}
        // Critical sections which are independent should be handled independently
        // Signal main thread that a chunk has been processed

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
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            // Process the line and push to queue
            // Ensure to handle the queue size limit
            char *chunk = strdup(buffer); // Duplicate the string to avoid using the same buffer
            if (chunk == NULL) {
                perror("Memory allocation failed");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            enqueue(queue, chunk);
        }

        fclose(file);
        pthread_mutex_lock(&queue->mutex);
        queue->done = 1;
        pthread_cond_broadcast(&queue->not_empty);
        pthread_mutex_unlock(&queue->mutex);

        for (int i = 0; i < nthreads; i++) {
            pthread_join(threads[i], NULL);
        }

    
    // Join threads
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
    
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    free(queue->chunks);
    free(queue);
    return 0;
}
