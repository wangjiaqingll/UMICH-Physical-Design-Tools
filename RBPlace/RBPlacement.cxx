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












// 97.8.8  Paul Tucker 
// 970820 pat  added x-coord sort to subRows in RBPlacement
// 970821 pat  initialize subrow spacing
// 970823 ilm  added orientations to RBPCoreRow and RBPIORow (Maogang's request)//                  they are initialized now
// 970902 ilm  RBPlacement::RBPlacement(const DB&) does not populate rows
//                  with cell Id numbers anymore. This is done with
//                  RBPlacement::populateRowsWith(const Placement&)
//                  (the code has to be exteneded to put IOPads intoIORows,
//                  and a lot more)
// 970917 pat  fix bug in RBCPlacement ctor for test creation
// 970918 pat  modify populateRowsWith to only put core cells in
//             core subrows, and use binary search.
//             improve operator<< for subRows.
// 970919 pat  more elaborate operator<< for RBCSubRowCol
// 970921 pat  upgraded constructors to split core subrows 
//             according to special net wiring;
//             added real RBCPlacement ctor
// 970923 pat  finish RBCPlacement; delete dead code; obey object interfaces
// 970930 pat  erase subRow index vectors at start of populateRowsWith,
//             so legalizer can move cells around
// 970930 pat  add findCoreRow and findSubRow search functions
// 971001 pat  moved comparison functors into .h file
// 971129 pat  comment out old diagnostic message about # of
//             cells not placed in core rows
// 980211 pat  add handling for missing cases to construction of
//             RBPSubRows based on special nets
// 982202 pat  corrected error of creating subRows
// 982202  dv  changed the interface of populateRowsWith, added two more
//             unsigned Data Members, added support for isPopulated.
// 980306  dv  removed orientedRBPlacement and added its functionality to
//             RBPlacement
// 980309 pat  more const declarations
// 980427 pat  added findSubRow as a coreRow method.
// 990527 aec  split RBPlacement into a base class + derived class
//			base class does not use DB
// 990609 s.m  added spaceCellsWithPinDimensionsAlg1()
// 990610 s.m  added spaceCellsWithPinDimensionsAlg1()
// 000803 ilm  fixed savePlacement() to save cell names    

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <ABKCommon/sgi_hash_map.h>
#include "ABKCommon/abkcommon.h"
#include "RBPlacement.h"
#include "Placement/transf.h"

#ifdef _MSC_VER
#ifndef rint
#define rint(a) floor((a)+0.5)
#endif
#endif

void RBPlacement::splitRowByObstacle(RBPCoreRow& cr, const BBox& obstacle)
{
  //for(unsigned srIdx = 0; srIdx < cr.getNumSubRows(); srIdx++)
  /* Xiaojian 05/12/2003: srIdx could be -1,causing trouble at pointer+srIdx+1
     use int instead of unsigned ported by sadya*/
  /*by sadya to make the halo sites parametrizable. by default 0*/
  const int numHaloSitesFront = 0;
  const int numHaloSitesEnd = 0;

  for(int srIdx = 0; srIdx < int(cr.getNumSubRows()); srIdx++)
    {
      RBPSubRow& sr = cr._subRows[srIdx];
      double     srXStart = sr.getXStart();
      double     srXEnd   = sr.getXEnd();

      if(obstacle.xMin >= srXEnd || obstacle.xMax <= srXStart)
		continue;

      // There are four ways the obstacle can intersect the subrow in x-dir
      // |----------| 		Obstacle
      //    |---|     		SubRow
      //
      //      |---|     	Obstacle
      //   |----------| 	SubRow
      //
      // |---|          	Obstacle
      //   |----------| 	SubRow
      //
      //            |---| 	Obstacle
      //   |----------|   	SubRow


      if((obstacle.xMin <= srXStart) && (obstacle.xMax >= srXEnd))
      { // Case1 : delete the subrow

        cr._subRows.erase(cr._subRows.begin() + srIdx);
        --srIdx;
        break;
      }
      else if((obstacle.xMin <= srXStart) && 
              (obstacle.xMax >  srXStart) && 
	      (obstacle.xMax <  srXEnd))
      { // Case3: trim the front of the subrow.
		
	double   newXStart    = obstacle.xMax - srXStart;
	unsigned removedSites = 
	  static_cast<unsigned>(ceil(newXStart/sr.getSpacing())) 
	  + numHaloSitesEnd;
	newXStart = srXStart + removedSites*sr.getSpacing();

        sr._xStart = newXStart;
	sr.resetNumSites();
	if(sr.getNumSites() <= 1)
	{
	    cr._subRows.erase(cr._subRows.begin() + srIdx);
	    srIdx--;
	}
      }
      else if((obstacle.xMin >  srXStart) && 
              (obstacle.xMin <  srXEnd)   && 
              (obstacle.xMax >= srXEnd)   )
      {	// Case4: trim the end of the subrow

        double newLength = (obstacle.xMin-srXStart)-
	  numHaloSitesFront*sr.getSiteWidth();
	if(newLength <= sr.getSiteWidth())
	{
	    cr._subRows.erase(cr._subRows.begin() + srIdx);
	    srIdx--;
	}
	else
	{ 
	    newLength -= sr.getSiteWidth();
	    unsigned numSites = 
			static_cast<unsigned>(floor(newLength / sr.getSpacing()));
	    double newXEnd = sr.getXStart() + numSites*sr.getSpacing() +
				sr.getSiteWidth();
            sr._xEnd = newXEnd;
	    sr.resetNumSites();
	    abkassert(sr.getNumSites() == numSites+1, "#sites mismatch");
	    if(sr.getNumSites() == 0)
	    {
		cr._subRows.erase(cr._subRows.begin() + srIdx);
		srIdx--;
	    }
	}
      }
      else if((obstacle.xMin > srXStart) && (obstacle.xMax < srXEnd))
      { 	// Case2: split the subrow in two
	      	// The new subRow will be the second one

        RBPSubRow newSR(sr);
	double newXStart = obstacle.xMax - srXStart;
	unsigned removedSites = 
	  static_cast<unsigned>(ceil(newXStart/sr.getSpacing())+numHaloSitesEnd);
	newXStart = srXStart + removedSites*sr.getSpacing();
        newSR._xStart = newXStart;
	newSR.resetNumSites();

	double newLength = (obstacle.xMin-srXStart)
	  - numHaloSitesFront*sr.getSiteWidth();
	newLength -= sr.getSiteWidth();
	if(newLength > 0)
	{
	    unsigned numSites = 
			static_cast<unsigned>(floor(newLength / sr.getSpacing()));
	    double newXEnd = srXStart + numSites*sr.getSpacing() +
				sr.getSiteWidth();
            sr._xEnd = newXEnd;
	    sr.resetNumSites();
	    abkassert(sr.getNumSites() == numSites+1, "#sites mismatch");
	}
	else
	    sr._numSites = 0;

	if(sr.getNumSites() <= 1)
	{
	    cr._subRows.erase(cr._subRows.begin() + srIdx);
	    srIdx--;
	}
	if(newSR.getNumSites() > 1)
	  cr._subRows.insert(cr._subRows.begin() + (srIdx +1), newSR); 
	/* Xiaojian: (srIdx+1) ported by sadya*/
      }
    }
}


void RBPlacement::resetPlacementWOri(const PlacementWOrient& pl)
{
    _placement    = pl;
    setCellsInRows();
    populateCC();
}

void RBPlacement::resetPlacement(const Placement& pl)
{
    _placement    = PlacementWOrient(pl, vector<Orient>(pl.size()));
    setCellsInRows();
    populateCC();
}

void RBPlacement::updatePlacementWOri(const PlacementWOrient& pl)
{ _placement = pl; }

void RBPlacement::setCellsInRows()
  // Puts all the cellIds onto their appropriate subRows.  
{
  _populated = true;
  _cellsNotInRows = 0;

  //clear all rows
  std::fill(_isInSubRow.begin(), _isInSubRow.end(), false);

  unsigned i;
  for(i=0; i<_coreRows.size(); i++)
    for(unsigned j=0; j<_coreRows[i].getNumSubRows(); j++)
    {
      RBPSubRow& sRow = _coreRows[i][j];
      sRow.clearAllCells();
    } 

  for(i=0; i<getNumCells(); i++)
  {
    if(_isCoreCell[i] && !_isFixed[i])
    {
      RBRowPtrs rowPtrs = findBothRows(_placement[i]);

      if(rowPtrs.first != NULL && rowPtrs.second != NULL)
      {
//        const_cast<RBPSubRow*>(rowPtrs.second)->addCellToEnd(i);
          const_cast<RBPSubRow*>(rowPtrs.second)->addCellToEnd(i);
	  _isInSubRow[i] = true;
      }	
      else
        {
          //cout<<"Cell "<<_hgWDims->getNodeByIdx(i).getName()<<" not placed "<<_placement[i]<<endl;
          _cellsNotInRows++;
        }
    }
  }
  if (_cellsNotInRows) 
    cout<<" RBPlacement:"<<_cellsNotInRows<<" core cell(s) not in rows"<<endl;

  sortSubRowsByX();
}

void RBPlacement::populateCC ()
{
    for(unsigned crIdx = 0; crIdx < _coreRows.size(); ++crIdx)
    {
    	const RBPCoreRow& cr = _coreRows[crIdx];
    	for(unsigned srIdx = 0; srIdx < cr.getNumSubRows(); ++srIdx)
    	{
      	    const RBPSubRow& sr = cr[srIdx];
      	    for(unsigned cOffset = 0; cOffset < sr.getNumCells(); cOffset++)
	      _cellCoords[sr[cOffset]] = RBCellCoord(crIdx, srIdx, cOffset);
    	}
    }
}

void RBPlacement::setLocation(unsigned id, const Point& pt)
  //puts cell <id> at location <pt>. updates the row structures
{
    if(_isInSubRow[id])	//need to remove the cell from the sub-row it was in
    {
	RBCellCoord& coord = _cellCoords[id];
	RBPSubRow& sr = _coreRows[coord.coreRowId][coord.subRowId];
	//sr.removeCell(id, coord);
	sr.removeCell(id);
	_isInSubRow[id] = false;
    }

    _placement[id] = pt;
    RBRowPtrs newRows = findBothRows(pt);

    abkassert(newRows.first != NULL, "Invalid new location in setLocation");

    if(newRows.first != NULL)
    {
	_cellCoords[id].coreRowId =newRows.first - &(*coreRowsBegin());
	_cellCoords[id].subRowId  =newRows.second -
		        &(*newRows.first->subRowsBegin());
	RBPSubRow* newSR = const_cast<RBPSubRow*>(newRows.second);
	
	bool found = false;
	for(unsigned idx = 0; idx < newSR->getNumCells(); idx++)
	  {
	    if(_placement[(*newSR)[idx]].x >= pt.x) //insert here
	      {
		_cellCoords[id].cellOffset = idx;
		newSR->addCell(id, _cellCoords[id]);
		for(unsigned nextIdx = idx+1; nextIdx < newSR->getNumCells(); ++nextIdx)
		  {
		    _cellCoords[(*newSR)[nextIdx]].cellOffset = nextIdx;
		  }
		found = true;
		break;
	      }
	  }
	if(!found) //needs to be added at the end
	  {
	    _cellCoords[id].cellOffset = newSR->getNumCells();
	    newSR->addCell(id, _cellCoords[id]);
	  }
	_isInSubRow[id] = true;
    }
}

void RBPlacement::sortSubRowsByX ()
{
  unsigned i,j;
  for(i=0; i<_coreRows.size(); i++) 
    for(j=0; j < _coreRows[i].getNumSubRows(); j++)
      _coreRows[i][j].sortCellsByLoc();
}

bool RBPlacement::isConsistent()
{
    bool looksOK = true;

    for(unsigned crId = 0; crId != _coreRows.size(); crId++)
    {
	RBPCoreRow& cr = _coreRows[crId];
	if(cr.getNumSubRows() == 0)
	{
	    looksOK = false;
	    cerr<<"CR: "<<crId<<endl;
	    cerr<<"CoreRow does not have any SubRows"<<endl;
	    continue;
	}

    	double prevXEnd = cr[0].getXStart();
    	for(unsigned srId = 0; srId != cr.getNumSubRows(); srId++)
        {
	    RBPSubRow& sr = cr[srId];
            if(sr._coreRow != &cr)
      	    {
		looksOK = false;
		cerr<<"CR: "<<crId<<" SR: "<<srId<<endl;
	    	cerr<<"SubRow does not point to the correct row"<<endl;
		if(sr._coreRow == NULL)
	    	    cerr<<"SubRow has NULL coreRow pointer"<<endl;
      	    }
      	    if(sr.getOrientation() != cr.getOrientation())
      	    {
        	looksOK = false;
        	cerr<<"Subrow orientation does not match its coreRow's"<<endl;
            }
            if(sr.getXStart() < prevXEnd)
            {
             	looksOK = false;
          	cerr<<"Mis-sorted x starts: "<<sr.getXStart()<<" "
			<<prevXEnd<< endl;
            }
	    if(sr.getNumSites() <= 0)
	    {
		looksOK = false;
		cerr<<"Empty SubRow"<<endl;
		cerr<<sr<<endl;
		cerr<<"Site width "<<sr.getSite().width<<endl;
	    }
	
	    for(unsigned cOffset = 0; cOffset != sr.getNumCells(); cOffset++)
	    {
		unsigned cellId = sr[cOffset];
		if(!_isInSubRow[cellId])
		{
		    looksOK = false;
		    cerr<<"subRow contains a cell marked as not in a SR"<<endl;
		}	
	 	else
		{
		    const RBCellCoord& cc = _cellCoords[cellId];
		    if( (cc.coreRowId != crId) ||
			(cc.subRowId  != srId) ||
			(cc.cellOffset != cOffset))
		    {
			looksOK = false;
			cerr<<"CellCoord does not match cells actual location";
			cerr<<endl;
		    }
		    if(_placement[cellId].y != sr.getYCoord())
		    {
			looksOK = false;
			cerr<<"Cell in subRow with mismatched y"<<endl;
		    }
		    if(_placement[cellId].x < sr.getXStart() ||
		       _placement[cellId].x > sr.getXEnd())
		    {
			looksOK = false;
			cerr<<"Cell in subRow with mismatched x"<<endl;
		    }
		}
	    }
   	}
    }

    return looksOK;
}

RBRowPtrs RBPlacement::findBothRows (const Point& point)
//   We assume that the RBPlacement contains _coreRows sorted by
//   y coordinate, and subrows sorted by x.  Return pointers
//   to the corerow and subrow that should contain the argument point.
{
  const RBPCoreRow* coreRowIdx = findCoreRow(point);

  if(coreRowIdx != NULL)
  {
    itRBPSubRow sr;
    for(sr = coreRowIdx->subRowsBegin(); sr != coreRowIdx->subRowsEnd(); sr++)
    {
      if(point.x >= sr->getXStart() && point.x <= sr->getXEnd())
      	return 
	  RBRowPtrs(std::pair<const RBPCoreRow*,const RBPSubRow*>
			   (coreRowIdx, &(*sr)));
    }

    return RBRowPtrs(coreRowIdx, static_cast<const RBPSubRow *>(NULL));
  }
  else
    return RBRowPtrs(static_cast<const RBPCoreRow *>(NULL),
                      static_cast<const RBPSubRow *>(NULL));
}

const RBPSubRow* RBPlacement::findSubRow (const Point& point) const
//   Return a pointer to the subrow containing <point>
{
  const RBPCoreRow* coreRowIdx = findCoreRow(point);

  if(coreRowIdx == NULL) return NULL;

  itRBPSubRow sr;
  for(sr = coreRowIdx->subRowsBegin(); sr != coreRowIdx->subRowsEnd(); sr++)
  {
    if(point.x >= sr->getXStart() && point.x <= sr->getXEnd())
	return &(*sr);
  }

  return NULL;	
}

const RBPCoreRow* RBPlacement::findCoreRow (const Point& point) const
  // Return a pointer to the _coreRow that should 
  // contain the argument point.
{
    RBPCoreRow cr;
    pair<const RBPCoreRow*, const RBPCoreRow*> coreRowIdx;
  
    cr._y = point.y;
    coreRowIdx = std::equal_range(&(*_coreRows.begin()), &(*_coreRows.end()), 
				cr, compareCoreRowY());

    if (coreRowIdx.first < coreRowIdx.second)
    	return coreRowIdx.first;
    else
    	return NULL;
}


void RBPlacement::extractCellsFromSR (RBPSubRow&        sr, 
				      vector<unsigned>& cellIds,
                                      vector<double>&   offsets,
                                      double beginOffset,  double endOffset)
{
    cellIds.erase(cellIds.begin(), cellIds.end());
    offsets.erase(offsets.begin(), offsets.end());
    cellIds.reserve(sr.getNumCells());
    offsets.reserve(sr.getNumCells());

    std::vector<unsigned>::iterator NULLit
	      = static_cast<vector<unsigned>::iterator>(NULL);
    std::vector<unsigned>::iterator eraseBegin = NULLit;
    std::vector<unsigned>::iterator eraseEnd   = NULLit;

    double epsilon = 0.01;

    beginOffset = max(0.0, beginOffset);
    endOffset   = min(sr.getLength() + epsilon, endOffset);

    double rangeBegin = sr.getXStart() + beginOffset;
    double rangeEnd   = sr.getXStart() + endOffset;

    for(unsigned i = 0; i < sr.getNumCells(); i++)
    {
	unsigned cellId = sr._cellIds[i];
	abkassert(_isInSubRow[cellId], "cell not marked as being in a sr");
        double  placedLoc = _placement[cellId].x;

	if(placedLoc < rangeBegin)
	{
	    abkassert(eraseBegin == NULLit, 
			"cell left of range after start found");
	    continue;
	}
	else if(placedLoc >= rangeEnd)
	{
	    if(eraseBegin == NULLit)	//all are right of the range
		return;
	    else if(eraseEnd == NULLit) //first one right of the range
		eraseEnd = sr._cellIds.begin() + i;

	    break;
	}
	else	//in the range..remove it
	{
	  if(eraseBegin == NULLit) eraseBegin = sr._cellIds.begin() + i;
	  abkassert(eraseEnd == NULLit,"sorting order of CellIds is incorrect");

	  cellIds.push_back(cellId);
	  offsets.push_back(placedLoc-sr.getXStart());

	  _cellsNotInRows++;
	  _isInSubRow[cellId] = false;
	  //maintain _CellCoords??
	}
    }
    if(eraseBegin == NULLit) return;
    if(eraseEnd   == NULLit) eraseEnd = sr._cellIds.end();

    sr._cellIds.erase(eraseBegin, eraseEnd);
}

void RBPlacement::extractCellsFromSR(
		 	RBPSubRow& 	  sr, 
			vector<unsigned>& cellIds,
                        vector<unsigned>& siteOffsets,
			unsigned beginSite, unsigned endSite)
{
    vector<double> offsets;
    extractCellsFromSR(sr, cellIds, offsets, beginSite*sr.getSpacing(),
						endSite*sr.getSpacing());
  
    siteOffsets.erase(siteOffsets.begin(), siteOffsets.end());
    siteOffsets.insert(siteOffsets.begin(), offsets.size(), 0);
    for(unsigned i = 0; i < offsets.size(); i++)
	siteOffsets[i] = static_cast<unsigned>(rint(offsets[i]/sr.getSpacing()));
}

void RBPlacement::extractCellsFromSR (RBPSubRow&        sr, 
				      vector<unsigned>& cellIds,
                                      double beginOffset,  double endOffset)
{
    cellIds.erase(cellIds.begin(), cellIds.end());
    cellIds.reserve(sr.getNumCells());

    std::vector<unsigned>::iterator NULLit
	      = static_cast<vector<unsigned>::iterator>(NULL);
    std::vector<unsigned>::iterator eraseBegin = NULLit;
    std::vector<unsigned>::iterator eraseEnd   = NULLit;

    double epsilon = 0.01;

    beginOffset = max(0.0, beginOffset);
    endOffset   = min(sr.getLength() + epsilon, endOffset);

    double rangeBegin = sr.getXStart() + beginOffset;
    double rangeEnd   = sr.getXStart() + endOffset;

    for(unsigned i = 0; i < sr.getNumCells(); i++)
    {
	unsigned cellId = sr._cellIds[i];
	abkassert(_isInSubRow[cellId], "cell not marked as being in a sr");
        double  placedLoc = _placement[cellId].x;

	if(placedLoc < rangeBegin)
	{
	    abkfatal(eraseBegin == NULLit, 
			"cell left of range after start found");
	    continue;
	}
	else if(placedLoc >= rangeEnd)
	{
	    if(eraseBegin == NULLit)	//all are right of the range
		return;
	    else if(eraseEnd == NULLit) //first one right of the range
		eraseEnd = sr._cellIds.begin() + i;

	    break;
	}
	else	//in the range..remove it
	{
	  if(eraseBegin == NULLit) eraseBegin = sr._cellIds.begin() + i;
	  abkassert(eraseEnd == NULLit, "sorting order of CellIds is incorrect");

	  cellIds.push_back(cellId);

	  _cellsNotInRows++;
	  _isInSubRow[cellId] = false;
	  //maintain _CellCoords??
	}
    }
    if(eraseBegin == NULLit) return;
    if(eraseEnd   == NULLit) eraseEnd = sr._cellIds.end();

    sr._cellIds.erase(eraseBegin, eraseEnd);
}


void RBPlacement::embedCellsInSR  (RBPSubRow& 	sr, 
				   const vector<unsigned>& cellIds,
                                   const vector<double>& spacing)
{
    abkfatal(spacing.size() == cellIds.size(), 
	"cellIds and spacing do not have the same number of entries");

    unsigned i = 0;
    for(unsigned newCell = 0; newCell < cellIds.size(); newCell++)
    {
	unsigned cellId      = cellIds[newCell];
	double newCellOffset = spacing[newCell];
		//find where cellId goes...

	for( ; i < sr._cellIds.size(); i++)
	  if(_placement[sr._cellIds[i]].x-sr.getXStart() > newCellOffset)
	 	break; 

	sr._cellIds.insert(sr._cellIds.begin()+i, cellId);

	double  loc = sr.getXStart() + newCellOffset;
	_placement[cellId].x = loc;
	_placement[cellId].y = sr.getYCoord();

	_cellsNotInRows--;
	_isInSubRow[cellId] = true;
	//maintain _CellCoords??
    }
}

void RBPlacement::embedCellsInSR   (RBPSubRow& 	sr, 
				    const vector<unsigned>& cellIds,
                                    const vector<unsigned>& siteSpacing)
{
    abkfatal(siteSpacing.size() == cellIds.size(), 
	"cellIds and spacing do not have the same number of entries");

    unsigned i = 0;
    for(unsigned newCell = 0; newCell < cellIds.size(); newCell++)
    {
	unsigned cellId = sr._cellIds[newCell];
	double newCellOffset = siteSpacing[newCell]*sr.getSpacing();
		//find where cellId goes...

	for( ; i < sr._cellIds.size(); i++)
	  if(_placement[sr._cellIds[i]].x-sr.getXStart() > newCellOffset)
	 	break; 

	sr._cellIds.insert(sr._cellIds.begin()+i, cellId);

	double  loc = sr.getXStart() + newCellOffset;
	_placement[cellId].x = loc;
	_placement[cellId].y = sr.getYCoord();

	_cellsNotInRows--;
	_isInSubRow[cellId] = true;
	//maintain _CellCoords??
    }
}

void RBPlacement::embedCellsInSR   (RBPSubRow& 	sr, 
				    const vector<unsigned>& cellIds,
                                    unsigned beginSite, unsigned endSite)
{
    if(cellIds.size() == 0) return;

    sr._cellIds.reserve(sr.getNumCells() + cellIds.size());

    beginSite = max((unsigned)0, beginSite);
    endSite   = min(sr.getNumSites(), endSite);

    double   rangeBegin = sr.getXStart() + sr.getSpacing()*beginSite;
    double   rangeEnd   = sr.getXStart() + sr.getSpacing()*endSite;

    cout<<"embedding range ["<<rangeBegin<<","<<rangeEnd<<")"<<endl;

    double totalCellWidth = 0;
    unsigned i;
    for(i = 0; i < cellIds.size(); i++)
    {
	double cellWidth = _hgWDims->getNodeWidth(cellIds[i]);
	totalCellWidth += sr.getCoreRow().siteRoundUp(cellWidth);
    }

    double spaceBetween = ((rangeEnd-rangeBegin)-totalCellWidth)/cellIds.size();

    cout<<"Total Site Width:      "<<rangeEnd-rangeBegin<<endl;
    cout<<"Total Cell Width:      "<<totalCellWidth<<endl;
    cout<<" ->Space Between Each: "<<spaceBetween<<endl<<endl;


    double leftEdge = rangeBegin;
    double cumExtraSpace = 0;	//extra space is accumulated as cells are
				//placed. when the extra space is > k*sitewidth,
				//k sites are skipped (for k any non-zero
				//integer)
    unsigned insPoint = 0;

    for(i = 0; i < cellIds.size(); i++)
    {
        unsigned newCellId = cellIds[i];

	for( ; insPoint < sr._cellIds.size(); insPoint++)
    	{
	  if(_placement[sr._cellIds[insPoint]].x > leftEdge)
		break;
	}

	sr._cellIds.insert(sr._cellIds.begin()+insPoint, newCellId);
	_placement[newCellId].x = leftEdge;
	_placement[newCellId].y = sr.getYCoord();

	_cellsNotInRows--;
	_isInSubRow[newCellId] = true;
	//maintain _CellCoords??

	double cellWidth = _hgWDims->getNodeWidth(newCellId);
	leftEdge += sr.getCoreRow().siteRoundUp(cellWidth);

	cumExtraSpace += spaceBetween;
	
	int numSitesOverlap = 
		static_cast<unsigned>(floor(cumExtraSpace/sr.getSpacing()));
	leftEdge += sr.getSpacing() * numSitesOverlap;
	cumExtraSpace -= sr.getSpacing() * numSitesOverlap;
    }
}

void RBPlacement::permuteCellsInSR(
			RBPSubRow& sr, 
			unsigned firstCellOffset,
                        const vector<unsigned>& cellPermutation)
{
abkassert(sr.getNumCells()-firstCellOffset >= cellPermutation.size(),
        "permuteCellsInSR has too many cells in the new ordering");

     std::vector<unsigned>::iterator permBegin = sr._cellIds.begin()+firstCellOffset;
     unsigned numToPermute = cellPermutation.size();

     double deltaSpace = sr.getSiteWidth(); //meaningless distance,
					//as a spacing function will be 
					//called after permuting

     double leftEdge = _placement[*permBegin].x;

     vector<unsigned> newCellOrder(numToPermute);
     unsigned k;
     for(k=0; k<numToPermute; k++)
        newCellOrder[k]=*(permBegin+cellPermutation[k]);

     for(k=0; k<numToPermute; k++)
     {
        unsigned newCellId = newCellOrder[k];
        *(permBegin+k) = newCellId;
        _placement[newCellId].x = leftEdge;

        leftEdge += deltaSpace;
     }

    switch(_params.spaceCellsAlg)
    {
        case RBPlacement::Parameters::EQUAL_SPACE:
            spaceCellsEqually(sr, firstCellOffset, numToPermute);
            break;
        case RBPlacement::Parameters::WITH_PIN_ALG1:
            spaceCellsWithPinDimensionsAlg1(sr, firstCellOffset, numToPermute);
            break;
        case RBPlacement::Parameters::WITH_PIN_ALG2:
            spaceCellsWithPinDimensionsAlg2(sr, firstCellOffset, numToPermute);
            break;
	default:
	    abkfatal(0,"unknown spacecells value");
    }
}

ostream& operator<<(ostream& out, const RBPlacement& arg)
{
  unsigned i,j;
  unsigned cellCount = 0;
  
  out << "RBPlacement object\n";
  out << static_cast<const Placement>(static_cast<const PlacementWOrient>(arg));
  out << "  " << arg._coreRows.size() << " core rows: ";
  for (i=0; i<arg._coreRows.size(); i++)
  {
    out << endl << "   " << arg._coreRows[i];
    for (j=0; j<arg._coreRows[i].getNumSubRows(); j++)
      cellCount += arg._coreRows[i][j].getNumCells();
  }
  out << endl;
  out << "   containing " << cellCount << " cell Id(s)" << endl;
  out << endl;
  return out;
}

RBPlacement::Parameters::Parameters(int argc, const char *argv[])
  :HGraphParameters(argc,argv),
   verb(argc,argv), numRowsToRemove(0), spaceCellsAlg(EQUAL_SPACE),
   remCongestion(0)
{
   UnsignedParam numRowsToRemove_("numRowsToRemove", argc, argv);
   StringParam spaceCellsAlg_("spaceCellsAlg", argc, argv);
   BoolParam remCongestion_("remCongestion", argc, argv);

   if(remCongestion_.found())
     remCongestion = 1;

   if(numRowsToRemove_.found())
	numRowsToRemove = numRowsToRemove_; 

   if(spaceCellsAlg_.found())
   {
      if(strcasecmp(spaceCellsAlg_,"EQUAL") == 0)
         spaceCellsAlg = EQUAL_SPACE;
      else if(strcasecmp(spaceCellsAlg_,"PIN1") == 0)
         spaceCellsAlg = WITH_PIN_ALG1;
      else if(strcasecmp(spaceCellsAlg_,"PIN2") == 0)
         spaceCellsAlg = WITH_PIN_ALG2;
      else
          abkwarn(0,"Unrecognize spaceCellsAlg! Using default value");
   }
}

ostream& operator<< (ostream& out, const RBPlaceParams& params)
{
    out<<" RBPlacement Parameters"<<endl;
    out<<"  Verbosity:         "<<params.verb<<endl;
    out<<"  NumRowsToRemove:   "<<params.numRowsToRemove<<endl;
    out<<"  SpaceCellsAlg:     ";
    switch(params.spaceCellsAlg)
    {
        case RBPlacement::Parameters::EQUAL_SPACE:
            out<<"Equal Spacing\n"; break;
        case RBPlacement::Parameters::WITH_PIN_ALG1:
            out<<"With Pin 1st algorithm\n"; break;
        case RBPlacement::Parameters::WITH_PIN_ALG2:
            out<<"With Pin 2nd algorithm\n"; break;
	default: out<<"Unknown Spacing Alg\n"; break;
    }
    out<<"  Remove Congestion: "<<params.remCongestion<<endl;
    out<< static_cast<HGraphParameters>(params) <<endl;
    return out;
}

void RBPlacement::savePlacement (const char* plFileName)   const
{
   ofstream out(plFileName); 
   
   out << "UCLA pl 1.0"<<endl;
   out <<TimeStamp() << User() << Platform() << endl << endl;

   for (unsigned i=0; i<_placement.getSize(); i++)
   {
        const char * namePtr=_hgWDims -> getNodeByIdx(i). getName();

        out<<setw(8) << (namePtr ? namePtr : "name unavailable")
	   <<"  "<<setw(10)<<static_cast <long long int> (_placement[i].x)
	   <<"  "<<setw(10)<<static_cast<long long int>(_placement[i].y)<<" : "
	   <<_placement.getOrient(i)<<endl;
   }

   out.close();          
}

void RBPlacement::savePlacementUnShredHW(const char* plFileName)   const
{
  ofstream out(plFileName); 
  
  out << "UCLA pl 1.0"<<endl;
  out <<TimeStamp() << User() << Platform() << endl << endl;

  unsigned i=0;
  for(i=0; i<_hgWDims->getNumNodes(); i++)
    {
      if (_placement[i].x > 1e308 || _placement[i].x > 1e308) continue;

      HGFNode& node   = _hgWDims->getNodeByIdx(i);
      const char* nodeName  = node.getName();
      char* actualName = new char[strlen(nodeName)+10];
      strcpy(actualName, nodeName);
      char* pos = strchr(actualName, '@');
      char* pos1 = strchr(actualName, '#');


      if(pos != NULL && pos1 != NULL)  //found
	{
	  *pos = '\0';
	  char *subNodeName = new char[strlen(nodeName)+10];
	  
	  double avgYLoc = 0;
	  double avgXLoc = 0;
	  unsigned numSubNodes = 0;
	  unsigned subVertIndex=0;
	  unsigned subHorizIndex=0;
	  char* indices = new char[strlen(nodeName)+10];

	  double nodeHeight = 0;
	  double nodeWidth = 0;
	  //Orientation nodeOrient = _placement.getOrient(i);

	  vector< vector<unsigned> > subNodes;
	  vector<unsigned> horizSubNodes;
	  unsigned lastVertIdx=0;
	  unsigned currVertIdx=0;

	  while(i<_hgWDims->getNumNodes())
	    {
	      HGFNode& subNode   = _hgWDims->getNodeByIdx(i);
	      nodeName = subNode.getName();
	      strcpy(subNodeName, nodeName);
	      pos = strchr(subNodeName, '@');
	      if(pos == NULL)
		{
		  subNodes.push_back(horizSubNodes);
		  horizSubNodes.clear();
		  break;
		}
	      *pos = '\0';
	      if(strcmp(actualName, subNodeName) != 0)
		{
		  subNodes.push_back(horizSubNodes);
		  horizSubNodes.clear();
		  break;
		}

	      strcpy(subNodeName, nodeName);
	      pos = strchr(subNodeName, '@');
	      *pos = '\0';
	      strcpy(indices, pos+1);
	      pos = strchr(indices, '#');
	      *pos = '\0';
	      subVertIndex = atoi(indices);
	      subHorizIndex = atoi(pos+1);
	      
	      currVertIdx = subVertIndex;
	      if(currVertIdx == lastVertIdx)
		horizSubNodes.push_back(i);
	      else
		{
		  subNodes.push_back(horizSubNodes);
		  horizSubNodes.clear();
		  horizSubNodes.push_back(i);
		  lastVertIdx = currVertIdx;
		}

	      if(subHorizIndex == 0)
		nodeHeight += _hgWDims->getNodeHeight(i);
	      if(subVertIndex == 0)
		nodeWidth += _hgWDims->getNodeWidth(i);

	      avgYLoc += (_placement[i].y + _hgWDims->getNodeHeight(i)/2);
	      avgXLoc += (_placement[i].x + _hgWDims->getNodeWidth(i)/2);
	      numSubNodes++;
	      i++;
	    }
	  if(horizSubNodes.size()!=0)
	    {
	      subNodes.push_back(horizSubNodes);
	      horizSubNodes.clear();
	    }
	  i--;
	  avgXLoc /= numSubNodes;
	  avgYLoc /= numSubNodes;

	  vector<int> orientScore(9,0);
	  unsigned k;
	  for(k=0; k<subNodes.size()-1; ++k)
	    {
	      for(unsigned l=0; l<subNodes[k].size()-1; ++l)
		{
		  clusterNodeOrient orient = 
		    getOrientSubNode(subNodes[k][l], 
				     subNodes[k][l+1], subNodes[k+1][l]);
		  switch(orient)
		    {
		    case N:
		      ++orientScore[N];
		      break;
		    case FN:
		      ++orientScore[FN];
		      break;
		    case S:
		      ++orientScore[S];
		      break;
		    case FS:
		      ++orientScore[FS];
		      break;
		    case W:
		      ++orientScore[W];
		      break;
		    case FW:
		      ++orientScore[FW];
		      break;
		    case E:
		      ++orientScore[E];
		      break;
		    case FE:
		      ++orientScore[FE];
		      break;
		    case I:
		      ++orientScore[I];
		      break;
		    default:
		      cout<<"Error in computing orientation "<<orient<<endl;
		    };
		}
	    }
	  
	  cout<<actualName<<endl;
	  for(k=0;k<9;++k)
	    cout<<setw(3)<<toChar(clusterNodeOrient(k));
	  cout<<endl;
	  
	  int maxScore=-1;
	  int maxScoreOrient=0;
	  for(unsigned k=0;k<8;++k)
	    {
	      if(orientScore[k] > maxScore)
		{
		  maxScore = orientScore[k];
		  maxScoreOrient = k;
		}
	      cout<<orientScore[k]<<" ";
	    }

	  //maxScoreOrient = N;

	  double nodeYLoc;
	  double nodeXLoc;
	  if(maxScoreOrient == N || maxScoreOrient == S || 
	     maxScoreOrient == FN || maxScoreOrient == FS)
	    {
	      nodeXLoc = avgXLoc - nodeWidth/2;
	      nodeYLoc = avgYLoc - nodeHeight/2;
	    }
	  else
	    {
	      nodeXLoc = avgXLoc - nodeHeight/2;
	      nodeYLoc = avgYLoc - nodeWidth/2;
	    }

	  out<<actualName<<"  ";
	  out<< setw(10) << nodeXLoc << "  ";
	  out<< setw(10) << nodeYLoc << " :  ";

	  cout<<orientScore[8];
	  cout<<endl;
	  out<<toChar(clusterNodeOrient(maxScoreOrient))<<endl;
	  //out<<" N"<<endl;
	}
      else
	{
	  out << _hgWDims->getNodeByIdx(i).getName() <<"  "
		<< setw(10) << _placement[i].x << "  "
		<< setw(10) << _placement[i].y << " : "
		<< _placement.getOrient(i)<<endl;
	}
    }
    out.close();
}

RBPlacement::~RBPlacement()
{
  if(_hgWDims) delete _hgWDims;
  unsigned k;
  for(k=0; k!=allFileNamesInAUX.size(); k++) free(allFileNamesInAUX[k]);
}


void RBPlacement::permuteCellsAndWSInSR(
			RBPSubRow& sr, 
			unsigned firstCellOffset,
                        const vector<unsigned>& cellPermutation)
{

     std::vector<unsigned>::iterator permBegin = sr._cellIds.begin()+firstCellOffset;
     unsigned numToPermute = cellPermutation.size();
     vector<unsigned> movables;
     vector<unsigned> WSpositions;
     vector<double> whiteSpaceWidths;
     unsigned totalMovables=0;
     unsigned nodeCtr=firstCellOffset;
     unsigned totalCells = sr.getNumCells();

     int numSitesWS=0;
     int sitesPerWS=0;
     unsigned i;
     for(i=firstCellOffset;i<firstCellOffset+numToPermute;++i)
       {
	 if(nodeCtr==totalCells-1)   //last cell of the subRow
	   {
	     movables.push_back(*(sr.cellIdsBegin()+nodeCtr));
	     ++totalMovables;
	     ++nodeCtr;
	     if(totalMovables == numToPermute)
	       break;	     

	     //if whitespace present after last cell
	     double currWSWidth=sr.getXEnd() - 
	       _placement[*(sr.cellIdsBegin()+nodeCtr-1)].x-
	       _hgWDims->getNodeWidth(*(sr.cellIdsBegin()+nodeCtr-1));
	     
	     if(currWSWidth >= 2*sr.getSpacing())
	       {
		 numSitesWS = int(currWSWidth/sr.getSpacing());

		 movables.push_back(UINT_MAX);
		 sitesPerWS = int(floor(numSitesWS/2+0.5));
		 numSitesWS -= sitesPerWS;
		 whiteSpaceWidths.push_back(sitesPerWS*sr.getSpacing());
		 WSpositions.push_back(totalMovables);
		 ++totalMovables;
		 if(totalMovables == numToPermute)
		   break;

		 movables.push_back(UINT_MAX);
		 sitesPerWS = numSitesWS;
		 numSitesWS -= sitesPerWS;
		 whiteSpaceWidths.push_back(sitesPerWS*sr.getSpacing());
		 WSpositions.push_back(totalMovables);
		 ++totalMovables;
		 if(totalMovables == numToPermute)
		   break;
	       }
	     else
	       {
		 movables.push_back(UINT_MAX);
		 whiteSpaceWidths.push_back(currWSWidth);
		 WSpositions.push_back(totalMovables);
		 ++totalMovables;
		 if(totalMovables == numToPermute)
		   break;		
	       }
	   }
	 else
	   {
	     //if whitespace found
	     if(_placement[*(sr.cellIdsBegin()+nodeCtr)].x+
		_hgWDims->getNodeWidth(*(sr.cellIdsBegin()+nodeCtr)) <
		_placement[*(sr.cellIdsBegin()+nodeCtr+1)].x)
	       {
		 movables.push_back(*(sr.cellIdsBegin()+nodeCtr));
		 ++totalMovables;
		 ++nodeCtr;
		 if(totalMovables == numToPermute)
		   break;

		 //better white space handling
		 double currWSWidth=_placement[*(sr.cellIdsBegin()+nodeCtr)].x-
		   _placement[*(sr.cellIdsBegin()+nodeCtr-1)].x-
		   _hgWDims->getNodeWidth(*(sr.cellIdsBegin()+nodeCtr-1));

		 
		 if(currWSWidth >= 3*sr.getSpacing())
		   {
		     numSitesWS = int(currWSWidth/sr.getSpacing());

		     movables.push_back(UINT_MAX);
		     sitesPerWS = int(floor(numSitesWS/3+0.5));
		     numSitesWS -= sitesPerWS;
		     whiteSpaceWidths.push_back(sitesPerWS*sr.getSpacing());
		     WSpositions.push_back(totalMovables);
		     ++totalMovables;
		     if(totalMovables == numToPermute)
		       break;

		     movables.push_back(UINT_MAX);
		     sitesPerWS = int(floor(numSitesWS/2+0.5));
		     numSitesWS -= sitesPerWS;
		     whiteSpaceWidths.push_back(sitesPerWS*sr.getSpacing());
		     WSpositions.push_back(totalMovables);
		     ++totalMovables;
		     if(totalMovables == numToPermute)
		       break;

		     movables.push_back(UINT_MAX);
		     sitesPerWS = numSitesWS;
		     numSitesWS -= sitesPerWS;
		     whiteSpaceWidths.push_back(sitesPerWS*sr.getSpacing());
		     WSpositions.push_back(totalMovables);
		     ++totalMovables;
		     if(totalMovables == numToPermute)
		       break;
		   }
		 else if(currWSWidth < 3*sr.getSpacing() && 
			 currWSWidth >=2*sr.getSpacing())
		 
		 //if(currWSWidth >=2*sr.getSpacing())
		   {
		     movables.push_back(UINT_MAX);
		     whiteSpaceWidths.push_back(currWSWidth/2);
		     WSpositions.push_back(totalMovables);
		     ++totalMovables;
		     if(totalMovables == numToPermute)
		       break;
		     movables.push_back(UINT_MAX);
		     whiteSpaceWidths.push_back(currWSWidth/2);
		     WSpositions.push_back(totalMovables);
		     ++totalMovables;
		     if(totalMovables == numToPermute)
		       break;
		   }
		 else
		 
		   {
		     movables.push_back(UINT_MAX);
		     whiteSpaceWidths.push_back(currWSWidth);
		     WSpositions.push_back(totalMovables);
		     ++totalMovables;
		     if(totalMovables == numToPermute)
		       break;
		   }
	       }
	     else
	       {
		 movables.push_back(*(sr.cellIdsBegin()+nodeCtr));
		 ++totalMovables;
		 ++nodeCtr;
		 if(totalMovables == numToPermute)
		   break;

	       }
	   }
       }

    abkassert(movables.size() == cellPermutation.size(),
      "permuteCellsInSR has too many cells in the new ordering");

     for(i=0;i<WSpositions.size();++i)
       {
	 for(unsigned j=0;j<numToPermute;++j)
	   {
	     if(WSpositions[i]==cellPermutation[j])
	       {
		 WSpositions[i]=j;
		 break;
	       }
	   }
       }

     double leftEdge=_placement[movables[0]].x;
     nodeCtr=0;
     for(unsigned i=0;i<numToPermute;++i)
       {
	 if(movables[cellPermutation[i]] != UINT_MAX)
	   {
	     _placement[movables[cellPermutation[i]]].x=leftEdge;
	     leftEdge+=_hgWDims->getNodeWidth(movables[cellPermutation[i]]);
	     *(permBegin+nodeCtr) = movables[cellPermutation[i]];
	     ++nodeCtr;
	   }
	 else
	   {
	     for(unsigned int j=0;j<WSpositions.size();++j)
	       {
		 if(i==WSpositions[j])
		   { 
		     leftEdge+=whiteSpaceWidths[j];
		     break;
		   }
	       }
	   }
       }
}

//added by sadya to update placement of ironed cells

void RBPlacement::updateIronedCells(const vector<unsigned>& movables,
				    const Placement& soln,
				    RBPSubRow& subrow1,
				    RBPSubRow& subrow2)
{
  vector<unsigned> row1Ids;
  vector<unsigned> row2Ids;
  double yCoord1=subrow1.getYCoord();
  double yCoord2=subrow2.getYCoord();

  unsigned i;
  for(i=0;i<movables.size();++i)
    {
      if(movables[i] != UINT_MAX)
	{
	  if(_placement[movables[i]].y == yCoord1)
	    {
	      subrow1.removeCell(movables[i]);
	      _isInSubRow[movables[i]] = false;
	    }
	  else if(_placement[movables[i]].y == yCoord2)
	    {
	      subrow2.removeCell(movables[i]);
	      _isInSubRow[movables[i]] = false;
	    }
	  else
	    abkfatal(0,"invalid small placement problem\n");
	}
    }
  for(i = 0; i < movables.size(); i++)
    {
      if(movables[i] != UINT_MAX)
	{
	  _placement[movables[i]] = Point(soln[i].x, soln[i].y);

	  if(soln[i].y == yCoord1)
	    {
	      subrow1.addCell(movables[i]);
	      _isInSubRow[movables[i]] = true;
	    }
	  else if(soln[i].y == yCoord2)
	    {
	      subrow2.addCell(movables[i]);
	      _isInSubRow[movables[i]] = true;
	    }
	  else
	    abkfatal(0,"invalid small placement solution\n");
	} 
    }
}



//added by sadya to update placement of ironed cells with clustering

void RBPlacement::updateIronedCellsLR(const vector< vector<unsigned> >& movables,
				    const Placement& soln,
				    RBPSubRow& subrow1,
				    RBPSubRow& subrow2)
{
  vector<unsigned> row1Ids;
  vector<unsigned> row2Ids;
  double yCoord1=subrow1.getYCoord();
  double yCoord2=subrow2.getYCoord();

  unsigned i;
  for(i=0;i<movables.size();++i)
    {
      if(movables[i][0] != UINT_MAX)
	{
	  for(unsigned k=0;k<movables[i].size();++k)
	    {
	      if(_placement[movables[i][k]].y == yCoord1)
		{
		  subrow1.removeCell(movables[i][k]);
		  _isInSubRow[movables[i][k]] = false;
		}
	      else if(_placement[movables[i][k]].y == yCoord2)
		{
		  subrow2.removeCell(movables[i][k]);
		  _isInSubRow[movables[i][k]] = false;
		}
	      else
		abkfatal(0,"invalid small placement problem\n");
	    }
	}
    }
  for(i = 0; i < movables.size(); i++)
    {
      if(movables[i][0] != UINT_MAX)
	{
	  double leftEdge = soln[i].x;
	  for(unsigned k=0;k<movables[i].size();++k)
	    {
	      _placement[movables[i][k]] = Point(leftEdge, soln[i].y);
	      leftEdge += (*_hgWDims).getNodeWidth(movables[i][k]);

	      if(soln[i].y == yCoord1)
		{
		  subrow1.addCell(movables[i][k]);
		  _isInSubRow[movables[i][k]] = true;
		}
	      else if(soln[i].y == yCoord2)
		{
		  subrow2.addCell(movables[i][k]);
		  _isInSubRow[movables[i][k]] = true;
		}
	      else
		abkfatal(0,"invalid small placement solution\n");
	    } 
	}
    }
}


//added by sadya to update placement of ironed cells with clustering R->L

void RBPlacement::updateIronedCellsRL(const vector< vector<unsigned> >& movables,
				    const Placement& soln,
				    RBPSubRow& subrow1,
				    RBPSubRow& subrow2)
{
  vector<unsigned> row1Ids;
  vector<unsigned> row2Ids;
  double yCoord1=subrow1.getYCoord();
  double yCoord2=subrow2.getYCoord();

  unsigned i;
  for(i=0;i<movables.size();++i)
    {
      if(movables[i][0] != UINT_MAX)
	{
	  for(unsigned k=0;k<movables[i].size();++k)
	    {
	      if(_placement[movables[i][k]].y == yCoord1)
		{
		  subrow1.removeCell(movables[i][k]);
		  _isInSubRow[movables[i][k]] = false;
		}
	      else if(_placement[movables[i][k]].y == yCoord2)
		{
		  subrow2.removeCell(movables[i][k]);
		  _isInSubRow[movables[i][k]] = false;
		}
	      else
		abkfatal(0,"invalid small placement problem\n");
	    }
	}
    }


  for(i = 0; i < movables.size(); i++)
    {
      if(movables[i][0] != UINT_MAX)
	{
	  double leftEdge = soln[i].x;
	  for(unsigned k=0 ; k < movables[i].size();++k)
	    {
	      _placement[movables[i][k]] = Point(leftEdge, soln[i].y);

	      leftEdge += (*_hgWDims).getNodeWidth(movables[i][k]);

	      if(soln[i].y == yCoord1)
		{
		  subrow1.addCell(movables[i][k]);
		  _isInSubRow[movables[i][k]] = true;
		}
	      else if(soln[i].y == yCoord2)
		{
		  subrow2.addCell(movables[i][k]);
		  _isInSubRow[movables[i][k]] = true;
		}
	      else
		abkfatal(0,"invalid small placement solution\n");
	    } 
	}
    }
}

//added by sadya to get the BBox of the core layout
void RBPlacement::initBBox(void)
{
  _coreBBox.clear();
  _fullBBox.clear();
  itRBPCoreRow itc;
  for(itc = coreRowsBegin(); itc != coreRowsEnd(); ++itc)
    {
      _coreBBox.add(itc->getXStart(), itc->getYCoord());
      _coreBBox.add(itc->getXEnd(), itc->getYCoord()+itc->getHeight());
      _fullBBox.add(itc->getXStart(), itc->getYCoord());
      _fullBBox.add(itc->getXEnd(), itc->getYCoord()+itc->getHeight());
    }

  //add to bbox all the fixed cells
  for(unsigned i=0; i<_hgWDims->getNumNodes(); i++)
    {
      if(isFixed(i))
	{
	  _fullBBox.add(_placement[i].x, _placement[i].y);
	  _fullBBox.add(_placement[i].x + _hgWDims->getNodeWidth(i),
			_placement[i].y + _hgWDims->getNodeHeight(i));
  
	}
    }
}

//added by sadya to get the BBox of the core layout
BBox RBPlacement::getBBox(bool addTerminals)
{
  BBox bbox;
  if(addTerminals)
    bbox=_fullBBox;
  else
    bbox=_coreBBox;
  return bbox;
}

clusterNodeOrient RBPlacement::getOrientSubNode(unsigned index, 
						unsigned indexRight, 
						unsigned indexTop) const
{
  clusterNodeOrient orient=I;
  Point node(_placement[index]);
  Point nodeRight(_placement[indexRight]);
  Point nodeAbove(_placement[indexTop]);

  Point V2(nodeRight.x-node.x, nodeRight.y-node.y);
  Point V1(nodeAbove.x-node.x, nodeAbove.y-node.y);

  int quad1 = getQuad(V1);
  int quad2 = getQuad(V2);

  //cout<<"V1: "<<V1<<" quad1: "<<quad1<<"V2: "<<V2<<" quad2: "<<quad2<<endl;

  switch(quad2)
    {
    case 0:
      switch(quad1)
	{
	case 0:
	case 2:
	  break;
	case 1:
	  orient = N;
	  break;
	case 3:
	  orient = FS;
	  break;
	};
      break;

    case 1:
      switch(quad1)
	{
	case 1:
	case 3:
	  break;
	case 0:
	  orient = FW;
	  break;
	case 2:
	  orient = W;
	  break;
	};
      break;

    case 2:
      switch(quad1)
	{
	case 2:
	case 0:
	  break;
	case 1:
	  orient = FN;
	  break;
	case 3:
	  orient = S;
	  break;
	};      
      break;

    case 3:
      switch(quad1)
	{
	case 3:
	case 1:
	  break;
	case 0:
	  orient = E;
	  break;
	case 2:
	  orient = FE;
	  break;
	};
      break;
    };

  //cout<<"quad1: "<<quad1<<" quad2: "<<quad2<<" orient "<<toChar(orient)<<endl;
  /*
  if(_placement[index].x < _placement[indexRight].x &&
     _placement[index].y <= _placement[indexTop].y)
    orient = N;
  else if(_placement[index].x > _placement[indexRight].x &&
	  _placement[index].y <= _placement[indexTop].y)
    orient = FN;
  else if(_placement[index].x <= _placement[indexRight].x &&
	  _placement[index].y > _placement[indexTop].y)
    orient = S;
  else if(_placement[index].x >= _placement[indexRight].x &&
	  _placement[index].y > _placement[indexTop].y)
    orient = FS;
  else if(_placement[index].x >= _placement[indexTop].x &&
	  _placement[index].y <= _placement[indexRight].y)
    orient = W;
  else if(_placement[index].x <= _placement[indexTop].x &&
	  _placement[index].y <= _placement[indexRight].y)
    orient = FW;
  else if(_placement[index].x <= _placement[indexTop].x &&
	  _placement[index].y >= _placement[indexRight].y)
    orient = E;
  else if(_placement[index].x >= _placement[indexTop].x &&
	  _placement[index].y >= _placement[indexRight].y)
    orient = FE;
  else
    cout<<"ERROR"<<endl;
  */
  //orient = N;

  return orient;
}

int RBPlacement::getQuad(Point& pt) const
{
  if(pt.x > pt.y)
    {
      if(pt.x > -pt.y)
	return 0;
      else
	return 3;
    }
  else
    {
      if(pt.x > -pt.y)
	return 1;
      else
	return 2;
    }
}

char * RBPlacement::toChar(clusterNodeOrient orient) const
{
  switch(orient)
    {
    case N:
      return("N");
      break;
    case FN:
      return("FN");
      break;
    case S:
      return("S");
      break;
    case FS:
      return("FS");
      break;
    case W:
      return("W");
      break;
    case FW:
      return("FW");
      break;
    case E:
      return("E");
      break;
    case FE:
      return("FE");
      break;
    default:
      return("I");
      break;
    };
  cout<<orient<<" ERROR"<<endl;
  return("N");
}

void RBPlacement::removeSitesBelowMacros(void)
{
  double nodeWidth, nodeHeight, rowHeight;
  double macroArea = 0;
  for(unsigned i=0; i<_hgWDims->getNumNodes(); i++)
    {
      //const HGFNode& node = _hgWDims->getNodeByIdx(i);
      Orient nodeOrient = _placement.getOrient(i);
      if(nodeOrient == Orient("N") || nodeOrient == Orient("S") || 
	 nodeOrient == Orient("FN") || nodeOrient == Orient("FS"))
	{
	  nodeWidth = _hgWDims->getNodeWidth(i);
	  nodeHeight = _hgWDims->getNodeHeight(i);
	}
      else
	{
	  nodeWidth = _hgWDims->getNodeHeight(i);
	  nodeHeight = _hgWDims->getNodeWidth(i);
	}

      BBox obstacle;
      obstacle.add(_placement[i].x, _placement[i].y);
      obstacle.add(_placement[i].x+nodeWidth, _placement[i].y+nodeHeight);

      RBRowPtrs rowPtrs = findBothRows(_placement[i]);

      if(rowPtrs.first != NULL && rowPtrs.second != NULL)
	rowHeight = (rowPtrs.first)->getHeight();
      else
	rowHeight = _coreRows[0].getHeight();

      if(/*nodeHeight > rowHeight &&*/_hgWDims->isTerminal(i))
	{
	  //if(nodeHeight > rowHeight)
	  macroArea += nodeWidth*nodeHeight;
	  for(unsigned i=0; i<_coreRows.size(); i++)
	    {
	      RBPCoreRow& cr = _coreRows[i];
	      double coreRowYMin = cr.getYCoord();
	      double coreRowYMax = cr.getYCoord() + cr.getHeight();
	      
	      if((obstacle.yMin >= coreRowYMax) || 
		 (obstacle.yMax <= coreRowYMin))  continue;
	      
	      splitRowByObstacle(cr, obstacle);
	    }
	}
    }
  //cout<<"Macro Area : "<<macroArea<<endl;
  //cout<<"New Sites Area : "<<getSitesArea()<<endl;
}

unsigned RBPlacement::getNumMacros(void)
{
  double nodeHeight, rowHeight = _coreRows[0].getHeight();
  unsigned numMacros = 0;
  for(unsigned i=0; i<_hgWDims->getNumNodes(); i++)
    {
      //const HGFNode& node = _hgWDims->getNodeByIdx(i);
      Orient nodeOrient = _placement.getOrient(i);
      if(nodeOrient == Orient("N") || nodeOrient == Orient("S") || 
	 nodeOrient == Orient("FN") || nodeOrient == Orient("FS"))
	nodeHeight = _hgWDims->getNodeHeight(i);
      else
	nodeHeight = _hgWDims->getNodeWidth(i);
      
      if(nodeHeight > rowHeight)
	++numMacros;
    }
  return numMacros;
}

void RBPlacement::updatePlacement(unsigned id, const Point& pt)
{
  _placement[id] = pt;
}

// added by MRG to update placement of arbitrary list of cells
void RBPlacement::updateCells(const vector<unsigned>& movables, 
			      const Placement& soln)
{
  abkassert(movables.size() == soln.size(), "misuse of update cells, # of movables doesn't equal # of points");

  for(unsigned i=0;i<movables.size();++i)
    {
      setLocation(movables[i],soln[i]);
    }
}

// added by MRG
// finds the current cellcoord at the point or makes one
unsigned RBPlacement::findCellIdx (const Point& point)
{
  unsigned coreidx,subidx,i;
  for(i=0;i<_coreRows.size();i++)
    {
      if (_coreRows[i].getYCoord()==point.y) {coreidx=i; break;}
    }
  abkassert(point.y==_coreRows[coreidx].getYCoord(),"Point isn't in this row");
  for(i=0;i<_coreRows[coreidx].getNumSubRows();i++)
    {
      if (point.x>=_coreRows[coreidx][i].getXStart()&&
	  point.x<_coreRows[coreidx][i].getXEnd()) 
	{ subidx=i; break; }
    }
  abkassert(point.x>=_coreRows[coreidx][subidx].getXStart()&&
	    point.x<_coreRows[coreidx][subidx].getXEnd(),
	    "Point isn't in this subrow");
  for(i=0;i<_coreRows[coreidx][subidx].getNumCells();i++)
    {
      // if there's a cell, return the cellcoord
      if (point.x==_placement[_coreRows[coreidx][subidx][i]].x) 
	return (_coreRows[coreidx][subidx][i]);
      else if (point.x<_placement[_coreRows[coreidx][subidx][i]].x) 
	break;
    }
  return(UINT_MAX);
}

bool RBPlacement::checkCC()
{
  bool isOK=true;
  for(unsigned i=0; i<_hgWDims->getNumNodes(); i++)
    {
      if(isCoreCell(i))
	{
	  if(!_isInSubRow[i])
	    {
	      cout<<endl<<"Cell not in valid row"<<endl;
	      cout<<_hgWDims->getNodeByIdx(i).getName();
	      cout<<_placement[i]<<endl;
	      isOK = false;
	    }
	}
    }
  return(isOK);
}

void RBPlacement::getCellsInBBox(BBox& bbox, vector<unsigned>& cellIds)
{
  Point bottomLeft(bbox.xMin, bbox.yMin);
  Point topRight(bbox.xMax, bbox.yMax);

  const RBPCoreRow* bottomCR = findCoreRow(bottomLeft);
  const RBPCoreRow* topCR = findCoreRow(topRight);
  
  if(bottomCR == NULL || topCR == NULL)
    return;

  unsigned crIdx;
  for(crIdx=0; crIdx<_coreRows.size(); crIdx++)
    {
      const RBPCoreRow& cr = _coreRows[crIdx];
      if(bottomCR == &cr)
	break;
    }
  if(crIdx == _coreRows.size())
    return;

  while(crIdx < _coreRows.size())
    {
      const RBPCoreRow& cr = _coreRows[crIdx];

      if(cr.getYCoord() >= topRight.y)
	break;

      for(unsigned srIdx = 0; srIdx < cr.getNumSubRows(); ++srIdx)
    	{
	  const RBPSubRow& sr = cr[srIdx];
	  double srXStart = sr.getXStart();
	  double srXEnd = sr.getXEnd();
	  if(srXStart < bottomLeft.x && srXEnd <= bottomLeft.x)
	    continue;
	  else if(srXStart >= topRight.x)
	    continue;

	  for(unsigned cOffset = 0; cOffset < sr.getNumCells(); cOffset++)
	    {
	      unsigned cellId = sr[cOffset];
	      Point pl = _placement[cellId];
	      double width = _hgWDims->getNodeWidth(cellId);
	      if(pl.x >= bottomLeft.x && pl.x <= topRight.x)
		cellIds.push_back(cellId);
	      else if(pl.x <= bottomLeft.x && (pl.x+width) >= bottomLeft.x)
		cellIds.push_back(cellId);
	      if(pl.x >= topRight.x)
		break;
	    }
    	}
      if(topCR == &cr)
	break;
      ++crIdx;
    }
}

//To Do
void RBPlacement::assignPinsToCells()
{
  itHGFEdgeLocal edge;
  for(unsigned i=0; i<_hgWDims->getNumNodes(); i++)
    {
      const HGFNode& node = _hgWDims->getNodeByIdx(i);
      double nodeHeight = _hgWDims->getNodeHeight(i);
      double nodeWidth = _hgWDims->getNodeWidth(i);

      unsigned N = node.getDegree();
      unsigned numPinsTB = unsigned(ceil(N*nodeWidth/(2*(nodeWidth+nodeHeight))));
      unsigned numPinsLR = unsigned(ceil(N*nodeHeight/(2*(nodeWidth+nodeHeight))));
      //double xGridSize = nodeWidth/numPinsTB;
      //double yGridSize = nodeHeight/numPinsLR;
      vector<bool> left(numPinsLR,0);
      vector<bool> right(numPinsLR,0);
      vector<bool> top(numPinsTB,0);
      vector<bool> bottom(numPinsTB,0);
      
      unsigned e=0; //edge offset
      for(edge = node.edgesBegin(); edge != node.edgesEnd(); edge++, e++)
	{
	  //Point newPinOffset(nodeWidth/2, nodeHeight/2); //default to center
	  Point optLoc(0,0);
	  unsigned edgeIdx = (*edge)->getIndex();
	  unsigned nodeOffset = 0;
	  for(itHGFNodeLocal currNode = (*edge)->nodesBegin(); 
	      currNode != (*edge)->nodesEnd(); ++currNode, ++nodeOffset)
	    {
	      unsigned currNodeIdx = (*currNode)->getIndex();
	      Point currNodePOffset = _hgWDims->nodesOnEdgePinOffset
		(nodeOffset,edgeIdx,_placement.getOrient(currNodeIdx));
	      Point currNodePlacement = 
		_placement[currNodeIdx] + currNodePOffset;
	      optLoc += currNodePlacement;
	    }
	  
	  Point pinLoc = (_placement[i] + _hgWDims->edgesOnNodePinOffset
			  (e,i,_placement.getOrient(i)));  

	  optLoc -= pinLoc;
	  optLoc.x /= (nodeOffset-1);
	  optLoc.y /= (nodeOffset-1);
	  
	  Point newPinLoc;

	  if(optLoc.x <= _placement[i].x)
	    newPinLoc.x = _placement[i].x;
	  else if(optLoc.x >= _placement[i].x + nodeWidth)
	    newPinLoc.x = _placement[i].x + nodeWidth;

	  if(optLoc.y <= _placement[i].y)
	    newPinLoc.y = _placement[i].y;
	  else if(optLoc.y >= _placement[i].y + nodeHeight)
	    newPinLoc.y = _placement[i].y + nodeHeight;

	  cout<<optLoc.x<<" "<<optLoc.y<<endl;
	}
    }
}

double RBPlacement::getSitesArea()
{
  double sitesArea=0;
  for(unsigned i=0; i<_coreRows.size(); i++)
    {
      for(unsigned j=0; j<_coreRows[i].getNumSubRows(); j++)
	{
	  RBPSubRow& sRow = _coreRows[i][j];
	  sitesArea += sRow.getNumSites()*sRow.getHeight()*sRow.getSiteWidth();
	} 
    }
  return (sitesArea);
}

void RBPlacement::alignCellsToRows()
{
  for(unsigned i=0; i<getNumCells(); i++)
    {
      if(_isCoreCell[i] && !_isFixed[i])
	{
	  double cellWidth = _hgWDims->getNodeWidth(i);
	  double cellHeight = _hgWDims->getNodeHeight(i);
	  if(_placement[i].y < _coreBBox.yMin)
	    _placement[i].y = _coreBBox.yMin;
	  if(_placement[i].y > (_coreBBox.yMax-cellHeight))
	    _placement[i].y = _coreBBox.yMax-cellHeight;
	  if(_placement[i].x < _coreBBox.xMin)
	    _placement[i].x = _coreBBox.xMin;
	  if(_placement[i].x > (_coreBBox.xMax-cellWidth))
	    _placement[i].x = _coreBBox.xMax-cellWidth;

	  RBPCoreRow cr;
	  const RBPCoreRow* lowerBound;

	  cr._y = _placement[i].y;
	  /*
	  lowerBound = std::lower_bound(&(*_coreRows.begin()),
				       &(*_coreRows.end()), cr, 
				       compareCoreRowY());
	  */
	  pair<const RBPCoreRow*, const RBPCoreRow*> coreRowIdx;
	  coreRowIdx = std::equal_range(&(*_coreRows.begin()), 
					&(*_coreRows.end()),cr, 
					compareCoreRowY());
	  if (coreRowIdx.first < coreRowIdx.second)
	    lowerBound = coreRowIdx.first;
	  else
	    lowerBound = NULL;

	  if(lowerBound != NULL)
	    {
	      _placement[i].y = lowerBound->getYCoord();
	      
	      itRBPSubRow sr;
	      bool foundSR=false;
	      for(sr = lowerBound->subRowsBegin(); 
		  sr != lowerBound->subRowsEnd(); sr++)
		{
		  if(_placement[i].x >= sr->getXStart() && 
		     _placement[i].x <= sr->getXEnd())
		    {
		      foundSR=true;
		      break;
		    }
		}

	      if(!foundSR)//not found. find the closest sub-row and fix it
		{
		  int srId=-1;
		  int minSrId=-1;
		  bool beginEnd=false; //closest to beginning or end of subrow
		  double minDiff = DBL_MAX;
		  double diff;
		  for(sr = lowerBound->subRowsBegin(); 
		      sr != lowerBound->subRowsEnd(); sr++)
		    {
		      ++srId;
		      diff = fabs(_placement[i].x-sr->getXStart());
		      if(diff < minDiff)
			{
			  minSrId = srId;
			  beginEnd = 0;
			  minDiff = diff;
			}
		      diff = fabs(_placement[i].x-(sr->getXEnd()-cellWidth));
		      if(diff < minDiff)
			{
			  minSrId = srId;
			  beginEnd = 1;
			  minDiff = diff;
			}
		    }
		  if(minSrId != -1)
		    {
		      foundSR = true;
		      sr = lowerBound->subRowsBegin() + minSrId;
		      if(beginEnd == false)
			_placement[i].x = sr->getXStart();
		      else
			_placement[i].x = sr->getXEnd()-cellWidth;
		    }
		}

	      if(foundSR)
		{
		  if(_placement[i].x > (sr->getXEnd()-cellWidth))
		    _placement[i].x = sr->getXEnd()-cellWidth;
		  if(_placement[i].x < sr->getXStart())
		    _placement[i].x = sr->getXStart();
		}
	    }
	}
    }
  setCellsInRows();
  populateCC();
}

double RBPlacement::getContainedSiteAreaInBBox(const BBox & bbox)
{
  BBox layoutBBox = getBBox(false);
  double containedArea=0;
  BBox intersect = layoutBBox.intersect(bbox);
  if(intersect.getArea() == 0)
    return(0);

  itRBPCoreRow CR;
  itRBPSubRow SR;
  for(CR=coreRowsBegin();CR!=coreRowsEnd();++CR)
    {
      const RBPCoreRow& curRow      = *CR;
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

      //this allows for fractional sites in the region
      double numSites=0;
      double siteSpacing = curRow.getSpacing();
      double siteWidth = curRow.getSiteWidth();

      for(SR=(CR)->subRowsBegin();SR!=(CR)->subRowsEnd();++SR)
	{
          double xStart = SR->getXStart();
          double xEnd = SR->getXEnd();
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

void RBPlacement::initGroupRegionConstr(void)
{
  for(unsigned i=0; i<constraints.getSize(); ++i)
    {
      const Constraint & constraint = constraints[i];
      if(!constraint.getType().isRectRgnType())
	continue;
      Constraint * nonConstConstraint = const_cast<Constraint *> (&constraint);
      RectRegionConstraint* rectConstr = dynamic_cast<RectRegionConstraint*> (nonConstConstraint);
      if(rectConstr == NULL)
	{
	  cout<<"Not RectRegionConstraint. Actual Constraint Type"<<(const char *)constraint.getType()<<" "<<endl;
	  abkfatal(0,"Cannot convert type Constraint to type RectRegionConstraint\n")
	}
      BBox rgn = rectConstr->getBBox();

      const Mapping&  mapping = constraint.getMapping();
      vector<unsigned> groupIds;
      for(unsigned j=0; j<constraint.getSize(); ++j)
	{
	  unsigned idx = mapping[j];
	  groupIds.push_back(idx);
	}
      pair<BBox, vector<unsigned> > temp(rgn, groupIds);
      groupRegionConstr.push_back(temp);
      groupIds.clear();
    }

  //TO DO REMOVE THE FOLLOWING FAKE GROUP CONSTRUCT
  /*
  BBox layoutBBox = getBBox();
  BBox regionBBox;
  
  regionBBox.xMin = layoutBBox.xMin+layoutBBox.getWidth()/4;
  regionBBox.xMax = layoutBBox.xMax-layoutBBox.getWidth()/4;
  regionBBox.yMin = layoutBBox.yMin+layoutBBox.getHeight()/4;
  regionBBox.yMax = layoutBBox.yMax-layoutBBox.getHeight()/4;
  vector<unsigned> groupIds;
  for(unsigned i=0; i<rint(_hgWDims->getNumNodes()/10); i++)
    {
      unsigned ru = RandomUnsigned(_hgWDims->getNumTerminals(),
				   _hgWDims->getNumNodes());
      groupIds.push_back(ru);
    }
  pair<BBox, vector<unsigned> > temp(regionBBox, groupIds);
  groupRegionConstr.push_back(temp);
  */

  cellToGrpMapping.resize( _hgWDims->getNumNodes(), UINT_MAX);
  for(unsigned j=0; j<groupRegionConstr.size(); ++j)
    {
      for(unsigned k=0; k<groupRegionConstr[j].second.size(); ++k)
	cellToGrpMapping[groupRegionConstr[j].second[k]] = j;
    }
  
  for(unsigned j=0; j<groupRegionConstr.size(); ++j)
    {
      cout<<"Group Region Constraint "<<groupRegionConstr[j].first;
      for(unsigned k=0; k<groupRegionConstr[j].second.size(); ++k)
	{
	  //cout<<_hgWDims->getNodeByIdx(groupRegionConstr[j].second[k]).getName()<<"  ";
	}
      cout<<endl;
    }
}


void RBPlacement::initRegionUtilization(void)
{
  for(unsigned i=0; i<constraints.getSize(); ++i)
    {
      const Constraint & constraint = constraints[i];
      if(!constraint.getType().isUtilRectRgnType())
	continue;
      Constraint * nonConstConstraint = const_cast<Constraint *> (&constraint);
      UtilRectRegionConstraint* utilRectConstr = 
	dynamic_cast<UtilRectRegionConstraint*> (nonConstConstraint);
      if(utilRectConstr == NULL)
	{
	  cout<<"Not UtilRectRegionConstraint. Actual Constraint Type"<<(const char *)constraint.getType()<<" "<<endl;
	  abkfatal(0,"Cannot convert type Constraint to type UtilRectRegionConstraint\n")
	}
      BBox rgn = utilRectConstr->getBBox();
      double utilization = utilRectConstr->getUtilization();
      pair<BBox, double> temp(rgn, utilization);
      regionUtilization.push_back(temp);
    }
  

  /* TODO REMOVE THE FOLLOWING FAKE CONSTRAINT
  BBox layoutBBox = getBBox();
  BBox regionBBox;
    
  regionBBox.xMin = layoutBBox.xMin+layoutBBox.getWidth()/4;
  regionBBox.xMax = layoutBBox.xMax-layoutBBox.getWidth()/4;
  regionBBox.yMin = layoutBBox.yMin+layoutBBox.getHeight()/4;
  regionBBox.yMax = layoutBBox.yMax-layoutBBox.getHeight()/4;
  pair <BBox , double> rgn1(regionBBox, 0.0);
  regionUtilization.push_back(rgn1);

  regionBBox.xMin = layoutBBox.xMin;
  regionBBox.xMax = layoutBBox.xMin+layoutBBox.getWidth()/3;
  regionBBox.yMin = layoutBBox.yMin;
  regionBBox.yMax = layoutBBox.yMin+layoutBBox.getHeight()/3;  
  pair <BBox , double> rgn1(regionBBox, 0.05);
  regionUtilization.push_back(rgn1);

  regionBBox.xMin = layoutBBox.xMax-layoutBBox.getWidth()/3;
  regionBBox.xMax = layoutBBox.xMax;
  regionBBox.yMin = layoutBBox.yMax-layoutBBox.getHeight()/3;
  regionBBox.yMax = layoutBBox.yMax;
  pair <BBox , double> rgn2(regionBBox, 0.05);
  regionUtilization.push_back(rgn2);
  */

  for(unsigned i=0; i<regionUtilization.size(); ++i)
    {
      cout<<"Utilization Region Constraint "<<regionUtilization[i].first<<" : "<<regionUtilization[i].second<<endl;
    }
}

struct pair_less_mag 
//: public binary_function< pair<unsigned, double>, 
//		       pair<unsigned, double> , bool > 
{
  bool operator()(pair<unsigned, double> x, pair<unsigned, double> y) 
  { return (x.second < y.second) ; }
};

void RBPlacement::reshapePlacement(double tetherNewAR, double tetherNewWS, 
				   const char* newPlFileName)
{
  BBox layoutBBox = getBBox(false);  
  BBox newLayoutBBox = layoutBBox;
  double currWidth = layoutBBox.getWidth();
  double currHeight = layoutBBox.getHeight();
  double currAR = currWidth/currHeight;
  double layoutArea = getSitesArea();
  double totalNodesArea = _hgWDims->getNodesArea();
  if(tetherNewWS != -1)
    {
      double newLayoutArea = totalNodesArea/(1-0.01*tetherNewWS);
      double newLayoutBBoxArea = 
	layoutBBox.getArea()*newLayoutArea/layoutArea;
      double newWidth = sqrt(newLayoutBBoxArea*currAR);
      double newHeight = newWidth/currAR;
      newLayoutBBox.xMax = layoutBBox.xMin + newWidth;
      newLayoutBBox.yMax = layoutBBox.yMin + newHeight;
    }
  if(tetherNewAR != -1)
    {
      double newLayoutBBoxArea = newLayoutBBox.getArea();
      double newWidth = sqrt(newLayoutBBoxArea*tetherNewAR);
      double newHeight = newWidth/tetherNewAR;
      newLayoutBBox.xMax = newLayoutBBox.xMin + newWidth;
      newLayoutBBox.yMax = newLayoutBBox.yMin + newHeight;
    }

  if(tetherNewWS != -1 || tetherNewAR != -1)
    {
      BBoxToBBox reshapeBBox(layoutBBox, newLayoutBBox);
      for(unsigned i=0; i<_hgWDims->getNumNodes(); i++)
	{
	  if (_placement[i].x > 1e308 || _placement[i].x > 1e308) continue;
	  Point cellLoc = _placement[i];
	  Point newCellLoc = reshapeBBox.apply(cellLoc);
	  _placement[i] = newCellLoc;
	}
    }
  if(tetherNewAR != -1 && newPlFileName != NULL)
    {
      //have to parse the placement file here, as it may have
      //node names, which only the HGraph knows.
      PlacementWOrient newPlacement = PlacementWOrient(_hgWDims->getNumNodes(),
						       Point(DBL_MAX,DBL_MAX));
      
      ifstream plFile(newPlFileName);
      abkfatal2(plFile, " Could not open ",newPlFileName);
      cout << " Reading " << newPlFileName << " ... " << endl;
      
      char     nodeName[256];
      Point    pt;
      char     oriStr[10];
      int lineno = 0;

      plFile>>needcaseword("UCLA")>>needcaseword("pl")
	    >>needword("1.0")>>skiptoeol; 
      plFile>>eathash(lineno);
      
      while(!plFile.eof())
        {
	  plFile>>eathash(lineno)>>nodeName>>pt.x>>pt.y>>eatblank;
	  unsigned nodeId = _hgWDims->getNodeByName(nodeName).getIndex();
	  newPlacement[nodeId] = pt;

	  if(plFile.peek() == ':')
	    {
	      plFile>>needword(":")>>oriStr;
	      newPlacement.setOrient(nodeId, Orientation(oriStr));
	    }
	  else if(plFile.peek() == '/')
	    {
	      plFile.get();
	      plFile>>oriStr;
	      newPlacement.setOrient(nodeId, Orientation(oriStr));
	    }
	  plFile>>skiptoeol>>eathash(lineno);
	}
      plFile.close();

      //now that we have new terminal locations we can rotate each node
      //according to new terminal locations
      Point center = newLayoutBBox.getGeomCenter();
      vector< pair <unsigned, double> > initTermAngles;
      vector< double > newTermAngles;

      for(unsigned i=0; i<_hgWDims->getNumTerminals(); i++)
	{
	  Point cellLoc = _placement[i] - center;
	  double angle = atan2(cellLoc.y, cellLoc.x);
	  initTermAngles.push_back( pair <unsigned, double> (i,angle));
	  
	  cellLoc = newPlacement[i] - center;
	  angle = atan2(cellLoc.y, cellLoc.x);
	  newTermAngles.push_back(angle);

	}

      stable_sort(initTermAngles.begin(), initTermAngles.end(), pair_less_mag());
      for(unsigned i=0; i<initTermAngles.size(); ++i)
	{
	  //cout<<_hgWDims->getNodeByIdx(initTermAngles[i].first).getName()<<" "<<initTermAngles[i].second<<" "<<newTermAngles[initTermAngles[i].first]<<endl;
	}

      BBox centeredBBox = newLayoutBBox;
      centeredBBox.xMin -= center.x;
      centeredBBox.xMax -= center.x;
      centeredBBox.yMin -= center.y;
      centeredBBox.yMax -= center.y;
      double Q1 = atan2(centeredBBox.yMin, centeredBBox.xMax);
      double Q2 = atan2(centeredBBox.yMax, centeredBBox.xMax);
      double Q3 = atan2(centeredBBox.yMax, centeredBBox.xMin);
      double Q4 = atan2(centeredBBox.yMin, centeredBBox.xMin);

      for(unsigned i=0; i<_hgWDims->getNumNodes(); i++)
	{
	  if (_placement[i].x > 1e308 || _placement[i].x > 1e308) continue;
	  Point cellLoc = _placement[i] - center;
	  if(cellLoc.x == 0 && cellLoc.y == 0)
	    continue;

	  double initAngle = atan2(cellLoc.y, cellLoc.x);
	  
	  //now find between which terminals this point rests
	  unsigned j;
	  for(j=0; j<_hgWDims->getNumTerminals()-1; j++)
	    {
	      if(initTermAngles[j].second <= initAngle && 
		 initTermAngles[j+1].second >= initAngle)
		break;
	    }
	  unsigned term1 = j;
	  unsigned term2 = j+1;
	  if(j == _hgWDims->getNumTerminals()-1)
	    term2 = 0;
	  
	  double alpha1 = initTermAngles[term1].second;
	  double alpha2 = initTermAngles[term2].second;
	  double beta1 = newTermAngles[initTermAngles[term1].first];
	  double beta2 = newTermAngles[initTermAngles[term2].first];

	  double initDiff = alpha2 - alpha1;
	  double newDiff = beta2-beta1;
	  double angleDiff = initAngle - alpha1;

	  if(initDiff <= -Pi)
	    initDiff += 2*Pi;
	  if(newDiff <= -Pi)
	    newDiff += 2*Pi;
	  if(angleDiff <= -Pi)
	    angleDiff += 2*Pi;
	  
	  double newAngle = beta1 + 
	    (angleDiff/initDiff)*newDiff;
	  
	  if (newAngle > Pi) 
	    newAngle -= 2*Pi;
	  else if (newAngle <= -Pi) 
	    newAngle += 2*Pi;

	  double blkWidth = centeredBBox.getWidth();
	  double blkHeight = centeredBBox.getHeight();

	  double initR = sqrt(cellLoc.x*cellLoc.x + cellLoc.y*cellLoc.y);
	  
	  double length;

	  //cout<<_hgWDims->getNodeByIdx(i).getName()<<" "<<initAngle<<"  "<<newAngle<<endl;

	  if(Q1 <= initAngle && initAngle <= Q2)
	    length = (blkWidth/2.0)/cos(initAngle);
	  else if(Q2 <= initAngle && initAngle <= Q3)
	    length = (blkHeight/2.0)/cos(Pi/2.0 - initAngle);
	  else if(Q3 <= initAngle || initAngle <= Q4)
	    length = (blkWidth/2.0)/cos(Pi - initAngle);
	  else
	    length = (blkHeight/2.0)/cos(Pi/2.0 + initAngle);
	  
	  initR /= length;
	  
	  if(Q1 <= newAngle && newAngle <= Q2)
	    length = (blkWidth/2.0)/cos(newAngle);
	  else if(Q2 <= newAngle && newAngle <= Q3)
	    length = (blkHeight/2.0)/cos(Pi/2.0 - newAngle);
	  else if(Q3 <= newAngle || newAngle <= Q4)
	    length = (blkWidth/2.0)/cos(Pi - newAngle);
	  else
	    length = (blkHeight/2.0)/cos(Pi/2.0 + newAngle);

	  double newR = initR*length;

	  Point newCellLoc;
	  newCellLoc.x = newR*cos(newAngle);
	  newCellLoc.y = newR*sin(newAngle);

	  _placement[i] = newCellLoc + center;
	}
    }
}

void RBPlacement::markMacrosAsFixed()
{
  unsigned j;
  for(j=0; j<_hgWDims->getNumNodes(); j++)
    {
      if(_hgWDims->isNodeMacro(j))
	_isFixed[j] = true;
    }      
}

void RBPlacement::unmarkMacrosAsFixed()
{
  unsigned j;
  for(j=0; j<_hgWDims->getNumNodes(); j++)
    {
      if(_hgWDims->isNodeMacro(j))
	_isFixed[j] = false;
    }      
}
