#Source file  
#SRC = wordcountMapper.cpp wordcountReducer.cpp
#Object file   
OBJ_MAPPER = wordcountMapper.o
OBJ_REDUCER = wordcountReducer.o  
#Output execution file  
MAPPER = wordcountMapper
REDUCER = wordcountReducer
#Compiler  
CC = g++  
#Include  
INCLUDE = -I/usr/include/  
#Linker Parameter  
LINKPARAM = -lrt
#Options for development  
#CFLAGS = -ansi -g -Wall  
#Options for release  
CFLAGS = -ansi -O2 -Wall
all: $(MAPPER) $(REDUCER)
$(MAPPER): $(OBJ_MAPPER)
	$(CC) $(CFLAGS) -o $(MAPPER) $(LINKPARAM) $(OBJ_MAPPER)

$(REDUCER): $(OBJ_REDUCER)
	$(CC) $(CFLAGS) -o $(REDUCER) $(LINKPARAM) $(OBJ_REDUCER)

clean: 
	-rm -f $(MAPPER) $(REDUCER)
	-rm -f *.o
