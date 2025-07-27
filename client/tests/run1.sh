#!/bin/bash

# Always run the server in another terminal and then run the testcase. You may change the port number.
make clean
make

./client ../testcase/t1/client1.txt 5 10 1 8080 > output/client1.txt