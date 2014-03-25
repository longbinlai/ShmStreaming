#Source file  
#SRC = wordcountMapper.cpp wordcountReducer.cpp
#Object file   
OBJ_MAPPER = textsortMapper.o
OBJ_REDUCER = textsortReducer.o  
#Output execution file  
MAPPER = textsortMapper
REDUCER = textsortReducer
#Compiler  
CC = g++  
#Include  
INCLUDE = -I/usr/include/  
#Linker Parameter  
LINKPARAM = -lboost_regex -lrt
#Options for development  
#CFLAGS = -ansi -g -Wall  
#Options for release  
CFLAGS = -ansi -O2 -Wall  
all: $(MAPPER) $(REDUCER)
$(MAPPER): $(OBJ_MAPPER)
	$(CC) -o $(MAPPER) $(CFLAGS) $(LINKPARAM) $(OBJ_MAPPER)

$(REDUCER): $(OBJ_REDUCER)
	$(CC) -o $(REDUCER) $(CFLAGS) $(LINKPARAM) $(OBJ_REDUCER)

clean: 
	-rm -f $(MAPPER) $(REDUCER)
	-rm -f *.o
