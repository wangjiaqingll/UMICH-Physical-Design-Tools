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

LEF_TABNAME     = lef
LEF_BISON_SRCS  = lef.y

FAKE_ALL: all

LIBTARGET =	libclef.a

HEADERS =	FEF_malloc.h \
		    FEF_stringhash.h \
			FEF_util.h \
			crypt.h \
			lef.tab.h \
			lex.h \
			lex.cpph

PUBLIC_HDRS = \
			lefiArray.h \
			lefiCrossTalk.h \
			lefiDebug.h \
			lefiDefs.h \
			lefiKRDefs.h \
			lefiLayer.h \
			lefiMacro.h \
			lefiMisc.h \
			lefiNonDefault.h \
			lefiProp.h \
			lefiPropType.h \
			lefiUnits.h \
			lefiUser.h \
			lefiUtil.h \
			lefiVia.h \
			lefiViaRule.h \
			lefrCallBacks.h \
			lefrReader.h \
			lefwWriter.h \
			lefwWriterCalls.h

LIBSRCS =	FEF_malloc.c \
			FEF_stringhash.c \
			crypt.c \
			lef.tab.c \
			lef_keywords.c \
			lefiArray.c \
			lefiCrossTalk.c \
			lefiDebug.c \
			lefiLayer.c \
			lefiMacro.c \
			lefiMisc.c \
			lefiNonDefault.c \
			lefiProp.c \
			lefiPropType.c \
			lefiTBExt.c \
			lefiUnits.c \
			lefiUtil.c \
			lefiVia.c \
			lefiViaRule.c \
			lefrReader.c \
			lefwWriter.c \
			lefwWriterCalls.c

# For lef, create '.cpp' object
lef.tab.c : $(LEF_BISON_SRCS)
	bison -v -p$(LEF_TABNAME)yy -d $(LEF_BISON_SRCS)

include ../template.mk
