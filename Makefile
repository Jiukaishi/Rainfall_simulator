#----- Modify CC and CFLAGS as appropriate in your environment
CC = g++
CFLAGS = -O3 -fopenmp -std=c++11 -pg

LIB = -lm -lc

UNITFILES = 

#------------ EXECUTABLE FILES ---------------
all : rainfall_seq rainfall_pt rainfall_pt_v2
.PHONY : all

rainfall_seq : rainfall_seq.o $(UNITFILES)
	$(CC) $(CFLAGS) -o rainfall_seq rainfall_seq.o $(UNITFILES) $(LIB)


rainfall_seq.o : rainfall_seq.cpp
	$(CC) $(CFLAGS) -c rainfall_seq.cpp $(INCLUDE) 

rainfall_pt : rainfall_pt.o $(UNITFILES)
	$(CC) $(CFLAGS) -o rainfall_pt rainfall_pt.o $(UNITFILES) $(LIB)


rainfall_pt.o : rainfall_pt.cpp
	$(CC) $(CFLAGS) -c rainfall_pt.cpp $(INCLUDE) 


rainfall_pt_v2 : rainfall_pt_v2.o $(UNITFILES)
	$(CC) $(CFLAGS) -o rainfall_pt_v2 rainfall_pt_v2.o $(UNITFILES) $(LIB)


rainfall_pt_v2.o : rainfall_pt_v2.cpp
	$(CC) $(CFLAGS) -c rainfall_pt_v2.cpp $(INCLUDE) 



clean:
	rm -f rainfall_seq rainfall_pt rainfall_pt_v2 *.o
	