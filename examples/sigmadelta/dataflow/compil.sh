#!/bin/bash
g++ -O3 -o $1 $1.cpp `pkg-config --cflags --libs hpx_application` -lhpx_component_dataflow -DHPX_APPLICATION_NAME=$1 -DHPX_ACTION_ARGUMENT_LIMIT=15 -DHPX_COMPONENT_CREATE_ARGUMENT_LIMIT=15 -DHPX_FUNCTION_LIMIT=18
