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


.SUFFIXES:	.o .cxx

#OPTFLAGS = -g -DABKDEBUG
 OPTFLAGS = -O5
CCFLAGS  = -G -pic +p +w2  # comment +w/+w2 when compiling with optimization
DEPENDOPT= -xM1
CC       = CC $(OPTFLAGS) $(CCFLAGS)
LD       = CC -G
LDFINAL  = CC
AR       = ar cr
PURIFY   = purify -cache_dir=cache -windows=no -log_file=purify.log

UCLA             = /home/userg/code/OUR/symlinks
IMPORT   = /home/userd/code/IMPORT

OBJS     = sd2.o mst8.o
SRCS     =  $(OBJS:%.o=%.cxx)

TESTNAME=msttest

all  : $(OBJS)

test:   mstmain.o
	/bin/rm -rf $(TESTNAME)
	$(LDFINAL) -o $(TESTNAME) mstmain.o $(OBJS) $(LIBS) 

.cxx.o:
	$(CC) $(INCLUDES) -c $*.cxx

depend:
	/bin/rm -f depend.mk

depend.mk:
	$(CC) $(DEPENDOPT) $(INCLUDES) $(SRCS) >depend.mk

# include depend.mk

clean:; /bin/rm -rf *.o Templates.DB core cache $(TESTNAME) li*pure* SunWS_cache
