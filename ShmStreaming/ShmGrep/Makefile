#Source file  
#SRC = wordcountMapper.cpp wordcountReducer.cpp
#Object file   
OBJ_MAPPER = grepMapper.o
OBJ_REDUCER = grepReducer.o  
#Output execution file  
MAPPER = grepMapper
REDUCER = grepReducer
#Compiler  
CC = g++  
#Include  
INCLUDE = -I/usr/include/  
#Linker Parameter  
LINKPARAM = -lrt -lboost_regex
#Options for development  
#CFLAGS = -ansi -g -Wall  
#Options for release  
CFLAGS = -ansi -O2 -Wall  
all: $(MAPPER) $(REDUCER)
$(MAPPER): $(OBJ_MAPPER)
	$(CC) -o $(MAPPER) $(LINKPARAM) $(OBJ_MAPPER)

$(REDUCER): $(OBJ_REDUCER)
	$(CC) -o $(REDUCER) $(LINKPARAM) $(OBJ_REDUCER)

clean: 
	-rm -f $(MAPPER) $(REDUCER)
	-rm -f *.o
