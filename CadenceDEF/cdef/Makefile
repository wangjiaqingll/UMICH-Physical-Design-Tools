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

DEF_TABNAME     = def
DEF_BISON_SRCS  = def.y

FAKE_ALL: all

LIBTARGET =	libcdef.a

HEADERS =	 \
			FEF_malloc.h \
			FEF_stringhash.h \
			FEF_util.h \
			def.tab.h \
			lex.h \
			lex.cpph

PUBLIC_HDRS = \
			defiAlias.h \
			defiAssertion.h \
			defiComponent.h \
			defiDebug.h \
			defiDefs.h \
			defiFPC.h \
			defiGroup.h \
			defiIOTiming.h \
			defiKRDefs.h \
			defiNet.h \
			defiPartition.h \
			defiPath.h \
			defiPinCap.h \
			defiPinProp.h \
			defiProp.h \
			defiPropType.h \
			defiRegion.h \
			defiRowTrack.h \
			defiScanchain.h \
			defiSite.h \
			defiTimingDisable.h \
			defiUser.h \
			defiUtil.h \
			defiVia.h \
			defrCallBacks.h \
			defrReader.h \
			defwWriter.h \
			defwWriterCalls.h

LIBSRCS =	\
			FEF_malloc.c \
			FEF_stringhash.c \
			def.tab.c \
			def_keywords.c \
			defiAlias.c \
			defiAssertion.c \
			defiComponent.c \
			defiDebug.c \
			defiFPC.c \
			defiGroup.c \
			defiIOTiming.c \
			defiNet.c \
			defiPartition.c \
			defiPath.c \
			defiPinCap.c \
			defiPinProp.c \
			defiProp.c \
			defiPropType.c \
			defiRegion.c \
			defiRowTrack.c \
			defiScanchain.c \
			defiSite.c \
			defiTBExt.c \
			defiTimingDisable.c \
			defiUtil.c \
			defiVia.c \
			defrReader.c \
			defwWriter.c \
			defwWriterCalls.c

def.tab.c : $(DEF_BISON_SRCS)
	bison -v -p$(DEF_TABNAME)yy -d $(DEF_BISON_SRCS)

include ../template.mk
