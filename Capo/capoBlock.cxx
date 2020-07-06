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














//created by Andrew Caldwell on 09/14/97  caldwell@cs.ucla.edu

// CHANGES

//aec 981124	reworked CapoBlock. Removed ClNets, and changed storage of
//		cell*s to HGFixedNode Ids.  See DOCS/CapoBlock.doc for
//		a description

//aec 990203	changed dbRows to RBPCoreRows

//ilm 990319    blocks now maintain a reflexive 4-directed neighborhood relation
//              and print their neighbors in op<<
//ilm 020822    ported to g++ 3.1.1

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
using std::sort;
#include <functional>
using std::less;
#include <vector>
using std::bit_vector;
#include "ABKCommon/abkcommon.h"
#include "Geoms/interval.h"
#include "capoBlock.h"
#include "capoPlacer.h"

unsigned CapoBlock::_nextBlockNum = 0;

CapoBlock::CapoBlock(
			const vector<unsigned>& cellIds,
			const HGraphWDimensions&      hgraph,
			itCBCoreRow rowsBegin,
			itCBCoreRow rowsEnd,
			const vector<CROffset>& startOffsets,
			const vector<CROffset>& endOffsets,
                        const vector<CapoBlock*>& neighbAbove,
                        const vector<CapoBlock*>& neighbBelow,
                        const vector<CapoBlock*>& leftNeighb,
                        const vector<CapoBlock*>& rightNeighb,
                        const CapoPlacer* capo,
			char* name
                     )
        : _coreRows(rowsBegin, rowsEnd), _startOffsets(startOffsets),
	  _endOffsets(endOffsets), _numSites(0), _capacity(0),
	  _cellIds(cellIds), _totalCellArea(0.0), _avgRowSpacing(0.0),
          _avgCellWidth(0.0), _avgCellHeight(0.0),
	  _largestCellArea(0.0), _name(NULL), _index(_nextBlockNum++),
	  _hgraph(hgraph), _capo(capo),_canSplitBlock(false), _isNewBlock(true),
	  _neighborsAbove(neighbAbove), _neighborsBelow(neighbBelow),
          _leftNeighbors(leftNeighb), _rightNeighbors(rightNeighb)
{
  //cerr << "coreRows.size() "<<_coreRows.size()<<endl;


    abkassert(_coreRows.size() == _endOffsets.size(), 
	"Mismatch CoreRows->EndOffsets");
    abkassert(_coreRows.size() == _startOffsets.size(), 
	"Mismatch CoreRows->StartOffsets");
//  abkfatal(cellIds.size() > 0, "attempt to create block with no cells");

    _sibling=this;

    double   minSiteHeight=DBL_MAX, minSiteSpacing=DBL_MAX;
    double   avgSiteHeight=0.0    , avgSiteSpacing=0.0;

    //cout << "coreRows.size() "<<_coreRows.size();
    //cout <<"  Y cood "<<_coreRows[0]->getYCoord()<<endl;

    double Ycoo = _coreRows[0]->getYCoord();
    unsigned r;
    for(r = 1; r != _coreRows.size(); r++)
    {
      double newYcoo = _coreRows[r]->getYCoord();
      _avgRowSpacing += abs(newYcoo - Ycoo);
      Ycoo =  newYcoo;
    }
    _avgRowSpacing /= (_coreRows.size() -1);

    for(r = 0; r != _coreRows.size(); r++)
    {
	const RBPCoreRow& cr = *_coreRows[r];
	unsigned sites = getContainedSitesInCoreRow(r);
	_numSites += sites;
	const RBPSite& crSite = cr.getSite();
	//by sadya in SYNP. get the correct capacity of the block
	_capacity += sites*crSite.width;
//	_capacity += sites*              cr.getSpacing();
//      _capacity += sites*crSite.height*cr.getSpacing();
//	_capacity += sites*crSite.height*crSite.width;
        minSiteHeight  =min(minSiteHeight,crSite.height);
        minSiteSpacing =min(minSiteSpacing,cr.getSpacing());
        avgSiteHeight +=sites*crSite.height;
        avgSiteSpacing+=sites*cr.getSpacing();

	double leftEdge = cr[_startOffsets[r].first].getXStart();
	leftEdge += _startOffsets[r].second * cr.getSpacing();
	double rightEdge =cr[_endOffsets[r].first].getXStart();
	rightEdge+= _endOffsets[r].second*cr.getSpacing()+cr.getSiteWidth();

	_bbox += Point(leftEdge,  cr.getYCoord());
	_bbox += Point(rightEdge, cr.getYCoord()+cr.getHeight());
    }

    //abkfatal3(_numSites != 0 || _cellIds.size()==0, 
    //"block with no sites and ",_cellIds.size()," cell(s) \n");
    abkwarn(_numSites != 0 || _cellIds.size()==0, "block with no sites\n");
    if (_numSites != 0)
    {
      _capacity *= minSiteHeight;
      avgSiteHeight /=_numSites;
      avgSiteSpacing/=_numSites;
    }
 /* 
    cout << " Site Height : " << avgSiteHeight << "(avg) "
         << minSiteHeight << "(min) "<< endl;
    cout << " Site Spacing: " << avgSiteSpacing<< "(avg) "
         << minSiteSpacing<< "(min) "<< endl;
 */
   
    //compute the total, and largest, cell area
    for(unsigned c = 0; c < _cellIds.size(); c++)
    {
        unsigned idx=cellIds[c];
        double cellArea=
            (ceil(hgraph.getNodeWidth(idx)/minSiteSpacing)*minSiteSpacing)
           *(ceil(hgraph.getNodeHeight(idx)/minSiteHeight)*minSiteHeight);
        _avgCellWidth   += hgraph.getNodeWidth(idx);
        _avgCellHeight  += hgraph.getNodeHeight(idx);
	_totalCellArea  += cellArea;
	_largestCellArea = max(_largestCellArea, cellArea);
        HGFNode& node=const_cast<HGFNode&>(hgraph.getNodeByIdx(idx));
        node.setWeight(cellArea);
    }

    if (_cellIds.size())
    { 
       _avgCellWidth  /= _cellIds.size();
       _avgCellHeight /= _cellIds.size();
    }

 // cout << " Average cell height :" << _avgCellHeight
 //      << " width : " << _avgCellWidth << endl;

    if(_capacity < _totalCellArea && _capo->getParams().verb.forMajStats)
    {
	cout<<"Warning: block's cell area exceeds block's capacity by "<<
		((_totalCellArea-_capacity)/_capacity)*100<<"%"<<endl;
	cout<<" Total Cell Area: "<<_totalCellArea<<endl;
	cout<<" Capacity:        "<<_capacity<<endl;
    }

    if(name)
    {
      _name = new char[strlen(name)+1];
      strcpy(_name, name);
    }
    else
    {
      _name = new char[1];
      _name[0] = 0;
    }
}


CapoBlock::CapoBlock(
			const vector<unsigned>&    cellIds,
			const HGraphWDimensions&   hgraph,
			const vector<const RBPCoreRow*>& candidateRows,
			const BBox& 		   boundary,
                        const vector<CapoBlock*>&  neighbAbove,
                        const vector<CapoBlock*>&  neighbBelow,
                        const vector<CapoBlock*>&  leftNeighb,
                        const vector<CapoBlock*>&  rightNeighb,
                        const CapoPlacer* capo,
			char* name
                     )
        : _numSites(0), _capacity(0), _cellIds(cellIds), _totalCellArea(0.0), 
          _avgRowSpacing(0.0), _avgCellWidth(0.0), _avgCellHeight(0.0), 
	  _largestCellArea(0.0), _name(NULL), _index(_nextBlockNum++),
	  _hgraph(hgraph), _capo(capo),_canSplitBlock(false), _isNewBlock(true),
	  _neighborsAbove(neighbAbove), _neighborsBelow(neighbBelow),
	  _leftNeighbors(leftNeighb), _rightNeighbors(rightNeighb)
{
    _sibling=this;

    compStartAndEndOffsets(boundary, candidateRows, _coreRows,
				_startOffsets, _endOffsets);

	/*
	for(unsigned so = 0; so < _startOffsets.size(); so++)
	{
		cout<<"("<<_startOffsets[so].first<<" - "
			 <<_startOffsets[so].second<<")"<<endl;
	}
	cout<<endl<<"EndOffsets: "<<endl;
	for(so = 0; so < _endOffsets.size(); so++)
	{
		cout<<"("<<_endOffsets[so].first<<" - "
			 <<_endOffsets[so].second<<")"<<endl;
	}
	*/
    double   minSiteHeight=DBL_MAX,minSiteSpacing=DBL_MAX;
    double   avgSiteHeight=0.0    ,avgSiteSpacing=0.0    ;

    double Ycoo = _coreRows[0]->getYCoord();
    unsigned r;
    for(r = 1; r != _coreRows.size(); r++)
    {
        double newYcoo = _coreRows[r]->getYCoord();
        _avgRowSpacing += abs(newYcoo - Ycoo);
        Ycoo =  newYcoo;
    }
        _avgRowSpacing /= _coreRows.size();

    for(r = 0; r != _coreRows.size(); r++)
    {
	const RBPCoreRow& cr = *_coreRows[r];
	unsigned sites = getContainedSitesInCoreRow(r);
	_numSites += sites;
	const RBPSite& crSite = cr.getSite();
	_capacity += sites*crSite.width;
//	_capacity += sites*              cr.getSpacing();
//      _capacity += sites*crSite.height*cr.getSpacing();
//	_capacity += sites*crSite.height*crSite.width;
        minSiteHeight  = min(minSiteHeight,crSite.height);
        minSiteSpacing = min(minSiteSpacing,cr.getSpacing());
        avgSiteHeight +=sites*crSite.height;
        avgSiteSpacing+=sites*cr.getSpacing();

	double leftEdge = cr[_startOffsets[r].first].getXStart();
	leftEdge += _startOffsets[r].second * cr.getSpacing();
	double rightEdge =cr[_endOffsets[r].first].getXStart();
	rightEdge+= _endOffsets[r].second*cr.getSpacing()+cr.getSiteWidth();

	_bbox += Point(leftEdge,  cr.getYCoord());
	_bbox += Point(rightEdge, cr.getYCoord()+cr.getHeight());
    }

    if(_bbox != boundary)
    {
      abkwarn(0, " boundary bbox does not match computed bbox");
      abkwarn(0, " (if running from floorplanned blocks, this is probably OK)");
    }

    abkfatal(_numSites != 0, "block with no sites");

    _capacity *= minSiteHeight;
    avgSiteHeight /=_numSites;
    avgSiteSpacing/=_numSites;
 /*
    cout << " Site Height : " << avgSiteHeight << "(avg) "
         << minSiteHeight << "(min) "<< endl;
    cout << " Site Spacing: " << avgSiteSpacing<< "(avg) "
         << minSiteSpacing<< "(min) "<< endl;
 */
  

	//compute the total, and largest, cell area
    for(unsigned c = 0; c < _cellIds.size(); c++)
    {
        unsigned idx=cellIds[c];
        double cellArea=
            (ceil(hgraph.getNodeWidth(idx)/minSiteSpacing)*minSiteSpacing)
           *(ceil(hgraph.getNodeHeight(idx)/minSiteHeight)*minSiteHeight );
	_totalCellArea  += cellArea;
        _avgCellWidth   += hgraph.getNodeWidth(idx);
        _avgCellHeight  += hgraph.getNodeHeight(idx);
	_largestCellArea = max(_largestCellArea, cellArea);
        HGFNode& node=const_cast<HGFNode&>(hgraph.getNodeByIdx(idx));
        node.setWeight(cellArea);
    }
    _avgCellWidth  /= _cellIds.size();
    _avgCellHeight /= _cellIds.size();

    if(_capacity < _totalCellArea && _capo->getParams().verb.forMajStats)
    {
	cout<<"Warning: block's cell area exceeds block's capacity by "<<
		((_totalCellArea-_capacity)/_capacity)*100<<"%"<<endl;
	cout<<" Total Cell Area: "<<_totalCellArea<<endl;
	cout<<" Capacity:        "<<_capacity<<endl;
    }

    if(name)
    {
      _name = new char[strlen(name)+1];
      strcpy(_name, name);
    }
    else
    {
      _name = new char[1];
      _name[0] = 0;
    }
}

       //constructor expects srcBlocks to be a contiguous set of
       //blocks, all with the same y-span.  The constructed block
       //contains all srcBlocks.

CapoBlock::CapoBlock(vector<CapoBlock*>& srcBlocks)
	: _numSites(0), _capacity(0), _totalCellArea(0), 
          _avgRowSpacing(0.0), _avgCellWidth(0.0), _avgCellHeight(0.0), 
	  _largestCellArea(0), _name(NULL), _index(UINT_MAX),
	  _hgraph(srcBlocks[0]->_hgraph),
          _capo(srcBlocks[0]->_capo), _canSplitBlock(false),
	  _isNewBlock(true)
{
    sort(srcBlocks.begin(), srcBlocks.end(), CompareSrcBlocksByX());

    for(unsigned b = 0; b < srcBlocks.size(); b++)
    {
	const CapoBlock& sBlk = *srcBlocks[b];

	if(!_bbox.isEmpty())
	    abkfatal2(_bbox.yMin == sBlk._bbox.yMin && 
		      _bbox.yMax == sBlk._bbox.yMax, 
		      "CapoBlock ctor expected row of blocks \n",
		      " with same y-projection");

	_bbox           += Point(sBlk._bbox.xMin, sBlk._bbox.yMin);
	_bbox           += Point(sBlk._bbox.xMax, sBlk._bbox.yMax);
	_numSites       += sBlk._numSites;
	_capacity       += sBlk._capacity;
	_totalCellArea  += sBlk._totalCellArea;
	_largestCellArea = max(_largestCellArea, sBlk._largestCellArea);
	_cellIds.insert(_cellIds.end(), sBlk._cellIds.begin(), 
					sBlk._cellIds.end());
    }

    _coreRows     = srcBlocks[0]->_coreRows;
    _startOffsets = srcBlocks[0]->_startOffsets;
    _endOffsets   = srcBlocks.back()->_endOffsets;

    abkfatal(_coreRows.size() == _startOffsets.size() && 
	     _startOffsets.size() == _endOffsets.size(),
	"#coreRows, startOffsets and endOffsets do not match");

    _name = new char[strlen("H-Row Composite Block")+2];
    strcpy(_name, "H-Row Composite Block");
}

//create new block with identical information
CapoBlock::CapoBlock(CapoBlock* src)
        : _coreRows(src->_coreRows),
	  _startOffsets(src->_startOffsets), _endOffsets(src->_endOffsets),
	  _numSites(unsigned(src->getNumSites())),
          _capacity(src->getCapacity()), 
          _totalCellArea(src->getTotalCellArea()), 
          _avgRowSpacing(src->getAvgRowSpacing()), 
          _avgCellWidth(src->getAvgCellWidth()), 
          _avgCellHeight(src->getAvgCellHeight()),
	  _largestCellArea(src->getLargestCellArea()), _index(src->getIndex()),
          _hgraph(src->_hgraph), _capo(src->_capo),
          _canSplitBlock(src->canSplitBlock()),
          _isNewBlock(src->isNewBlock())
	
{
   const CapoBlock& sBlk = *src;
   
   _bbox           += Point(sBlk._bbox.xMin, sBlk._bbox.yMin);
   _bbox           += Point(sBlk._bbox.xMax, sBlk._bbox.yMax);
   _cellIds.insert(_cellIds.end(), sBlk._cellIds.begin(), 
		   sBlk._cellIds.end());

   abkfatal(_coreRows.size() == _startOffsets.size() && 
	    _startOffsets.size() == _endOffsets.size(),
	    "#coreRows, startOffsets and endOffsets do not match");
   
   _name = new char[strlen(src->getName())+2];
   strcpy(_name,src->getName());
}


double CapoBlock::getContainedLeftEdge(unsigned r) const
{
    const CROffset&   startOffset = _startOffsets[r];
    const RBPCoreRow& curRow      = *_coreRows[r];

    abkassert(startOffset.first < curRow.getNumSubRows(),
		"start subRow offset is too large");

    const RBPSubRow& startSR = curRow[startOffset.first];

    abkassert(startOffset.second < startSR.getNumSites(),
		"start site offset is too large");

    double leftEdge = curRow[startOffset.first].getXStart();
    leftEdge += startOffset.second * curRow.getSpacing();

    return leftEdge;
}

double CapoBlock::getContainedRightEdge(unsigned r) const
{
    const CROffset&   endOffset   = _endOffsets[r];
    const RBPCoreRow& curRow      = *_coreRows[r];

    abkassert2(endOffset.first   < curRow.getNumSubRows(),
		"end subRow offset is too large: ",endOffset.first);

    const RBPSubRow& endSR   = curRow[endOffset.first];

    abkassert2(endOffset.second   < endSR.getNumSites(),
		"end site offset is too large: ",endOffset.second);

    double rightEdge = curRow[endOffset.first].getXStart();
    rightEdge += (endOffset.second*curRow.getSpacing()+curRow.getSiteWidth());

    return rightEdge;
}

unsigned CapoBlock::getContainedSitesInCoreRow(unsigned r) const
{
    const CROffset&   startOffset = _startOffsets[r];
    const CROffset&   endOffset   = _endOffsets[r];
    const RBPCoreRow& curRow      = *_coreRows[r];

    abkassert2(startOffset.first < curRow.getNumSubRows(),
		"start subRow offset is too large: ",startOffset.first);
    abkassert2(endOffset.first   < curRow.getNumSubRows(),
		"end subRow offset is too large: ", endOffset.first);

    const RBPSubRow& startSR = curRow[startOffset.first];
    const RBPSubRow& endSR   = curRow[endOffset.first];

    abkassert2(startOffset.second < startSR.getNumSites(),
		"start site offset is too large: ", startOffset.second);
    abkassert2(endOffset.second   < endSR.getNumSites(),
		"end site offset is too large: ", endOffset.second);

    unsigned numSites=0;
    for(unsigned sr = startOffset.first; sr <= endOffset.first; sr++)
    {
	int sitesInThisSR = curRow[sr].getNumSites();
	if(sr == endOffset.first)
	    sitesInThisSR = endOffset.second+1;
	if(sr == startOffset.first)
	    sitesInThisSR -= startOffset.second;

	numSites += sitesInThisSR; 
    }
    return numSites;
}

double CapoBlock::getContainedAreaInCoreRow(unsigned r) const
{
    const RBPSite& crSite = _coreRows[r]->getSite();
    return  getContainedSitesInCoreRow(r)*
		crSite.height*crSite.width;
}

double CapoBlock::getContainedAreaInBBox(const BBox& rect) const
{
  double containedArea=0;
  BBox intersect = _bbox.intersect(rect);
  if(intersect.getArea() == 0)
    return(0);
  
  for(unsigned r = 0; r < getNumRows(); r++)
    {
      const RBPCoreRow& curRow      = *_coreRows[r];
      double yCoord = curRow.getYCoord();
      double rowHeight = curRow.getHeight();
      double thisRowHeight = rowHeight;

      if((yCoord < intersect.yMin && (yCoord+rowHeight)<=intersect.yMin) ||
         (yCoord >= intersect.yMax ))
        continue;

      if((yCoord < intersect.yMin && (yCoord+rowHeight) >= intersect.yMin))
        thisRowHeight = (yCoord+rowHeight)-intersect.yMin;
      else if(yCoord < intersect.yMax && (yCoord+rowHeight)>intersect.yMax)
        thisRowHeight = intersect.yMax - yCoord;
    
      const CROffset&   startOffset = _startOffsets[r];
      const CROffset&   endOffset   = _endOffsets[r];

      abkassert2(startOffset.first < curRow.getNumSubRows(),
		 "start subRow offset is too large: ",startOffset.first);
      abkassert2(endOffset.first   < curRow.getNumSubRows(),
		 "end subRow offset is too large: ", endOffset.first);

      const RBPSubRow& startSR = curRow[startOffset.first];
      const RBPSubRow& endSR   = curRow[endOffset.first];

      abkassert2(startOffset.second < startSR.getNumSites(),
		 "start site offset is too large: ", startOffset.second);
      abkassert2(endOffset.second   < endSR.getNumSites(),
		 "end site offset is too large: ", endOffset.second);


      //this allows for fractional sites in the region
      double numSites=0;
      double siteSpacing = curRow.getSpacing();
      double siteWidth = curRow.getSiteWidth();

      for(unsigned sr = startOffset.first; sr <= endOffset.first; sr++)
	{
	  double xStart = curRow[sr].getXStart();
	  double xEnd = curRow[sr].getXEnd();
	  if(xStart < intersect.xMin)
	    xStart = intersect.xMin;
	  if(xEnd > intersect.xMax)
	    xEnd = intersect.xMax;

	  if(xEnd <= xStart)
	    continue;
	  double sitesInThisSR = (xEnd-xStart)/siteSpacing;

	  numSites += sitesInThisSR; 
	}
      containedArea += numSites*thisRowHeight*siteWidth;
    }
  return(containedArea);
}

double CapoBlock::findXSplit(double c0, double c1, double roundPct,
				vector<double>& partCaps) const
{
	//it's easier to just include all begin/end of (sub) rows,
	//even if they're outside the block...they won't have any
	//effect
    vector<double> rowEnds;
    for(unsigned c = 0; c < _coreRows.size(); c++)
    {
	const RBPCoreRow& cr = *(_coreRows[c]);

        itRBPSubRow srIdx;
	for(srIdx = cr.subRowsBegin(); srIdx != cr.subRowsEnd(); srIdx++)
	{
	    rowEnds.push_back(srIdx->getXStart());
	    rowEnds.push_back(srIdx->getXEnd());
	}
    }
 
    sort(rowEnds.begin(), rowEnds.end(), std::less<double>());
    rowEnds.erase(std::unique(rowEnds.begin(),rowEnds.end()),
		       rowEnds.end());

    double xSplit = findXSplitUsingSkyline(c0, c1, partCaps);

    if(xSplit == _bbox.xMin || xSplit == _bbox.xMax)
      {
	xSplit = rint((_bbox.xMin + _bbox.xMax)/2.0);
	computePartAreas(xSplit, partCaps);
      }
    double maxRoundDist = getWidth()*(roundPct/100);
    double roundedSplit = DBL_MAX;
	//check to see if the desired xSplit is within 
	//maxRoundDist of a break. if so..take the closest one.
	//otherwise, go with the original split.
    for(unsigned s = 0; s < rowEnds.size(); s++)
    {
	if(rowEnds[s] >= xSplit-maxRoundDist &&
	   rowEnds[s] <= xSplit+maxRoundDist)
	{
	   roundedSplit = rowEnds[s];
	   maxRoundDist = max(rowEnds[s]-xSplit,
				 xSplit-rowEnds[s]); 
	}
    }

    if(roundedSplit != DBL_MAX) 
    {
	xSplit = roundedSplit;
	//changed the splitting-line. recompute partition areas
	computePartAreas(xSplit, partCaps);
    }

    /*
     * Xiaojian: xSplit cannot be 0% or 100%
     * otherwise it causes infinite loop
     */
    if(xSplit <= _bbox.xMin || xSplit >= _bbox.xMax)
      {
	xSplit = rint((_bbox.xMin + _bbox.xMax)/2.0);
	computePartAreas(xSplit, partCaps);
      }


    abkfatal(partCaps[0] + partCaps[1] == getCapacity(),
	"partCapacities do not sum to total block capacity");

    return xSplit;
}
	//find the split which produces the largest min relative whitespace
	//put the resulting partition site areas in partCaps
double CapoBlock::findXSplitUsingSkyline(double c0, double c1, 
					vector<double>& partCaps) const
{
    vector<double> beginEdges;
    vector<double> endEdges;
    beginEdges.push_back(_bbox.xMax+100);       //acts as an end point
    endEdges.push_back(_bbox.xMax+100); //acts as an end point
    const RBPSite&  crSite = _coreRows[0]->getSite();
    double spacing = _coreRows[0]->getSpacing();

	//iterate through the contained rows, and store up the beginning
	//and end of row-intervals
    for(unsigned r = 0; r < _coreRows.size(); r++)
    {
        const RBPCoreRow& cr = (*_coreRows[r]);
        abkfatal(cr.getSite() == crSite, "coreRows w/ different sites");
        abkfatal(cr.getSpacing() == spacing, "coreRows w/ different spacing");

        unsigned srIdx = _startOffsets[r].first;
        for(srIdx=_startOffsets[r].first; srIdx<=_endOffsets[r].first;srIdx++)
        {
            double leftEdge = cr[srIdx].getXStart();
            if(srIdx == _startOffsets[r].first)
                leftEdge += _startOffsets[r].second*cr[srIdx].getSpacing();

            abkassert(_bbox.xMin <= leftEdge, "bbox or leftEdge is wrong");
            beginEdges.push_back(leftEdge);

            double rightEdge;
            if(srIdx == _endOffsets[r].first)
                rightEdge = cr[srIdx].getXStart() +
                        _endOffsets[r].second*cr[srIdx].getSpacing() +
                        cr[srIdx].getSiteWidth();
            else
                rightEdge = cr[srIdx].getXEnd();

            abkassert(_bbox.xMax >= rightEdge, "bbox or rightEdge is wrong");
            endEdges.push_back(rightEdge);
        }
    }

    sort(beginEdges.begin(), beginEdges.end(), std::less<double>());
    sort(endEdges.begin(),   endEdges.end(),   std::less<double>());

    double   siteArea    = crSite.height * crSite.width;
    unsigned numRows     = 0;
    double   areaLeftOfX = 0;
    unsigned beginIdx    = 0;
    unsigned endIdx      = 0;

    double   bestMinRelWS= -DBL_MAX;
    double   bestXSplit  = 0;
    partCaps = vector<double>(2);

    for(double xSplit = _bbox.xMin; xSplit <= _bbox.xMax; xSplit+= spacing)
    {
        while(beginEdges[beginIdx] < xSplit)
        { numRows++; beginIdx++; }

        while(endEdges[endIdx] < xSplit)
        { numRows--; endIdx++; }

        areaLeftOfX += numRows * siteArea;
	double w0 = c0/areaLeftOfX;
	double w1 = c1/(_capacity-areaLeftOfX);
	double minRelWS = min(w0,w1);

        if(minRelWS > bestMinRelWS)
        {   bestMinRelWS = minRelWS; 
	    bestXSplit   = xSplit;
            partCaps[0]  = areaLeftOfX;
            partCaps[1]  = _capacity - areaLeftOfX;
        }
    }

    return bestXSplit;
}

	//assuming the block is split at xSplit...put the resulting
	//site areas in partCaps
void CapoBlock::computePartAreas(double xSplit, vector<double>& partCaps) const
{
    vector<double> beginEdges;
    vector<double> endEdges;
    beginEdges.push_back(_bbox.xMax+100);       //acts as an end point
    endEdges.push_back(_bbox.xMax+100); //acts as an end point
    const RBPSite&  crSite = _coreRows[0]->getSite();
    double spacing = _coreRows[0]->getSpacing();
    for(unsigned r = 0; r < _coreRows.size(); r++)
    {
        const RBPCoreRow& cr = (*_coreRows[r]);
        abkassert(cr.getSite() == crSite, "coreRows w/ different sites");
        abkassert(cr.getSpacing() == spacing, "coreRows w/ different spacing");

        unsigned srIdx = _startOffsets[r].first;
        for(srIdx=_startOffsets[r].first; srIdx<=_endOffsets[r].first;srIdx++)
        {
            double leftEdge = cr[srIdx].getXStart();
            if(srIdx == _startOffsets[r].first)
                leftEdge += _startOffsets[r].second*cr[srIdx].getSpacing();

            abkassert(_bbox.xMin <= leftEdge, "bbox or leftEdge is wrong");
            beginEdges.push_back(leftEdge);

            double rightEdge;
            if(srIdx == _endOffsets[r].first)
                rightEdge = cr[srIdx].getXStart() +
                        _endOffsets[r].second*cr[srIdx].getSpacing() +
                        cr[srIdx].getSiteWidth();
            else
                rightEdge = cr[srIdx].getXEnd();

            abkassert(_bbox.xMax >= rightEdge, "bbox or rightEdge is wrong");
            endEdges.push_back(rightEdge);

        }
    }
    sort(beginEdges.begin(), beginEdges.end(), less<double>());
    sort(endEdges.begin(),   endEdges.end(),   less<double>());

    double   siteArea    = crSite.height * crSite.width;
    unsigned numRows     = 0;
    double   areaLeftOfX = 0;
    unsigned beginIdx    = 0;
    unsigned endIdx      = 0;

    for(double x = _bbox.xMin; x <= xSplit; x+= spacing)
    {
        while(beginEdges[beginIdx] < x)
        { numRows++; beginIdx++; }

        while(endEdges[endIdx] < x)
        { numRows--; endIdx++; }

        areaLeftOfX += numRows * siteArea;
    }
    partCaps = vector<double>(2);
    partCaps[0] = areaLeftOfX;
    partCaps[1] = _capacity-areaLeftOfX;
}

void CapoBlock::resetCellIds(const vector<unsigned>& newCellIds)
{
    _cellIds = newCellIds;
    _totalCellArea = _largestCellArea = 0;

    abkfatal(newCellIds.size() > 0, "attempted reset with no cells");

    for(unsigned c = 0; c < _cellIds.size(); c++)
    {
	abkfatal( _cellIds[c] < _hgraph.getNumNodes(),
		"cellId is too large during reset");

	_totalCellArea  += _hgraph.getNodeByIdx(_cellIds[c]).getWeight();
	_largestCellArea = max(_largestCellArea, 
			double(_hgraph.getNodeByIdx(_cellIds[c]).getWeight()));
    }

    if(_capacity < _totalCellArea && _capo->getParams().verb.forMajStats)
    {
	cout<<"After reset: block's cell area exceeds block's capacity by "<<
		((_totalCellArea-_capacity)/_capacity)*100<<"%"
		<<" after resetCellIds"<<endl;
	cout<<" Total Cell Area: "<<_totalCellArea<<endl;
	cout<<" Capacity:        "<<_capacity<<endl;
    }
}

double CapoBlock::getMedianCellArea() const
{
    CapoBlock* access = const_cast<CapoBlock*>(this);
    vector<unsigned>::iterator medianItr 
	      = access->_cellIds.begin() + (_cellIds.size()/2);
    std::nth_element(access->_cellIds.begin(), medianItr,
		    access->_cellIds.end(), CompareCellIdsByArea(_hgraph));

    return _hgraph.getNodeByIdx(*medianItr).getWeight();
}

double CapoBlock::getWidestCellWidth() const
{
    double widestWidth = 0;
    for(unsigned c = 0; c < _cellIds.size(); c++)
	widestWidth = max(widestWidth, _hgraph.getNodeWidth(_cellIds[c]));	

    return widestWidth;
}

void CapoBlock::linkNeighbors()
{
  unsigned k;
  for(k=0; k!=_neighborsAbove.size(); k++) 
      _neighborsAbove[k]->_addNeighborBelow (this);
  for(k=0; k!=_neighborsBelow.size(); k++) 
       _neighborsBelow[k]->_addNeighborAbove(this);
  for(k=0; k!=_leftNeighbors .size(); k++) 
       _leftNeighbors [k]->_addRightNeighbor(this);
  for(k=0; k!=_rightNeighbors.size(); k++) 
       _rightNeighbors[k]->_addLeftNeighbor (this);
}

void CapoBlock::unLinkNeighbors()
{
   unsigned k;
   for(k=0; k!=_neighborsAbove.size(); k++) 
      _neighborsAbove[k]->_removeNeighborBelow(this);
   for(k=0; k!=_neighborsBelow.size(); k++) 
      _neighborsBelow[k]->_removeNeighborAbove(this);
   for(k=0; k!=_leftNeighbors .size(); k++) 
      _leftNeighbors [k]->_removeRightNeighbor(this);
   for(k=0; k!=_rightNeighbors.size(); k++) 
      _rightNeighbors[k]->_removeLeftNeighbor (this);
}

void CapoBlock::compStartAndEndOffsets(
                const BBox&                	 boundary,
                const vector<const RBPCoreRow*>& candidateRows,
                vector<const RBPCoreRow*>&  	 coreRows,
                vector<CROffset>&          	 startOffsets,
                vector<CROffset>&          	 endOffsets)
{
    for(unsigned crId = 0; crId < candidateRows.size(); crId++)
    {
        const RBPCoreRow& cr = *(candidateRows[crId]);
        const double spacing = cr.getSpacing();
    
        if(cr.getXStart() >= boundary.xMax || cr.getXEnd() <= boundary.xMin ||
           cr.getYCoord() < boundary.yMin  || cr.getYCoord() >= boundary.yMax)
                continue;

        coreRows.push_back(candidateRows[crId]);
        startOffsets.push_back(CROffset(UINT_MAX,UINT_MAX));
        endOffsets.  push_back(CROffset(UINT_MAX,UINT_MAX));
    
                //find left edge        
        double leftEdge = boundary.xMin;
        itRBPSubRow srIt;
        for(srIt = cr.subRowsBegin(); srIt != cr.subRowsEnd(); srIt++)
        {
            if((*srIt).getXEnd() >= leftEdge)  //1st contained site in this sr
            {
                startOffsets.back().first = srIt-cr.subRowsBegin();
                double delta = leftEdge-(*srIt).getXStart();
    
                if(delta <= 0)
                    startOffsets.back().second = 0;
                else 
                    startOffsets.back().second = 
                                static_cast<unsigned>(ceil(delta/spacing));
                break;
            }
        }  
           
       abkfatal(startOffsets.back().first != UINT_MAX,
                "error: contained subrow not found - StartOffsets is UINT_MAX");
    
                //find right edge
        double rightEdge = boundary.xMax;
        for( ; srIt != cr.subRowsEnd(); srIt++)
        {
            if((*srIt).getXStart() >= rightEdge)
                break;

            endOffsets.back().first  = srIt-cr.subRowsBegin();

            double delta = (*srIt).getXEnd()-rightEdge;
            if(delta <= 0)      //contain the whole subrow
	    {
                endOffsets.back().second = (*srIt).getNumSites()-1;
                break;
	    }
            else
            {
                unsigned excludedSites =
                        static_cast<unsigned>(floor(delta/spacing));

                endOffsets.back().second =
                        ((*srIt).getNumSites()-excludedSites)-1;
                        //minus -1 because the 'end' site is to be the
                        //last site contained within inside the blk
                break;
            }
        }

        abkfatal(startOffsets.back().first != UINT_MAX,
                "error: contained subrow not found - EndOffsets is UINT_MAX");
    }
}

unsigned CapoBlock::getNumAdjacentCells()   const
{
   unsigned numAdj=0;

   const vector<unsigned>& nodeIds=getCellIds();
   bit_vector  nodes(_hgraph.getNumNodes(),false); // visited
   bit_vector  nets (_hgraph.getNumEdges(),false);  // insident to block

   unsigned i;
   for(i=0; i!=nodeIds.size(); i++) nodes[nodeIds[i]]=true;
   for(i=0; i!=nodeIds.size(); i++)
   {
     const HGFNode& node=_hgraph.getNodeByIdx(nodeIds[i]);

     for(itHGFEdgeLocal edgeIt =node.edgesBegin();
                        edgeIt!=node.edgesEnd(); edgeIt++)
     {
        const HGFEdge& edge=(**edgeIt);
        if (nets[edge.getIndex()]) continue;
        nets[edge.getIndex()]=true;
        for(itHGFNodeLocal nodeIt =edge.nodesBegin();
                         nodeIt!=edge.nodesEnd(); nodeIt++)
        {
           unsigned nodeIdx=(*nodeIt)->getIndex();
           if (nodes[nodeIdx]) continue;
           nodes[nodeIdx]=true;
           numAdj++;
        }
     }
   }
  return numAdj;
}

unsigned CapoBlock::getNumAdjacentCellsWithDuplicates()   const
{
   unsigned numAdj=0;

   const vector<unsigned>& nodeIds=getCellIds();
   bit_vector  nodes(_hgraph.getNumNodes(),false); // visited
   bit_vector  nets (_hgraph.getNumEdges(),false);  // insident to block

   unsigned i;
   for(i=0; i!=nodeIds.size(); i++) nodes[nodeIds[i]]=true;
   for(i=0; i!=nodeIds.size(); i++)
   {
     const HGFNode& node=_hgraph.getNodeByIdx(nodeIds[i]);

     for(itHGFEdgeLocal edgeIt =node.edgesBegin();
                        edgeIt!=node.edgesEnd(); edgeIt++)
     {
        const HGFEdge& edge=(**edgeIt);
        if (nets[edge.getIndex()]) continue;
        nets[edge.getIndex()]=true;
        for(itHGFNodeLocal nodeIt =edge.nodesBegin();
                         nodeIt!=edge.nodesEnd(); nodeIt++)
        {
           unsigned nodeIdx=(*nodeIt)->getIndex();
           if (nodes[nodeIdx]) continue;
           //nodes[nodeIdx]=true;   // commented out by s.m on 01/10/01
                                    // this is to allow multiple count of
                                    // an adjacent cell
           numAdj++;
        }
     }
   }
  return numAdj;
}

ostream& operator<<(ostream& os, const CapoBlock& cb)
{
    os << "   Block ";
    if(cb._name != NULL)
       os << "\""<<cb._name<<"\"  ";
//  os <<  " " << setbase(16) << unsigned(&cb) << setbase(10) << "   ";
    os << "[" <<cb.getIndex()<<"]"<<endl;
       
    unsigned aboveN=cb._neighborsAbove.size(),belowN=cb._neighborsBelow.size();
    unsigned  leftN=cb._leftNeighbors .size(),rightN=cb._rightNeighbors.size();
    os <<"   Neighboring blockIds :  ";
    if (aboveN+belowN+leftN+rightN>0) 
    {
      if (aboveN+belowN+leftN+rightN>3) os << endl;
      unsigned k;
      for(k=0; k!=aboveN;k++) os<<"["<<cb._neighborsAbove[k]->getIndex()<<"] ";
      if (aboveN) os << "(above)  ";    

      for(k=0; k!=belowN;k++)os<<"["<<cb._neighborsBelow[k]->getIndex()<<"] ";
      if (belowN) os << "(below)  ";    

      for(k=0; k!=leftN; k++)os<<"["<<cb._leftNeighbors[k] ->getIndex()<<"] ";
      if (leftN) os << "(left)  ";    

      for(k=0; k!=rightN;k++)os<<"["<<cb._rightNeighbors[k]->getIndex() <<"] ";
      if (rightN) os << "(right)  ";    
    }
    else os << " none ";
    os << endl;

    os << "   BBox        : " << cb._bbox;
    os << "\n CoreRows    : " << cb.getNumRows()
	<<" (First CR has "<<1+(cb._endOffsets[0].first - cb._startOffsets[0].first)
	<<" SubRows)";
    os << "\n Sites       : " << cb._numSites;
    os << "\n Capacity    : " << cb._capacity;
    os << "\n Cells       : " << cb._cellIds.size();
    os << "\n TotCellArea : " << cb._totalCellArea;
    os << "\n LargestCell : " << cb._largestCellArea;
    os << "\n Whitespace %: " << cb.getRelativeWhitespace() * 100.0;
    os << endl;
    
    return os;
}

//desiredDir = 0 == vertical Split ; desiredDir = 1 == horizontal Split
bool CapoBlock::findSplitPtDir(bool desiredDir, 
		vector< pair<BBox, vector<unsigned> > >& groupRegionConstr,
	        bool &splitDir, double& splitPt)
{
  if(getNumCells() <= 200)
    return(false);

  //bool needToChangeDefaultFlow = false;
  double blkArea = _bbox.getArea();
  vector<unsigned> intersectingRegions;
  for(unsigned i=0; i<groupRegionConstr.size(); ++i)
    {
      BBox& rgnBBox = groupRegionConstr[i].first;
      BBox intersection = rgnBBox.intersect(_bbox);
      double intArea = intersection.getArea();
      if(((blkArea-intArea)/blkArea) < 0.05) //region almost covers entire blk
	return(false);
      else if((intArea/blkArea) > 0.01)
	intersectingRegions.push_back(i);
    }
  if(intersectingRegions.size()==0) //no intersecting regions found
    {
      if(desiredDir == 0)
	splitPt = rint((_bbox.xMin + _bbox.xMax)/2);
      else
	splitPt = rint((_bbox.yMin + _bbox.yMax)/2);
      splitDir = desiredDir;
      return(true);
    }

  double blockArea = _capacity;
  vector<double> potentialSplitPts;
  //desiredDir = 0 == vertical Split ; desiredDir = 1 == horizontal Split
  bool partDir = desiredDir;
  bool success = false;
  double bestSplitPt=DBL_MAX;
  double relativeDiff=DBL_MAX;
  for(unsigned num=0; num<4; ++num)
    {
      potentialSplitPts.clear();
      if(partDir == false)
	{
	  for(unsigned i=0; i<intersectingRegions.size(); ++i)
	    {
	      BBox& rgnBBox = groupRegionConstr[intersectingRegions[i]].first;
	      BBox intersection = rgnBBox.intersect(_bbox);
	      potentialSplitPts.push_back(intersection.xMin);
	      potentialSplitPts.push_back(intersection.xMax);
	    }
	  bestSplitPt=DBL_MAX;
	  relativeDiff=DBL_MAX;
	  for(unsigned i=0; i<potentialSplitPts.size(); ++i)
	    {
	      BBox p0BBox = _bbox;
	      p0BBox.xMax = potentialSplitPts[i];
	      //check if this split pt intersects any other region
	      bool intersectsOtherRgns=false;
	      if(num < 2)
		{
		  for(unsigned j=0; j<intersectingRegions.size(); ++j)
		    {
		      BBox& rgnBBox = groupRegionConstr[intersectingRegions[j]].first;
		      if(potentialSplitPts[i] > rgnBBox.xMin && 
			 potentialSplitPts[i] < rgnBBox.xMax)
			{
			  intersectsOtherRgns=true;
			  break;
			}
		    }
		}
	      if(!intersectsOtherRgns)
		{
		  double p0Area = getContainedAreaInBBox(p0BBox);
		  double p1Area = blockArea-p0Area;
		  double thisRelDiff = fabs(p0Area-p1Area)/blockArea;
		  if(thisRelDiff < relativeDiff)
		    {
		      relativeDiff=thisRelDiff;
		      bestSplitPt=potentialSplitPts[i];
		      success = true;
		    }
		}
	    }
	}
      else //horizontal split
	{
	  for(unsigned i=0; i<intersectingRegions.size(); ++i)
	    {
	      BBox& rgnBBox = groupRegionConstr[intersectingRegions[i]].first;
	      BBox intersection = rgnBBox.intersect(_bbox);
	      potentialSplitPts.push_back(intersection.yMin);
	      potentialSplitPts.push_back(intersection.yMax);
	    }
	  bestSplitPt=DBL_MAX;
	  relativeDiff=DBL_MAX;
	  for(unsigned i=0; i<potentialSplitPts.size(); ++i)
	    {
	      BBox p0BBox = _bbox;
	      p0BBox.yMin = potentialSplitPts[i];
	      //check if this split pt intersects any other region
	      bool intersectsOtherRgns=false;
	      if(num < 2)
		{
		  for(unsigned j=0; j<intersectingRegions.size(); ++j)
		    {
		      BBox& rgnBBox = groupRegionConstr[intersectingRegions[j]].first;
		      if(potentialSplitPts[i] > rgnBBox.yMin && 
			 potentialSplitPts[i] < rgnBBox.yMax)
			{
			  intersectsOtherRgns=true;
			  break;
			}
		    }
		}
	      if(!intersectsOtherRgns)
		{
		  double p0Area = getContainedAreaInBBox(p0BBox);
		  double p1Area = blockArea-p0Area;
		  double thisRelDiff = fabs(p0Area-p1Area)/blockArea;
		  if(thisRelDiff < relativeDiff)
		    {
		      relativeDiff=thisRelDiff;
		      bestSplitPt=potentialSplitPts[i];
		      success = true;
		    }
		}
	    }
	}
      if(!success)
	partDir = !partDir;
      else
	break; //done
    }

  if(success && partDir == 0)
    {
      double spacing = _coreRows[0]->getSpacing();
      bestSplitPt = rint(bestSplitPt/spacing)*spacing;
      if(bestSplitPt <= _bbox.xMin || bestSplitPt >= _bbox.xMax)
	{
	  success = false;
	  bestSplitPt = rint((_bbox.xMin + _bbox.xMax)/2);
	}
    }
  else if(success && partDir == 1)
    {
      unsigned r;
      for(r = 0; r < getNumRows()-1; r++)
	{
	  double y0 = _coreRows[r]->getYCoord();
	  double y1 = _coreRows[r+1]->getYCoord();
	  if(bestSplitPt >= y0 && bestSplitPt <= y1)
	    {
	      if((bestSplitPt-y0) <= (y1 - bestSplitPt))
		bestSplitPt = y0;
	      else
		bestSplitPt = y1;
	      break;
	    }
	}
      if(r ==  (getNumRows()-1))
	{
	  if(bestSplitPt >= _coreRows[r]->getYCoord() && 
	     bestSplitPt <= _bbox.yMax)
	    bestSplitPt = _coreRows[r]->getYCoord();
	}
      if(bestSplitPt <= _bbox.yMin || bestSplitPt >= _bbox.yMax)
	{
	  success = false;
	  bestSplitPt = rint((_bbox.yMin + _bbox.yMax)/2);
	}
    }
  
  splitPt = bestSplitPt;
  splitDir = partDir;
  return(true);
}
