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


# 970801 Created and tained by Igor Markov,  VLSICAD ABKGroup, UCLA/CS
# 970814 ilm added ir.out to clean, added +p switch, changed DEBUG to ABKDEBUG

.SUFFIXES:	.o .cxx .exe .exeS .exeP

 OPTFLAGS = -g -DABKDEBUG
#OPTFLAGS= -O5 # use this for optimization instead of the above
CCFLAGS  = -G -KPIC +p # comment +w/+w2 when compiling with optimization
DEPENDOPT= -xM1
CC       = CC $(OPTFLAGS) $(CCFLAGS)
CC4LIBS          = CC6
LD       = CC -G
LDFINAL  = CC
AR               = ar cr
SODIRKEY =  -R
SONAMEKEY        = -R # change to -h if you need "immobile"dynamic libraries.
                      # LD_LIBRARY_PATH overrides -h  
UCLA        = /home/code/OUR/packages
IMPORT      = /home/code/IMPORT
#LIBBASE    = /home/code/lib/$(CC4LIBS)/g
 LIBBASE    = /home/code/lib/$(CC4LIBS)/O5
SLIBBASE    = $(LIBBASE)/static
ARNAME      = lib$(THISLIB).a
SONAME      = lib$(THISLIB).so
SODIR       = $(PWD)
SOFULL      = $(SODIR)/$(SONAME)
SOLABEL     = $(SONAMEKEY)$(SOFULL)
TESTNAME    = $(THISLIB)Test

STL      = 
# -I$(IMPORT)/STL/CC

# #########################################################
# standard variables that need modification in each project
# #########################################################
       
THISLIB = Constraints
INCLUDES = $(STL) -I$(UCLA)
OBJS     =constraint.o constraints.o fxTypeConstr.o regionConstr.o otherConstr.o
SRCS     = $(OBJS:%.o=%.cxx)
LIBPATHS = $(SODIRKEY)$(LIBBASE) 
LIBS     = -L.  $(SODIRKEY). -l$(THISLIB) $(LIBPATHS) -L$(LIBBASE)\
	   -lABKCommon -lStats -lCombi -lGeoms -lPlacement
SLIBS    = $(ARNAME)        $(LIBPATHS) -L$(LIBBASE)\
	   -lABKCommon -lStats -lCombi -lGeoms -lPlacement

PURIFY   = purify -cache_dir=cache -windows=no -log_file=purify.log\
	   -always_use_cache_dir

# ################
# standard targets
# ################

all  : lib

lib  : $(OBJS);
	/bin/rm -f $(ARNAME) $(SODIR)/$(SONAME)
	$(AR) $(ARNAME) $(OBJS)
	$(LD) $(INCLUDES) -o $(SOFULL) $(OBJS) $(SOLABEL)

#  test11 and test13 should not be compiled
test: ConstraintsTest0.exe ConstraintsTest1.exe ConstraintsTest2.exe\
      ConstraintsTest3.exe ConstraintsTest4.exe ConstraintsTest5.exe

# test11 in the Placement package should be compiled separately since
# it needs to link with the Stats package
#
#test11: 11.o
#	/bin/rm -rf $(TESTNAME)11
#	$(LDFINAL) -o $(TESTNAME)11 11.o  $(LIBS)

# uses rwalk in BTW
#test12: 12.o
#	/bin/rm -rf $(TESTNAME)12
#	$(LDFINAL) -o $(TESTNAME)12 12.o  $(LIBS)

# test13 in the Placement package should be compiled separately since
# it needs to link with the Stats package
#
#test13: 13.o
#	/bin/rm -rf $(TESTNAME)13
#	$(LDFINAL) -o $(TESTNAME)13 13.o  $(LIBS)


.cxx.o:
	$(CC) $(INCLUDES) -c $*.cxx

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
	$(PURIFY) $(LDFINAL) -o $*.exeP $*.o $(LIBS)

depend:
	/bin/rm -f depend.mk

depend.mk:
	$(CC) $(DEPENDOPT) $(INCLUDES) $(SRCS) *.cxx >depend.mk

# include depend.mk

clean:
	/bin/rm -rf *.o Templates.DB $(ARNAME)* $(SONAME)* *.exe *.exe? core \
	cache ir.out lib*pure* rotd.pla orig.pla *clump*.pla *.gz out.xy \
	seeds.{in,out} SunWS_cache new.out *notime
