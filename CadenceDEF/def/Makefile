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

LIBTARGET =	libdef.a

HEADERS =	 \
			FEF_malloc.h \
			FEF_stringhash.h \
			FEF_util.h \
			def.tab.h \
			lex.h \
			lex.cpph

PUBLIC_HDRS = \
			defiAlias.hpp \
			defiAssertion.hpp \
			defiComponent.hpp \
			defiDebug.hpp \
			defiDefs.hpp \
			defiFPC.hpp \
			defiGroup.hpp \
			defiIOTiming.hpp \
			defiKRDefs.hpp \
			defiNet.hpp \
			defiPartition.hpp \
			defiPath.hpp \
			defiPinCap.hpp \
			defiPinProp.hpp \
			defiProp.hpp \
			defiPropType.hpp \
			defiRegion.hpp \
			defiRowTrack.hpp \
			defiScanchain.hpp \
			defiSite.hpp \
			defiTimingDisable.hpp \
			defiUser.hpp \
			defiUtil.hpp \
			defiVia.hpp \
			defrCallBacks.hpp \
			defrReader.hpp \
			defwWriter.hpp \
			defwWriterCalls.hpp

LIBSRCS =	\
			FEF_malloc.cpp \
			FEF_stringhash.cpp \
			def.tab.cpp \
			def_keywords.cpp \
			defiAlias.cpp \
			defiAssertion.cpp \
			defiComponent.cpp \
			defiDebug.cpp \
			defiFPC.cpp \
			defiGroup.cpp \
			defiIOTiming.cpp \
			defiNet.cpp \
			defiPartition.cpp \
			defiPath.cpp \
			defiPinCap.cpp \
			defiPinProp.cpp \
			defiProp.cpp \
			defiPropType.cpp \
			defiRegion.cpp \
			defiRowTrack.cpp \
			defiScanchain.cpp \
			defiSite.cpp \
			defiTBExt.cpp \
			defiTimingDisable.cpp \
			defiUtil.cpp \
			defiVia.cpp \
			defrReader.cpp \
			defwWriter.cpp \
			defwWriterCalls.cpp

def.tab.cpp : $(DEF_BISON_SRCS)
	bison -v -p$(DEF_TABNAME)yy -d $(DEF_BISON_SRCS)
	sed s/std::size_t/size_t/ $(DEF_TABNAME).tab.c > /tmp/cdnlef$$
	sed 's/YYSTD (size_t)/size_t/' /tmp/cdnlef$$ > $(DEF_TABNAME).tab.cpp
	rm /tmp/cdnlef$$

# mv $(DEF_TABNAME).tab.c $(DEF_TABNAME).tab.cpp ;

include ../template.mk
