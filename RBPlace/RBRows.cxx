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




// 990123   aec  ver2.0
//		split from RBPlacement.cxx
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <utility>
#include <algorithm>

using std::pair;
#include "ABKCommon/sgi_hash_map.h"
#include "ABKCommon/abkcommon.h"
#include "RBRows.h"

#if (defined(_MSC_VER) && !defined(rint))
#define rint(a) floor((a)+0.5)
#endif

RBPSubRow::RBPSubRow(double xStart, double xEnd, RBPCoreRow& coreRow)
    : _xStart(xStart), _xEnd(xEnd), _needsSorting(false), _coreRow(&coreRow)
{ 
    resetNumSites();
}

void RBPSubRow::resetNumSites()
{
    double length = _xEnd - _xStart;

    if(length < getSiteWidth())
    { _numSites = 0; return;}

    length -= getSiteWidth();
    _numSites = 1;

    double num = length / _coreRow->getSpacing();
    if(floor(num) != ceil(num))
    {
	cout<<"subRow length is not a multiple of site spacing"<<endl;
	cout<<*this<<endl;
	cout<<"SiteWidth: "<<getSite().width<<endl;
	abkwarn(floor(num) == ceil(num), 
		"subRow length is not a multiple of site spacing");
    }
    _numSites += static_cast<unsigned>(rint(num));
}
 
itRBPSubRow RBPCoreRow::findSubRow (const double x) const
  // find the subRow within this CoreRow containing the x-coord
{
    abkfatal(getNumSubRows() > 0, "Request to find subrow in empty corerow.");

    RBPSubRow target(x,x);
//  pair <const RBPSubRow*, const RBPSubRow* > subRowIdx;
    pair <itRBPSubRow, itRBPSubRow> subRowIdx;

    subRowIdx = std::equal_range(subRowsBegin(), subRowsEnd(), target);

    if(subRowIdx.first < subRowIdx.second)  //inside *subRowIdx.first
        return subRowIdx.first;  
    if(subRowIdx.first == subRowsEnd())     //to the right of all subrows
        return subRowIdx.first - 1; 
    if(subRowIdx.first == subRowIdx.second) //just before *subRowIdx.first
        return subRowIdx.first;
    else        
        abkfatal(false,"Logic error in program");
 
    return subRowIdx.first;  // should really be NULL
}

void RBPSubRow::removeCell(unsigned id)
{
    abkassert(!_needsSorting, "can't use removeCell on unsorted SubRows");

    std::vector<unsigned>::iterator i;
    bool brokeFromLoop=0;

    for(i = _cellIds.begin(); i != _cellIds.end(); i++)
      {
	if((*i) == id)   
	  {
	    _cellIds.erase(i);
	    brokeFromLoop=1;
	    break;
	  }
      }
    abkfatal(brokeFromLoop, "tried to remove a cell from a subrow it is not in");
}

void RBPSubRow::addCell(unsigned id)
{
    abkassert(!_needsSorting, "can't use addCell on unsorted SubRows");

    const Point& pt = (*_coreRow->_placement)[id];

    abkassert(_xStart <= pt.x && _xEnd >= pt.x,
	"can't add a cell to a row it isn't located inside (x-range problem)");

    abkassert(getYCoord() == pt.y,
	"can't add a cell to a row it isn't located inside (y-coord problem)");

    CompareX compFun(*_coreRow->_placement);

    std::vector<unsigned>::iterator i;
    for(i = _cellIds.begin(); i != _cellIds.end(); i++)
	if(!compFun(*i, id))   
	  break;
    _cellIds.insert(i, id);

}

void RBPSubRow::addCellToEnd(unsigned id)
{
    _cellIds.push_back(id); 
    _needsSorting = true; 
}

void RBPSubRow::sortCellsByLoc()
{ 
   std::sort(_cellIds.begin(), _cellIds.end(), CompareX(*_coreRow->_placement));
    _needsSorting = false;
}

void RBPSubRow::addCell(unsigned id, const RBCellCoord& coord)
{
    abkassert(_cellIds.size() < coord.cellOffset, 
	"subRow not long enough for coord in addCell");
    _cellIds.insert(_cellIds.begin()+coord.cellOffset, id); 
}

void RBPSubRow::removeCell(unsigned id, const RBCellCoord& coord)
{
    static_cast<void>(id);
    abkassert(_cellIds.size() < coord.cellOffset, 
	"subRow not long enough for coord in removeCell");
    abkassert(_cellIds[coord.cellOffset] == id, "mismatch ids in removeCell");
    _cellIds.erase(_cellIds.begin() + coord.cellOffset);
}

ostream& operator<< (ostream& out, const RBPCoreRow& arg)
{
  unsigned i, totalSites = 0;
  for(i=0; i<arg.getNumSubRows(); i++)
	totalSites += arg[i].getNumSites();

  out<<"RBPCoreRow y: " << arg.getYCoord()<<"(+"<<arg.getHeight()<<")"
     <<"  SubRows: "<<arg.getNumSubRows()<<" Total Sites: "<<totalSites<<endl;

  for(i=0; i<arg.getNumSubRows(); i++)
     out<<arg[i] ;

  return out;
}

void RBPCoreRow::save(ostream& os) const
{
    os<<"CoreRow Horizontal"<<endl;
    os<<" Coordinate   : "<<setw(10)<<_y<<endl;
    os<<" Height       : "<<setw(10)<<_site.height<<endl;
    os<<" Sitewidth    : "<<setw(10)<<_site.width<<endl;
    os<<" Sitespacing  : "<<setw(10)<<_spacing<<endl;
    os<<" Siteorient   : "<<setw(10)<<_orient<<endl;

    if(_site.symmetry.rot90 || _site.symmetry.y || _site.symmetry.x)
	os<<" Sitesymmetry : "<<setw(10)<<_site.symmetry<<endl;
    
    for(unsigned s = 0; s < _subRows.size(); s++)
    	os<<" SubrowOrigin : "<<setw(10) <<_subRows[s]._xStart
		<<" Numsites : "<<setw(10)<<_subRows[s]._numSites<<endl;

    os<<"End"<<endl;
}

ostream& operator<< (ostream& out, const RBPSubRow& arg)
{
  out<<"  RBPSubRow ("<<setw(7)<<arg.getXStart()<<" ->"
				<<setw(7)<<arg.getXEnd()<<") "
	 <<" cells: "<<setw(4)<<arg.getNumCells()
	 <<" sites: "<<setw(4)<<arg.getNumSites()
	 <<" spacing: "<<setw(4)<<arg.getSpacing()
	 <<"("<<setw(4)<<arg.getSiteWidth()<<")"<<endl;
  return out;
}
