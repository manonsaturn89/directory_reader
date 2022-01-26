.PHONY = all clean

CC = g++-10
CFLAGS = -pthread -g -Wall -pthread -O3  -std=gnu++1z

all: main.cpp File.h DirectoryTree.h sync_cout.h ThreadPool.h
	$(CC) $(CFLAGS) $< -o alessandro_grassi
	
clean:
	$(RM) alessandro_grassi
