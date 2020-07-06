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













// Created: April 1, 1998 by I.Markov by splitting A.Caldwell's capoPlacer.cxx

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

//CHANGES
//970923 aec moving to new Capo structure: major changes include removal pf
//		padBlock, addition of a persistent slice, move away from
//		the 'changeNetlist' 'changeLayout' design into a unified
//		'refineBlock' structure.
//971113 aec added class CapoPlacer::Parameters
//971118 aec added SA multi-start and multi-level params to 
//			CapoPlacer::Paremeters
//980215 aec moved parameter code to capoParams.cxx
//980305 ilm removed fixedConstraint handling for Partitioner::Parameters
//       ilm split capoPlacer.cxx into this file, splitBlock.cxx and the rest
//       ilm introduced "CapoPlacer::splitBlock()"
//990319 ilm refineBlock() now returns bool "this block has children that
//           need to be partitioned/placed"
//001203 phm Fixed refine block to use aspect ratio, in the same manner
//           as Feng Shui.  This should be a cmd line arg at some point
//           in the future, or calculated within the code.//001206 ilm The above kicks in only when average row spacing is at least 1.5x
//           the row height



#include "ABKCommon/abkcommon.h"
#include "Placement/placement.h"
#include "SmallPlacers/sRowSmPlacer.h"
#include "capoPlacer.h"
#include "capoBlock.h"
#include "partProbForCapo.h"
#include "splitSmall.h"
#include "splitLarge.h"
#include "laSplitter.h"
#include "splitRow.h"
#include "RBPlace/RBPlacement.h"
#include "CongestionMaps/CongestionMaps.h"

//#include "kPlSplitter.h"

bool CapoPlacer::refineBlock(CapoBlock&          block,
			     vector<CapoBlock*>& newLayout, 
			     unsigned            layerNum, 
			     AllowedPartDir      partDir,
			     unsigned            minNumCellsToPart,
			     bool                useCongestionInfo)
{
    static_cast<void>(layerNum);
    unsigned numLeaves = block.getNumCells();   

    //if(_params.trackRentNums && numLeaves >= 10)
    //	storeRentNumbers(block);
    bool isHCut=0;
    
    if(numLeaves == 0)
      {
	block._canSplitBlock = false;
	_placedBlocks.push_back(&block);
        return false;	
      }
    else if(numLeaves == 1)
      {	
	unsigned nodeId = *(block.cellIdsBegin());
	const BBox& blkBBox = block.getBBox();
	_placement[nodeId] = 
		Point(blkBBox.xMin,(*block.rowsBegin())->getYCoord());

	//find weber loc and place closest to this loc as possible
	Point weberLoc = getWeberLocation(nodeId);
	double nodeWidth = _hgraphOfNetlist.getNodeWidth(nodeId);
	double nodeHeight = _hgraphOfNetlist.getNodeHeight(nodeId);
	if(weberLoc.x > blkBBox.xMax)
	  weberLoc.x = blkBBox.xMax-nodeWidth;
	if(weberLoc.x < blkBBox.xMin)
	  weberLoc.x = blkBBox.xMin;
	if(weberLoc.y > blkBBox.yMax)
	  weberLoc.y = blkBBox.yMax-nodeHeight;
	if(weberLoc.y < blkBBox.yMin)
	  weberLoc.y = blkBBox.yMin;
	
	
	itCBCoreRow cr;
	itRBPSubRow sr;
	unsigned crIdx=0;
	for(cr = block.rowsBegin(); cr != (block.rowsEnd()-1); cr++)
	  {
	    double rowLowYCoord = (*cr)->getYCoord();
	    double rowHighYCoord = (*(cr+1))->getYCoord();
	    if(weberLoc.y >= rowLowYCoord && weberLoc.y < rowHighYCoord)
	      break;
	    ++crIdx;
	  }
	weberLoc.y = (*cr)->getYCoord();

	pair<unsigned,unsigned> stOffset = block.getStartOffset(crIdx);
	pair<unsigned,unsigned> eOffset = block.getEndOffset(crIdx);

	unsigned startOffset = stOffset.first;
	unsigned endOffset = eOffset.first;

	bool foundValidLoc=false;
	for(sr=(*cr)->subRowsBegin()+startOffset;sr!=(*cr)->subRowsBegin()+endOffset+1;++sr)
	  {
	    if((weberLoc.x >= sr->getXStart())&&(weberLoc.x <= sr->getXEnd()))
	      {
		if(sr->getXEnd() < (weberLoc.x+nodeWidth))
		  weberLoc.x = sr->getXEnd()-nodeWidth;
		unsigned numSitesFrmStart = 
		  static_cast<unsigned>((weberLoc.x-sr->getXStart())/sr->getSpacing());
		
		weberLoc.x = sr->getXStart()+numSitesFrmStart*sr->getSpacing();
		foundValidLoc = true;
	      }
	  }
	if(!foundValidLoc)
	  {
	    if(weberLoc.x <= (*cr)->getXStart())
	      weberLoc.x = (*cr)->getXStart();
	    else if(weberLoc.x >= (*cr)->getXEnd())
	      weberLoc.x = (*cr)->getXEnd() - nodeWidth;
	    else
	      {
		for(sr=(*cr)->subRowsBegin()+startOffset;sr!=(*cr)->subRowsBegin()+endOffset;++sr)
		  {
		    if(sr->getXEnd() < weberLoc.x && (sr+1)->getXStart() > weberLoc.x)
		      {
			if((sr->getXEnd()-nodeWidth-weberLoc.x) < ((sr+1)->getXStart()-weberLoc.x))
			  weberLoc.x = sr->getXEnd()-nodeWidth;
			else
			  weberLoc.x = (sr+1)->getXStart();
			foundValidLoc = true;
		      }
		  }
	      }
	  }
	
	//final check
	
	if(weberLoc.x > blkBBox.xMax)
	  weberLoc.x = blkBBox.xMax-nodeWidth;
	if(weberLoc.x < blkBBox.xMin)
	  weberLoc.x = blkBBox.xMin;
	if(weberLoc.y > blkBBox.yMax)
	  weberLoc.y = blkBBox.yMax-nodeHeight;
	if(weberLoc.y < blkBBox.yMin)
	  weberLoc.y = blkBBox.yMin;
	
	_placement[nodeId] = weberLoc;
	_placed[nodeId] = true;
	_cellToBlockMap[nodeId] = NULL;

	if(_params.verb.forActions > 0) cerr<<"1";
	block._canSplitBlock = false;
	_placedBlocks.push_back(&block);

        return false;
      }
    /*sadya:take care of special case where xmin == xmax or 0 capacity blocks*/
    else if ((block.getBBox().xMin >= block.getBBox().xMax && 
	      block.getNumRows() == 1) || block.getCapacity() == 0)
      {
	if(block.getCapacity() == 0)
	  {abkwarn(0, "skipping partitioning for 0 Capacity block");}
	else
	  {abkwarn(0, "skipping partitioning for 0-xspan block w/ 1 subrow");}
	
	const vector<unsigned>& cellIds = block.getCellIds();
	for(unsigned i=0; i<cellIds.size(); i++) 
	  {
	    _placed[cellIds[i]] = true;
	    _cellToBlockMap[cellIds[i]] = NULL;
	    _placement[cellIds[i]] = Point(block.getBBox().xMin, block.getBBox().yMin);
	  }
	if(_params.verb.forActions > 0) cerr<<"-";
	
	block._canSplitBlock = false;
	_placedBlocks.push_back(&block);
	
	return false;
      }
    else if(block.getNumRows() == 1 && numLeaves < _params.smallPlaceThreshold)
      {
	abkwarn(block._startOffsets[0].first == block._endOffsets[0].first,
			"smallPlacer called on a block w/ subrows");

	const RBPCoreRow& onlyRow = **block.rowsBegin();
	double spacing = onlyRow.getSpacing();
        double yCoord  = onlyRow.getYCoord();

        SmallPlacementRow splRow(block.getBBox(), spacing);

	//added by sadya to take care of white space by fake cells
	vector <unsigned> movables;
	vector <double> whiteSpaceWidths;
	vector <unsigned> cellIds;
	double totalWSWidth = block.getWidth() -
	  block.getAvgCellWidth()*block.getNumCells();

	unsigned numSitesWS = 0;   //default is 0
	
	if(totalWSWidth > 0)
	  numSitesWS = unsigned(floor(totalWSWidth/spacing));
	
	unsigned sitesPerWS = 0;

	if(numSitesWS >= 3)
	  {
	    movables.push_back(UINT_MAX);
	    sitesPerWS = unsigned(floor(numSitesWS/3+0.5));
	    numSitesWS -= sitesPerWS;
	    whiteSpaceWidths.push_back(sitesPerWS*spacing);

	    movables.push_back(UINT_MAX);
	    sitesPerWS = unsigned(floor(numSitesWS/2+0.5));
	    numSitesWS -= sitesPerWS;
	    whiteSpaceWidths.push_back(sitesPerWS*spacing);

	    movables.push_back(UINT_MAX);
	    sitesPerWS = numSitesWS;
	    whiteSpaceWidths.push_back(sitesPerWS*spacing);
	  }
	else if(numSitesWS == 2)
	  {
	    movables.push_back(UINT_MAX);
	    sitesPerWS = unsigned(floor(numSitesWS/2+0.5));
	    numSitesWS -= sitesPerWS;
	    whiteSpaceWidths.push_back(sitesPerWS*spacing);

	    movables.push_back(UINT_MAX);
	    sitesPerWS = numSitesWS;
	    whiteSpaceWidths.push_back(sitesPerWS*spacing);
	  }
	else if(numSitesWS == 1)
	  {
	    movables.push_back(UINT_MAX);
	    sitesPerWS = numSitesWS;
	    whiteSpaceWidths.push_back(sitesPerWS*spacing);
	  }

	cellIds = block.getCellIds();
	for(unsigned i=0;i<block.getNumCells();++i)
	  {
	    movables.push_back(cellIds[i]);
	  }
	
	vector<SmallPlacementRow> smallPlRows;
	smallPlRows.push_back(splRow);
	//assume all cells are not fixed
	bit_vector fixed(movables.size(),0);
        SmallPlacementProblem problem(
			       _hgraphOfNetlist, _placement, _placed, fixed,
  			       movables, smallPlRows, whiteSpaceWidths);

	if (_params.saveSmallPlProb)
	  {
	    static unsigned numInstance=0;
	    if (numInstance==0) 
	      cout << "Begin saving small placement problems..."<< endl;
	    char baseFileName[1000];
	    sprintf(baseFileName,"smallPla-r%u-c%u-n%u-%u",
		    problem.getNumRows(),
		    problem.getNumCells(),
		    problem.getNumNets(),
		    numInstance++);
/*                           static_cast<int>(problem.getNumRows()),
			     static_cast<int>(problem.getNumCells()),
			     static_cast<int>(problem.getNumNets()),
			     static_cast<int>(numInstance++));
*/
	    problem.save(baseFileName);
	  }

        SmallPlParams splParams = _params.smplParams;
        splParams.verb.forMajStats /= 10;
        splParams.verb.forActions  /= 10;
        splParams.verb.forSysRes   /= 10;
//	BranchSmallPlacer placer(problem, splParams);

        Timer tm18;
	SmallPlacer       placer(problem, splParams);
        tm18.stop();
        CapoPlacer::capoTimer::SmTime+=tm18.getUserTime();

	const Placement& soln = problem.getBestSoln().placement;
	abkassert(soln.getSize() == movables.size(),"undersized placement");

	double leftMostSite = 
	  onlyRow[block._startOffsets[0].first].getXStart();

  	for(unsigned n = 0; n < movables.size(); n++)
	  {
	    if(movables[n] != UINT_MAX)
	      {
		_placed[movables[n]] = true;
		_cellToBlockMap[movables[n]] = NULL;
		double locOffset = soln[n].x - leftMostSite;
		unsigned numSites;
		if(locOffset <= 0) numSites = 0;
		else   numSites = static_cast<unsigned>(floor(locOffset/spacing));
		_placement[movables[n]] = 
		  Point(numSites*spacing+leftMostSite, yCoord);
	      }
	  }
	if(_params.verb.forActions > 0) cerr<<"-";

	block.setIndexToNextIndex();

	if(_params.replaceSmallBlocks == AtEveryLayer)
            newLayout.push_back(&block);
	else
            _placedBlocks.push_back(&block);

        return false;
        // without deleting the block
    } 
    else
    {
	unsigned numRows = block.getNumRows();
	Verbosity sVerb = _params.verb;
	sVerb.forActions  /= 2;
	sVerb.forMajStats /= 2;
	sVerb.forSysRes   /= 2;

	if(numLeaves < minNumCellsToPart)
	{
	    if(_params.verb.forActions > 4)
		cout<<"Block below current min to split...skipping"<<endl;
	    newLayout.push_back(&block);
	    cerr<<"o";
	    return false;
        }

	BaseBlockSplitter* splitter;
	vector<CapoBlock*> newBlocks;
	if(_params.capoNE || _params.useQuad)
	  updateMapInfoAboutBlock(&block);
	else
	  updateInfoAboutBlock(&block);

	unsigned bFactor = _params.splitterParams.numMLSets;
	unsigned lAhead  = _params.lookAhead;
	if(numLeaves < 500) lAhead = 0;

	if(_layerNum == 0)
		bFactor *= 3;
	else if(_layerNum == 1)
		bFactor *= 2;

        double stretchFactor=
          ( block.getAvgRowSpacing() > 1.5 * block.getAvgCellHeight() ? 2 : 1);
//      bool simpleBenchmark = 
//        ( block.getAvgCellWidth() == block.getAvgCellHeight() );
//      cout << "Row spacing: " << block.getAvgRowSpacing() 
//           << " Cell Height: " << block.getAvgCellHeight()
//           << " Cell Width: " << block.getAvgCellWidth() << endl;
//      double endCaseSplitFactor 
//             = block.getAvgCellWidth() / block.getAvgRowSpacing();

//	stretchFactor = 0.5;
        bool splitHoriz= stretchFactor * block.getHeight() > block.getWidth();

        if (!splitHoriz)
            splitHoriz= (2*block.getHeight() >  block.getWidth() &&
                        numLeaves <= _params.smallPlaceThreshold*numRows) ;

	//in presence of groups in regions. need to decide here how/where to partition
	bool changeDefaultFlow=false;
	double splitPt;
	if(_params.useGrpConstr)
	  {
	    unsigned numRegionedCells = getNumRegionedCellsInBlock(&block);

	    if(numRegionedCells > 0)
	      {
		bool desiredDir = splitHoriz;
		bool splitDir = splitHoriz;
		changeDefaultFlow = block.findSplitPtDir(desiredDir, 
							 groupRegionConstr, 
							 splitDir, splitPt);
		if(changeDefaultFlow && numLeaves>_params.smallSplitThreshold)
		  splitHoriz = splitDir;
		
		if(_params.verb.forActions > 2)
		  {
		    cout<<"changeDefaultFlow = "<<changeDefaultFlow<<" orig splitDir "<<splitHoriz<<" new splitDir "<<splitDir<<" splitPoint "<<splitPt<<" blockBBox "<<block.getBBox()<<endl;
		  }	    
	      }
	  }

       isHCut = splitHoriz; 
       if( numRows > 1 && splitHoriz )
	{		//Horizontal partitioning
	    if(partDir == VOnly)
	    {
		if(_params.verb.forActions > 4)
		    cout<<"Block wants to split the other way...skipping"<<endl;
		newLayout.push_back(&block);
	    	cerr<<"x";
	        return false;
	    }

	    if(numLeaves <= _params.smallSplitThreshold) 
		splitter = new SplitSmallBlockHorizontally(block,*this,
						    //_params.splitterParams);
							   sVerb);
	    else
	    {
		if(lAhead > 0)
	            splitter = new LookAheadSplitter(block,*this, bFactor,
					lAhead, sVerb);
                else
		if(partDir == HAndV || !_params.allowRowSplits)
		  {
		    if(!changeDefaultFlow)
		      splitter = new SplitLargeBlockHorizontally(block,*this, 
				          _params.splitterParams, &_placement);
		    else
		      splitter = new SplitLargeBlockHorizontally(block,*this, 
				  splitPt,_params.splitterParams, &_placement);
		  }
		else
		{
				//collect a row of blocks
		    vector<CapoBlock*> rowOfBlocks;
		    rowOfBlocks.push_back(&block);
		    double rowTop = rowOfBlocks[0]->getBBox().yMax;
		    double rowBot = rowOfBlocks[0]->getBBox().yMin;

		    CapoBlock*  adjBlk = &block;

		    while(adjBlk->_leftNeighbors.size() == 1)
		    {
			CapoBlock* lNbor = adjBlk->_leftNeighbors[0];

			if(lNbor->getNumCells() < 100 ||
			   lNbor->getNumCells() < minNumCellsToPart ||
			   !lNbor->canSplitBlock()) 
				break;

			if((lNbor->getBBox().yMin != rowBot)  ||
			   (lNbor->getBBox().yMax != rowTop))   
				break;
		    
			adjBlk = lNbor;
		        rowOfBlocks.push_back(adjBlk);
		    }

		    adjBlk = &block;
		    while(adjBlk->_rightNeighbors.size() == 1)
		    {
			CapoBlock* rNbor = adjBlk->_rightNeighbors[0];

			if(rNbor->getNumCells() < 100 ||
			   rNbor->getNumCells() < minNumCellsToPart ||
			   !rNbor->canSplitBlock()) 
				break;

			if((rNbor->getBBox().yMin != rowBot)  ||
			   (rNbor->getBBox().yMax != rowTop))   
				break;
		    
			adjBlk = rNbor;
		        rowOfBlocks.push_back(adjBlk);
		    }

		    if(rowOfBlocks.size() > 1)	
		    {
	            	splitter = new SplitRowOfBlocks(rowOfBlocks,
		    				*this, _params.splitterParams);
			if(static_cast<SplitRowOfBlocks*>(splitter)->
				wasGoodSplit())
		 	{
			    for(unsigned sb = 0; sb < rowOfBlocks.size(); sb++)
				rowOfBlocks[sb]->_canSplitBlock = false;
			}
			else
			{
			    cout<<"Failed Row Split"<<endl;	
			    delete splitter;
		            splitter = new SplitLargeBlockHorizontally(
					block,*this, _params.splitterParams);
			}
		    }
		    else
		        splitter = new SplitLargeBlockHorizontally(
					block,*this, _params.splitterParams);
		}
	    }
	}
	else
	{		//Vertical partitioning
	    if(partDir == HOnly)
	    {
		if(_params.verb.forActions > 4)
		    cout<<"Block wants to split the other way...skipping"<<endl;
		newLayout.push_back(&block);
	    	cerr<<"x";
	        return false;
	    }

	    if(numLeaves < _params.smallSplitThreshold)
		splitter = new SplitSmallBlockVertically(block,*this,
						    //_params.splitterParams);
							 sVerb);
	    else
	    {
		if(lAhead > 0)
	            splitter = new LookAheadSplitter(block,*this,
						bFactor,lAhead,
						sVerb);
		else
		{
/*
		    if(_params.useKPl & _layerNum > 1)
			splitter = new KPlaceSplitter(
					block, *this, _kPlacement, 
					_params.splitterParams);
		    else
*/
		  if(!changeDefaultFlow)
		    splitter = new SplitLargeBlockVertically(block, *this, 
				          _params.splitterParams, &_placement);
		  else
		    splitter = new SplitLargeBlockVertically(block, *this, 
				 splitPt, _params.splitterParams, &_placement);
		}
	    }
	}

	newBlocks = splitter->getNewBlocks();
	delete splitter;

	if(numLeaves <= _params.smallPartThreshold)
	    cerr<<"*";
	else
	    cerr<<".";


	if(_params.capoNE || _params.useQuad)
	  {
	    for(unsigned b = 0; b < newBlocks.size(); b++)
	      updatePlInfoAboutBlock(newBlocks[b]);
	  }



	//by sadya to incorporate top-down congestion control
	if(useCongestionInfo && _params.tdCongestionCtl)
	  {
	    double congestionPart0, congestionPart1;
	    double relCongestionP0, relCongestionP1;
	    const vector<unsigned>& blockCellIds = block.getCellIds();

	    _congestionMap->remCongByCells(blockCellIds, true);

	    for(unsigned b = 0; b < newBlocks.size(); b++)
	      {
		newLayout.push_back(newBlocks[b]);
		updateInfoAboutBlock(newBlocks[b]);
		updatePlInfoAboutBlock(newBlocks[b]);
	      }
	    
	    _congestionMap->addCongByCells(blockCellIds, true);

	    block._canSplitBlock = false;

	    unsigned blockNumCells = block.getNumCells();
	    unsigned threshold = 
	      500*_congestionMap->getAvgNumCellsPerGrid();
	    
	    if(block.getNumCells() > 30  && blockNumCells < threshold)
	      {
		cout<<"Threshhold : "<<threshold<<" numCells "<<blockNumCells<<endl;
		congestionPart0 = 
		  _congestionMap->getCongestion(newBlocks[0]->getBBox());
		congestionPart1 = 
		  _congestionMap->getCongestion(newBlocks[1]->getBBox());
		
		relCongestionP0 =congestionPart0/(congestionPart0+congestionPart1);
		relCongestionP1 =congestionPart1/(congestionPart0+congestionPart1);
		
		cout<<"Before "<<congestionPart0<<"  "<<congestionPart1<<endl;
		cout<<relCongestionP0<<"  "<<relCongestionP1<<endl;
		
		if(1)
		  {
		    repartitionBlocks(newBlocks[0],newBlocks[1], 
				      relCongestionP0,relCongestionP1);
		  }
		else   //TO DO change areas of partitions according to congestion
		  {
		    /*     
			   double cap0 = newBlocks[0]->getCapacity();
			   double cap1 = newBlocks[1]->getCapacity();
			   double nodeArea0 = newBlocks[0]->getTotalCellArea();
			   double nodeArea1 = newBlocks[1]->getTotalCellArea();
			   double totWS = cap0+cap1 - nodeArea0-nodeArea1;
			   if(totWS > 0)
			   {
			   double ws0 = relCongestionP0*totWS;
			   double ws1 = relCongestionP1*totWS;
			   double reqdCap0 = nodeArea0 + ws0;
			   double reqdCap1 = nodeArea1 + ws1;
			   if(isHCut)  //block is horizontally cut   
			   {
			   unsigned splitRow    = 1;
			   double   p1Cap =  block.getContainedAreaInCoreRow(0);
			   unsigned r;
			   for(r = 1; r < block.getNumRows()-1; r++)
			   {
			   if(p1Cap >= reqdCap1)
			   break;
			   p1Cap += block.getContainedAreaInCoreRow(r);
			   }
			   if(r == block.getNumRows()-1)
			   splitRow = r-1;
			   else
			   splitRow = r;
			   //reSplitBlockH(block,*newBlocks[0],*newBlocks[1],splitRow);
			   }
			   else
			   {
			   vector<double> partCaps;
			   double xSplit = 
			   block.findXSplit(reqdCap0,reqdCap1,0,partCaps);
			   //reSplitBlockV(block,*newBlocks[0],*newBlocks[1],xSplit);
			   } 
			   }
		    */  
		  }
		_congestionMap->remCongByCells(blockCellIds, true);
		for(unsigned b = 0; b < newBlocks.size(); b++)
		  {
		    updateInfoAboutBlock(newBlocks[b]);
		    updatePlInfoAboutBlock(newBlocks[b]);
		  }
		_congestionMap->addCongByCells(blockCellIds, true);
		congestionPart0 = 
		  _congestionMap->getCongestion(newBlocks[0]->getBBox());
		congestionPart1 = 
		  _congestionMap->getCongestion(newBlocks[1]->getBBox());
		
		relCongestionP0 =congestionPart0/(congestionPart0+congestionPart1);
		relCongestionP1 =congestionPart1/(congestionPart0+congestionPart1);
		
		cout<<"After "<<congestionPart0<<"  "<<congestionPart1<<endl;
		cout<<relCongestionP0<<"  "<<relCongestionP1<<endl<<endl;
		
		return true;
	      }
	  }
	else
	  {
	    for(unsigned b = 0; b < newBlocks.size(); b++)
	      {
		newLayout.push_back(newBlocks[b]);
		updateInfoAboutBlock(newBlocks[b]);
	      }
	    
	    block._canSplitBlock = false;
	    return true;
	  }
    }
    return false;
}

