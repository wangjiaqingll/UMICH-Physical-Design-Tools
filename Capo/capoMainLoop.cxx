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








// Created: May 10, 1999 by Andrew Caldwell,
//	taken from capoPlacer.cxx by Igor Markov and Andrew Caldwell


//CHANGES

// 001218  ilm   empty blocks are now removed from layout

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "capoPlacer.h"
#include "capoBlock.h"
#include "RBPlace/RBPlacement.h"
#include "PlaceEvals/placeEvals.h"
#include "Stats/trivStats.h"
#include "Placement/xyDraw.h"

bool CapoPlacer::doOneLayer(AllowedPartDir partDir)
{
    Timer capoLoopTimer;

    vector<CapoBlock*>& activeLayout = _layout[_layerNum %2];
    vector<CapoBlock*>& newLayout    = _layout[(_layerNum +1) %2];
    _curLayout = &(_layout[_layerNum %2]);

    newLayout.clear();

    //refine each block in the active layout, and put the
    //newly created blocks into newLayout

    _totalOverfill = 0;

//NOTE:  This has been put back to do alternating RB

/*
    if(partDir == VOnly)
   	doVOnlyLayer(activeLayout, newLayout, _layerNum);
    else if(partDir == HOnly) 
   	doHOnlyLayer(activeLayout, newLayout, _layerNum);
    else
*/
   	doHAndVLayer(activeLayout, newLayout, _layerNum);
    _curLayout = &(_layout[(_layerNum+1) %2]);	//switch to the new layer

    if(_params.verb.forMajStats > 0) cerr<<endl;

    capoLoopTimer.stop();

    //if(true || _params.verb.forMajStats > 1)
    if(_params.verb.forMajStats > 0)
    {
	  cout<<endl;
          cout<<"Stats for Layer "<<_layerNum<<endl;

	  BBoxPlEval		hpWLEval   (_hgraphOfNetlist, _placement);
	  BBoxWChengPlEval      chengWLEval(_hgraphOfNetlist, _placement);
	  BBoxWRSMTPlEval       rsmtWLEval (_hgraphOfNetlist, _placement);

	  double		bboxWL  = hpWLEval.getCost();
	  double		chengWL = chengWLEval.getCost();
	  double		rsmtWL  = rsmtWLEval.getCost();

	  cout<<"Layer "<<_layerNum<<" Stats:  WL   (HP): "<<bboxWL <<"\n";
	  cout<<"Layer "<<_layerNum<<" Stats: WWL(Cheng): "<<chengWL<<"\n";
	  cout<<"Layer "<<_layerNum<<" Stats: WWL (RSMT): "<<rsmtWL <<"\n";

	  cout<<"Layer "<<_layerNum<<" Stats:  Time:  " 
	      << capoLoopTimer.getUserTime() << endl;

	  cout<<"Layer "<<_layerNum<<" Stats:  Memory: "<<MemUsage()<<endl;

	  _totalLayoutArea = _totalOverfill = _maxOverfill = 0;
          unsigned b;
	  for(b = 0; b < _curLayout->size(); b++)
	  {
		double cap = (*_curLayout)[b]->getCapacity();
		double ovr = (*_curLayout)[b]->getOverfill();
		_totalLayoutArea += cap;
		_totalOverfill   += ovr;

		_maxOverfill = max(_maxOverfill, ovr/cap);
	  }
	  for(b = 0; b < _placedBlocks.size(); b++)
	  {
		double cap = _placedBlocks[b]->getCapacity();
		double ovr = _placedBlocks[b]->getOverfill();
		_totalLayoutArea += cap;
		_totalOverfill   += ovr;

		_maxOverfill = max(_maxOverfill, ovr/cap);
	  }

	  cout<<"Layer "<<_layerNum<<" Ave Overfill: "
			<<100*(_totalOverfill/_totalLayoutArea)<<"%"<<endl;
	  cout<<"Layer "<<_layerNum<<" Max Overfill: "
			<<_maxOverfill*100<<"%"<<endl;
	
	
	  cout<<endl;
    }

    if( _params.doOverlapping &&
        (_layerNum >= _params.startOverlappingLayer) &&
        (_layerNum <= _params.endOverlappingLayer))
    {
          capoLoopTimer.start();
          doOverlapping(*_curLayout);
          capoLoopTimer.stop();

          if(true || _params.verb.forMajStats > 1)
          {
	    double hpWL = estimateWL(); 
	    cout<<"Layer "<<_layerNum<<" Stats:  Post-Overlapping HP WL: "
					<<hpWL<<"\n";
	    cout<<"Layer "<<_layerNum<<" Stats:  Overlapping Time:  " 
	        << capoLoopTimer.getUserTime() << endl;
	    cout<<endl;

	    _totalOverfill = 0;
	    for(unsigned i = 0; i < _curLayout->size(); i++)
		_totalOverfill += (*_curLayout)[i]->getOverfill();
	    cout<<"Layer "<<_layerNum<<" Stats:  Ave Relative Overfill: "
		<<_totalOverfill / _totalLayoutArea<<endl;
          }
    }
 
    if (_params.plotBlocks) plotBlocks(_layerNum,*_curLayout);

    if(_params.saveLayerBBoxes)
    {
	LayoutBBoxes regions;
   	for(unsigned box = 0; box < _curLayout->size(); box++)
            regions.push_back((*_curLayout)[box]->getBBox());

	char* layerStr = new char[30];
        sprintf(layerStr, "LevelBBoxes_%d", _layerNum);
        regions.save(layerStr);
        delete layerStr;
    }

    _layerNum++;

    return _curLayout->size() != 0;
}


void CapoPlacer::doVOnlyLayer(vector<CapoBlock*>& curLayout,
			      vector<CapoBlock*>& newLayout,
			      unsigned layerNum)
{
    if(_params.verb.forMajStats > 0)
    {
        cerr<<"Layer{V}: "<<layerNum<<"  Blocks: "<<curLayout.size()<<"\n";
        cout<<"Layer{V}: "<<layerNum<<"  Blocks: "<<curLayout.size()<<"\n";
    }

    unsigned totalNumBlks = _curLayout->size() + _placedBlocks.size();

    abkfatal(totalNumBlks > 0, 
		"There are no blocks at the start of doVOnlyLayer");

    unsigned minBlkSize = _hgraphOfNetlist.getNumNodes()/totalNumBlks;
    minBlkSize = minBlkSize < 50 ? 0 : (minBlkSize*2)/3;
	//min becomes 0 when we get to small block partitioning
	//so that we can't get stuck, unable to partition a very small
	//multi-row block

    bool didVSplit = true;

    newLayout.clear();

    while(didVSplit)
    {
	didVSplit = false;
	if(_params.verb.forMajStats > 1)
	    cout<<"Begin V-Split pass"<<endl;

        unsigned b;
        for(b = 0; b < curLayout.size(); b++)
	    curLayout[b]->_canSplitBlock = true;

	cerr<<"   [";

        for(b = 0; b < curLayout.size(); b++)
        {
	    CapoBlock& block = *curLayout[b];

	    if(_params.verb.forMajStats > 3)
	    {
	       cout<<"Block: "<<b<<" ["<<block.getNumCells() <<" cells]"<<endl;
	       if(_params.verb.forMajStats > 4)
	           cout<<block<<endl;
	    }

            if (block.getNumCells()==0) 
            {
               unsigned num=block.getIndex();
               block.unLinkNeighbors();
               delete curLayout[b];
               curLayout[b]=0;
               cout<<"- deleted empty block [" <<num<<"] " << endl;
            }
            else
            { 
   	       bool blockWasSplit = refineBlock(block,newLayout,_layerNum, 
						VOnly, minBlkSize, 
						_params.tdCongestionCtl);
	       didVSplit |= blockWasSplit;
	       if(blockWasSplit) delete curLayout[b];
            }

            curLayout[b]=0;
	}

	cerr<<"]\n";
	if(didVSplit)	//prepare to do it again...
	{
            curLayout = newLayout;
            newLayout.clear();
	}
    }
    curLayout.clear();	//everything is in newLayout
}

void CapoPlacer::doHOnlyLayer(vector<CapoBlock*>& curLayout,
			      vector<CapoBlock*>& newLayout,
			      unsigned layerNum)
{
    if(_params.verb.forMajStats > 0)
    {
        cerr<<"Layer{H}: "<<layerNum<<"  Blocks: "<<curLayout.size()<<"\n";
        cout<<"Layer{H}: "<<layerNum<<"  Blocks: "<<curLayout.size()<<"\n";
    }

    unsigned totalNumBlks = _curLayout->size() + _placedBlocks.size();
    unsigned minBlkSize = _hgraphOfNetlist.getNumNodes()/totalNumBlks;
    minBlkSize = minBlkSize < 50 ? 0 : minBlkSize/2;
	//min becomes 0 when we get to small block partitioning
	//so that we can't get stuck, unable to partition a very small
	//multi-row block

    cerr<<"   [";

    unsigned b;
    for(b = 0; b < curLayout.size(); b++)
    {
	curLayout[b]->_canSplitBlock = true;
	curLayout[b]->_isNewBlock    = false;
    }

    if(_params.verb.forMajStats > 1)
	    cout<<"H-Split pass"<<endl;

    vector<CapoBlock*> tempNewLayout;
    tempNewLayout.reserve(curLayout.size()*2);

	//now, do horizontal splits
    for(b = 0; b < curLayout.size(); b++)
    {
	CapoBlock& block = *curLayout[b];
	_totalOverfill += block.getOverfill();

	if(!block.canSplitBlock())
	{
	   if(_params.verb.forActions > 5)
		cout<<"Already partitioned: blk "<<b<<"("
			<<block.getIndex()<<")"<<endl;
	   //will all be deleted after this loop
	   continue;
	}

	if(_params.verb.forMajStats > 2)
	{
	       cout<<"Block: "<<b<<" ["<<block.getNumCells() <<" cells]"<<endl;
	       if(_params.verb.forMajStats > 4)
	           cout<<block<<endl;
	}

        if (block.getNumCells()==0)
        { 
           unsigned num=block.getIndex();
           block.unLinkNeighbors();
           delete curLayout[b];
           curLayout[b]=0;
           cout<<"- deleted empty block [" <<num<<"] " << endl;
        }
        else 
  	   refineBlock(block,tempNewLayout,_layerNum, HOnly, minBlkSize,
		       _params.tdCongestionCtl);
    }

    cerr<<"]";

	//blocks may have been passed over initially, then
	//split as a member of a chain..in which case they woul
	//already have been transfered to 'tempNewLayout'..
    for(b = 0; b < tempNewLayout.size(); b++)
    {
	if(tempNewLayout[b]->isNewBlock() ||
	   tempNewLayout[b]->canSplitBlock())
		newLayout.push_back(tempNewLayout[b]);
    }

    for(b = 0; b < curLayout.size(); b++)
    {
	if(!curLayout[b]->canSplitBlock())	//was split....delete it
	    delete curLayout[b];

	curLayout[b] = 0;
    }
    curLayout.clear();

}


void CapoPlacer::boost(vector<BBox>& bboxes)
{
  itHGFEdgeGlobal edge;
  
  bboxes.clear();
  for(edge = _hgraphOfNetlist.edgesBegin(); edge != _hgraphOfNetlist.edgesEnd(); edge++) {
    int eindex=(*edge)->getIndex();
    itHGFNodeLocal node;
    BBox box;
    box.clear();
    for(node = (*edge)->nodesBegin(); node != (*edge)->nodesEnd(); node++) {
      unsigned nodeId = (*node)->getIndex();
      box+=_placement[nodeId];
    }
    bboxes[eindex]=box;
  }

  /*  for(edge = _hgraphOfNetlist.edgesBegin(); edge != _hgraphOfNetlist.edgesEnd(); edge++) {
    int eindex=(*edge)->getIndex();
    printf("Edge %d from %.2f %.2f in xrange and %.2f %.2f in yrange\n", eindex, bboxes[eindex].xMin, bboxes[eindex].xMax, bboxes[eindex].yMin, bboxes[eindex].yMax);
    }*/
}

int CapoPlacer::probeBlock(CapoBlock& block)
{
  unsigned numLeaves = block.getNumCells();   
  
  if(numLeaves == 1) return 0;
  else if(block.getNumRows() == 1 && numLeaves < _params.smallPlaceThreshold) return 0;
  else {
    unsigned numRows = block.getNumRows();
    Verbosity sVerb = _params.verb;
    sVerb.forActions  /= 2;
    sVerb.forMajStats /= 2;
    sVerb.forSysRes   /= 2;
        
    vector<CapoBlock*> newBlocks;
    updateInfoAboutBlock(&block);
    
    unsigned lAhead  = _params.lookAhead;
    if(numLeaves < 500) lAhead = 0;
    
    double stretchFactor=( block.getAvgRowSpacing() > 1.5 * block.getAvgCellHeight() ? 2 : 1);
    bool splitHoriz= stretchFactor * block.getHeight() > block.getWidth();
    
    if (!splitHoriz)
      splitHoriz= (2*block.getHeight() >  block.getWidth() &&
		   numLeaves <= _params.smallPlaceThreshold*numRows) ;
    
    if( numRows > 1 && splitHoriz ) return 1;
    else return 2;
  }
  return 0;
}


void CapoPlacer::boostNets(CapoBlock & block, vector<BBox> & bboxes)
{
  const vector<unsigned>& cellids=block.getCellIds();
  int dir=probeBlock(block);
  int BF=_params.boostFactor;
  int boost=0;

  if(dir==0) return;

  for(unsigned i=0; i< cellids.size(); i++) {
    HGFNode node=_hgraphOfNetlist.getNodeByIdx(cellids[i]);
    unsigned nid = node.getIndex();
    itHGFEdgeGlobal edge;
    for(edge= node.edgesBegin(); edge != node.edgesEnd(); edge++) {
      if((*edge)->getWeight() == BF) continue;
      int eindex=(*edge)->getIndex();

      if(dir==2) {
	// vertical
	if(bboxes[eindex].xMin < _placement[nid].x &&  _placement[nid].x < bboxes[eindex].xMax) continue;
	else if(bboxes[eindex].xMin == _placement[nid].x && bboxes[eindex].xMax != _placement[nid].x) {(*edge)->setWeight(BF); boost++;}
	else if(bboxes[eindex].xMax == _placement[nid].x && bboxes[eindex].xMin != _placement[nid].x) {(*edge)->setWeight(BF); boost++;}
      } else if(dir==1) {
	// horizontal
	if(bboxes[eindex].yMin < _placement[nid].y &&  _placement[nid].y < bboxes[eindex].yMax) continue;
	else if(bboxes[eindex].yMin == _placement[nid].y && bboxes[eindex].yMax != _placement[nid].y) { (*edge)->setWeight(BF); boost++;}
	else if(bboxes[eindex].yMax == _placement[nid].y && bboxes[eindex].yMin != _placement[nid].y) { (*edge)->setWeight(BF); boost++;}
      }
    }
  }
  printf("BOOSTED %d nets\n", boost);
}

void CapoPlacer::deboostNets(CapoBlock & block)
{
  const vector<unsigned>& cellids=block.getCellIds();

  for(unsigned i=0; i< cellids.size(); i++) {
    HGFNode node=_hgraphOfNetlist.getNodeByIdx(cellids[i]);
    itHGFEdgeGlobal edge;
    for(edge= node.edgesBegin(); edge != node.edgesEnd(); edge++)  (*edge)->setWeight(1);   
  }
}

double CapoPlacer::squareCost()
{
  double square=0;
  itHGFEdgeGlobal edge;

  for(edge = _hgraphOfNetlist.edgesBegin(); edge != _hgraphOfNetlist.edgesEnd(); edge++) {
    itHGFNodeLocal node;
    BBox box;
    box.clear();
    for(node = (*edge)->nodesBegin(); node != (*edge)->nodesEnd(); node++) {
      unsigned nodeId = (*node)->getIndex();
      box+=_placement[nodeId];
    }
    square+=pow(box.xMax-box.xMin, 2)+pow(box.yMax-box.yMin, 2);
  }
  return square;
}

void CapoPlacer::doHAndVLayer(vector<CapoBlock*>& curLayout,
			      vector<CapoBlock*>& newLayout,
			      unsigned layerNum)
{
  if(_params.boost)  printf("Calculating the bounding boxes of nets\n");
  vector <BBox> bboxes(_hgraphOfNetlist.getNumEdges());
  
    if(_params.verb.forMajStats > 0)
    {
        cerr<<"Layer:    "<<layerNum<<"  Blocks: "<<curLayout.size()<<"\n";
        cout<<"Layer:    "<<layerNum<<"  Blocks: "<<curLayout.size()<<"\n";
    }

     unsigned minBlkSize = 0; //will partition everything

    unsigned b;
    for(b = 0; b < curLayout.size(); b++)
	    curLayout[b]->_canSplitBlock = true;

    cerr<<"   [";

    for(b = 0; b < curLayout.size(); b++)
    {
	CapoBlock& block = *curLayout[b];
	_totalOverfill += block.getOverfill();

	if(_params.verb.forMajStats > 2)
	{
	       cout<<"Block: "<<b<<" ["<<block.getNumCells() <<" cells]"<<endl;
	       if(_params.verb.forMajStats > 4)
	           cout<<block<<endl;
	}

        if (block.getNumCells()==0)
        {
           unsigned num=block.getIndex();
	   block.unLinkNeighbors();
           delete curLayout[b];
           curLayout[b]=0;
           cout << " - deleted empty block ["<<num<<"] " << endl;
        }
	/*
	else if(block.getNumSites() == 0)
	{
	  _placedBlocks.push_back(curLayout[b]);
	  curLayout[b] = 0;
	}
	*/
        else 
        {
	  if(_params.boost) 
	    if(layerNum <= _params.boostLayer) 
	      boost(bboxes);
	  if(_params.boost) 
	    if(layerNum <= _params.boostLayer) 
	      boostNets(block, bboxes);
	   bool splitBlock = refineBlock(block,newLayout,_layerNum, 
					 HAndV,minBlkSize, 
					 _params.tdCongestionCtl);
	   if(splitBlock) delete curLayout[b];
   	   curLayout[b] = 0;
        }
    }
    cerr<<"]";

    curLayout.clear();
}

