# Copyright (c) Axway 2006-2012 - Synchrony Transfer CFT
# Makefile for APIXMP1, API2XMP1, API2XMP2 and TCFTSYN
#

OBJ=./obj
SRC=./src
EXE=./bin

CC=$(CFT_CC)
LD=$(CFT_LD)
CFLAGS=-I$(CFTDIRINSTALL)/inc $(CFT_CFLAGS)
LFLAGS=-L$(CFTDIRINSTALL)/lib $(CFT_LFLAGS)

TARGETS=$(EXE)/queryTest $(EXE)/recv_sample $(EXE)/send_sample $(EXE)/sync_sample 

ALL:$(TARGETS)

$(EXE)/queryTest : $(OBJ)/queryTest.o $(OBJ)/fjnxAxway.o
	$(LD) $(LFLAGS)  $(OBJ)/queryTest.o $(OBJ)/fjnxAxway.o -o $(EXE)/queryTest -lcftapi

$(EXE)/recv_sample : $(OBJ)/recv_sample.o $(OBJ)/fjnxAxway.o
	$(LD) $(LFLAGS)  $(OBJ)/recv_sample.o $(OBJ)/fjnxAxway.o -o $(EXE)/recv_sample -lcftapi

$(EXE)/send_sample : $(OBJ)/send_sample.o $(OBJ)/fjnxAxway.o
	$(LD) $(LFLAGS) $(OBJ)/send_sample.o $(OBJ)/fjnxAxway.o -o $(EXE)/send_sample -lcftapi
	
$(EXE)/sync_sample : $(OBJ)/sync_sample.o $(OBJ)/fjnxAxway.o
	$(LD) $(LFLAGS)  $(OBJ)/sync_sample.o $(OBJ)/fjnxAxway.o -o $(EXE)/sync_sample -lcftapi

$(OBJ)/fjnxAxway.o  :	$(SRC)/fjnxAxway.c 
	$(CC) -c $(SRC)/fjnxAxway.c -o $(OBJ)/fjnxAxway.o $(CFLAGS) 
$(OBJ)/recv_sample.o : $(SRC)/recv_sample.c
	$(CC) -c $(SRC)/recv_sample.c -o $(OBJ)/recv_sample.o $(CFLAGS)  
$(OBJ)/send_sample.o : $(SRC)/send_sample.c
	$(CC) -c $(SRC)/send_sample.c -o $(OBJ)/send_sample.o $(CFLAGS)
$(OBJ)/sync_sample.o : $(SRC)/sync_sample.c
	$(CC) -c $(SRC)/sync_sample.c -o $(OBJ)/sync_sample.o $(CFLAGS)
$(OBJ)/queryTest.o : $(SRC)/queryTest.c
	$(CC) -c $(SRC)/queryTest.c -o $(OBJ)/queryTest.o $(CFLAGS)

#.c.o:
#	$(CC) -c $< -o $@ $(CFLAGS)   



clean :
	rm -vf $(OBJ)/*.o
distclean: clean
	rm $(TARGETS)



