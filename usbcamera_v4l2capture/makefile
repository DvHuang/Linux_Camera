all:video_capture

# which compiler
#CC = arm-linux-gcc
CC=gcc

#Where are include files kept
INCLUDE = .

# Options for development
CFLAGS = -O -Wall 

# Options for release
# CFLAGS - -O -Wall -ansi

video_capture:main.o capture.o
	$(CC) -o video_capture  main.o capture.o

main.o:main.c capture.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c main.c

capture.o:capture.c capture.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c capture.c

clean:
	-rm main.o capture.o
