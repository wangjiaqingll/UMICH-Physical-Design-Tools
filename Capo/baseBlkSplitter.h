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


#ifndef _BASEBLKSPLITTER_H_
#define _BASEBLKSPLITTER_H_

#include <vector>
using std::vector;
#include "ABKCommon/abkcommon.h"
#include "capoBlock.h"
#include "capoPlacer.h"
#include "partProbForCapo.h"
#include "capoParams.h"

inline double dbl_abs(double val) {return val >= 0 ? val : -val;}

class BaseBlockSplitter
{
  protected:

    CapoBlock&			_block;
    vector<CapoBlock*>	        _newBlocks;
    const CapoPlacer&		_capo;

    CapoSplitterParams		_params;

    unsigned findBestSplitRow(const vector<double>& cellAreas,
			            vector<double>& actualCaps);


	//create blocks after a horizontal split
	//first version finds the best splitRow..the
	//second uses the one it's given
    void createHSplitBlocks(PartitioningProblemForCapo& problem);
    void createHSplitBlocks(PartitioningProblemForCapo& problem,
			    unsigned splitRow);

	//create blocks after a vertical split
    void createVSplitBlocks(PartitioningProblemForCapo& problem, 
				double xSplit);
    void createVSplitBlocks(const vector<vector<unsigned> >& 
				cellsInNewBlocks);
    void createVSplitBlocks(
		const vector<vector<unsigned> >& cellsInNewBlocks, 
		const vector<double>& partAreas,
		double xSplit);
  public:

    BaseBlockSplitter(      CapoBlock&  blockToSplit,
		      const CapoPlacer& capo,
		      CapoSplitterParams params)
	: _block(blockToSplit), _capo(capo), _params(params)
	{}

    virtual ~BaseBlockSplitter(){}	//does not own new blocks

    vector<CapoBlock*>& getNewBlocks() {return _newBlocks;}

    bool hPartCapsFrmRgnConstr(unsigned splitRow, vector<double>& partCaps);
    bool vPartCapsFrmRgnConstr(double xSplit, vector<double>& partCaps);

    //give the partition areas in presence of filler cells
    bool partAreasForFillerCells(bool splitDir, vector<double>& partAreas);
}; 


#endif
