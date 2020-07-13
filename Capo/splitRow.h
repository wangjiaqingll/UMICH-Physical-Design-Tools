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


//SplitRowOfBlocks takes a vector of blocks (a contig row w same y-span),
//combines then into one block, splits this horizontally, and then
//constructs 2*orig# blocks new blocks that have as equal as possible
//whitespace distrubition


#ifndef __SPLITROW_H_
#define __SPLITROW_H_

#include "baseBlkSplitter.h"

class SplitRowOfBlocks : public BaseBlockSplitter
{
    vector<CapoBlock*>& _rowOfBlocks;
    static bit_vector isInTopBlk;

    bool		_wasGoodSplit;

    void buildSubBlocks(vector<CapoBlock*>& bigBlocks);
    
    void buildOneSubRow(CapoBlock* subRowBlock, bool isTopBlk, 
			vector<CapoBlock*>& newBlks);

  public:

    SplitRowOfBlocks( vector<CapoBlock*>&  rowOfBlocks,
		const CapoPlacer& capo,
		      CapoSplitterParams params=CapoSplitterParams());

    virtual ~SplitRowOfBlocks() {}

    bool wasGoodSplit() const { return _wasGoodSplit;}
}; 

#endif
