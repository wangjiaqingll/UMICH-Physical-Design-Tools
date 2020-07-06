#**************************************************************************
#**
#**    Copyright (c) 1995-1997 by ABKGroup, UCLA VLSI CAD Laboratory,
#**    UCLA Computer Science Department, Los Angeles, CA 90095-1596 USA,
#**    and by the Regents of the University of California.
#**    All rights reserved.
#**
#**    No portion of this file may be used, copied, or transmitted for
#**    any purpose, including any research or commercial purpose,
#**    without the express written consent of Professor Andrew B. Kahng,
#**    UCLA Computer Science Department, 3713 Boelter Hall, Los Angeles,
#**    CA 90095-1596 USA.   Any copy made of this file must include this
#**    notice.    ABKGroup and the Regents of the University of California
#**    retain title to all copies of this file and/or its compiled
#**    derivatives.
#**
#**************************************************************************/

# 970821 Created by Max Moroz and Igor Markov,  VLSICAD ABKGroup, UCLA/CS

# 971006 ilm added dependency on StazenPlotz
# 971110 mro adapted for separation of ParserLEFDEF project

.SUFFIXES:        .o .cxx

# ##################
# standard variables
# ##################

 OPTFLAGS        = -g -DABKDEBUG -time # comment -time when using optimization
#OPTFLAGS        = -O5 # use this for optimization instead of the above
CCFLAGS          = -G -pic +p +w2 # comment +w2 when compiling with optimization
DEPENDOPT        = -xM1
CC               = CC $(OPTFLAGS) $(CCFLAGS)
LD               = CC -G
LDFINAL          = CC
AR               = CC -xar
RHKEY            = -h  # change to -R if you need "movable"dynamic libraries.
                       # LD_LIBRARY_PATH overrides -R
                       # Make sure there is no conflict

UCLA             = /home/userd/code/OUR
IMPORT           = /home/userd/code/IMPORT
 LIBBASE         = /home/userd/code/lib/g
#LIBBASE          = /home/userd/code/lib/O5
STL              = $(IMPORT)/STL/CC
COMMON           = $(UCLA)/ABKCommon
DB               = $(UCLA)/DB
PLACE            = $(UCLA)/Placement
CONSTR           = $(UCLA)/Constraints
STATS            = $(UCLA)/StatzenPlotz
GRAPH            = $(UCLA)/GraphLib
HGRAPH           = $(UCLA)/HGraph

PARSER0          = RESERVED
PARSER1          = $(PWD)/ParserLEFDEF
PARSER2          = # $(PWD)/ParserXXX

ARNAME           = lib$(THISLIB).a
SONAME           = lib$(THISLIB).so
SODIR            = $(PWD)
SOFULL           = $(SODIR)/$(SONAME)
TESTNAME         = $(THISLIB)Test

NSLLIBS          =  -lnsl -lsocket  # or /usr/lib/libnsl.a /usr/lib/libsocket.a 

LEFDEFLIB        =  $(LIBBASE)/libldef.a
PLIB0            =  $(LIBBASE)/libparserLEFDEFq.a  
PLIB1            =  $(LIBBASE)/libparserLEFDEF.a -ly -lfl
#PLIB2           =  $(LIBBASE)/libparserXXX.a

STATICLIBS       =  $(ARNAME) $(PLIB0) $(LEFDEFLIB) $(NSLLIBS) $(ARNAME)\
		    $(PLIB1) $(LIBBASE)/libconstr.a $(LIBBASE)/libplace.a\
		    $(LIBBASE)/libcommon.a $(LIBBASE)/libstats.a

#LIBS            = -L$(PWD) -R$(PWD) -l$(THISLIB) -L$(PARSER1) -lparserLEFDEF \

LIBS             = -L$(PWD) -R$(PWD)  -L$(LIBBASE) -R$(LIBBASE)\
		   -L$(LIBBASE) -R$(LIBBASE) \
		   -ldb -lcommon -lconstr -lplace -lstats -lhgraph \
		   -lparserLEFDEFq -lparserLEFDEF -lldef $(NSLLIBS) -ly -lfl

SRCS                =  $(OBJS:%.o=%.cxx)

# #########################################################
# standard variables that need modification in each project
# #########################################################

THISLIB = countPs
INCLUDES= -I$(STL) -I$(COMMON) -I$(PLACE) -I$(CONSTR) -I$(DB) \
	  -I$(STATS)
OBJS    = 
PURIFY  = purify -cache_dir=cache -always_use_cache_dir\
	  -windows=no -log_file=purify.log

# ################
# standard targets
# ################

all: test

lib: $(SOFULL) $(ARNAME)

$(SOFULL):        $(OBJS)
	/bin/rm -f $(SOFULL)
	$(LD) $(INCLUDES) -o $(SOFULL) $(OBJS) $(RHKEY) $(SOFULL)

$(ARNAME):        $(OBJS)
	/bin/rm -f $(ARNAME)
	$(AR) -o $(ARNAME) $(OBJS)

.cxx.o:
	$(CC) $(INCLUDES) -c $*.cxx

depend:
	/bin/rm -f depend.mk

depend.mk:
	$(CC) $(DEPENDOPT) $(INCLUDES) $(SRCS) >depend.mk

clean:        
	rm -rf *.o Templates.DB $(TESTNAME)* $(SOFULL) $(ARNAME) core ir.out\
	$(TEST_OUT) $(TEST_DIFF) lib*pure* cache

test:  test0  # test1  test2  test3  test4  test5
testP: testP0 # testP1 testP2 testP3 testP4 testP5  # tests with purify
testS: testS0 # testS1 testS2 testS3 testS4 testS5  # static tests 

test0: main0.o $(SOFULL)
	/bin/rm -rf $(TESTNAME)0
	$(LDFINAL) -o $(TESTNAME)0 main0.o  $(LIBS) 

testP0: main.o $(SOFULL)
	/bin/rm -rf $(TESTNAME)0 $(SODIR)/$(SONAME)
	$(PURIFY) $(LDFINAL) -o $(TESTNAME)0 main0.o  $(LIBS)

testS0: main0.o $(ARNAME)
	/bin/rm -rf $(TESTNAME)0
	$(LDFINAL) -o $(TESTNAME)0 main0.o  $(STATICLIBS)

.PHONY: depend, new, clean, test, testP, lib

# Uncomment for automatic handling of dependencies
# (use make depend to [re-]build dependencies)

# include depend.mk

# #################################
# sample project-specific variables 
# #################################

TEST_OUT        = test.out
TEST_IN         = test.in
TEST_EXP        = test.exp
#TEST_EXP       = aztec.out.D.A.dag
TEST_DIFF       = test.diff
#TEST_ARGS      = -aux aztec.aux -wm D -fm A
TEST_ARGS       = -f mini2


# ########################
# project-specific targets
# ########################

# Using a C-shell regression script is recommended instead
# (see file "regression")

regression:        test0
	$(TESTNAME)0 $(TEST_ARGS) > $(TEST_OUT) < $(TEST_IN)
	perl -n -i -e "print unless /Created|sec/;" $(TEST_OUT)
	-@diff $(TEST_OUT) $(TEST_EXP) > $(TEST_DIFF)
	@echo "  lines:  words:  bytes: in error log:"
	@wc $(TEST_DIFF)

correct:
	$(TESTNAME) $(TEST_ARGS) > $(TEST_EXP) < $(TEST_IN)
	perl -n -i -e "print unless /Created|sec/;" $(TEST_EXP)

