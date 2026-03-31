# Bibleajax program
# Bob Kasper, MVNU Computer Science
# updated January 2020 to use
# c++11 compiler option, current paths on cs.mvnu.edu

# This is a Makefile for the Bible web app demo program.
# Copy this directory to a location within your home directory.
# Change the USER name value below to your own user name.
# Then use "make" to build the server program,
# and deploy it to the live web server directory.
# To test the program, go to http://cs.mvnu.edu/class/csc3004/USER/
# and open the bibleajax.html link.

# TODO: Must replace "username" by your username
USER= elilitman

# Use GNU C++ compiler with C++11 standard
CC= g++
CFLAGS= -g -std=c++11

all:	lookupserver bibleindex.cgi PutCGI PutHTML
#all:	lookupserver

# Build the lookupserver executable
lookupserver: lookupserver.o Bible.o Ref.o Verse.o fifo.o
	$(CC) $(CFLAGS) -o lookupserver lookupserver.o Bible.o Ref.o Verse.o fifo.o

# lookupserver
lookupserver.o: lookupserver.cpp fifo.h Bible.h Ref.h Verse.h
	$(CC) $(CFLAGS) -c lookupserver.cpp

# Fifo Object
fifo.o: fifo.cpp fifo.h
	$(CC) $(CFLAGS) -c fifo.cpp

# compiled classes from Project 1 to be linked into the executable program
bibleindex.cgi:	bibleindex.o fifo.o
	$(CC) $(CFLAGS) -o bibleindex.cgi bibleindex.o -lcgicc fifo.o
	# -l option is necessary to link with cgicc library

# main program to handle AJAX/CGI requests for Bible references
bibleindex.o:	bibleindex.cpp logfile.h
	$(CC) $(CFLAGS) -c bibleindex.cpp

# Ref Object
Ref.o : Ref.h Ref.cpp
	$(CC) $(CFLAGS) -c Ref.cpp

# Verse Object
Verse.o : Ref.h Verse.h Verse.cpp
	$(CC) $(CFLAGS) -c Verse.cpp

# Bible Object
Bible.o : Ref.h Verse.h Bible.h Bible.cpp
	$(CC) $(CFLAGS) -c Bible.cpp

PutCGI:	bibleindex.cgi
	rm -f /var/www/html/class/csc3004/elilitman/cgi-bin/bibleindex.cgi
	chmod 755 bibleindex.cgi
	cp bibleindex.cgi /var/www/html/class/csc3004/$(USER)/cgi-bin

	echo "Current contents of your cgi-bin directory: "
	ls -l /var/www/html/class/csc3004/$(USER)/cgi-bin/

PutHTML:
	cp bibleindex.html /var/www/html/class/csc3004/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/csc3004/$(USER)

clean:
	rm *.o core bibleindex.cgi
