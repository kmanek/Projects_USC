#!/bin/bash
gcc node1send.c -o node1send -lpthread -g
gcc node2f3.c -o node2f3 -lpthread -g 
gcc node2f1.c -o node2f1 -lpthread -g 
gcc node3f1.c -o node3f1 -lpthread -g
gcc node3f2.c -o node3f2 -lpthread -g 
gcc node1f3.c -o node1f3 -lpthread -g
gcc node1f2.c -o node1f2 -lpthread -g
gcc node2send.c -o node2send -lpthread -g
gcc node3send.c -o node3send -lpthread -g
