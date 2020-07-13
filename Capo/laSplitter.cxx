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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "laSplitter.h"
#include "splitLarge.h"
#include "PlaceEvals/bboxPlEval.h"

BitBoard LookAheadSplitter::_netIsInBlock(0);


bool splitHorizontally(const CapoBlock& block)
{
    unsigned numRows =block.getNumRows();
    unsigned numCells=block.getNumCells();

    return( numRows > 1 && (numCells < 15*numRows ||
                block.getHeight() > block.getWidth()));
}

LookAheadSplitter::LookAheadSplitter(      
		      CapoBlock&  blockToSplit,
		const CapoPlacer& capo,
		      unsigned    branchingFactor,
		      unsigned    lookAheadLvls,
		      Verbosity   verb)
	: BaseBlockSplitter(blockToSplit, capo, verb),
	  _branchingFactor(branchingFactor), _lookAheadLvls(lookAheadLvls),
	  _estWL(_branchingFactor,0.0), _pinCounts(_branchingFactor)
{
    if(_params.verb.forActions > 1)
	cout<<" Using LookAhead Splitter:  BF == "<<_branchingFactor
		<<"  LA == "<<_lookAheadLvls<<endl;

    _params.numMLSets = branchingFactor;


    setNetsBitBoard();

	//this is so that the temp blocks which are created don't
	//screw up adjacent block's neighbor pointers.
	//1) disconect blockToSplit from its neighbors
	//2) store blockToSplit's vectors of neighbors
	//3) clear bTS's vectors of neighbors (so it thinks it doesn't have any)
	//4) split the block as many times/levels as desired
	//5) from the saved neighbor vectors, hook-up the new blocks!

    _block.unLinkNeighbors();
    _bTSLeftNeighbors   = _block._leftNeighbors;
    _bTSRightNeighbors  = _block._rightNeighbors;
    _bTSNeighborsAbove  = _block._neighborsAbove;
    _bTSNeighborsBelow  = _block._neighborsBelow;
    _block._leftNeighbors.clear(); 
    _block._rightNeighbors.clear(); 
    _block._neighborsAbove.clear(); 
    _block._neighborsBelow.clear(); 

    //unsigned numMLSets = 1;

    Verbosity sVerb = _params.verb;
    sVerb.forActions  /=4;
    sVerb.forMajStats /=4;
    sVerb.forSysRes   /=4;

    unsigned b;
    for(b = 0; b < _branchingFactor; b++)
    {
	if(_params.verb.forActions > 2)
		cout<<"  Branch  "<<b<<endl;
	vector<CapoBlock*>  miniLayout;
	vector<CapoBlock*>  newMiniLayout;

	miniLayout.push_back(&_block);
	const_cast<CapoPlacer&>(_capo).updateInfoAboutBlock(&_block);

	for(unsigned lvl = 0; lvl <= _lookAheadLvls; lvl++)
	{
	    if(_params.verb.forActions > 3)
		cout<<"    LookAhead level "<<lvl<<endl;
  	    for(unsigned blk = 0; blk < miniLayout.size(); blk++)
	    {
		BaseBlockSplitter* subSplitter;
		CapoBlock& subBlk = *miniLayout[blk];
		if(splitHorizontally(subBlk))
	    	    subSplitter = new SplitLargeBlockHorizontally(subBlk, 
						_capo, _params);
		else
	    	    subSplitter = new SplitLargeBlockVertically(subBlk, 
						_capo, _params);

		if(miniLayout[blk] == &_block)	//top level block..
		{
		    //this is one of the splits we're deciding between
	            _topLevelBlocks.push_back(subSplitter->getNewBlocks());
		}
		else if(miniLayout[blk] != _topLevelBlocks.back()[0] && 
		        miniLayout[blk] != _topLevelBlocks.back()[1])
			delete miniLayout[blk];

	//yes..this is a Bad Thing...but I don't have a better soln
	//immediately w/o changing a lot of const->non-cost, and
	//making this public
	  const_cast<CapoPlacer&>(_capo).updateInfoAboutBlock(
					subSplitter->getNewBlocks()[0]);
	  const_cast<CapoPlacer&>(_capo).updateInfoAboutBlock(
					subSplitter->getNewBlocks()[1]);

		newMiniLayout.push_back(subSplitter->getNewBlocks()[0]);
		newMiniLayout.push_back(subSplitter->getNewBlocks()[1]);

		delete subSplitter;
	    }
	    miniLayout = newMiniLayout;
	    newMiniLayout.clear();
	}

  	for(unsigned blk = 0; blk < miniLayout.size(); blk++)
	{
	    if(miniLayout[blk] != _topLevelBlocks.back()[0] && 
	       miniLayout[blk] != _topLevelBlocks.back()[1])
	    {
		delete miniLayout[blk];
	        miniLayout[blk] = NULL;
	    }
	}

        _estWL[b] = evalBlocksWL(_capo.getPlacement());

	if(_params.verb.forMajStats > 2)	
   	{
	  cout<<"  Measured HPWL:  "<<_estWL[b]<<endl;
	  cout<<endl;
	}
    }

    double   bestWL   = _estWL[0];
    unsigned bestSoln = 0;

    for(b = 1; b < _branchingFactor; b++)
	if(_estWL[b] < bestWL)
	{
	    bestSoln = b;
	    bestWL = _estWL[b]; 
	}

    if(_params.verb.forMajStats > 2)
        cout<<"Best solution was "<<bestSoln<<endl;

    _newBlocks = _topLevelBlocks[bestSoln];
    _topLevelBlocks[bestSoln][0] = NULL;
    _topLevelBlocks[bestSoln][1] = NULL;

	//now, we have to hookup the neighbors!!
    attachNeighbors();
    _newBlocks[0]->_index = _block.getIndex()+1;
    _newBlocks[1]->_index = _block.getIndex()+2;
    CapoBlock::_nextBlockNum = _block.getIndex()+3;

    _netIsInBlock.clear();
}

void LookAheadSplitter::setNetsBitBoard()
{
    const HGraphFixed& hgraph = _capo.getNetlistHGraph();

    if(_netIsInBlock.getSize() < hgraph.getNumEdges())
	_netIsInBlock = BitBoard(hgraph.getNumEdges());

    std::vector<unsigned>::const_iterator cIt;
    for(cIt = _block.cellIdsBegin(); cIt != _block.cellIdsEnd(); cIt++)
    {
	const HGFNode& node = hgraph.getNodeByIdx(*cIt);
	for(itHGFEdgeLocal e = node.edgesBegin(); e != node.edgesEnd(); e++)
	    _netIsInBlock.setBit((*e)->getIndex());
    }
}

double LookAheadSplitter::evalBlocksWL(const Placement& pl)
{
    double totalWL = 0;
    BBox   netsBBox;

    const HGraphFixed& netlistHG = _capo.getNetlistHGraph();

    std::vector<unsigned>::const_iterator eId;
    const vector<unsigned>& setBits = _netIsInBlock.getIndicesOfSetBits();
    for(eId = setBits.begin(); eId != setBits.end(); eId++)
    {
	netsBBox.clear();
	const HGFEdge& edge = netlistHG.getEdgeByIdx(*eId);	
	for(itHGFNodeLocal n = edge.nodesBegin(); n != edge.nodesEnd(); n++)
	    netsBBox += pl[(*n)->getIndex()];

	totalWL += netsBBox.getHalfPerim();
    }     

    return totalWL;
}

LookAheadSplitter::~LookAheadSplitter()
{
   for(unsigned b = 0; b < _topLevelBlocks.size(); b++)
   {
     	if(_topLevelBlocks[b][0]) delete _topLevelBlocks[b][0]; 
     	if(_topLevelBlocks[b][1]) delete _topLevelBlocks[b][1]; 
	_topLevelBlocks[b][0] = NULL;
	_topLevelBlocks[b][1] = NULL;
   }
}

void LookAheadSplitter::attachNeighbors()
{
    if(_newBlocks[0]->getCenter().x < _newBlocks[1]->getCenter().x)
    {
	abkfatal(_newBlocks[0]->getCenter().y == 
		 _newBlocks[1]->getCenter().y , "blocks are off center-y");

		//the blocks will be attached to eachother, and we
		//don't want to link this 2x...so..
	_newBlocks[0]->_rightNeighbors.clear();
	_newBlocks[1]->_leftNeighbors.clear();

	double xSplit = (_newBlocks[0]->getBBox().xMax + 
			 _newBlocks[0]->getBBox().xMin)/2.0;

        vector<CapoBlock*> p0NeighborsAbove, p1NeighborsAbove;
        unsigned k;
        for(k=0; k!=_bTSNeighborsAbove.size(); k++)
        {
           CapoBlock* tempB=_bTSNeighborsAbove[k];
           double xMax=tempB->getBBox().xMax;
           double xMin=tempB->getBBox().xMin;
           if (xMin<xSplit) p0NeighborsAbove.push_back(tempB);
           if (xMax>xSplit) p1NeighborsAbove.push_back(tempB);
        }

        vector<CapoBlock*> p0NeighborsBelow, p1NeighborsBelow;
        for(k=0; k!=_bTSNeighborsBelow.size(); k++)
        {
           CapoBlock* tempB=_bTSNeighborsBelow[k];
           double xMax=tempB->getBBox().xMax;
           double xMin=tempB->getBBox().xMin;
           if (xMin<xSplit) p0NeighborsBelow.push_back(tempB);
           if (xMax>xSplit) p1NeighborsBelow.push_back(tempB);
        }

        for(k = 0; k < p0NeighborsAbove.size(); k++)
	    _newBlocks[0]->_addNeighborAbove(p0NeighborsAbove[k]);
        for(k = 0; k < p0NeighborsBelow.size(); k++)
	    _newBlocks[0]->_addNeighborBelow(p0NeighborsBelow[k]);
        for(k = 0; k < _bTSLeftNeighbors.size(); k++)
	    _newBlocks[0]->_addLeftNeighbor(_bTSLeftNeighbors[k]);


        for(k = 0; k < p1NeighborsAbove.size(); k++)
	    _newBlocks[1]->_addNeighborAbove(p1NeighborsAbove[k]);
        for(k = 0; k < p1NeighborsBelow.size(); k++)
	    _newBlocks[1]->_addNeighborBelow(p1NeighborsBelow[k]);
        for(k = 0; k < _bTSRightNeighbors.size(); k++)
	    _newBlocks[1]->_addRightNeighbor(_bTSRightNeighbors[k]);
 
        _newBlocks[0]->linkNeighbors();
        _newBlocks[1]->linkNeighbors();

        _newBlocks[0]->_addRightNeighbor(_newBlocks[1]);
        _newBlocks[1]->_addLeftNeighbor(_newBlocks[0]);
    }
    else	//horizontal cut
    {
	abkfatal(_newBlocks[0]->getCenter().x == 
		 _newBlocks[1]->getCenter().x , "blocks are off center-x");

		//the blocks will be attached to eachother, and we
		//don't want to link this 2x...so..
	_newBlocks[0]->_neighborsBelow.clear();
	_newBlocks[1]->_neighborsAbove.clear();

        double ySplit = (_newBlocks[0]->getBBox().yMin + 
		         _newBlocks[1]->getBBox().yMax)/2.0;

        vector<CapoBlock*> p0LeftNeighbors, p1LeftNeighbors;

        unsigned k;
        for(k=0; k!=_bTSLeftNeighbors.size(); k++)
        {
           CapoBlock* tempB=_bTSLeftNeighbors[k];
           double yMax=tempB->getBBox().yMax;
           double yMin=tempB->getBBox().yMin;
           if (yMax> ySplit) p0LeftNeighbors.push_back(tempB);
           if (yMin< ySplit) p1LeftNeighbors.push_back(tempB);
        }

	vector<CapoBlock*> p0RightNeighbors, p1RightNeighbors;
        for(k=0; k!=_bTSRightNeighbors.size(); k++)
        {
           CapoBlock* tempB=_bTSRightNeighbors[k];
           double yMax=tempB->getBBox().yMax;
           double yMin=tempB->getBBox().yMin;
           if (yMax> ySplit) p0RightNeighbors.push_back(tempB);
           if (yMin< ySplit) p1RightNeighbors.push_back(tempB);
        }

	for(k = 0; k < p0LeftNeighbors.size(); k++)
	    _newBlocks[0]->_addLeftNeighbor(p0LeftNeighbors[k]);
	for(k = 0; k < p0RightNeighbors.size(); k++)
	    _newBlocks[0]->_addRightNeighbor(p0RightNeighbors[k]);
	for(k = 0; k < _bTSNeighborsAbove.size(); k++)
	    _newBlocks[0]->_addNeighborAbove(_bTSNeighborsAbove[k]);

	for(k = 0; k < p1LeftNeighbors.size(); k++)
	    _newBlocks[1]->_addLeftNeighbor(p1LeftNeighbors[k]);
	for(k = 0; k < p1RightNeighbors.size(); k++)
	    _newBlocks[1]->_addRightNeighbor(p1RightNeighbors[k]);
	for(k = 0; k < _bTSNeighborsBelow.size(); k++)
	    _newBlocks[1]->_addNeighborBelow(_bTSNeighborsBelow[k]);

        _newBlocks[0]->linkNeighbors();
        _newBlocks[1]->linkNeighbors();

        _newBlocks[0]->_addNeighborBelow(_newBlocks[1]);
        _newBlocks[1]->_addNeighborAbove(_newBlocks[0]);
    }
} 

