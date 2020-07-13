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


#include "partProbForCapo.h"
#include "capoBlock.h"
#include "subHGForCapo.h"
#include "Partitioning/termiProp.h"
#include "capoPlacer.h"

PartitioningProblemForCapo::PartitioningProblemForCapo(
		const CapoBlock&		block,
		const CapoPlacer&		capo,
		      bool                      isHCut,
		      double                    splitPt,	
		      Verbosity                 verb,
		      bool honourGrpConstr)
	: PartitioningProblem(), 
	  _hgraphOfNetlist(capo.getNetlistHGraph()), 
	  _cellToBlockMap (capo.getCellToBlockMap()),
          _capoPl(capo), _verb(verb)
{
    _blocks = vector<const CapoBlock*>(1);
    _blocks[0] = &block;

    initToNULL();

    double cellArea = block.getTotalCellArea();
    unsigned numCells = block.getNumCells();
    _capacities = new vector<vector<double> >
			(2,vector<double>(1,cellArea/2.0));
    _totalWeight = new vector<double>(1, cellArea);
    _maxCapacities=new vector<vector<double> >(2, vector<double>(1,0));
    _minCapacities=new vector<vector<double> >(2, vector<double>(1,0));
    setTolerance(0);

	//partitions get set only so we can construct the HGraph
	//and do termi-prop..it is expected that someone
	//will set the capacities, etc. before use
    const BBox& blockBBox = block.getBBox();
    _partitions = new vector<BBox>(2,BBox());
    if(isHCut)
    {
	_nDim = 2; 
	_mDim = 1;
        (*_partitions)[0] = BBox(blockBBox.xMin,splitPt,
                              blockBBox.xMax,blockBBox.yMax);
        (*_partitions)[1] = BBox(blockBBox.xMin,blockBBox.yMin,
                              blockBBox.xMax,splitPt);
    }
    else
    {
	_nDim = 1; 
	_mDim = 2;
   	(*_partitions)[0] = BBox(blockBBox.xMin, blockBBox.yMin,
                              splitPt, blockBBox.yMax);
    	(*_partitions)[1] = BBox(splitPt, blockBBox.yMin,
                              blockBBox.xMax, blockBBox.yMax);
    }

    _setPartitionCenters();


    if(_capoPl.getParams().capoNE && numCells > 200)
      buildHGraphNE(honourGrpConstr); //build HGraph With Net Effect
    else
      buildHGraph(honourGrpConstr);

    setBuffer();

    _ownsData = false;
}

PartitioningProblemForCapo::PartitioningProblemForCapo(
                const vector<const CapoBlock*>&        blocks,
                const HGraphFixed&              hgraphOfNetlist,
                const CapoPlacer&               capo,
                const vector<const CapoBlock*>& cellToBlockMap,
                Verbosity verb)
        : PartitioningProblem(), _blocks(blocks),
          _hgraphOfNetlist(hgraphOfNetlist), 
	  _cellToBlockMap(cellToBlockMap),
          _capoPl(capo), _hCutWSMethod(UINT_MAX),
          _optimisticAdj(UINT_MAX), _verb(verb)
{
    abkfatal(_blocks.size() == 2, "can only build problems for 2 blocks");

    initToNULL();

    setPartDims(); //use the block shapes/capacities for the partitions

    buildHGraph();

    setBuffer();

    _ownsData = false;
}

PartitioningProblemForCapo::PartitioningProblemForCapo(
                const vector<const CapoBlock*>&        blocks,
                const HGraphFixed&              hgraphOfNetlist,
                const CapoPlacer&               capo,
                const vector<const CapoBlock*>& cellToBlockMap,
		const vector<double>&           partCaps,
                Verbosity verb)
        : PartitioningProblem(), _blocks(blocks),
          _hgraphOfNetlist(hgraphOfNetlist), 
	  _cellToBlockMap(cellToBlockMap),
          _capoPl(capo), _hCutWSMethod(UINT_MAX),
          _optimisticAdj(UINT_MAX), _verb(verb)
{
    abkfatal(_blocks.size() == 2, "can only build problems for 2 blocks");

    initToNULL();

    setPartDims(partCaps); //use the block shapes/capacities for the partitions

    buildHGraph();

    setBuffer();

    _ownsData = false;
}

PartitioningProblemForCapo::~PartitioningProblemForCapo()
{
        if(_hgraph != NULL)
        {
            delete _subHG;
            _hgraph = _subHG = NULL;
        }

        if(_fixedConstr != NULL)
        {
            delete _fixedConstr;
            _fixedConstr = NULL;
        }

        if(_solnBuffers != NULL)
        {
            delete _solnBuffers;
            _solnBuffers = NULL;
        }

        if(_capacities != NULL)
        {
            delete _capacities;
            _capacities = NULL;
        }

        if(_partitions != NULL)
        {
            delete _partitions;
            _partitions = NULL;
        }

        if(_padBlocks != NULL)
        {
            delete _padBlocks;
            _padBlocks = NULL;
        }
        if(_terminalToBlock != NULL)
        {
            delete _terminalToBlock;
            _terminalToBlock = NULL;
        }
}


