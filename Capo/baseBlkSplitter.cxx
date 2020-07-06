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

#include "baseBlkSplitter.h"
#include "ABKCommon/abkcommon.h"
#include "RBPlace/RBPlacement.h"

	//this verson finds the best row to split at
void BaseBlockSplitter::createHSplitBlocks
		(PartitioningProblemForCapo& problem)
{
    const SubHGraph&    hgraph = problem.getSubHGraph();
    const Partitioning&  soln  = problem.getBestSoln();

    double totalCellArea       = _block.getTotalCellArea();
    double p0CellArea = 0;
    double p1CellArea = 0;

    unsigned n;
    for(n = hgraph.getNumTerminals(); n != hgraph.getNumNodes(); n++)
    {
	unsigned part   = soln[n].isInPart(0) ? 0 : 1;

	if(part==0)
		p0CellArea += hgraph.getNodeByIdx(n).getWeight();
	else
		p1CellArea += hgraph.getNodeByIdx(n).getWeight();
    }

    if(p0CellArea+p1CellArea != totalCellArea)
    {
	char errorString[1024];
	sprintf(errorString, "Areas do not sum: %f %f : %f\n",
		p0CellArea, p1CellArea, totalCellArea);
        abkfatal(0, errorString);
    }

    double   capacity    = _block.getCapacity();
    unsigned splitRow    = 1;
    double   p1Cap       = _block.getContainedAreaInCoreRow(0);
    double   p0Cap       = capacity - p1Cap;
    double   p0RelWS     = (p0Cap - p0CellArea)/p0Cap;
    double   p1RelWS     = (p1Cap - p1CellArea)/p1Cap;

    double minRelWSDelta = dbl_abs(p0RelWS - p1RelWS);

    for(unsigned r = 1; r < _block.getNumRows()-1; r++)
    {
        p1Cap += _block.getContainedAreaInCoreRow(r);

	p0Cap = capacity - p1Cap;
	p0RelWS = (p0Cap - p0CellArea)/p0Cap;
	p1RelWS = (p1Cap - p1CellArea)/p1Cap;

	double relWSDelta = dbl_abs(p0RelWS - p1RelWS);

        if(relWSDelta < minRelWSDelta)
        {
            minRelWSDelta = relWSDelta;
            splitRow = r+1;
        }
    }

//  if(_params.verb.forMajStats > 4)
// out<<"Computed splitrow is "<<splitRow<<endl;

    createHSplitBlocks(problem, splitRow);
}

	//this version uses the splitRow it's given
void BaseBlockSplitter::createHSplitBlocks
		(PartitioningProblemForCapo& problem, unsigned splitRow)
{
    vector<double>                newBlockCellAreas(2,0.0);
    vector<vector<unsigned> >     cellsInNewBlocks(2);

    const SubHGraph&    hgraph = problem.getSubHGraph();
    const Partitioning&  soln  = problem.getBestSoln();
    unsigned nId;
    for(nId = hgraph.getNumTerminals(); 
        nId != hgraph.getNumNodes(); nId++)
    {
        unsigned origId = hgraph.newNode2OrigIdx(nId);
        unsigned part = soln[nId].lowestNumPart();

        cellsInNewBlocks[part].push_back(origId);
        newBlockCellAreas[part] += hgraph.getNodeByIdx(nId).getWeight();
    }

    vector<double>         newBlockSiteAreas(2);
   
    for(unsigned r = 0; r < splitRow; r++)
        newBlockSiteAreas[1] += _block.getContainedAreaInCoreRow(r);

    newBlockSiteAreas[0] = _block.getCapacity()-newBlockSiteAreas[1];

    const vector<const RBPCoreRow*>& rows = _block.getRows();

    itCBRowOffset sBegin = _block._startOffsets.begin();
    itCBRowOffset sEnd   = _block._startOffsets.end();
    itCBRowOffset eBegin = _block._endOffsets.begin();
    itCBRowOffset eEnd   = _block._endOffsets.end();

    vector<CROffset>    p0StartOffsets(sBegin+splitRow, sEnd);
    vector<CROffset>    p0EndOffsets  (eBegin+splitRow, eEnd);
    vector<CROffset>    p1StartOffsets(sBegin, sBegin+splitRow);
    vector<CROffset>    p1EndOffsets  (eBegin, eBegin+splitRow);

    double ySplit = rows[splitRow]->getYCoord();

    _block.unLinkNeighbors();  // will link children instead
        // determine neighbors of forthcoming child blocks
    vector<CapoBlock*> p0LeftNeighbors, p1LeftNeighbors;

    unsigned k;
    for(k=0; k!=_block._leftNeighbors.size(); k++)
    {
           CapoBlock* tempB=_block._leftNeighbors[k];
           double yMax=tempB->getBBox().yMax;
           double yMin=tempB->getBBox().yMin;
           if (yMax> ySplit) p0LeftNeighbors.push_back(tempB);
           if (yMin< ySplit) p1LeftNeighbors.push_back(tempB);
    }

    vector<CapoBlock*> p0RightNeighbors, p1RightNeighbors;
    for(k=0; k!=_block._rightNeighbors.size(); k++)
    {
           CapoBlock* tempB=_block._rightNeighbors[k];
           double yMax=tempB->getBBox().yMax;
           double yMin=tempB->getBBox().yMin;
           if (yMax> ySplit) p0RightNeighbors.push_back(tempB);
           if (yMin< ySplit) p1RightNeighbors.push_back(tempB);
    }

    vector<CapoBlock*> noNeighbors; //to prevent duplicate links
                                        //between child blk... see below

    const HGraphWDimensions& nlHGraph = _capo.getNetlistHGraph();

    unsigned oldNameLen = strlen(_block.getName());
    char* blockName = new char[oldNameLen+4];
    strcpy(blockName, _block.getName()); 
    strcat(blockName, "H0_");

    if(cellsInNewBlocks[0].size() > 0)
    {
          _newBlocks.push_back(new CapoBlock( cellsInNewBlocks[0],
                                nlHGraph,
                                _block.rowsBegin()+splitRow, _block.rowsEnd(),
                                p0StartOffsets, p0EndOffsets,
                                _block._neighborsAbove, noNeighbors,
                                p0LeftNeighbors,  p0RightNeighbors,&_capo, 
			        blockName));
          _newBlocks.back()->linkNeighbors();
                         // children get linked in instead of block
    }

    blockName[oldNameLen+1] = '1';

    if(cellsInNewBlocks[1].size() > 0)
    {
	  _newBlocks.push_back(new CapoBlock( cellsInNewBlocks[1],
                                nlHGraph,
                                _block.rowsBegin(), _block.rowsBegin()+splitRow,
                                p1StartOffsets, p1EndOffsets,
                                noNeighbors, _block._neighborsBelow,
                                p1LeftNeighbors,  p1RightNeighbors,&_capo, 
				blockName));
	  _newBlocks.back()->linkNeighbors();
                        // children get linked in instead of block
    }

    if (cellsInNewBlocks[0].size() && cellsInNewBlocks[1].size())
    {
          abkfatal( _newBlocks.size() == 2, "didn't create a block");
          _newBlocks[0]->_addNeighborBelow(_newBlocks[1]);
          _newBlocks[1]->_addNeighborAbove(_newBlocks[0]);
          _newBlocks[0]->_sibling=_newBlocks[1];
          _newBlocks[1]->_sibling=_newBlocks[0];
    }
    delete blockName;
}


void BaseBlockSplitter::createVSplitBlocks
	(PartitioningProblemForCapo& problem, double xSplit)
{
    vector<double>            partAreas(2,0.0);
    vector<vector<unsigned> > cellsInNewBlocks(2);

    const Partitioning& soln = problem.getBestSoln();
    const SubHGraph&    hgraph = problem.getSubHGraph();

    for(unsigned c = hgraph.getNumTerminals();c < hgraph.getNumNodes(); c++)
    {
        unsigned origId = hgraph.newNode2OrigIdx(c);
        unsigned part = soln[c].isInPart(0) ? 0 : 1;

        cellsInNewBlocks[part].push_back(origId);
        partAreas[part] += hgraph.getNodeByIdx(c).getWeight();
    }

    createVSplitBlocks(cellsInNewBlocks, partAreas, xSplit);
}

void BaseBlockSplitter::createVSplitBlocks
		(const vector<vector<unsigned> >& cellsInNewBlocks)
{
	//we're given the cell Ids (can be assumed to all be in 
	//the block being split) which go in each new block
    vector<double>            partAreas(2,0.0);

    const HGraphWDimensions& netlistHG = _capo.getNetlistHGraph();

    unsigned n;
    for(n = 0; n < cellsInNewBlocks[0].size(); n++)
    {
	unsigned cellId = cellsInNewBlocks[0][n];
        partAreas[0] += netlistHG.getNodeByIdx(cellId).getWeight();
    }

    for(n = 0; n < cellsInNewBlocks[0].size(); n++)
    {
	unsigned cellId = cellsInNewBlocks[1][n];
        partAreas[1] += netlistHG.getNodeByIdx(cellId).getWeight();
    }

    double xSplit = _block.getBBox().xMin + 
      _block.getBBox().getWidth()*(partAreas[0]/(partAreas[0]+partAreas[1]));

    createVSplitBlocks(cellsInNewBlocks, partAreas, xSplit);
}

void BaseBlockSplitter::createVSplitBlocks
		(const vector<vector<unsigned> >& cellsInNewBlocks,
		 const vector<double>&            partAreas, 
		 double				  xSplit)
{

    static_cast<void>(partAreas);

    vector<double>            siteAreas(2,0.0);
    _block.computePartAreas(xSplit, siteAreas);

    const vector<const RBPCoreRow*>& rows = _block.getRows();

    //construct the vectors of coreRows and [start/end]offsets

    vector<const RBPCoreRow*> p0CoreRows;
    vector<const RBPCoreRow*> p1CoreRows;
    vector<CROffset>          p0StartOffsets;
    vector<CROffset>          p0EndOffsets;
    vector<CROffset>          p1StartOffsets;
    vector<CROffset>          p1EndOffsets;
    BBox                      p0BBox(_block.getBBox());
    BBox                      p1BBox(_block.getBBox());
    p0BBox.xMax = xSplit;
    p1BBox.xMin = xSplit;

    for(unsigned crId = 0; crId < rows.size(); crId++)
    {
            const RBPCoreRow& cr = *(rows[crId]);
            const double spacing   = cr.getSpacing();
            const double width   = cr.getSiteWidth();

            const CROffset&   sOffset = _block._startOffsets[crId];
            const CROffset&   eOffset = _block._endOffsets[crId];
            const RBPSubRow&  leftSR  = cr[sOffset.first];
            const RBPSubRow&  rightSR = cr[eOffset.first];

            double leftSiteEdge  = leftSR.getXStart() + spacing*sOffset.second;
	    /*
            double rightSiteEdge = rightSR.getXStart()+
                                spacing*(eOffset.second+1);
	    */
            double rightSiteEdge = rightSR.getXStart()+
	      spacing*(eOffset.second)+width;
                //eOffset.second is the last CONTAINED site, so the right
                //edge of it is the left edge of the 1st not contained site...
                //or eOffset.second+1

            if(rightSiteEdge <= xSplit)
            {   //core row is completely inP0
                p0CoreRows.    push_back(rows[crId]);
                p0StartOffsets.push_back(sOffset);
                p0EndOffsets.  push_back(eOffset);
            }
            else if(leftSiteEdge >= xSplit)
            {   //core row is completely in P1
                p1CoreRows.    push_back(rows[crId]);
                p1StartOffsets.push_back(sOffset);
                p1EndOffsets.  push_back(eOffset);
            }
            else
            {   //the coreRow is split
                p0CoreRows.    push_back(rows[crId]);
                p1CoreRows.    push_back(rows[crId]);
                p0StartOffsets.push_back(sOffset);
                p1EndOffsets.  push_back(eOffset);

                unsigned srId;
                for(srId = sOffset.first; srId <= eOffset.first; srId++)
                {
                    if(cr[srId].getXEnd() <= xSplit) continue; //totally in P0

                    if(cr[srId].getXStart() >= xSplit) //1st one totally in P1
                    {
                        p1StartOffsets.push_back(CROffset(srId,0));
                        p0EndOffsets.  push_back(
                                CROffset(srId-1, cr[srId-1].getNumSites()-1));
                        break;
                    }
                    else //this subRow crosses xSplit
                    {
                        CROffset splitOffset(srId, UINT_MAX);
                        p1StartOffsets.push_back(splitOffset);
                        p0EndOffsets.  push_back(splitOffset);

                        double siteOffset = xSplit - cr[srId].getXStart();
                        siteOffset /= spacing;

                        unsigned splitSite =
                                static_cast<unsigned>(rint(siteOffset));
                        p1StartOffsets.back().second = splitSite;
                        p0EndOffsets.back().second   = splitSite-1;
                        break;
                    }
                }
                abkassert(p0StartOffsets.size() == p0EndOffsets.size(),
                        "p0Offset size mismatch");
                abkassert(p1StartOffsets.size() == p1EndOffsets.size(),
                        "p1Offset size mismatch");
            }
    }

    // if  cellArea < siteArea in both blocks, we are fine.. quit
    // Compute the _lacking site area_ in each block. If it's the same in the two blocks, quit.
    // Otherwise, if the difference is 1 site, quit.
    // If the difference is >1, then divide by 2, to find out how many sites must be moved
    // (and we also know in which direction)
    // Allocate the moving sites to rows evenly 
    // (if we run into trouble with cell packing, we will worry about that when we see how this happens)
    //By sadya Create jagged blocks for better WS handling
    if ((rows.size()==2 || rows.size()==3) && 0)
    { 
      const HGraphWDimensions& hgraph = _capo.getNetlistHGraph();

      const RBPCoreRow& cr = *(rows[0]);
      double siteSpacing = cr.getSpacing();
      double siteHeight = cr.getHeight();

      double p0CellArea=0, p1CellArea=0;
      double p0SiteArea=0, p1SiteArea=0;
      unsigned i;
      for(i=0; i<cellsInNewBlocks[0].size(); ++i)
	{
	  unsigned idx=cellsInNewBlocks[0][i];
	  double cellArea=
            (ceil(hgraph.getNodeWidth(idx)/siteSpacing)*siteSpacing)
	    *(ceil(hgraph.getNodeHeight(idx)/siteHeight)*siteHeight);
	  p0CellArea += cellArea;
	}
      for(i=0; i<cellsInNewBlocks[1].size(); ++i)
	{
	  unsigned idx=cellsInNewBlocks[1][i];
	  double cellArea=
            (ceil(hgraph.getNodeWidth(idx)/siteSpacing)*siteSpacing)
	    *(ceil(hgraph.getNodeHeight(idx)/siteHeight)*siteHeight);
	  p1CellArea += cellArea;
	}
      
      unsigned p0NumSites=0, p1NumSites=0;
      for(i=0; i<p0CoreRows.size(); ++i)
	{
	  CROffset startOffset = p0StartOffsets[i];
	  CROffset endOffset = p0EndOffsets[i];
	  const RBPCoreRow& curRow = *p0CoreRows[i];
//        const RBPSubRow& startSR = curRow[startOffset.first];
//	  const RBPSubRow& endSR   = curRow[endOffset.first];

	  for(unsigned sr = startOffset.first; sr <= endOffset.first; sr++)
	    {
	      int sitesInThisSR = curRow[sr].getNumSites();
	      if(sr == endOffset.first)
		sitesInThisSR = endOffset.second+1;
	      if(sr == startOffset.first)
		sitesInThisSR -= startOffset.second;
	      p0NumSites += sitesInThisSR; 
	    }
	}
      for(i=0; i<p1CoreRows.size(); ++i)
	{
	  CROffset startOffset = p1StartOffsets[i];
	  CROffset endOffset = p1EndOffsets[i];
	  const RBPCoreRow& curRow = *p1CoreRows[i];
//	  const RBPSubRow& startSR = curRow[startOffset.first];
//	  const RBPSubRow& endSR   = curRow[endOffset.first];

	  for(unsigned sr = startOffset.first; sr <= endOffset.first; sr++)
	    {
	      int sitesInThisSR = curRow[sr].getNumSites();
	      if(sr == endOffset.first)
		sitesInThisSR = endOffset.second+1;
	      if(sr == startOffset.first)
		sitesInThisSR -= startOffset.second;
	      p1NumSites += sitesInThisSR; 
	    }
	}
      p0SiteArea = p0NumSites*siteHeight*siteSpacing;
      p1SiteArea = p1NumSites*siteHeight*siteSpacing;
      
      double p0WS = p0SiteArea-p0CellArea;
      double p1WS = p1SiteArea-p1CellArea;
      bool reDoSplit = true;
      if(p0WS < 0 || p1WS < 0) //special case only if -ve WS in one partition
	{
	  double p0WSsites = ceil((fabs(p0WS)/siteHeight)/siteSpacing)*
	    siteSpacing;
	  double p1WSsites = ceil((fabs(p1WS)/siteHeight)/siteSpacing)*
	    siteSpacing;
	  double p0ExtraSites=0, p1ExtraSites=0;
	  if(p0WS < 0 && p1WS < 0 && fabs(p0WSsites-p1WSsites) > 1)
	    {
	      double avg = floor((p0WSsites+p1WSsites)/2.0);
	      p0ExtraSites = avg - p0WSsites;
	      p1ExtraSites = -p0ExtraSites;
	    }
	  else if(p0WS < 0 && p1WS > 0 && (p1WSsites + p0WSsites) > 1)
	    {
	      if(p0WSsites > p1WSsites)
		{
		  p0ExtraSites = p1WSsites;
		  p1ExtraSites = -p1WSsites;
		}
	      else
		{
		  p0ExtraSites = p0WSsites;
		  p1ExtraSites = -p0WSsites;
		}
	    }
	  else if(p0WS > 0 && p1WS < 0 && (p0WSsites + p1WSsites) > 1)
	    {
	      if(p1WSsites > p0WSsites)
		{
		  p1ExtraSites = p0WSsites;
		  p0ExtraSites = -p0WSsites;
		}
	      else
		{
		  p1ExtraSites = p1WSsites;
		  p0ExtraSites = -p1WSsites;
		}
	    }
	  else
	    reDoSplit=false;
	  
	  if(reDoSplit)
	    {
	      //for now do all adds and subtracts to second row
	      if(p0CoreRows.size() > 1 && p1CoreRows.size() > 1)
		{
		  CROffset& p0StartOffset = p0StartOffsets[1];
		  CROffset& p0EndOffset = p0EndOffsets[1];
		  const RBPCoreRow& p0CurRow = *p0CoreRows[1];

		  CROffset& p1StartOffset = p1StartOffsets[1];
//		  CROffset& p1EndOffset = p1EndOffsets[1];
		  const RBPCoreRow& p1CurRow = *p1CoreRows[1];
		  
		  if(p0CurRow.getYCoord() == p1CurRow.getYCoord() && 
		     p0EndOffset.first == p1StartOffset.first) //same subrow
		    {
		      const RBPSubRow& SR = p0CurRow[p0StartOffset.first];

//		      double addedSites=0;
		      if(p0ExtraSites > 0) //add sites to P0
			{
			  double curEnd = SR.getXStart() + 
			    (p0StartOffset.second+1)*siteSpacing;
			  double newEnd = curEnd + p0ExtraSites*siteSpacing;
			  if(newEnd >= SR.getXEnd())
			    {
			      newEnd = SR.getXEnd() - siteSpacing;
			    }
			  double new0EndOffset = 
			    ceil((newEnd - SR.getXStart())/siteSpacing);
			  double new1StartOffset = new0EndOffset+1;

			  p0EndOffset.second = unsigned(new0EndOffset);
			  p1StartOffset.second = unsigned(new1StartOffset);
			}
		      else //add sites to P1
			{
			  double curEnd = SR.getXStart() + 
			    (p0StartOffset.second+1)*siteSpacing;
			  double newEnd = curEnd + p0ExtraSites*siteSpacing;
			  if(newEnd <= SR.getXStart())
			    {
			      newEnd = SR.getXStart() + siteSpacing;
			    }
			  double new0EndOffset = 
			    ceil((newEnd - SR.getXStart())/siteSpacing);
			  double new1StartOffset = new0EndOffset+1;
			  p0EndOffset.second = unsigned(new0EndOffset);
			  p1StartOffset.second = unsigned(new1StartOffset);
			}
		    }
		}
	    }
	}      
    }

    _block.unLinkNeighbors();  // will link children instead

      // determine neighbors of forthcoming child blocks
        vector<CapoBlock*> p0NeighborsAbove, p1NeighborsAbove;
        unsigned k;
        for(k=0; k!=_block._neighborsAbove.size(); k++)
        {
           CapoBlock* tempB=_block._neighborsAbove[k];
           double xMax=tempB->getBBox().xMax;
           double xMin=tempB->getBBox().xMin;

	   if (xMin<xSplit) p0NeighborsAbove.push_back(tempB);
	   if (xMax>xSplit) p1NeighborsAbove.push_back(tempB);
        }

        vector<CapoBlock*> p0NeighborsBelow, p1NeighborsBelow;
        for(k=0; k!=_block._neighborsBelow.size(); k++)
        {
           CapoBlock* tempB=_block._neighborsBelow[k];
           double xMax=tempB->getBBox().xMax;
           double xMin=tempB->getBBox().xMin;

	   if (xMin<xSplit) p0NeighborsBelow.push_back(tempB);
	   if (xMax>xSplit) p1NeighborsBelow.push_back(tempB);
        }

        vector<CapoBlock*> noNeighbors;
                        //to prevent duplicate links between child blk
    if((p0CoreRows.size() == 0 || p1CoreRows.size() == 0) && 0)
      _newBlocks = vector<CapoBlock*>(1);
    else
      _newBlocks = vector<CapoBlock*>(2);

    unsigned oldNameLen = strlen(_block.getName());
    char* blockName = new char[oldNameLen+5];
    strcpy(blockName, _block.getName()); 
    strcat(blockName, "V0_");

    const HGraphWDimensions& nlHGraph = _capo.getNetlistHGraph();

    if((p0CoreRows.size() == 0 || p1CoreRows.size() == 0) && 0)
      {
	cout<<"Block BBox is "<<p0BBox.xMin<<" "<<p0BBox.yMin<<" : "
	    <<p1BBox.xMax<<" : "<<p1BBox.yMax<<endl;
	cout<<"This xSplit is "<<xSplit<<endl;
	cout<<"numRows "<<p0CoreRows.size()<<"  "<<p1CoreRows.size()<<endl;
      }
    
    if((p0CoreRows.size() !=0 && p1CoreRows.size() !=0) || 1)
      {
	_newBlocks[0] = new CapoBlock(cellsInNewBlocks[0],
				      nlHGraph,
				      p0CoreRows.begin(), p0CoreRows.end(),
				      p0StartOffsets, p0EndOffsets,
				      p0NeighborsAbove, p0NeighborsBelow,
				      _block._leftNeighbors, noNeighbors,&_capo,
				      blockName);

	blockName[oldNameLen+1] = '1';
	_newBlocks[1] = new CapoBlock(cellsInNewBlocks[1],
				      nlHGraph,
				      p1CoreRows.begin(), p1CoreRows.end(),
				      p1StartOffsets, p1EndOffsets,
				      p1NeighborsAbove, p1NeighborsBelow,
				      noNeighbors, _block._rightNeighbors,&_capo,
				      blockName);
	_newBlocks[0]->linkNeighbors();
	_newBlocks[1]->linkNeighbors();
	_newBlocks[0]->_addRightNeighbor(_newBlocks[1]);
	_newBlocks[1]->_addLeftNeighbor(_newBlocks[0]);
	_newBlocks[0]->_sibling=_newBlocks[1];
	_newBlocks[1]->_sibling=_newBlocks[0];
	// cout<<"done building"<<endl;
      }
    else if(p0CoreRows.size() !=0 && p1CoreRows.size() == 0)
      {
	vector<unsigned> cellsInNewBlocksLocal(cellsInNewBlocks[0]);
	cellsInNewBlocksLocal.insert(cellsInNewBlocksLocal.end(),
				     cellsInNewBlocks[1].begin(),
				     cellsInNewBlocks[1].end());

	_newBlocks[0] = new CapoBlock(cellsInNewBlocksLocal,
				      nlHGraph,
				      p0CoreRows.begin(), p0CoreRows.end(),
				      p0StartOffsets, p0EndOffsets,
				      _block._neighborsAbove,
				      _block._neighborsBelow,
				      _block._leftNeighbors, 
				      _block._rightNeighbors,&_capo,
				      blockName);
	_newBlocks[0]->linkNeighbors();
      }
    else if(p0CoreRows.size() == 0 && p1CoreRows.size() != 0)
      {
	vector<unsigned> cellsInNewBlocksLocal(cellsInNewBlocks[1]);
	cellsInNewBlocksLocal.insert(cellsInNewBlocksLocal.end(),
				     cellsInNewBlocks[0].begin(),
				     cellsInNewBlocks[0].end());

	_newBlocks[0] = new CapoBlock(cellsInNewBlocksLocal,
				      nlHGraph,
				      p1CoreRows.begin(), p1CoreRows.end(),
				      p1StartOffsets, p1EndOffsets,
				      _block._neighborsAbove,
				      _block._neighborsBelow,
				      _block._leftNeighbors, 
				      _block._rightNeighbors,&_capo,
				      blockName);
	_newBlocks[0]->linkNeighbors();
      }
    else
      abkfatal(0,"While splitting block vertically no rows found\n");
    delete blockName;
}

    //P0 is the top partition
    //capoBlock rows are sorted in INCREASING Y
    //so..P0 will contain rows [_splitRow->(numrows-1)]
    //and P1 rows [0->(_splitRow-1)]
    //(_splitRow, then, gives the number of rows in p1..or,
    //equivlently, the index of the bottom row in P0)

    //WARNING: side-effect! populates actualCaps

unsigned BaseBlockSplitter::findBestSplitRow(const vector<double>& cellAreas,
					     vector<double>& actualCaps)
{
  double   totalCap    = _block.getCapacity();
  unsigned splitRow    = 1;
  double   p1Cap       = _block.getContainedAreaInCoreRow(0);
  double   p0Cap       = totalCap-p1Cap;
  double relP1WS = (p1Cap - cellAreas[1])/p1Cap;
  double relP0WS = (p0Cap - cellAreas[0])/p0Cap;
  double   bestWSDelta = dbl_abs(relP0WS - relP1WS);
  double   bestP1Cap   = p1Cap;

  for(unsigned r = 1; r < _block.getNumRows()-1; r++)
    {
      p1Cap  += _block.getContainedAreaInCoreRow(r);
      p0Cap   = totalCap-p1Cap;
      relP1WS = (p1Cap - cellAreas[1])/p1Cap;
      relP0WS = (p0Cap - cellAreas[0])/p0Cap;
      double relWSDelta = dbl_abs(relP0WS - relP1WS);

      if(relWSDelta < bestWSDelta)
        {
	  bestP1Cap   = p1Cap;
	  bestWSDelta = relWSDelta;
	  splitRow    = r+1;
        }
    }

  if(splitRow == 0 || splitRow > _block.getNumRows())
    {
      cout<<endl<<endl;
      cout<<"ERROR: new block has no rows"<<endl;

      cout<<"Cell Areas: "<<cellAreas[0]<<"  "<<cellAreas[1]<<endl;
      cout<<"SplitRow:   "<<splitRow<<endl;
      cout<<_block<<endl;
      abkfatal(splitRow != 0 && splitRow < _block.getNumRows(),
	       "new blk has no rows");
    }

  actualCaps = vector<double>(2,0.0);
  actualCaps[0] = _block.getCapacity()-bestP1Cap;
  actualCaps[1] = bestP1Cap;
  return splitRow;
}

bool BaseBlockSplitter::hPartCapsFrmRgnConstr(unsigned splitRow,
					      vector<double>& partCaps)
{
  bool changeDefaultFlow=true;
  const BBox& bbox = _block.getBBox();
  BBox bbox0 = bbox;
  BBox bbox1 = bbox;
  const vector<const RBPCoreRow*>& rows = _block.getRows();
  double ySplit = rows[splitRow]->getYCoord();
  bbox0.yMin = ySplit;
  bbox1.yMax = ySplit;

  double p0InitSiteArea=0;
  double p1InitSiteArea=0;

  for(unsigned r = 0; r < splitRow; r++)
    p1InitSiteArea += _block.getContainedAreaInCoreRow(r);

  p0InitSiteArea = _block.getCapacity()-p1InitSiteArea;

  double reqdCellArea0=0;
  double reqdCellArea1=0;
  double containedArea0=0;
  double containedArea1=0;

  for(unsigned i=0; i<_capo.regionUtilization.size(); ++i)
    {
      const BBox& rgnBBox = _capo.regionUtilization[i].first;
      double rgnUtil = _capo.regionUtilization[i].second;
      BBox intersect0 = bbox0.intersect(rgnBBox);
      BBox intersect1 = bbox1.intersect(rgnBBox);
      double temp0 = _block.getContainedAreaInBBox(intersect0);
      double temp1 = _block.getContainedAreaInBBox(intersect1);
      containedArea0 += temp0;
      containedArea1 += temp1;
      reqdCellArea0 += rgnUtil*temp0;
      reqdCellArea1 += rgnUtil*temp1;
    }
  if(containedArea0 == 0 && containedArea1 == 0)
    changeDefaultFlow = false;
  
  double remainingCellArea = _block.getTotalCellArea()-reqdCellArea0-reqdCellArea1;

  double p0SiteArea = p0InitSiteArea - containedArea0;
  double p1SiteArea = p1InitSiteArea - containedArea1;

  //cout<<"p0InitSiteArea "<<p0InitSiteArea<<"p1InitSiteArea "<<p1InitSiteArea<<endl;
  //cout<<"p0SiteArea "<<p0SiteArea<<" p1SiteArea "<<p1SiteArea<<endl;
  //cout<<"splitRow "<<splitRow<<" ySplit "<<ySplit<<endl;

  if((p0SiteArea+p1SiteArea) <= 0)
    {
      //partCaps[0] = reqdCellArea0/(reqdCellArea0+reqdCellArea1);
      //partCaps[1] = reqdCellArea1/(reqdCellArea0+reqdCellArea1);
      partCaps[0] = p0InitSiteArea/(p0InitSiteArea+p1InitSiteArea);
      partCaps[1] = p1InitSiteArea/(p0InitSiteArea+p1InitSiteArea);      
      return (false);
    }

  //TO DO minimize rel WS here
  reqdCellArea0 += remainingCellArea*p0SiteArea/(p0SiteArea+p1SiteArea);
  reqdCellArea1 += remainingCellArea*p1SiteArea/(p0SiteArea+p1SiteArea);

  //cout<<"reqdCellArea0 "<<reqdCellArea0<<" reqdCellArea1 "<<reqdCellArea1<<endl;

  if (reqdCellArea0 < 0)
    reqdCellArea0 = 0;
  if (reqdCellArea1 < 0)
    reqdCellArea1 = 0;

  double totalCellArea = reqdCellArea0 + reqdCellArea1;
  if(totalCellArea == 0)
    {
      partCaps[0] = 0.5;
      partCaps[1] = 0.5;
    }
  else
    {
      partCaps[0] = reqdCellArea0/totalCellArea;
      partCaps[1] = reqdCellArea1/totalCellArea;
    }
  return(changeDefaultFlow);
}

bool BaseBlockSplitter::vPartCapsFrmRgnConstr(double xSplit,
					      vector<double>& partCaps)
{
  bool changeDefaultFlow=true;
  const BBox& bbox = _block.getBBox();
  BBox bbox0 = bbox;
  BBox bbox1 = bbox;
  bbox0.xMax = xSplit;
  bbox1.xMin = xSplit;

  double p0InitSiteArea = _block.getContainedAreaInBBox(bbox0);
  double p1InitSiteArea = _block.getCapacity()-p0InitSiteArea;;

  double reqdCellArea0=0;
  double reqdCellArea1=0;
  double containedArea0=0;
  double containedArea1=0;

  for(unsigned i=0; i<_capo.regionUtilization.size(); ++i)
    {
      const BBox& rgnBBox = _capo.regionUtilization[i].first;
      double rgnUtil = _capo.regionUtilization[i].second;
      BBox intersect0 = bbox0.intersect(rgnBBox);
      BBox intersect1 = bbox1.intersect(rgnBBox);
      double temp0 = _block.getContainedAreaInBBox(intersect0);
      double temp1 = _block.getContainedAreaInBBox(intersect1);
      containedArea0 += temp0;
      containedArea1 += temp1;
      reqdCellArea0 += rgnUtil*temp0;
      reqdCellArea1 += rgnUtil*temp1;
    }
  if(containedArea0 == 0 && containedArea1 == 0)
    changeDefaultFlow = false;
  
  double remainingCellArea = _block.getTotalCellArea()-reqdCellArea0-reqdCellArea1;

  //cout<<"p0SiteArea "<<p0SiteArea<<" p1SiteArea "<<p1SiteArea<<endl;
  //cout<<"containedArea0 "<<containedArea0<<" containedArea1 "<<containedArea1<<endl;
  double p0SiteArea = p0InitSiteArea - containedArea0;
  double p1SiteArea = p1InitSiteArea - containedArea1;

  if((p0SiteArea+p1SiteArea) <= 0)
    {
      //partCaps[0] = reqdCellArea0/(reqdCellArea0+reqdCellArea1);
      //partCaps[1] = reqdCellArea1/(reqdCellArea0+reqdCellArea1);
      partCaps[0] = p0InitSiteArea/(p0InitSiteArea+p1InitSiteArea);
      partCaps[1] = p1InitSiteArea/(p0InitSiteArea+p1InitSiteArea);
      return (false);
    }

  //TO DO minimize rel WS here
  reqdCellArea0 += remainingCellArea*p0SiteArea/(p0SiteArea+p1SiteArea);
  reqdCellArea1 += remainingCellArea*p1SiteArea/(p0SiteArea+p1SiteArea);

  //cout<<"reqdCellArea0 "<<reqdCellArea0<<"reqdCellArea1 "<<reqdCellArea1<<endl;

  if (reqdCellArea0 < 0)
    reqdCellArea0 = 0;
  if (reqdCellArea1 < 0)
    reqdCellArea1 = 0;

  double totalCellArea = reqdCellArea0 + reqdCellArea1;
  if(totalCellArea == 0)
    {
      partCaps[0] = 0.5;
      partCaps[1] = 0.5;
    }
  else
    {
      partCaps[0] = reqdCellArea0/totalCellArea;
      partCaps[1] = reqdCellArea1/totalCellArea;
    }
  //cout<<"partCaps "<<partCaps<<endl;
  return(changeDefaultFlow);
}

//splitDir=0 is horiz split    splitDir=1 is vertical split
bool BaseBlockSplitter::partAreasForFillerCells(bool splitDir, 
						vector<double>& partAreas)
{
  double stdCellArea=0;
  double fillerCellArea=0;
  const BBox& blkBBox = _block.getBBox();
  const RBPlacement& rbplace = _capo.getRBP();
  const BBox& coreBBox = const_cast<RBPlacement&>(rbplace).getBBox(false);
  const HGraphWDimensions& hgraph = _capo.getRBP().getHGraph();
  double coreXMid = (coreBBox.xMax + coreBBox.xMin)/2.0;
  double coreYMid = (coreBBox.yMax + coreBBox.yMin)/2.0;
  double blkXMid = (blkBBox.xMax + blkBBox.xMin)/2.0;
  double blkYMid = (blkBBox.yMax + blkBBox.yMin)/2.0;

  const vector<unsigned>& cellIds = _block.getCellIds();
  for(unsigned i=0; i<cellIds.size(); ++i)
    {
      unsigned idx=cellIds[i];
      HGFNode& node=const_cast<HGFNode&>(hgraph.getNodeByIdx(idx));
      double cellArea=
	hgraph.getNodeWidth(idx)*hgraph.getNodeHeight(idx);
      unsigned numEdges = node.getNumEdges();
      if(numEdges > 0)
	stdCellArea  += cellArea;
      else
	fillerCellArea += cellArea;
    }
  double totalArea = stdCellArea + fillerCellArea;
  double fillerCellRatio = fillerCellArea/totalArea;
  //double stdCellRatio = 1 - fillerCellRatio;
  if(fillerCellRatio < 0.1 || fillerCellRatio > 0.9)
    return false;
  double fillerCellPartRatio = fillerCellArea/(3.0*totalArea);
  double totalCellArea = _block.getTotalCellArea();

  bool fillerCellPart=0; //in which partition do filler cells go
  if(splitDir == 0)//horiz split
    {
      if(blkBBox.yMax <= coreYMid)
	fillerCellPart = 1;
      else if(blkBBox.yMin >= coreYMid)
	fillerCellPart = 0;
      else if(blkYMid <= coreYMid)
	fillerCellPart = 1;
      else
	fillerCellPart = 0;
    }
  else //vertical split
    {
      if(blkBBox.xMax <= coreXMid)
	fillerCellPart = 0;
      else if(blkBBox.xMin >= coreXMid)
	fillerCellPart = 1;
      else if(blkXMid <= coreXMid)
	fillerCellPart = 0;
      else
	fillerCellPart = 1;
    }
  
  if(fillerCellPart == 0)
    {
      partAreas[0] = fillerCellPartRatio*totalCellArea;
      partAreas[1] = totalCellArea - partAreas[0];
    }
  else
    {
      partAreas[1] = fillerCellPartRatio*totalCellArea;
      partAreas[0] = totalCellArea - partAreas[1];
    }
  return true;
}
