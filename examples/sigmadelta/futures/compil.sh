#!/bin/bash
g++ -O3 -o $1 $1.cpp `pkg-config --cflags --libs hpx_application` -DHPX_APPLICATION_NAME=$1

