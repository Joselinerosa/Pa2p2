# Pa2p2
Phase 2

## Team Information
1. Team members: Joseline Rosa & Peyton Tregembo
2. x500: Rosa0168 & Trege006
3. Division of tasks:
Client.c: Joseline.
Server.c: Peyton.
README.md: Peyton.
5. Lab machine used: SSH trege006@csel-khl1260-17.cselabs.umn.edu 
                     SSH rosa0168@csel-khl1260-17.cselabs.umn.edu 

### Phase 2 Design
We implemented a multithreaded client-server model where clients communicate with a server to register, send digit counts, retrieve the latest digit count and deregister. 
The SERVER listens for incoming client connections using (socket, bind, listen and accept), each connection creates a new thread using (pthread_create) and is handled concurrently. A global array stores cumulative digit frequencies across clients and a linked list tracks registered clients (protected by client_list_mutex). Another mutex ensures (digit_mutex) ensures therad-safe access to the shared digits array. 
The server handels four request types (register- adds a client to the list, digitcount- updates the digit counts, latestcount- returns the global digit count array, and deregister- removes client from the list). 
The CLIENT connects to the server and sends requests based on command-line options. Valid requests are packaged as requestMessage structs and transmitted via the socket. The client waits for a corresponding reponseMessage and displays the result. Each request includes a clientid and a reqcode with data[10] used when needed. 

## Challenges Faced
Peyton:
- Debugging multithreaded logic in VSCode
- ensuring proper synchronization of shared resoruces
- maintaining thread safety during register/deregister operations without leaking memory or corrupting the linked list
Joseline:
- Making sure the client part aligns with the server
- Understanding the logic behind having them talk to eachother
- Ensuring no memory leak
- Making sure that the correct output was printed


## AI Tools Usage
Peyton: I used AI to to verify correct usage of socket API functions and struct packing
Joseline: I used AI to correct my syntax for establishing the TCP connection


## Additional Notes
