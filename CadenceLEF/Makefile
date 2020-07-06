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

#  $Source: /usr1/mfg/3.4C/solaris_bld/group/util/makefiles/RCS/dir.mk,v $
#
#  $Author: hld $
#  $Revision: 1.9 $
#  $Date: 1998/03/11 07:43:24 $
#  $State: Exp $
#

.PHONY: all
all: install release
	rm -f libCadenceLEF.a
	ln -s lib/liblef.a libCadenceLEF.a

BUILD_ORDER	= \
			clef \
			lef \
			lefrw \
			lefwrite \
			lefdiff

# MFLAGS = -O2
OPTIMIZE_FLAG = -O2
# OS_TYPE := $(shell uname)
# ifeq ($(OS_TYPE),HP-UX)
# OPTIMIZE_FLAG = +O2
# else
# OS_VER := $(shell uname -r)
# ifeq ($(findstring 4.1,$(OS_VER)),4.1)
# OPTIMIZE_FLAG = -O
# else
# OPTIMIZE_FLAG = -O
# endif
# endif

install:
	@$(MAKE) $(MFLAGS) installhdrs installlib installbin

release:
	@$(MAKE) "DEBUG=$(OPTIMIZE_FLAG)" install

test:
test1:
	@$(MAKE) "BUILD_ORDER=TEST" dotest

.PHONY: clean
clean:
	@$(MAKE) "BUILD_ORDER += TEST" doclean;
	echo $(BUILD_ORDER);
	@$(MAKE) doclean;
	rm -f libCadenceLEF.a;
	rm -f lefrw/*.o
	rm -f *.o

.DEFAULT:
	@for i in $(BUILD_ORDER) ;do \
		echo $(MAKE) $@ in $$i ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) $@ ; \
		cd .. ; \
	done

.DELETE_ON_ERROR:
