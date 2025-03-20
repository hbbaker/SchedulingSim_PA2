#!/bin/bash

mkdir -p ./build
cd ./build

gcc ../process.c ../queue.c ../main.c -o PA2-build 

