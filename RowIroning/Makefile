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

# 970821 Created by Max Moroz and Igor Markov,  VLSICAD ABKGroup, UCLA/CS

# 971006 ilm added dependency on StazenPlotz
# 971110 mro adapted for separation of ParserLEFDEF project
# 990812 ilm upgraded for use with g++  

.SUFFIXES:        .o .cxx .exe .exeS .exeP

# ##################
# standard variables
# ##################

 OPTFLAGS        = -g -DABKDEBUG -DHGDEBUG -DPARTDEBUG -DCAPODEBUG $(ABKSYMBOLS)
#OPTFLAGS        = -O5 # use this for optimization instead of the above
CCFLAGS          = -G -PIC +p     # comment +w2 when compiling with optimization
DEPENDOPT        = -xM1
CC               = CC $(OPTFLAGS) $(CCFLAGS)
LD               = CC -G
LDFINAL          = CC
CC4LIBS          = CC6
# could be g++
AR               = ar cr
SODIRKEY         = -R
SONAMEKEY        = -R
# change to -h if you need "immobile"dynamic libraries.
# LD_LIBRARY_PATH overrides -h
# Make sure there is no conflict
             
UCLA     = /home/code/OUR/packages
IMPORT   = NOT_USED
LIBBASE  = /home/code/lib/$(CC4LIBS)/O5
SLIBBASE         = $(LIBBASE)
STL              =
# -I$(IMPORT)/STL/CC

PARSER0          = RESERVED
PARSER1          = $(DB)/ParserLEFDEF
PARSER2          = # $(PWD)/ParserXXX

ARNAME           = lib$(THISLIB).a
SONAME           = lib$(THISLIB).so
SODIR            = $(PWD)
SOFULL           = $(SODIR)/$(SONAME)
SOLABEL          = $(SONAMEKEY)$(SOFULL)
TESTNAME         = $(THISLIB)Test

NSLLIBS     =  -lnsl -lsocket -lintl

# #########################################################
# standard variables that need modification in each project
# #########################################################

THISLIB = RowIroning
INCLUDES= $(STL) -I$(UCLA)

QLIBS  = -lParserLEFDEFq -lCadenceLEF -lCadenceDEF

YACC_LIB         = 
#
LIBPATHS         = $(SODIRKEY)$(LIBBASE)
LIBS             =-L. $(SODIRKEY). -l$(THISLIB) $(LIBPATHS) -L$(LIBBASE)\
		   -lFixOr -lRowIroning -lCongestionMaps\
                   -lMLPart -lFMPart -lPartEvals -lPartitioning -lPartitioners\
                   -lNetTopology -lPartLegality -lDB\
		   -lABKCommon -lConstraints -lPlacement -lStats -lHGraph \
                   -lParserLEFDEF $(QLIBS) $(YACC_LIB) -lfl -lFilledHier \
		   -lGenHier -lCombi -lPlaceEvals -lClusteredHGraph -lSmallPart\
                   -lSmallPlacers -lRBPlace -lRBPlFromDB -lHGraphWDims\
		   -lGeomTrees -lGeoms -lCtainers -lSmallPlacement -lShellPart

SLIBS            =-L$(SLIBBASE) -L$(PWD) -l$(THISLIB)\
		   -lFixOr -lShellPart -lRBPlFromDB -lCongestionMaps\
		   -lSmallPlacers -lSmallPlacement -lRBPlace -lMLPart\
		   -lFilledHier -lGenHier\
		   -lFMPart -lSmallPart -lPartitioners \
		   -lClusteredHGraph -lPartEvals -lPartLegality\
		   -lNetTopology  -lDB -lParserLEFDEF $(QLIBS) \
		   -lPlaceEvals -lPartitioning -lHGraphWDims -lHGraph $(YACC_LIB) -lfl\
		   -lConstraints -lPlacement -lCongestionMaps -lGeomTrees\
	           -lGeoms -lStats -lCtainers -lCombi -lABKCommon 
	           #-lefence


OBJS	 = rowIroning.o

SRCS     =  $(OBJS:%.o=%.cxx)

PURIFY   = purify -cache_dir=cache -always_use_cache_dir\
	   -windows=no -log_file=purify.log

# The four definitions below are for use with the PRINT_VERSION_INFO macro
# defined in ABKCommon/abkversion.h
ABKPACKAGE =`pwd | sed 's/.*\///g'`
ABKSYSTEM  ="`uname -n` running `uname -sr | sed 's/ /-/g'`"
ABKUSER    =`whoami`@`uname -n`
ABKSYMBOLS =-D__ABKPACKAGE__=\"$(ABKPACKAGE)\" -D__ABKSYSTEM__=\"$(ABKSYSTEM)\"\
        -D__ABKUSER__=\"$(ABKUSER)\" -D__ABKLIBBASE__=\"$(LIBBASE)\"
# add -(ABKSYMBOLS) to the CC call defined by the .cxx.o rule

# ################
# standard targets
# ################

all:        lib

lib: $(SOFULL) $(ARNAME)

$(SOFULL):        $(OBJS)
	/bin/rm -f $(SOFULL)
	$(LD) $(INCLUDES) -o $(SOFULL) $(OBJS) $(SOLABEL)

$(ARNAME):        $(OBJS)
	/bin/rm -f $(ARNAME)
	$(AR) $(ARNAME) $(OBJS)

.cxx.o:
	$(CC) $(INCLUDES) -c $*.cxx

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

depend.mk:
	$(CC) $(DEPENDOPT) $(INCLUDES) $(SRCS) *.cxx >depend.mk

clean:        
	rm -rf *.o Templates.DB *.exe *.exe? $(SOFULL) $(ARNAME) core ir.out\
	$(TEST_OUT) $(TEST_DIFF) lib*pure* cache SunWS_cache *~

test:  RowIroningTest0.exe RowIroningTest1.exe

testP: RowIroningTest0.exeP RowIroningTest1.exeP

testS: RowIroningTest0.exeS RowIroningTest1.exeS


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

regression:        RowIroningTest0.exe
	$(TESTNAME)0 $(TEST_ARGS) > $(TEST_OUT) < $(TEST_IN)
	perl -n -i -e "print unless /Created|sec/;" $(TEST_OUT)
	-@diff $(TEST_OUT) $(TEST_EXP) > $(TEST_DIFF)
	@echo "  lines:  words:  bytes: in error log:"
	@wc $(TEST_DIFF)

correct:
	$(TESTNAME) $(TEST_ARGS) > $(TEST_EXP) < $(TEST_IN)
	perl -n -i -e "print unless /Created|sec/;" $(TEST_EXP)

