/**************************************************************************
***    
*** Copyright (c) 1995-2000 Regents of the University of California,
***               Andrew E. Caldwell, Andrew B. Kahng and Igor L. Markov
*** Copyright (c) 2000-2002 Regents of the University of Michigan,
***               Saurabh N. Adya and Igor L. Markov
***
***  Contact author(s): abk@cs.ucsd.edu, imarkov@umich.edu
***  Original Affiliation:   UCLA, Computer Science Department,
***                          Los Angeles, CA 90095-1596 USA
***
***  Permission is hereby granted, free of charge, to any person obtaining 
***  a copy of this software and associated documentation files (the
***  "Software"), to deal in the Software without restriction, including
***  without limitation 
***  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
***  and/or sell copies of the Software, and to permit persons to whom the 
***  Software is furnished to do so, subject to the following conditions:
***
***  The above copyright notice and this permission notice shall be included
***  in all copies or substantial portions of the Software.
***
*** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
*** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
*** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
*** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
*** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
*** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***
***
***************************************************************************/












#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "subHGForCapo.h"


SubHGraphForCapo::SubHGraphForCapo(
	unsigned 		numTerminals,
	unsigned 		numNonTerms,
	unsigned		edgeBound)

     	: SubHGraph(numTerminals, numNonTerms, edgeBound)

{
    _nodeNames = vector<const char*>(numTerminals+numNonTerms, 
						static_cast<const char*>(NULL));
    _nodes[0]->setWeight(0);
    _nodes[1]->setWeight(0);	//they aren't mapped, as they
				//are 'conglomerate' nodes
    char* termName;
    termName = new char[7];
    strcpy(termName, "term0");
    _nodeNames[0] = termName;
    _nodeNamesMap[termName] = 0;

    termName = new char[7];
    strcpy(termName, "term1");
    _nodeNames[1] = termName;
    _nodeNamesMap[termName] = 1;
}

