#     gmake concord
#        Makes concord executable
#
#     gmake grades
#        Makes grades executable
#
#     gmake pa5list
#	 Makes pa5list executable.  (This is a test driver for your list functions)
#
#     gmake submit
#        Submits the assignment.
#

# need to use gmake

all: client.o edge.o server_or.o server_and.o
	g++ -ggdb -Wall -Wno-sign-compare -o client client.o
	g++ -ggdb -Wall -Wno-sign-compare -o edge edge.o
	g++ -ggdb -Wall -Wno-sign-compare -o server_or server_or.o
	g++ -ggdb -Wall -Wno-sign-compare -o server_and server_and.o

.PHONY: server_or
server_or:
	./server_or

.PHONY: server_and
server_and:
	./server_and

.PHONY: edge
edge:
	./edge

client.o: client.h client.cpp
	g++ -ggdb -Wall -Wno-sign-compare -c client.cpp

edge.o: edge.h edge.cpp
	g++ -ggdb -Wall -Wno-sign-compare -c edge.cpp

server_or.o: backend_server.h server_or.cpp
	g++ -ggdb -Wall -Wno-sign-compare -c server_or.cpp

server_and.o: backend_server.h server_and.cpp
	g++ -ggdb -Wall -Wno-sign-compare -c server_and.cpp

clean:
	\rm -f *.o *~ client edge server_or server_and

