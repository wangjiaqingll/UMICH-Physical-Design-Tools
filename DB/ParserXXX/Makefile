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


OPTFLAGS = -g -DDEBUG
CCFLAGS  = -G -pic
DEPENDOPT= -xM1
CC       = CC +w2 $(OPTFLAGS) $(CCFLAGS)
LD       = CC -G
LDFINAL  = CC
AR       = CC -xar
PURIFY   = purify -cache_dir=cache -windows=no -log_file=purify.log

UCLA             = /home/userg/code/OUR/symlinks
IMPORT   = /home/userg/code/IMPORT
STL      = $(IMPORT)/STL/CC
COMMON   = $(UCLA)/ABKCommon
PLACE    = $(UCLA)/Placement
CONSTR   = $(UCLA)/Constraints
DB       = $(PWD)/..
THISLIB  = parserXXX
ARNAME   = lib$(THISLIB).a
SONAME   = lib$(THISLIB).so
SODIR    = $(PWD)
TESTNAME = $(THISLIB)Test


INCLUDES = -I$(STL) -I$(COMMON) -I$(PLACE) -I$(CONSTR) -I$(DB)

LIBS        = -L$(COMMON) -lcommon -L$(PLACE) -lplace -L$(CONSTR) -lconstr
STATICLIBS  = 

OBJS        =  DBparseXXX.o parserXXX.o
SRCS        =  $(OBJS:%.o=%.cxx)

all  : lib

lib  : $(OBJS);
	/bin/rm -f $(ARNAME) $(SODIR)/$(SONAME)
	$(AR) -o $(ARNAME) $(OBJS)
	$(LD) -o $(SODIR)/$(SONAME) $(OBJS) -h $(SODIR)/$(SONAME)

test: test0 test1 test2

test0: main0.o
	/bin/rm -rf $(TESTNAME)0
	$(LDFINAL) -o $(TESTNAME)0 main0.o  $(LIBS) -L. -l$(THISLIB)

test1: main1.o
	/bin/rm -rf $(TESTNAME)1
	$(LDFINAL) -o $(TESTNAME)1 main1.o  $(LIBS) -L. -l$(THISLIB)

test2: main2.o
	/bin/rm -rf $(TESTNAME)2
	$(LDFINAL) -o $(TESTNAME)2 main2.o  $(LIBS) -L. -l$(THISLIB)

testP0: main0.o
	/bin/rm -rf $(TESTNAME)0 $(SODIR)/$(SONAME)
#	$(LD) -o $(SODIR)/$(SONAME) $(OBJS) -h $(SODIR)/$(SONAME)
	$(PURIFY) $(LDFINAL) -o $(TESTNAME)0 main0.o  $(LIBS)  -L. -l$(THISLIB)

testStatic: main.o

.cxx.o:
	$(CC) $(INCLUDES) -c $*.cxx

depend:
	/bin/rm -f depend.mk

depend.mk:
	$(CC) $(DEPENDOPT) $(INCLUDES) $(SRCS) >depend.mk

# include depend.mk

clean:; /bin/rm -rf *.o Templates.DB $(ARNAME)* $(SONAME)* $(TESTNAME)* \
	core cache
