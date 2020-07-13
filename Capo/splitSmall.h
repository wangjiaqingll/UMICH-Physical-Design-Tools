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















//created by Andrew Caldwell on 10/17/99 caldwell@cs.ucla.edu


//SplitSmall contains classes that are designed to partiton
//small CapoBlocks.  Small is expected to mean <= 200 nodes..
//however this is not a requirment - determination of which
//blocks get split this way is in fact left up to Capo itself.

//The following 2 classes are defined:
//SplitSmallBlockHorizontally(CapoBlock&, const MLParams&, Verbosity)
//SplitSmallBlockVertically  (CapoBlock&, const MLParams&, Verbosity)


//each take as input a reference to a CapoBlock, and populate
//vector of pointers to new CapoBlocks.


#ifndef __SPLITSMALL_H_
#define __SPLITSMALL_H_

#include "baseBlkSplitter.h"

class SplitSmallBlockHorizontally : public BaseBlockSplitter
{
    unsigned _splitRow;

    void callPartitioner(PartitioningProblemForCapo& problem);
    bool callBBFM       (PartitioningProblemForCapo& problem);
    bool callFMPart     (PartitioningProblemForCapo& problem);

  public:

    SplitSmallBlockHorizontally(      CapoBlock&  blockToSplit,
				const CapoPlacer& capo,
				CapoSplitterParams params=CapoSplitterParams());

    virtual ~SplitSmallBlockHorizontally() {}
}; 

class SplitSmallBlockVertically : public BaseBlockSplitter
{
    double _xSplit;

    void callPartitioner(PartitioningProblemForCapo& problem);
    bool callBBFM       (PartitioningProblemForCapo& problem);
    bool callFMPart     (PartitioningProblemForCapo& problem);

  public:

    SplitSmallBlockVertically  (      CapoBlock&  blockToSplit,
				const CapoPlacer& capo,
				CapoSplitterParams params=CapoSplitterParams());

    virtual ~SplitSmallBlockVertically() {}
}; 

#endif
