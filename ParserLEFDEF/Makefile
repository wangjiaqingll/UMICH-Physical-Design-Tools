#/**************************************************************************
#***    
#*** Copyright (c) 1995-2000 Regents of the University of California,
#***               Andrew E. Caldwell, Andrew B. Kahng and Igor L. Markov
#*** Copyright (c) 2000-2002 Regents of the University of Michigan,
#***               Saurabh N. Adya and Igor L. Markov
#***
#***  Contact author(s): abk@cs.ucsd.edu, imarkov@umich.edu
#***  Original Affiliation:   UCLA, Computer Science Department,
#***                          Los Angeles, CA 90095-1596 USA
#***
#***  Permission is hereby granted, free of charge, to any person obtaining 
#***  a copy of this software and associated documentation files (the
#***  "Software"), to deal in the Software without restriction, including
#***  without limitation 
#***  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
#***  and/or sell copies of the Software, and to permit persons to whom the 
#***  Software is furnished to do so, subject to the following conditions:
#***
#***  The above copyright notice and this permission notice shall be included
#***  in all copies or substantial portions of the Software.
#***
#*** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
#*** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#*** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
#*** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#*** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
#*** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
#*** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#***
#***
#***************************************************************************/


.SUFFIXES:	.o .cxx .exe .exeS .exeP

 OPTFLAGS = -g -DABKDEBUG
#OPTFLAGS= -O5 # use for optimization instead of the above
CCFLAGS  = -G -PIC +p     # comment +w/+w2 when compiling with O5
DEPENDOPT= -xM1
CC       = CC $(OPTFLAGS) $(CCFLAGS)

 # could be g++
CC4LIBS          = CC6
LD       = CC -G
LDFINAL  = CC
AR       = ar cr

LEX      = flex 
LFLAGS   = -it
OFLAG    = -c
YACC     = bison
YFLAGS   = -dv

SODIRKEY =  -R
SONAMEKEY        = -R # change to -h if you need "immobile"dynamic libraries.
                      # LD_LIBRARY_PATH overrides -h
                      # Make sure there is no conflict
             
PURIFY   = purify -cache_dir=cache -windows=no -log_file=purify.log

UCLA             = /home/code/OUR/packages
IMPORT           = /home/code/IMPORT
 LIBBASE         = /home/code/lib/$(CC4LIBS)/g
#LIBBASE          = /home/code/lib/$(CC4LIBS)/O5
SLIBBASE         = $(LIBBASE)/static
STL              =
#  -I$(IMPORT)/STL/CC

THISLIB = ParserLEFDEF
ARNAME   = lib$(THISLIB).a
SONAME   = lib$(THISLIB).so
SODIR    = $(PWD)
SOFULL   = $(SODIR)/$(SONAME)
SOLABEL  = $(SONAMEKEY)$(SOFULL)
TESTNAME = $(THISLIB)Test

INCLUDES = $(STL) -I$(UCLA)

fOBJS      = frontend.o parseArgs.o
pOBJS      = lefparse.tab.o lex.lef.o defparse.tab.o lex.def.o
dbOBJS     = DBparseLEFDEF.o parserLEFDEF.o 
OBJS       = $(dbOBJS) $(pOBJS) $(fOBJ)
SRCS       = $(OBJS:%.o=%.cxx)


LIBPATHS   = $(SODIRKEY)$(LIBBASE)
QLIBS      = -ParserLEFDEFq -lCadenceLEF -lCadenceDEF
LIBS       = -L. $(SODIRKEY). -l$(THISLIB) $(LIBPATHS) -L$(LIBBASE) -lDB \
             -lParserLEFDEFq -lCadenceLEF -lCadenceDEF \
	     -lConstraints -lPlacement  -lCtainers \
	     -lStats -lCombi -lGeoms -lABKCommon  -lfl
# $(QLIBS)
SLIBS      = -L$(SLIBBASE) -L$(PWD) -l$(THISLIB) -lDB -lParserLEFDEFq\
	     -lCadenceLEF -lCadenceDEF \
	     -lConstraints -lPlacement -lStats -lCombi -lGeoms -lABKCommon -lfl
# $(QLIBS)

#EXTRALIBS  = -lRBPlace

all : lib

lib : $(OBJS) 
	/bin/rm -f $(ARNAME) $(SONAME)
	$(AR) $(ARNAME) $(OBJS)
	$(LD) -o $(SODIR)/$(SONAME) $(OBJS) $(SOLABEL)

1.o : 1.cxx
	$(CC) $(INCLUDES) $(EXTRAINCLUDES) -c 1.cxx

2.o : 2.cxx
	$(CC) $(INCLUDES) $(EXTRAINCLUDES) -c 2.cxx

9.o : 9.cxx
	$(CC) $(INCLUDES) $(EXTRAINCLUDES) -c 9.cxx

test: ParserLEFDEFTest0.exe ParserLEFDEFTest2.exe ParserLEFDEFTest3.exe ParserLEFDEFTest4.exe

.cxx.o:
	$(CC) $(INCLUDES) -c $*.cxx
.c.o:
	$(CC) $(INCLUDES) -c $*.c

.cxx.exe:
	/bin/rm -f $*.exe
	$(CC) $(INCLUDES) -c $*.cxx
	$(LDFINAL) -o $*.exe $*.o $(LIBS)

.cxx.exeS:
	/bin/rm -f $*.exeS
	$(CC) $(INCLUDES) -c $*.cxx
	$(LDFINAL) -o $*.exeS $*.o $(SLIBS)

.cxx.exeP:
	/bin/rm -f $*.exeP
	$(CC) $(INCLUDES) -c $*.cxx
	$(PURIFY) $(LDFINAL) -O $*.exeP $*.o $(LIBS)

depend:
	/bin/rm -f depend.mk

depend.mk: $(SRCS)
	$(CC) $(DEPENDOPT) $(INCLUDES) $(SRCS) *.cxx >depend.mk

#include depend.mk

clean:;
	/bin/rm -rf *.o Templates.DB lib* ldp *.zip ir.out depend.mk
	/bin/rm -rf $(CFILES) cache .pure *.output *.tab.h core SunWS_cache
	/bin/rm -rf scanopen.net scanopen.net.trn parserout
	/bin/rm -rf lex.lef.cxx lex.def.cxx defparse.tab.cxx lefparse.tab.cxx\
		    lefparse.out defparse.out lefparse.tab.h defparse.tab.h
	/bin/rm -rf *.exe *.exe? new.* *.notime *def

#=============================================================================
parser: $(pOBJS) $(fOBJS)
	/bin/rm -rf parser
	$(LDFINAL) $(fOBJS) $(SODIR)/$(SONAME) $(LIBS) -o ldp

parserP: $(pOBJS) $(fOBJS)
	$(PURIFY) $(LDFINAL) $(fOBJS) $(pOBJS) $(LIBS) -o ldp
#	/bin/rm -rf parser $(SODIR)/$(SONAME)

lefScanDeb:$(CONSTpOBJS) $(fOBJS) lefparse.tab.o LLdebO defparse.tab.o lex.def.o
	$(CC) $(pOBJS) $(CONSTpOBJS) $(fOBJS)      -o ldp

lefParseDeb:$(CONSTpOBJS) $(fOBJS) LTdebO lex.lef.o defparse.tab.o lex.def.o
	$(CC) $(pOBJS) $(CONSTpOBJS) $(fOBJS)      -o ldp

lefBothDeb:$(CONSTpOBJS) $(fOBJS) LTdebO LLdebO defparse.tab.o lex.def.o
	$(CC) $(pOBJS) $(CONSTpOBJS) $(fOBJS)      -o ldp

defScanDeb:$(CONSTpOBJS) $(fOBJS) lefparse.tab.o lex.lef.o defparse.tab.o LDdebO
	$(CC) $(pOBJS) $(CONSTpOBJS) $(fOBJS)       -o ldp

defParseDeb:$(CONSTpOBJS) $(fOBJS) lefparse.tab.o lex.lef.o DTdebO lex.def.o
	$(CC) $(pOBJS) $(fOBJS) $(CONSTpOBJS)      -o ldp

defBothDeb:$(CONSTpOBJS) $(fOBJS) lefparse.tab.o lex.lef.o DTdebO LDdebO
	$(CC) $(pOBJS) $(CONSTpOBJS) $(fOBJS)      -o ldp

lex.lef.o:lex.lef.cxx lefparse.tab.cxx
	$(CC) $(OFLAG) lex.lef.cxx $(INCLUDES)

lex.lef.cxx:lefscan.l
	$(LEX) $(LFLAGS) -Plef lefscan.l | sed -f sed-file > lex.lef.cxx

lefparse.tab.o:lefparse.tab.cxx
	$(CC) $(OFLAG) lefparse.tab.cxx $(INCLUDES)

lefparse.tab.cxx:lefparse.y
	$(YACC) $(YFLAGS) -p lef lefparse.y
	mv lefparse.tab.c lefparse.tab.cxx

lex.def.o:lex.def.cxx defparse.tab.cxx
	$(CC) $(OFLAG) lex.def.cxx $(INCLUDES)

lex.def.cxx:defscan.l
	$(LEX) $(LFLAGS) -Pdef defscan.l | sed -f sed-file > lex.def.cxx

defparse.tab.o:defparse.tab.cxx
	$(CC) $(OFLAG) defparse.tab.cxx $(INCLUDES)

defparse.tab.cxx:defparse.y
	$(YACC) $(YFLAGS) -p def defparse.y
	mv defparse.tab.c defparse.tab.cxx

parseArgs.o:parseArgs.cxx
	$(CC) $(OFLAG) parseArgs.cxx $(INCLUDES)

frontend.o:frontend.cxx
	$(CC) $(OFLAG) frontend.cxx $(INCLUDES)


LLdebO: LLdeb lefparse.tab.h
	$(CC) $(INCLUDES) $(OFLAG) lex.lef.cxx 

LLdeb:lefscan.l
	$(LEX) $(LFLAGS)d  -Plef lefscan.l | sed -f sed-file > lex.lef.cxx

LDdebO: LDdeb defparse.tab.h
	$(CC) $(INCLUDES) $(OFLAG) lex.def.cxx

LDdeb:defscan.l
	$(LEX) $(LFLAGS)d  -Pdef defscan.l | sed -f sed-file > lex.def.cxx

DTdebO: DTdeb
	$(CC) $(INCLUDES) $(OFLAG) defparse.tab.cxx

DTdeb:defparse.y
	$(YACC) $(YFLAGS)t -p def defparse.y
	mv defparse.tab.c defparse.tab.cxx

LTdebO: LTdeb
	$(CC) $(INCLUDES) $(OFLAG) -DBISON_DEB lefparse.tab.cxx

LTdeb:lefparse.y
	$(YACC) $(YFLAGS)t -p lef lefparse.y
	mv lefparse.tab.c lefparse.tab.cxx
