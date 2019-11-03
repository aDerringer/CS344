# Source, Executable, Includes, Library Defines
INCL   = 
SRC    = derringa.adventure.c
OBJ    = $(SRC:.c=.o)
LIBS   = 
EXE    = derringa.buildrooms
BIN    = $(EXE).bin

# Compiler, Linker Defines
CC      = /usr/bin/gcc
CFLAGS  = -ansi -pedantic -Wall -O2 -std=c99
LIBPATH = 
LDFLAGS = -o $(EXE) $(LIBPATH) $(LIBS)
CFDEBUG =  -ansi -pedantic -Wall -g -DDEBUG $(LDFLAGS)
RM      = /bin/rm -f
VAL     = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes

# Compile and Assemble C Source Files into Object Files
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c

# Link all Object Files with external Libraries into Binaries
$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -lpthread

# Objects depend on these Libraries
$(OBJ): $(INCL)

# Create a gdb/dbx Capable Executable with DEBUG flags turned on
debug: 
	$(VAL) ./$(EXE)

# Clean Up Objects, Exectuables, Dumps out of source directory
clean:
	$(RM) $(OBJ) $(EXE)
