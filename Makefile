CC	=	g++
CFLAGS	=	-ggdb3 -Wall -Woverloaded-virtual -Wno-deprecated
#CC	  =	CC
#CFLAGS  =	-g +p +w
GCFLAGS =	-g -Wno-deprecated
LDFLAGS =	-Wno-deprecated
DPFLAGS =	-MM

BASESRC =       symtab.cc string.cc ast.cc codegen.cc main.cc
SOURCES =	$(BASESRC) parser.cc scanner.cc
BASEHDR =	symtab.hh ast.hh string.hh codegen.hh
HEADERS =       $(BASEHDR) parser.hh
OBJECTS =	$(SOURCES:%.cc=%.o)
OUTFILE =	compiler

FLEX	= /home/TDDB29/bin/flex
BISON	= bison

DPFILE  =	Makefile.dependencies

all : $(OUTFILE)

$(OUTFILE) : $(OBJECTS)
	$(CC) -o $(OUTFILE) $(OBJECTS) $(LDFLAGS)

parser.hh parser.cc : parser.y Makefile
	$(BISON) --defines --output-file=parser.cc parser.y

scanner.cc : scanner.l parser.hh Makefile
	$(FLEX) scanner.l

scanner.o : scanner.cc
	$(CC) $(GCFLAGS) -c $<

parser.o : parser.cc
	$(CC) $(GCFLAGS) -c $<

.cc.o: $(DPFILE) Makefile
	$(CC) $(CFLAGS) -c $<

clean : 
	rm -f $(OBJECTS) core *~ scanner.cc parser.cc parser.hh $(DPFILE) $(OUTFILE) parser.cc.output
	touch $(DPFILE)


$(DPFILE) depend : $(BASESRC) $(HEADERS)
	$(CC) $(DPFLAGS) $(CFLAGS) $(BASESRC) > $(DPFILE)

include $(DPFILE)
