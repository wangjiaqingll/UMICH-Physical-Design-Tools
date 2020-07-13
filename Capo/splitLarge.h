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


//SplitLarge contains classes that are designed to partiton
//small CapoBlocks.  Large is expected to mean > 200 nodes.

//The following 2 classes are defined:
//SplitLargeBlockHorizontally(CapoBlock&, const MLParams&, Verbosity)
//SplitLargeBlockVertically  (CapoBlock&, const MLParams&, Verbosity)


//each take as input a reference to a CapoBlock, and populate
//vector of pointers to new CapoBlocks.

#ifndef __SPLITLARGE_H_
#define __SPLITLARGE_H_

#include "baseBlkSplitter.h"

class SplitLargeBlockHorizontally : public BaseBlockSplitter
{
    unsigned _splitRow;
    unsigned _numMLSets;

    void callPartitioner(PartitioningProblemForCapo& problem, 
			 bool splitPtFixed=false,
			 PlacementWOrient* placement=NULL);
    bool callMLPart     (PartitioningProblemForCapo& problem, 
			 bool splitPtFixed=false,
			 PlacementWOrient* placement=NULL);

  public:

    SplitLargeBlockHorizontally(      CapoBlock&  blockToSplit,
				const CapoPlacer& capo,
				      CapoSplitterParams params = 
				      CapoSplitterParams(),
				      PlacementWOrient* placement=NULL);

    //by sadya in SYNP to take care of region constraints
    //splits a block at the given ySplit. also pre-fixes any cells
    //in group constraints that are relevant to this block
    SplitLargeBlockHorizontally(      CapoBlock&  blockToSplit,
				const CapoPlacer& capo,
				      double ySplit,
				      CapoSplitterParams params = 
				      CapoSplitterParams(),
				      PlacementWOrient* placement=NULL);

    virtual ~SplitLargeBlockHorizontally() {}
}; 

class SplitLargeBlockVertically : public BaseBlockSplitter
{
    double _xSplit;
    unsigned _numMLSets;

    void callPartitioner(PartitioningProblemForCapo& problem, 
			 PlacementWOrient* placement=NULL, 
			 bool splitPtFixed=false);
    bool callMLPart     (PartitioningProblemForCapo& problem, 
			 PlacementWOrient* placement=NULL,
			 bool splitPtFixed=false);

  public:

    SplitLargeBlockVertically  (      CapoBlock&  blockToSplit,
				const CapoPlacer& capo,
				      CapoSplitterParams  params =
				      CapoSplitterParams(),
				      PlacementWOrient* placement=NULL);

    //by sadya in SYNP to take care of region constraints
    //splits a block at the given xSplit. also pre-fixes any cells
    //in group constraints that are relevant to this block    
    SplitLargeBlockVertically  (      CapoBlock&  blockToSplit,
				const CapoPlacer& capo,
				      double xSplit,
				      CapoSplitterParams  params =
				      CapoSplitterParams(),
				      PlacementWOrient* placement=NULL);

    virtual ~SplitLargeBlockVertically() {}
}; 

#endif
