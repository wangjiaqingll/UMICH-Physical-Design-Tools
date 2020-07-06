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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
using std::sort;
#include "splitRow.h"
#include "splitLarge.h"

bit_vector SplitRowOfBlocks::isInTopBlk;


SplitRowOfBlocks::SplitRowOfBlocks( vector<CapoBlock*>&  rowOfBlocks,
			   	    const CapoPlacer&    capo,
		      		    CapoSplitterParams   params)
	: BaseBlockSplitter(*rowOfBlocks[0], capo, params),
	  _rowOfBlocks(rowOfBlocks), _wasGoodSplit(true)
{
  abkfatal(_rowOfBlocks.size() > 1, "SplitRowOfBlocks expects > 1 block");

  if(_params.verb.forMajStats > 2)
	cout<<"Using RowSplitter on a row of "<<rowOfBlocks.size()
	    <<" blocks"<<endl;

  unsigned k;
  for(k = 0; k < rowOfBlocks.size(); k++)
  {
    if(_params.verb.forMajStats > 3) cout<<*rowOfBlocks[k]<<endl;
    else if(_params.verb.forMajStats > 2) 
		cout<<"["<<rowOfBlocks[k]->getIndex()<<"]"<<endl;
  }

  if(isInTopBlk.size() < capo.getNetlistHGraph().getNumNodes())
	isInTopBlk = bit_vector(capo.getNetlistHGraph().getNumNodes());

  //construct the big block
  sort(rowOfBlocks.begin(), rowOfBlocks.end(), CompareSrcBlocksByX());
  CapoBlock rowBlock(rowOfBlocks);
  const_cast<CapoPlacer&>(_capo).updateInfoAboutBlock(&rowBlock);

  if(_params.verb.forMajStats > 4)
      cout<<"  Created RowBlock \n"<<rowBlock<<endl<<endl;

  SplitLargeBlockHorizontally rowBlkSplitter(rowBlock, capo, _params);

  buildSubBlocks(rowBlkSplitter.getNewBlocks());

  if(_wasGoodSplit)
  {
      for(k = 0; k < rowOfBlocks.size(); k++)
          rowOfBlocks[k]->unLinkNeighbors();
      for(k = 0; k < _newBlocks.size(); k++)
	  _newBlocks[k]->linkNeighbors();
  }
  else	//put it back the way it was
  {
      for(k = 0; k < rowOfBlocks.size(); k++)
	const_cast<CapoPlacer&>(_capo).updateInfoAboutBlock(rowOfBlocks[k]);
      for(k = 0; k < _newBlocks.size(); k++)
      {
	delete _newBlocks[k];
	_newBlocks[k] = 0;
      }
      _newBlocks.clear();
  }

}
 
void SplitRowOfBlocks::buildSubBlocks(vector<CapoBlock*>& bigBlocks)
{
  CapoBlock* topBlk = bigBlocks[0];
  CapoBlock* botBlk = bigBlocks[1];

  std::vector<unsigned>::const_iterator cId;
  for(cId = topBlk->cellIdsBegin(); cId != topBlk->cellIdsEnd(); cId++)
	isInTopBlk[*cId] = true;
  for(cId = botBlk->cellIdsBegin(); cId != botBlk->cellIdsEnd(); cId++)
	isInTopBlk[*cId] = false;

  vector<CapoBlock*> newTopBlocks;
  buildOneSubRow(topBlk, true, newTopBlocks);

  vector<CapoBlock*> newBotBlocks;
  buildOneSubRow(botBlk, false, newBotBlocks);

	//hook up the neighbors, sibblings and names

  _newBlocks = newTopBlocks;
  _newBlocks.insert(_newBlocks.end(), newBotBlocks.begin(),
			newBotBlocks.end());
}

void SplitRowOfBlocks::buildOneSubRow(CapoBlock* subRowBlock, 
				bool isTopBlk, vector<CapoBlock*>& newBlks)
{
  double partLayoutArea = subRowBlock->getCapacity();

	//layout area to allocate per unit of cell area
  double layoutAreaRatio = partLayoutArea/subRowBlock->getTotalCellArea();

  vector<CapoBlock*> noNeighbor;//for top and bottom neighbors..currently
				//unpopulated!!
  vector<CapoBlock*> lNeighbors;
  vector<CapoBlock*> rNeighbors;

  vector<unsigned> cellsInBlk;
  double           areaInBlk;
  double           leftEdge = subRowBlock->getBBox().xMin;
  double	   rightEdge;

  vector<double>   newBlkCaps;

	//total area of all blocks left of the current one.
  double cumLayoutArea   = 0;

  const HGraphWDimensions& hgraph = _capo.getNetlistHGraph();
  unsigned b;
  for(b = 0; b < _rowOfBlocks.size(); b++)
  { 
	const CapoBlock& oldBlk = *_rowOfBlocks[b];
	cellsInBlk.clear();
	areaInBlk = 0;

        std::vector<unsigned>::const_iterator cId;
	for(cId = oldBlk.cellIdsBegin(); cId != oldBlk.cellIdsEnd(); cId++)
	    if(( isInTopBlk[*cId] &&  isTopBlk) ||
	       (!isInTopBlk[*cId] && !isTopBlk))
	    {
		cellsInBlk.push_back(*cId);	
		areaInBlk += hgraph.getNodeByIdx(*cId).getWeight();
	    }

	if(areaInBlk < oldBlk.getTotalCellArea()*0.2)
	{
		cout<<"Part Area: "<<areaInBlk<<" total blk area: "
			<<oldBlk.getTotalCellArea()<<endl;
		_wasGoodSplit = false;
	}
	if(areaInBlk == 0) continue;	//don't produce 0 area blocks

	cumLayoutArea       += (areaInBlk * layoutAreaRatio);

	if(b < _rowOfBlocks.size()-1)	//not the end...find the xSplit
	    rightEdge = subRowBlock->findXSplit(
			cumLayoutArea, partLayoutArea-cumLayoutArea,
			0, newBlkCaps);
	else
	    rightEdge = subRowBlock->getBBox().xMax;

	BBox partBoundary = subRowBlock->getBBox();
	partBoundary.xMin = leftEdge;
	partBoundary.xMax = rightEdge;

	char* name = new char[strlen(oldBlk.getName())+3];
	strcpy(name, oldBlk.getName());
	if(isTopBlk)
	    strcat(name, "R0");
	else
	    strcat(name, "R1");

	CapoBlock* newBlk = new CapoBlock(cellsInBlk, hgraph, 
				subRowBlock->getRows(), partBoundary,
				noNeighbor, noNeighbor, lNeighbors, 
				rNeighbors,&_capo, name);

        if(_params.verb.forActions > 4)
	    cout<<"\nBuilt new block: \n"<<*newBlk<<endl;

	leftEdge = rightEdge;	
	newBlks.push_back(newBlk);
        delete name;
  }
}

