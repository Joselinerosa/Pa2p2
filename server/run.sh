#!/bin/bash

make clean
make

./server 8080 > output/server.txt