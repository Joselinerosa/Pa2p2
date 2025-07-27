#!/bin/bash

qsize=5
nthreads=10
nclients=20
port=8080
testpath=../testcase/t4/client
outpath=output/client
pgm=./client

make clean
make


for (( i = 1; i <= ${nclients}; i++ ))
    do
        ${pgm} ${testpath}${i}.txt ${qsize} ${nthreads} ${i} ${port} > ${outpath}${i}.txt &
    done