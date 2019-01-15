# Makefile for Project 1 count.count
# Author: Sam Wolfe

CC = gcc

all: count

count: count.c
	$(CC) count.c -o count
		
clean:
	rm count