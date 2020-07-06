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

DIRNAME = lef
LEF_BISON_SRCS  = lef.y

LIBTARGET =	liblef.a

HEADERS =	FEF_malloc.h \
		    FEF_stringhash.h \
		    FEF_util.h \
		    lef.tab.h \
		    lex.h \
			crypt.hpp \
			lex.cpph

PUBLIC_HDRS = \
			lefiArray.hpp \
			lefiCrossTalk.hpp \
			lefiDebug.hpp \
			lefiDefs.hpp \
			lefiKRDefs.hpp \
			lefiLayer.hpp \
			lefiMacro.hpp \
			lefiMisc.hpp \
			lefiNonDefault.hpp \
			lefiProp.hpp \
			lefiPropType.hpp \
			lefiUnits.hpp \
			lefiUser.hpp \
			lefiUtil.hpp \
			lefiVia.hpp \
			lefiViaRule.hpp \
			lefrCallBacks.hpp \
			lefrReader.hpp \
			lefwWriter.hpp \
			lefwWriterCalls.hpp

LIBSRCS =	FEF_malloc.cpp \
		    FEF_stringhash.cpp \
			crypt.o \
		    lef.tab.cpp \
		    lef_keywords.cpp \
		    lefiArray.cpp \
		    lefiCrossTalk.cpp \
		    lefiDebug.cpp \
		    lefiLayer.cpp \
		    lefiMacro.cpp \
		    lefiMisc.cpp \
		    lefiNonDefault.cpp \
		    lefiProp.cpp \
		    lefiPropType.cpp \
		    lefiTBExt.cpp \
		    lefiUnits.cpp \
		    lefiUtil.cpp \
		    lefiVia.cpp \
		    lefiViaRule.cpp \
		    lefrReader.cpp \
		    lefwWriter.cpp \
		    lefwWriterCalls.cpp \

# For lef, create '.cpp' object
lef.tab.cpp : $(LEF_BISON_SRCS)
	bison -v -p$(LEF_TABNAME)yy -d $(LEF_BISON_SRCS)
	sed s/std::size_t/size_t/ $(LEF_TABNAME).tab.c > /tmp/cdnlef$$
	sed 's/YYSTD (size_t)/size_t/' /tmp/cdnlef$$ > $(LEF_TABNAME).tab.cpp
	rm /tmp/cdnlef$$

# mv $(LEF_TABNAME).tab.c $(LEF_TABNAME).tab.cpp ;

include ../template.mk
