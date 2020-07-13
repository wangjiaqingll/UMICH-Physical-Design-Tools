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

UCLA     = /home/code/OUR/packages
CADENCELEF  = $(UCLA)/CadenceLEF/include
CADENCEDEF  = $(UCLA)/CadenceDEF/include
IMPORT   = NOT_USED
LIBBASE  = /home/code/lib/$(CC4LIBS)/g
#LIBBASE          = /home/code/lib/$(CC4LIBS)/O5
SLIBBASE         = $(LIBBASE)/static
STL              =
#  -I$(IMPORT)/STL/CC

THISLIB = ParserLEFDEFq
ARNAME   = lib$(THISLIB).a
SONAME   = lib$(THISLIB).so
SODIR    = $(PWD)
SOFULL   = $(SODIR)/$(SONAME)
SOLABEL  = $(SONAMEKEY)$(SOFULL)
TESTNAME = $(THISLIB)Test

INCLUDES = $(STL) -I$(UCLA) -I$(CADENCELEF) -I$(CADENCEDEF)

fOBJS      = 
pOBJS      = 
dbOBJS     = DBparseLEFDEFq.o ParserLEFDEFq.o 
OBJS       = $(dbOBJS) $(pOBJS) $(fOBJ)
SRCS       = $(OBJS:%.o=%.cxx)


LIBPATHS   = $(SODIRKEY)$(LIBBASE)
QLIBS      = -lParserLEFDEFq -lParserLEFDEF -lnsl -lsocket -lintl
LIBS       = -L. $(SODIRKEY). -l$(THISLIB) $(LIBPATHS) -L$(LIBBASE) -lDB \
             -lConstraints -lPlacement  -lCtainers -lParserLEFDEF \
	     -lStats -lCombi -lGeoms -lABKCommon -lCadenceLEF -lCadenceDEF -lfl
# $(QLIBS)
SLIBS      = -L$(SLIBBASE) -L$(PWD) -l$(THISLIB) -lDB \
             -lConstraints -lPlacement -lParserLEFDEF\
	     -lStats -lCombi -lGeoms -lABKCommon -lCadenceLEF -lCadenceDEF -lfl
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

test: ParserLEFDEFqTest0.exe ParserLEFDEFqTest2.exe ParserLEFDEFqTest3.exe ParserLEFDEFqTest4.exe

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
	$(LDFINAL) -o $*.exeS $*.o -static $(SLIBS)

.cxx.exeP:
	/bin/rm -f $*.exeP
	$(CC) $(INCLUDES) -c $*.cxx
	$(PURIFY) $(LDFINAL) -o $*.exeP $*.o $(LIBS)

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
	/bin/rm -rf *.exe *.exe? new.* *notime *def easyAccess.out *rbplacemt.out

#=============================================================================
parser: $(pOBJS) $(fOBJS)
	/bin/rm -rf parser
	$(LDFINAL) $(fOBJS) $(SODIR)/$(SONAME) $(LIBS) -o ldp

parserP: $(pOBJS) $(fOBJS)
	$(PURIFY) $(LDFINAL) $(fOBJS) $(pOBJS) $(LIBS) -o ldp
#	/bin/rm -rf parser $(SODIR)/$(SONAME)
