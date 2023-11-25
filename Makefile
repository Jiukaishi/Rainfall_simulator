#----- Modify CC and CFLAGS as appropriate in your environment
CC = g++
CFLAGS = -O3 -fopenmp -std=c++11

LIB = -lm -lc

UNITFILES = 

#------------ EXECUTABLE FILES ---------------
all : rainfall_seq rainfall_pt
.PHONY : all

rainfall_seq : rainfall_seq.o $(UNITFILES)
	$(CC) $(CFLAGS) -o rainfall_seq rainfall_seq.o $(UNITFILES) $(LIB)


rainfall_seq.o : rainfall_seq.cpp
	$(CC) $(CFLAGS) -c rainfall_seq.cpp $(INCLUDE) 

rainfall_pt : rainfall_pt.o $(UNITFILES)
	$(CC) $(CFLAGS) -o rainfall_pt rainfall_pt.o $(UNITFILES) $(LIB)


rainfall_pt.o : rainfall_pt.cpp
	$(CC) $(CFLAGS) -c rainfall_pt.cpp $(INCLUDE) 



clean:
	rm -f rainfall_seq rainfall_pt *.o