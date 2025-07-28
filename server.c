#include "include/server.h"

// Structure representing a node in the client linked list
typedef struct clientNode {
    int clientid; // Unique ID for the client
    struct clientNode *next; // Pointer to the next node
} client_node_t;

// Global linked list head for all connected clients
client_node_t *client_list = NULL;

// Mutex for synchronizing access to the client list
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;

// Mutex for synchronizing access to digit data
pthread_mutex_t digit_mutex = PTHREAD_MUTEX_INITIALIZER;

// Add a new client to the client list (returns 1 = success, 0 = already registered, -1 = error)
int add_client(int clientid) {
    pthread_mutex_lock(&client_list_mutex); // Lock the list before modification

    client_node_t *curr = client_list;
    while (curr) {
        if (curr->clientid == clientid) {
            pthread_mutex_unlock(&client_list_mutex); // Unlock and return if already registered
            return 0;
        }
        curr = curr->next;
    }

    client_node_t *new_node = malloc(sizeof(client_node_t));
    if (!new_node) {
        pthread_mutex_unlock(&client_list_mutex); // Unlock and return on malloc failure
        return -1;
    }

    new_node->clientid = clientid;
    new_node->next = client_list; // Insert at head of list
    client_list = new_node;

    pthread_mutex_unlock(&client_list_mutex); // Unlock after modification
    return 1;
}

// Remove a client from the client list (returns 1 = success, 0 = not found)
int remove_client(int clientid) {
    pthread_mutex_lock(&client_list_mutex); // Lock before modifying list

    client_node_t **curr = &client_list;
    while (*curr) {
        if ((*curr)->clientid == clientid) {
            client_node_t *to_delete = *curr;
            *curr = (*curr)->next; // Bypass node to delete
            free(to_delete);       // Free memory
            pthread_mutex_unlock(&client_list_mutex);
            return 1;
        }
        curr = &((*curr)->next);
    }

    pthread_mutex_unlock(&client_list_mutex); // Unlock if not found
    return 0;
}

// Thread function to handle client requests
void *clientHandler(void *args) {
    int client_sock = *(int *)args;
    free(args); // Free dynamically allocated socket descriptor

    req_message_t req;
    resp_message_t resp;

    while (1) {
        // Receive request from the client
        ssize_t recvd = recv(client_sock, &req, sizeof(req_message_t), 0);
        if (recvd == 0) {
            // Client closed connection
            break;
        } else if (recvd < 0) {
            perror("recv"); // Print error if recv fails
            break;
        }

        memset(&resp, 0, sizeof(resp)); // Clear response struct
        resp.reqcode = req.reqcode;// Echo back the request code

        // Handle request types
        if (req.reqcode == REGISTER) {
            // If client is requesting to register
            printf("Client [%d] registering with Server\n", req.clientid);

            // Try to add the client to the global list
            if (add_client(req.clientid) == 1) {
                // Registration succeeded
                printf("Client [%d] registered with Server\n", req.clientid);
                resp.respcode = RSP_OK; // Respond with success
                memset(resp.data, 0, sizeof(resp.data)); // Clear the response payload
                resp.data[0] = req.clientid;  // Echo client ID back as confirmation
            } else {
                // Registration failed (already registered or error)
                resp.respcode = RSP_NOK; // Respond with failure
            }
        }
        else if (req.reqcode == DIGITCOUNT) {
            // If client sends digit count update (e.g., histogram of digit usage)
            printf("Client [%d] shared digit count with Server\n", req.clientid);

            pthread_mutex_lock(&digit_mutex); // Lock shared digit array for thread safety

            // Add the client's submitted digit counts to the global digits array
            for (int i = 0; i < DIGIT; i++) {
                digits[i] += req.data[i]; // Accumulate digit counts
            }

            pthread_mutex_unlock(&digit_mutex); // Unlock digit array after update

            resp.respcode = RSP_OK; // Respond with success
            memset(resp.data, 0, sizeof(resp.data)); // Clear response payload
            resp.data[0] = req.clientid;  // Echo client ID
        }
        else if (req.reqcode == LATESTCOUNT) {
            // If client is requesting the latest digit count snapshot
            printf("Client [%d] requests current digit count\n", req.clientid);

            pthread_mutex_lock(&digit_mutex); // Lock digit array before reading
            resp.respcode = RSP_OK; // Respond with success
            memcpy(resp.data, digits, sizeof(digits));  // Copy current digit array into response
            pthread_mutex_unlock(&digit_mutex); // Unlock after reading
        }
        else if (req.reqcode == DEREGISTER) {
            // If client requests to deregister from server
            printf("Client [%d] deregistering\n", req.clientid);

            if (remove_client(req.clientid) == 1) {
                // Successfully removed from client list
                resp.respcode = RSP_OK; // Respond with success
                memset(resp.data, 0, sizeof(resp.data)); // Clear response payload
                resp.data[0] = req.clientid; // Echo client ID
            } else {
                // Client ID was not found in list
                resp.respcode = RSP_NOK; // Respond with failure
            }
        }
        else {
            // Request code is unrecognized or unsupported
            resp.respcode = RSP_NOK; // Send negative acknowledgment
        }


        // Send response back to client
        if (send(client_sock, &resp, sizeof(resp_message_t), 0) < 0) {
            perror("send"); // Handle send failure
            break;
        }

        // Close connection after deregistration
        if (req.reqcode == DEREGISTER) {
            break;
        }
    }

    close(client_sock); // Close client socket
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]); // Check argument count
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]); // Convert port number to integer

    // Create a TCP socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup server address struct
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_port = htons(port);       // Host to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

    // Bind socket to the specified port
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Start listening on the socket
    if (listen(server_sock, BACKLOG) < 0) {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    // Server loop to accept incoming client connections
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int *client_sock = malloc(sizeof(int)); // Allocate memory for new socket
        if (!client_sock) {
            perror("malloc");
            continue;
        }

        // Accept new client connection
        *client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (*client_sock < 0) {
            perror("accept");
            free(client_sock);
            continue;
        }

        // Create a new thread to handle the client
        pthread_t tid;
        if (pthread_create(&tid, NULL, clientHandler, client_sock) != 0) {
            perror("pthread_create");
            close(*client_sock);
            free(client_sock);
            continue;
        }

        pthread_detach(tid); // Detach thread to reclaim resources automatically
    }

    // Close server socket (unreachable code in infinite loop)
    close(server_sock);
    return 0;
}

