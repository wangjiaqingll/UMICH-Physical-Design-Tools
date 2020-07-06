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














// Created: Sep 11, 1997 by Igor Markov & Andy Caldwell
// Portions based on Wenyong Deng's Cadence-confidential outline.

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
//980401 ilm split into refineBlock.cxx, splitBlock.cxx and the rest
//990202 aec capoPlacer now operates on an RBPlacement and an HGraphWDims,
//		rather than DB
//990319 ilm the main Capo loop now copies all placed blocks to the new
//           level and stops when there are no blocks to refine

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <strings.h>
#include "ABKCommon/abkcommon.h"
#include "capoPlacer.h"
#include "capoBlock.h"
#include "PlaceEvals/placeEvals.h"
#include "Stats/trivStats.h"
#include "Placement/xyDraw.h"
#include "RBPlace/RBPlacement.h"
#include "AnalytPl/analytPl.h"
#include "Constraints/allConstr.h"

double CapoPlacer::capoTimer::FMTime;
double CapoPlacer::capoTimer::MLTime;
double CapoPlacer::capoTimer::BBTime;
double CapoPlacer::capoTimer::SmTime;
double CapoPlacer::capoTimer::SetupTime;

void CapoPlacer::printNetStat()
{
  int first=1;
  double len, mlen, ncost;
  itHGFEdgeGlobal edge;
  double alen[10];
  BBox coreBBox;
  vector<const RBPCoreRow*> rows;
  vector<CROffset> endOffsets;
  
  itRBPCoreRow r;
  for(r = _rbplace.coreRowsBegin(); r != _rbplace.coreRowsEnd(); r++)
    {
      rows.push_back(&(*r));
      const RBPSubRow& lastSR = *(r->subRowsBegin() + (r->getNumSubRows()-1));
      CROffset tmpOffset(r->getNumSubRows()-1, lastSR.getNumSites()-1);
      endOffsets.push_back(tmpOffset);
      coreBBox += Point((*r)[0].getXStart(),r->getYCoord());
      coreBBox += Point((*r)[endOffsets.back().first].getXEnd(),
			r->getYCoord()+r->getHeight());
    }

  bzero(alen, sizeof(double)*10);

  for(edge = _hgraphOfNetlist.edgesBegin(); edge != _hgraphOfNetlist.edgesEnd(); edge++) {
    itHGFNodeLocal node;
    BBox box;
    box.clear();
    for(node = (*edge)->nodesBegin(); node != (*edge)->nodesEnd(); node++) {
      unsigned nodeId = (*node)->getIndex();
      box+=_placement[nodeId];
    }
    len=fabs(box.xMax-box.xMin)+fabs(box.yMax-box.yMin);
    if(len > mlen || first) {
      mlen=len;
      first=0;
    }
  }
  
  printf("Max edge len is %.2f\n", mlen);

  ncost=coreBBox.xMax-coreBBox.xMin;
  ncost+=coreBBox.yMax-coreBBox.yMin;

  for(edge = _hgraphOfNetlist.edgesBegin(); edge != _hgraphOfNetlist.edgesEnd(); edge++) {
    itHGFNodeLocal node;
    BBox box;
    box.clear();
    for(node = (*edge)->nodesBegin(); node != (*edge)->nodesEnd(); node++) {
      unsigned nodeId = (*node)->getIndex();
      box+=_placement[nodeId];
    }

    len=fabs(box.xMax-box.xMin)+fabs(box.yMax-box.yMin);
    //    printf("%.2f %.2f\n", len, ncost);
    int bin=(int)(len/(ncost/9));
    //    printf("%d bin\n\n", bin);
    alen[bin]++;
  }

  for(int i=0; i<10; i++) {
    printf("%d %.2f\n", i, alen[i]);
  }
}

CapoPlacer::CapoPlacer(RBPlacement& rbplace, const CapoParameters& params,
		       CongestionMaps* congestionMap)
//                     const vector<char*>* altCellNames)
	: _params(params), _rbplace(rbplace),
          _hgraphOfNetlist(rbplace.getHGraph()),
	  _coreBBox(/*BBoxFromRBPlace*/), 
	  _placement(static_cast<PlacementWOrient>(rbplace)), 
	  _kPlacement(static_cast<PlacementWOrient>(rbplace)), 
	  _congestionMap(congestionMap),
          _placed(rbplace.getFixed()),
          _pinLocCalc(_hgraphOfNetlist, _placement),
	  _layerNum(0), 
	  _cellToBlockMap(_hgraphOfNetlist.getNumNodes(), 
			  static_cast<const CapoBlock*>(NULL)),
	  _nodeSeen(_hgraphOfNetlist.getNumNodes()), 
	  _edgeSeen(_hgraphOfNetlist.getNumEdges()),
	  _numOrigNets(100,0), _numEssentialNets(100,0),
	  _origNetPins(100,0), _essentialNetPins(100,0),
	  _numProblemsOfSize(100,0),
	  _netHasBeenCut(_hgraphOfNetlist.getNumEdges(), false),
	  _numNotSolved(0), _numSmallPartProbs(0),
	  constraints(_rbplace.constraints), 
	  regionUtilization(_rbplace.regionUtilization),
	  groupRegionConstr(_rbplace.groupRegionConstr),
	  cellToGrpMapping(_rbplace.cellToGrpMapping)
{
    Timer  capoTimer;

    if(_params.verb.forActions > 0)
	cerr<<endl<<"Launching UCLA's CapoPlacer"<<endl;

    setupAndCheck();

    //printNetStat();

    unsigned maxBlocks = _params.stopAtBlocks ? _params.stopAtBlocks : UINT_MAX;

    unsigned saveAtBlocks = _params.saveAtBlocks ? _params.saveAtBlocks : UINT_MAX;

    bool haveUnplacedBlocks = true;
    bool doVCuts = true;
    bool saveBlocks = true;

    while(haveUnplacedBlocks && _curLayout->size() < maxBlocks )
    {
        if(_curLayout->size() >= saveAtBlocks && saveBlocks)
	  {
	    //take a snapshot of blocks
	    saveBlocksCopy();
	    saveBlocks = false;
	  }
        
	AllowedPartDir partDir;
	if(_layerNum < 3) 
	     partDir = HAndV;
	else if(_layerNum == 3 || doVCuts)
	{
	     partDir = VOnly;
	     doVCuts = false;	//next time will be horizontal
	}
	else 
	{
	     partDir = HOnly;
	     doVCuts = true;
	}

	if(_params.capoNE || _params.useQuad)
	  {
	    //if(_curLayout->size() >= maxBlocks/2)
	    {
	    solveQuadraticMinSoln();
	    //_rbplace.updatePlacementWOri(_placement);
	    //_rbplace.saveAsPlot("plotNodes",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,"out");
	    //exit(0);
	    }
	  }

	haveUnplacedBlocks  = doOneLayer(partDir);

	//if replaceSmallBlocks == AtEveryLayer, the placed blocks
	//will still be in curLayout.  Otherwise, they will have
	//been moved into _placedBlocks

        if(_params.verb.forMajStats > 1)
        {
            cout << "\nTotal cut nets: " << getTotalNetCut() << endl;
            vector<unsigned> externalCutNets, containedNets;
            getNetCutInfo(externalCutNets, containedNets);

	    if(externalCutNets.size() == _curLayout->size()) 
	    {
	      cout<<"Per Block Stats:  "
		  <<"<numCells><cellArea><cutEdges>"<<endl;
	
              for(unsigned ijk = 0; ijk < externalCutNets.size(); ijk++)
	      {
                cout<<"Block " << ijk <<" has "
		    <<(*_curLayout)[ijk]->getNumCells()<<" cells, "
		    <<(*_curLayout)[ijk]->getTotalCellArea()
		    <<" cell area, "
		    <<externalCutNets[ijk] << " cut nets, "
//                  <<containedNets[ijk] << " contained nets, "
                    <<(*_curLayout)[ijk]->getNumAdjacentCells()
                    <<" adj. cells and "
                    <<(*_curLayout)[ijk]->getNumAdjacentCellsWithDuplicates()
                    <<" adj. cells with duplicates"
                    << "\n";
	      }
	    }
        }
    }


    if(_params.saveBlocks)
    {
           cout << "Saving block membership in block_mem.dat" << endl;
           printBlockMembership("block_mem.dat");
           cout << "Saving hierarchical cell names cell_names.dat" << endl;
           printHierCellNames("cell_names.dat");
    }

    if(_params.replaceSmallBlocks == AtTheEnd)
    {
	Timer replaceTimer;
	cout<<" Replacing Small Blocks"<<endl;
   	replaceSmallBlocks();
	replaceTimer.stop();
	cout<<" Replacing Small Blocks took: "<<
		replaceTimer.getUserTime()<<endl;
    }
    
    if(_params.boost)
      printf("SQUARE COST %.2f\n", squareCost());
    capoTimer.stop();

    if(_params.verb.forMajStats > 0)
        cout << "   CapoPlacer took : " << capoTimer << endl;
        double realtot = capoTimer.getUserTime();
        cout << "    MLPart took:       "<<CapoPlacer::capoTimer::MLTime<<
            "sec ("<< CapoPlacer::capoTimer::MLTime/capoTimer.getUserTime()*100
             <<"%)"<<endl;
        cout << "    FMPart took:       "<<CapoPlacer::capoTimer::FMTime<<
            "sec ("<< CapoPlacer::capoTimer::FMTime/capoTimer.getUserTime()*100
             <<"%)"<<endl;
        cout << "    BBPart took:       "<<CapoPlacer::capoTimer::BBTime<<
            "sec ("<< CapoPlacer::capoTimer::BBTime/capoTimer.getUserTime()*100
             <<"%)"<<endl;
        cout << "    SmPlace took:      "<<CapoPlacer::capoTimer::SmTime<<
            "sec ("<< CapoPlacer::capoTimer::SmTime/capoTimer.getUserTime()*100
             <<"%)"<<endl;
        cout << "    ProblemSetup took: "<<CapoPlacer::capoTimer::SetupTime<<
          "sec ("<< CapoPlacer::capoTimer::SetupTime/capoTimer.getUserTime()*100
             <<"%)"<<endl;
        double tot = CapoPlacer::capoTimer::MLTime+
                                  CapoPlacer::capoTimer::FMTime+
                                  CapoPlacer::capoTimer::BBTime+
                                  CapoPlacer::capoTimer::SmTime+
                                  CapoPlacer::capoTimer::SetupTime;


        cout <<"     Total runtime of measured components: "<< tot << "sec ("
             <<tot/realtot*100<<"%)\n" <<endl;


    if(_params.verb.forMajStats > 0)
    {
	cout<<"   Final Average Relative Overfill: "<<
		100*(_totalOverfill/_totalLayoutArea)<<"%"<<endl;
	cout<<"  Max 1block Overfill: "<<100*_maxOverfill<<"%"<<endl;
    }

    rbplace.resetPlacementWOri(_placement);

    if(_params.saveBlocksFloorplan)
    {
      if(!saveBlocks)
	{
	  cout << "Saving Blocks as Floorplanning instance with base FP"<<endl;
	  saveBlocksAsFloorplan("FP", _saveBlocks);
	}
      else
	{
	  cout<< " Could not save blocks because capo run ended earlier"<<endl;
	}
    }
}


double CapoPlacer::estimateWL()
{
 /*
    LayoutBBoxes 	regions; //CapoBlocks + Points of all fixed nodes

    unsigned            numNodes = _hgraphOfNetlist.getNumNodes();

    vector<unsigned>    assignment (numNodes, UINT_MAX);
	//for each node, this gives the index into the regions vector
	//of the region it is in

    itHGFNodeGlobal node; 
    for(node = _hgraphOfNetlist.nodesBegin(); 
        node != _hgraphOfNetlist.nodesEnd(); node++)
    {
	unsigned nodeId = (*node)->getIndex();

	if(_placed[nodeId]) 
	{
	    assignment[nodeId] = regions.size();
	    regions.push_back(BBox(_placement[nodeId]));
	}
    }

    for(unsigned b = 0; b < _curLayout->size(); b++)
    {
	const CapoBlock& curBlk = *(*_curLayout)[b];

	vector<unsigned>::const_iterator nItr;
	for(nItr = curBlk.cellIdsBegin(); nItr != curBlk.cellIdsEnd(); nItr++)
	    assignment[*nItr] = regions.size();

	regions.push_back(curBlk.getBBox());
    }
 */

    Timer bboxTM;
    BBoxPlEval       bboxEval    (_hgraphOfNetlist, _placement);
    bboxTM.stop();

/*
    Timer bboxWChengTM;
    BBoxWChengPlEval bboxWCEval  (_hgraphOfNetlist, _placement);
    bboxWChengTM.stop();
    Timer bboxWRSMTTM;
    BBoxWRSMTPlEval  bboxWREval  (_hgraphOfNetlist,  _placement);
    bboxWRSMTTM.stop();

    Timer hbboxTM;
    HBBoxPlEval       hbboxEval   (_hgraphOfNetlist, regions, assignment);
    hbboxTM.stop();
    Timer hbboxWChengTM;
    HBBoxWChengPlEval hbboxWCEval (_hgraphOfNetlist, regions, assignment);
    hbboxWChengTM.stop();
    Timer hbboxWRSMTTM;
    HBBoxWRSMTPlEval  hbboxWREval (_hgraphOfNetlist, regions, assignment);
    hbboxWRSMTTM.stop();

    Timer hbbox0TM;
    HBBox0PlEval       hbbox0Eval  (_hgraphOfNetlist, regions, assignment);
    hbbox0TM.stop();
    Timer hbbox0WChengTM;
    HBBox0wChengPlEval hbbox0WCEval(_hgraphOfNetlist, regions, assignment);
    hbbox0WChengTM.stop();
    Timer hbbox0WRSMTTM;
    HBBox0wRSMTPlEval  hbbox0WREval(_hgraphOfNetlist, regions, assignment);
    hbbox0WRSMTTM.stop();

    Timer hbboxNTM;
    HBBoxNPlEval       hbboxNEval(_hgraphOfNetlist, regions, 
					assignment,_placement);
    hbboxNTM.stop();

    Timer hbboxNWChengTM;
    HBBoxNwChengPlEval  hbboxNWCEval(_hgraphOfNetlist, regions,
                                    	assignment, _placement);
    hbboxNWChengTM.stop();
    Timer hbboxNWRSMTTM;
    HBBoxNwRSMTPlEval  hbboxNWREval(_hgraphOfNetlist, regions,
                                    	assignment, _placement);
    hbboxNWRSMTTM.stop();

*/
    double bboxWL     = bboxEval.  getCost();
/*
    double bboxWCWL   = bboxWCEval.getCost();
    double bboxWRWL   = bboxWREval.getCost();
    double hbboxWL    = hbboxEval. getCost();
    double hbboxWCWL  = hbboxWCEval.getCost();
    double hbboxWRWL  = hbboxWREval.getCost();

    double hbbox0WL    = hbbox0Eval.getCost();
    double hbbox0WCWL  = hbbox0WCEval.getCost();
    double hbbox0WRWL  = hbbox0WREval.getCost();
    double hbboxNWL    = hbboxNEval.getCost();

    double hbboxNWCWL  = hbboxNWCEval.getCost();
    double hbboxNWRWL  = hbboxNWREval.getCost();

    if(_params.verb.forMajStats > 9)
    {
	cout<<"WL Estimation Results: (cost + runtimes)"<<endl;
    	cout<<"BBox         "<<bboxWL     <<bboxTM <<  endl;
        cout<<"BBoxWCheng   "<<bboxWCWL   <<bboxWChengTM << endl;
        cout<<"BBoxWRSMT    "<<bboxWRWL   <<bboxWRSMTTM << endl;

        cout<<"HBBox        "<<hbboxWL    <<hbboxTM << endl;
        cout<<"HBBoxWCheng  "<<hbboxWCWL  <<hbboxWChengTM << endl;
        cout<<"HBBoxWRSMT   "<<hbboxWRWL  <<hbboxWRSMTTM << endl;

        cout<<"HBBox0       "<<hbbox0WL   << hbbox0TM << endl;
        cout<<"HBBox0WCheng "<<hbbox0WCWL << hbbox0WChengTM << endl;
        cout<<"HBBox0WRSMT  "<<hbbox0WRWL << hbbox0WRSMTTM << endl;

        cout<<"HBBoxN       "<<hbboxNWL   << hbboxNTM << endl;
        cout<<"HBBoxNWCheng "<<hbboxNWCWL << hbboxNWChengTM << endl;
        cout<<"HBBoxNWRSMT  "<<hbboxNWRWL << hbboxNWRSMTTM << endl;
    }

    if(_params.verb.forMajStats > 2)
    {
	cout<<"WL Estimation Results: (cost + runtimes)"<<endl;
        cout<<"HBBoxN       "<<hbboxNWL   << hbboxNTM << endl;
    }
*/
    return bboxWL;
}


void CapoPlacer::replaceSmallBlocks()
{
    std::vector<CapoBlock*>::iterator blk;
    double prevWL, newWL;
 
    if(_params.verb.forMajStats > 0)
    {
      prevWL =  estimateWL();
      cout<<"\nEst WL before re-placing small blocks "<<prevWL<<endl;
    }

    vector<CapoBlock*> placedBlocksCpy = _placedBlocks;
    _placedBlocks.clear();

    for(blk = placedBlocksCpy.begin(); blk != placedBlocksCpy.end(); blk++)
      	refineBlock(**blk, *_curLayout, _layerNum,HAndV,0);
		//curLayout is a dummy var here..nothing will be
		//added to it, as these are all small blocks

    if(_params.verb.forMajStats > 0) 
    {
      newWL = estimateWL(); 
      cout<<"Est WL after re-placing small blocks "<<newWL<<endl;
    }

    for(blk= _placedBlocks.begin();blk != _placedBlocks.end(); blk++)
      delete *blk;
}


void CapoPlacer::setupAndCheck()
{
    vector<unsigned> coreCells;
    coreCells.reserve(_hgraphOfNetlist.getNumNodes());

   
    Orientation nOrient; //the 'default' N orientation
    itHGFNodeGlobal cell; 
    for(cell = _hgraphOfNetlist.nodesBegin();
	cell != _hgraphOfNetlist.nodesEnd(); cell++)
    {
        unsigned cellId = (*cell)->getIndex();

	if(_rbplace.isCoreCell(cellId))
	{ 
	    if(_rbplace.isFixed(cellId))  continue;

            coreCells.push_back(cellId);
	    
	    _placement.setOrient(cellId,nOrient);
	    abkassert((*cell)->getWeight() != 0, "core cell has 0 area ");
        }
	else  //must be fixed
	{
	    if(!_hgraphOfNetlist.isTerminal(cellId) && 
	        _params.verb.forMajStats > 3)
	    {
		cout<<"Non-Terminal is not a movable core cell"<<endl;
	 	cout<<"Cell "<<cellId<<" located at "<<_placement[cellId]<<endl;
	    }
	    abkassert(_rbplace.isFixed(cellId), "unfixed non-core cell");
	}
    }

    vector<const RBPCoreRow*> rows;
    vector<CROffset> endOffsets;
    itRBPCoreRow r;
    for(r = _rbplace.coreRowsBegin(); r != _rbplace.coreRowsEnd(); r++)
    {
	rows.push_back(&(*r));
	const RBPSubRow& lastSR = *(r->subRowsBegin() + (r->getNumSubRows()-1));
	CROffset tmpOffset(r->getNumSubRows()-1, lastSR.getNumSites()-1);
	endOffsets.push_back(tmpOffset);
	_coreBBox += Point((*r)[0].getXStart(),r->getYCoord());
	_coreBBox += Point((*r)[endOffsets.back().first].getXEnd(),
			   r->getYCoord()+r->getHeight());
    }

    vector<CROffset> startOffsets(rows.size(), CROffset(0,0));

    if(_params.verb.forMajStats > 1)
	cout<<endl<<" Core BBox: "<<_coreBBox<<endl;

    vector<CapoBlock*> noNeighbors; 
    _layout[0].push_back(new 
                            CapoBlock(coreCells, _hgraphOfNetlist,
	   		    rows.begin(), rows.end(), startOffsets, endOffsets,
                            noNeighbors, noNeighbors, noNeighbors, noNeighbors,
                            this,
			    (char*)"BLOCK"));

    CapoBlock* newBlk = _layout[0].back();
    Point center = newBlk->getCenter();

    for(unsigned c = 0; c < coreCells.size(); c++)
    {
	_cellToBlockMap[coreCells[c]] = newBlk;
	_placement[coreCells[c]] = center;
    }
                          
    _curLayout = &(_layout[0]);

    if(_params.verb.forMajStats > 4) printNetlistStats();
}

void CapoPlacer::updateInfoAboutBlock(const CapoBlock* block)
{
    std::vector<unsigned>::const_iterator cIt;
    Point blockCenter = block->getCenter();
    for(cIt  = block->cellIdsBegin(); cIt != block->cellIdsEnd(); cIt++)
    {
	_cellToBlockMap[*cIt] = block;
	_placement[*cIt] = blockCenter;
    }
}

void CapoPlacer::updatePlInfoAboutBlock(const CapoBlock* block)
{
    std::vector<unsigned>::const_iterator cIt;
    Point blockCenter = block->getCenter();
    for(cIt  = block->cellIdsBegin(); cIt != block->cellIdsEnd(); cIt++)
    {
	_placement[*cIt] = blockCenter;
	_rbplace.updatePlacement(*cIt, blockCenter);
    }
}

void  CapoPlacer::updateMapInfoAboutBlock(const CapoBlock* block)
{
    std::vector<unsigned>::const_iterator cIt;
    for(cIt  = block->cellIdsBegin(); cIt != block->cellIdsEnd(); cIt++)
    {
	_cellToBlockMap[*cIt] = block;
    }
}

void CapoPlacer::plotBlocks(unsigned layerNum, vector<CapoBlock*>& layout)
{
   double sum=0.0;
   unsigned b;
   vector<const CapoBlock*> tmpBlocks;
   tmpBlocks.reserve(layout.size() + _placedBlocks.size() );
   for(b=0; b!=layout.size(); b++)
   {  sum+=layout[b]->getNumCells();
      tmpBlocks.push_back(layout[b]);
   }

   for(b=0; b!=_placedBlocks.size(); b++) 
   {   sum+=_placedBlocks[b]->getNumCells();
       tmpBlocks.push_back(_placedBlocks[b]);
   }

   if ( (layout.size() + _placedBlocks.size()) > 200 &&
       sum / (layout.size() + _placedBlocks.size()) < 10 ) return;

   if (layerNum>15) return;
   char fileName[100], baseName[100];
   sprintf(baseName,"capoBlocks-layer%d",layerNum);
   sprintf(fileName,"%s.gpl",baseName);
   ofstream out(fileName);
   out << "# "<<layout.size() << " Capo blocks from layer " << layerNum << endl
       << "# Need gnuplot 3.7 or later version \n"
       << "#set noxtics \n#set noytics \nset noborder \nset nokey\n"
       << "set title 'Capo Layer " <<layerNum<<"'\n";
   for(b=0; b!=tmpBlocks.size(); b++)
   {
     const CapoBlock& curBlk = *tmpBlocks[b];
     Point ctr=curBlk.getBBox().getGeomCenter();
     out <<"set label '"<<int(curBlk.getRelativeWhitespace()*100)
// << "%/" << int(nodesToMove(curBlk)*100.0/curBlk.getCellIds().size())
   <<"%' at "<<ctr.x<<", "<<ctr.y<<"\n";
     Point weberLoc=getWeberLocation(curBlk);
     out <<"set arrow from "<<ctr.x<<", "<<ctr.y
                    <<" to "<<weberLoc.x<<","<<weberLoc.y<<endl;
   } 
   out << "# set terminal postscript\n# set output '"
       <<baseName<<".eps'"<<endl;
   out<< " plot '-' w l\n";
   for(b=0; b!=tmpBlocks.size(); b++) 
   {
     xyDrawRectangle(out,tmpBlocks[b]->getBBox());
     out << endl;
   }
   out << "EOF\n";
   out << "pause -1 'Press any key' "<<endl;
   
   cout << " Saved " << fileName << endl;
}

Point CapoPlacer::getPinLocation(unsigned cellId, unsigned netId) const
{
   if(!_params.useActualPinLocs || !_rbplace.isFixed(cellId))
       return _placement[cellId];
   else
   {
	return _placement[cellId] + 
	       _pinLocCalc.getPinOffset(cellId, netId);
   }
}


unsigned CapoPlacer::getTotalNetCut() const
{
    vector<int> nodeBlock;
    getBlockMembership(nodeBlock);
    unsigned numTotalCut = 0;
    itHGFEdgeGlobal edge;
    for(edge = _hgraphOfNetlist.edgesBegin();
        edge != _hgraphOfNetlist.edgesEnd(); edge++)
    {
        int blockID = -2;
        itHGFNodeLocal node;
        for(node = (*edge)->nodesBegin(); node != (*edge)->nodesEnd(); node++)
        {
            unsigned nodeId = (*node)->getIndex();

            // uncomment the following line if we don't want to count
            // nets that are connected to pads
            //if(nodeBlock[nodeId] == -1) continue;

            if(blockID == -2)
            {
                blockID = nodeBlock[nodeId];
                continue;
            }
            if (blockID != nodeBlock[nodeId])
            {
                numTotalCut ++;
                break;
            }
        }
    }
    return numTotalCut;
}

void CapoPlacer::getNetCutInfo(vector<unsigned>& externalCutNets,
                               vector<unsigned>& containedNets) const
{
    vector<int> nodeBlock;
    getBlockMembership(nodeBlock);
    unsigned numBlocks = _placedBlocks.size() + _curLayout->size();
    externalCutNets.clear();
    externalCutNets.reserve(numBlocks);
    containedNets.clear();
    containedNets.reserve(numBlocks);
    for(unsigned i = 0; i < numBlocks; i++)
    {
        externalCutNets.push_back(0);
        containedNets.push_back(0);
    }

    itHGFEdgeGlobal edge;
    BitBoard blockUsed(numBlocks);
    for(edge = _hgraphOfNetlist.edgesBegin();
        edge != _hgraphOfNetlist.edgesEnd(); edge++)
    {
        int blockID = -2;
        itHGFNodeLocal node;
        blockUsed.clear();
        for(node = (*edge)->nodesBegin(); node != (*edge)->nodesEnd(); node++)
        {
            unsigned nodeId = (*node)->getIndex();

            // uncomment the following line if we don't want to count
            // nets that are connected to pads
            //if(nodeBlock[nodeId] == -1) continue;

            if(blockID == -2)
            {
                blockID = nodeBlock[nodeId];
//              if (blockID>=0) blockUsed.setBit(blockID);
                continue;
            }
            if (blockID != nodeBlock[nodeId])
            {
                if(blockID >= 0 && !blockUsed.isBitSet(blockID))
                {
                    blockUsed.setBit(blockID);
                    externalCutNets[blockID]++;
                }
                if(nodeBlock[nodeId] >= 0 &&
                   !blockUsed.isBitSet(nodeBlock[nodeId]))
                {
                    blockUsed.setBit(nodeBlock[nodeId]);
                    externalCutNets[nodeBlock[nodeId]]++;
                }
            }
        }
        if(blockUsed.getNumBitsSet() == 1 && blockID >=0 )
            containedNets[*(blockUsed.getIndicesOfSetBits().begin())] ++;
    }
}

void CapoPlacer::getBlockMembership(vector<int>& nodeBlock) const
{
    nodeBlock.clear();
    nodeBlock.reserve(_hgraphOfNetlist.getNumNodes());
    for(unsigned i = 0; i < _hgraphOfNetlist.getNumNodes(); i++)
        nodeBlock.push_back(-1);
    // Initialize the block number to -1 so that cells that are not
    // in any block will have -1 as its block id

    // cells that are in placed blocks
    int b, vectorSize = static_cast<int>(_placedBlocks.size());
    for(b = 0; b < vectorSize; b++)
    {
        const CapoBlock& curBlk = *(_placedBlocks[b]);
        std::vector<unsigned>::const_iterator nItr;
        for(nItr = curBlk.cellIdsBegin(); nItr != curBlk.cellIdsEnd(); nItr++)
            nodeBlock[*nItr] = b;
    }

    // cells that are in unplaced blocks
    // The block id continues from the placed blocks
    int baseID = static_cast<int>(_placedBlocks.size());
    vectorSize = static_cast<int>(_curLayout->size());
    for(b = 0; b < vectorSize; b++)
    {
        const CapoBlock& curBlk = *(*_curLayout)[b];
        std::vector<unsigned>::const_iterator nItr;
        for(nItr = curBlk.cellIdsBegin(); nItr != curBlk.cellIdsEnd(); nItr++)
            nodeBlock[*nItr] = b + baseID;
    }
}

void CapoPlacer::getBlockMembership(vector<int>& nodeBlock, 
				    vector<CapoBlock *> blocks) const
{
    nodeBlock.clear();
    nodeBlock.reserve(_hgraphOfNetlist.getNumNodes());
    for(unsigned i = 0; i < _hgraphOfNetlist.getNumNodes(); i++)
        nodeBlock.push_back(-1);
    // Initialize the block number to -1 so that cells that are not
    // in any block will have -1 as its block id

    // cells that are in blocks
    int b, vectorSize = static_cast<int>(blocks.size());
    for(b = 0; b < vectorSize; b++)
    {
        const CapoBlock& curBlk = *(blocks[b]);
        std::vector<unsigned>::const_iterator nItr;
        for(nItr = curBlk.cellIdsBegin(); nItr != curBlk.cellIdsEnd(); nItr++)
            nodeBlock[*nItr] = b;
    }
}

void CapoPlacer::getHierCellNames(vector<const char*>& cellNames) const
{
    unsigned k;
    for(k=0; k!=cellNames.size(); k++)
     if (cellNames[k]) delete (char*)cellNames[k];
    cellNames.clear();
    cellNames.insert(cellNames.end(),_hgraphOfNetlist.getNumNodes(),
                     (char*)NULL);

    // cells that are in placed blocks
    unsigned vectorSize = _placedBlocks.size();
    bit_vector		seenCell(_hgraphOfNetlist.getNumNodes(), false);

    for(unsigned b = 0; b < vectorSize; b++)
    {
        const CapoBlock& curBlk = *(_placedBlocks[b]);
        const char* blkName=curBlk.getName();
        abkfatal(blkName," NULL block name in CapoPlacer::getHierCellName()"
        " --- make sure that small blocks were not replaced earlier\n");
        unsigned len=strlen(blkName)+20;
	char  *shortBlkName = new char[len];
	std::fill(shortBlkName, shortBlkName+len, 0);
	strncpy(shortBlkName, blkName, 17);	//only the first 4 levels
	//strcpy(shortBlkName, blkName);
	
// cout<<"Shortened Block name: "<<shortBlkName<<endl;

        std::vector<unsigned>::const_iterator nItr;
        for(nItr = curBlk.cellIdsBegin(); nItr != curBlk.cellIdsEnd(); nItr++)
        {
	   abkfatal(!seenCell[*nItr], "already found this cell");
	   seenCell[*nItr] = true;
           cellNames[*nItr] = new char[len+10];
           sprintf((char*)cellNames[*nItr],"%s%d",shortBlkName,*nItr);
        }
        delete shortBlkName;
    }

    // cells that are in unplaced blocks
    // The block id continues from the placed blocks
    for(k=0; k!=cellNames.size(); k++)
    {
        if (cellNames[k]) continue;
        const char* name=_hgraphOfNetlist.getNodeByIdx(k).getName();
        cellNames[k] = new char[strlen(name)+2];
        strcpy((char*)cellNames[k],name);
    }
}

void CapoPlacer::printBlockMembership(ostream& os) const
{
    os << "# every line below is of the form  \"<CellName> <BlockNumber>\"\n"
       << "# cells that do not belong to any block will have "
       << "BlockNumber = -1\n";
    if(_placedBlocks.size() > 0)
    os << "# blocks with BlockNumber from 0 to " << _placedBlocks.size() - 1
       << " are placed blocks\n";

    vector<int> nodeBlock;
    getBlockMembership(nodeBlock);

    itHGFNodeGlobal node;
    for(node = _hgraphOfNetlist.nodesBegin();
        node != _hgraphOfNetlist.nodesEnd(); node++)
            os << setw(50) << (*node)->getName() << "  " << setw(4)
               << nodeBlock[(*node)->getIndex()] << endl;
}             

void CapoPlacer::printBlockMembership(const char* fileName) const
{
    ofstream outFile(fileName);
    printBlockMembership(outFile);
    outFile.close();
}          

void CapoPlacer::printHierCellNames(ostream& os) const
{
    os << "# every line below is of the form  \"<CellName> <NewCellName>\"\n"
       << "# cells that do not belong to any block keep original names \n";
//     << "# cells that do not belong to any block will have "
//     << " NoBlock|<id> as name\n";

    vector<const char*> cellNames;
    getHierCellNames(cellNames);

    itHGFNodeGlobal node;
    for(node = _hgraphOfNetlist.nodesBegin();
        node != _hgraphOfNetlist.nodesEnd(); node++)
            os << setw(45) << (*node)->getName() << "  " << setw(25)
               << cellNames[(*node)->getIndex()] << endl; 
    for(unsigned k=0; k!=cellNames.size(); k++) delete (char*)cellNames[k];
}

void CapoPlacer::printHierCellNames(const char* fileName) const
{
    ofstream outFile(fileName);
    printHierCellNames(outFile);
}

void CapoPlacer::saveBlocksCopy()
{
  int b, vectorSize = static_cast<int>(_placedBlocks.size());

  for(b = 0; b < vectorSize; b++)
    {
      CapoBlock * newBlk = new CapoBlock(_placedBlocks[b]);
      _saveBlocks.push_back(newBlk);
   }
  
  // cells that are in unplaced blocks
  vectorSize = static_cast<int>(_curLayout->size());
  for(b = 0; b < vectorSize; b++)
    {
      CapoBlock * newBlk = new CapoBlock((*_curLayout)[b]);
      _saveBlocks.push_back(newBlk);
    }
}


void CapoPlacer::saveBlocksAsFloorplan(char* fileName, 
				       vector<CapoBlock *> blocks)
{
  char * blocksFileName = new char[strlen(fileName) + 10];
  char * netsFileName = new char[strlen(fileName) + 10];
  char * plFileName = new char[strlen(fileName) + 10];
  
  sprintf(blocksFileName, "%s%s", fileName, ".blocks");
  sprintf(netsFileName, "%s%s", fileName, ".nets");
  sprintf(plFileName, "%s%s", fileName, ".pl");

  ofstream blocksFile(blocksFileName);
  ofstream netsFile(netsFileName);
  ofstream plFile(plFileName);

  
  //write the .blocks file and .pl file
  blocksFile << "UCSC blocks 1.0"<<endl;
  blocksFile <<TimeStamp()<<User()<<Platform()<<endl;
  blocksFile << "NumSoftRectangularBlocks : "<<blocks.size()<<endl;
  blocksFile << "NumHardRectilinearBlocks : 0"<<endl;
  blocksFile << "NumTerminals : "<<_hgraphOfNetlist.getNumTerminals()
	     <<endl<<endl;

  plFile <<  "UCSC pl 1.0"<<endl;
  plFile <<TimeStamp()<<User()<<Platform()<<endl;

  int b, vectorSize = static_cast<int>(blocks.size());
  vector<const char *> blockNames;

  for(b = 0; b < vectorSize; b++)
    {
      const CapoBlock& curBlk = *(blocks[b]);
      const BBox& blkBox = curBlk.getBBox();
      const char* blkName = curBlk.getName();
      blockNames.push_back(blkName);
      //double blkArea = (blkBox.xMax-blkBox.xMin)*(blkBox.yMax-blkBox.yMin);
      double blkArea = 0;
      std::vector<unsigned>::const_iterator nItr;
      for(nItr = curBlk.cellIdsBegin(); nItr != curBlk.cellIdsEnd(); nItr++)
	blkArea += _hgraphOfNetlist.getNodeWidth(*nItr)
	           *_hgraphOfNetlist.getNodeHeight(*nItr);

      double blkAR = (blkBox.xMax-blkBox.xMin)/(blkBox.yMax-blkBox.yMin);
      double blkWidth = sqrt(blkArea*blkAR);
      double blkHeight = blkWidth/blkAR;

      blocksFile<<blkName<<" softrectangular "<<blkArea<<" 0.5 2.0 "<<endl;
      
      /*
      blocksFile<<blkName<<" hardrectilinear 4 (0, 0) ("
		<<"0, "<<blkBox.yMax-blkBox.yMin<<") ("
		<<blkBox.xMax-blkBox.xMin<<", "<<blkBox.yMax-blkBox.yMin<<") ("
 		<<blkBox.xMax-blkBox.xMin<<", 0)"<<endl;
      plFile<<setw(8) <<blkName<<" "<<setw(10)<<blkBox.xMin
	    <<setw(10)<<blkBox.yMin<<endl;
      */

      plFile<<setw(8) <<blkName<<" "<<setw(10)<<blkBox.xMin
	    <<setw(10)<<blkBox.yMin<<" DIMS = ("<<blkWidth<<", "<<blkHeight
	    <<")"<<endl;

   }
  
  itHGFNodeGlobal nIt;
  for(nIt = _hgraphOfNetlist.terminalsBegin(); 
      nIt != _hgraphOfNetlist.terminalsEnd(); nIt++)
    {
      HGFNode& node   = **nIt;
      unsigned termIdx = node.getIndex();
      if (node.getName())
	{
	  blocksFile<<node.getName()<<" ";
	  plFile<<setw(8) <<node.getName()<<" ";
	}
      else  
	{
	  blocksFile<<"p"<< termIdx+1 <<" ";
	  plFile<<"p"<< termIdx+1 <<" ";
	}
      blocksFile<<"terminal "<<endl;

      const Point termLoc = _rbplace[termIdx];
      plFile<<setw(10)<<termLoc.x<<setw(10)<<termLoc.y<<endl;
    }

  //write the .nets file
  vector<int> netDegrees;
  vector<int> nodeBlock;
  getBlockMembership(nodeBlock, blocks); //mapping from cells to blocks

  unsigned numEdges = 0;
  unsigned numPins = 0;
  itHGFEdgeGlobal e;
  for (e=_hgraphOfNetlist.edgesBegin(); e!=_hgraphOfNetlist.edgesEnd(); ++e)
    {
      HGFEdge&        edge   = **e;
      bool needNet = 0;
      if (edge.getDegree() > 1)
	{
	  itHGFNodeLocal firstNode = (*e)->nodesBegin();
	  if(_hgraphOfNetlist.isTerminal((*firstNode)->getIndex()))
	    needNet = 1;
	  else
	    {
	      unsigned firstBlockId = nodeBlock[(*firstNode)->getIndex()];
	      itHGFNodeLocal v;
	      for (v=(*e)->nodesBegin()+1; v!=(*e)->nodesEnd(); v++)
		{
		  if(_hgraphOfNetlist.isTerminal((*v)->getIndex()))
		    {
		      needNet = 1;
		      break;
		    }
		  unsigned blockId = nodeBlock[(*v)->getIndex()];
		  if(blockId != firstBlockId) //atleast 1 different block
		    {
		      needNet = 1;
		      break;
		    }
		}
	    }
	}
      if(needNet)
	{
	  ++numEdges;
	  unsigned netDegree = 0;
	  bit_vector seenBlocks(blocks.size(), 0);
	  itHGFNodeLocal v;
	  for (v=(*e)->nodesBegin(); v!=(*e)->nodesEnd(); v++)
	    {
	      const HGFNode& node=(**v);
	      unsigned idx=node.getIndex();
	      if(_hgraphOfNetlist.isTerminal(idx))
		{
		  ++netDegree;
		  continue;
		}
	      
	      unsigned blockId = nodeBlock[idx];
	      if(!seenBlocks[blockId])
		{
		  seenBlocks[blockId] = 1;
		  ++netDegree;
		}
	    }
	  netDegrees.push_back(netDegree);
	  numPins += netDegree;
 	}
    }
  
  netsFile <<TimeStamp()<<User()<<Platform()<<endl;
  netsFile<<"NumNets : "<<numEdges<<endl;
  netsFile<<"NumPins : "<<numPins<<endl<<endl;

  unsigned netIdx = 0;
  for (e=_hgraphOfNetlist.edgesBegin(); e!=_hgraphOfNetlist.edgesEnd(); ++e)
    {
      HGFEdge&        edge   = **e;
      bool needNet = 0;
      if (edge.getDegree() > 1)
	{
	  itHGFNodeLocal firstNode = (*e)->nodesBegin();
	  if(_hgraphOfNetlist.isTerminal((*firstNode)->getIndex()))
	    needNet = 1;
	  else
	    {
	      unsigned firstBlockId = nodeBlock[(*firstNode)->getIndex()];
	      itHGFNodeLocal v;
	      for (v=(*e)->nodesBegin()+1; v!=(*e)->nodesEnd(); v++)
		{
		  if(_hgraphOfNetlist.isTerminal((*v)->getIndex()))
		    {
		      needNet = 1;
		      break;
		    }
		  unsigned blockId = nodeBlock[(*v)->getIndex()];
		  if(blockId != firstBlockId) //atleast 1 different block
		    {
		      needNet = 1;
		      break;
		    }
		}
	    }
	}

      if(needNet)
	{
	  netsFile<<"NetDegree : "<<netDegrees[netIdx]<<" "<<endl;
	  ++netIdx;
	  bit_vector seenBlocks(blocks.size(), 0);
	  itHGFNodeLocal v;
	  unsigned nodeOffset=0;
	  unsigned edgeId = (**e).getIndex();
	  for (v=(*e)->nodesBegin(); v!=(*e)->nodesEnd(); v++, nodeOffset++)
	    {
	      const HGFNode& node=(**v);
	      unsigned idx=node.getIndex();
	      if(_hgraphOfNetlist.isTerminal(idx))
		{
		  netsFile<<setw(10)<<node.getName()<<" B "<<endl;
		  continue;
		}

	      Point pOffset =
		_hgraphOfNetlist.nodesOnEdgePinOffset(nodeOffset, edgeId);

	      Point absOffset = _rbplace[idx];
	      absOffset.x = absOffset.x + pOffset.x;
	      absOffset.y = absOffset.y + pOffset.y;

	      unsigned blockId = nodeBlock[idx];
	      CapoBlock * block = blocks[blockId];
	      Point blkCenter = block->getCenter();
	      double blkWidth = block->getWidth();
	      double blkHeight = block->getHeight();
	      double xOff = 100*(absOffset.x - blkCenter.x)/blkWidth;
	      double yOff = 100*(absOffset.y - blkCenter.y)/blkHeight;

	      if(!seenBlocks[blockId])
		{
		  seenBlocks[blockId] = 1;
		  netsFile<<blockNames[blockId]<<" B  : %"<<xOff<<" %"<<yOff
			  <<endl;
		}
	    }
	}
    }
  
  blocksFile.close();
  netsFile.close();
  plFile.close();
}

void CapoPlacer::solveQuadraticMinSoln()
{
  vector<CapoBlock*> blocks;
  int b, numBlocks = static_cast<int>(_curLayout->size());
  for(b = 0; b < numBlocks; b++)
    {
      CapoBlock * newBlk = new CapoBlock((*_curLayout)[b]);
      blocks.push_back(newBlk);
    }

  vector< vector<unsigned> > nodeIds(numBlocks);
  vector<BBox> binBBox(numBlocks);

  for(b = 0; b < numBlocks; b++)
    {
      const CapoBlock& curBlk = *(blocks[b]);
      const BBox& blkBox = curBlk.getBBox();
      binBBox[b] = blkBox;
      std::vector<unsigned>::const_iterator nItr;
      for(nItr = curBlk.cellIdsBegin(); nItr != curBlk.cellIdsEnd(); nItr++)
	nodeIds[b].push_back(*nItr);
    }

  AnalyticalSolver solver(_rbplace, _placement, nodeIds, binBBox);
  if(_params.quadSkipNetsLargerThan != UINT_MAX)
    solver.setSkipNetsLargerThan(_params.quadSkipNetsLargerThan);

  BBox layoutBBox = _rbplace.getBBox(true);
  double maxDimension = max(layoutBBox.getHeight(),layoutBBox.getWidth());
  double epsilon = maxDimension/100;
  if(!_params.noCOG)
    solver.solveQuadraticMin(epsilon);
  else
    solver.solveSOR(epsilon, 0);
  vector< vector<Point> >& nodeLocs = solver.getNodeLocs();
  for(b=0; b<numBlocks; ++b)
    {
      for(unsigned i=0; i<nodeIds[b].size(); ++i)
        {
          _placement[nodeIds[b][i]] = nodeLocs[b][i];
        }
    }
}

unsigned CapoPlacer::getNumRegionedCellsInBlock(CapoBlock * block)
{
  unsigned numConstrCells=0;
  const vector<unsigned>& nodeIds=block->getCellIds();
  for(unsigned i=0; i!=nodeIds.size(); i++)
    {
      unsigned cellId = nodeIds[i];
      unsigned groupId = cellToGrpMapping[cellId];
      if(groupId != UINT_MAX)
	numConstrCells++;
    }
  return(numConstrCells);
}
