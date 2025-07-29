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
 int connectToServer(int serverport) {
    int sockfd;
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket  failed");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(serverport);       
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

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

    // Join threads

        for (int i = 0; i < nthreads; i++) {
            pthread_join(threads[i], NULL);
        }

    
    // print per digit count
    printf("Digit counts:\n");
    for (int i = 0; i < 10; i++) {
        fprintf(stdout, "Digit %d: %d\n", i, queue->digit_count[i]);
    }   
    
    // establish TCP connection with server
    int sockfd = connectToServer(serverport);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }
     // register
    req_message_t req;
    resp_message_t resp;
    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.reqcode = REGISTER;
    req.clientid = clientid;

    send(sockfd, &req, sizeof(req), 0);
    if (recv(sockfd, &resp, sizeof(resp), 0) <= 0 || resp.respcode != RSP_OK) {
        fprintf(stderr, "Registration failed\n");
        close(sockfd);
        exit(EXIT_FAILURE);
}
    printf("Client %d registered successfully.\n", clientid);
    
   

    // digit
    req.reqcode = DIGITCOUNT;
    for (int i = 0; i < 10; i++) {
        req.data[i] = queue->digit_count[i]; // Fill request data with digit counts
    }
    send(sockfd, &req, sizeof(req), 0);
    recv(sockfd, &resp, sizeof(resp), 0);
    
    // latestcount
    req.reqcode = LATESTCOUNT;
    memset(req.data, 0, sizeof(req.data));
    send(sockfd, &req, sizeof(req), 0);
    recv(sockfd, &resp, sizeof(resp), 0);
    if (resp.respcode == RSP_OK) {
        printf("Latest digit counts received:\n");
        for (int i = 0; i < 10; i++) {
            printf("Digit %d: %d\n", i, resp.data[i]);
        }
    } else {
        fprintf(stderr, "Failed to digit counts.\n");
    }   

        // closing connection


    // De-register
    req.reqcode = DEREGISTER;
    req.clientid = clientid;
    memset(req.data, 0, sizeof(req.data)); 
    send(sockfd, &req, sizeof(req), 0);
    recv(sockfd, &resp, sizeof(resp), 0);
    if (resp.respcode == RSP_OK) {
        printf("Client %d deregistered successfully.\n", clientid);
    } else {
        fprintf(stderr, "Failed to deregister client %d.\n", clientid);
    }
    close(sockfd);
    
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    free(queue->chunks);
    free(queue);
    return 0;
}
