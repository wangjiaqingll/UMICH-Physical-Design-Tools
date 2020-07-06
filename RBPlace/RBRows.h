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












// Created 970804 by Igor Markov and Paul Tucker, VLSI CAD ABKGROUP, UCLA/CS
// 970821 pat  added spacing to subRows
// 970823 ilm  multiple minor corrections (removing redundancies, adding refs)
// 970823 ilm  added orientations to RBPCoreRow and RBPIORow (Maogang's request)
//                  they are initialized now
// 970902 ilm  RBPlacement::RBPlacement(const DB&) does not populate rows
//                  with cell Id numbers anymore. This is done with
//                  RBPlacement::populateRowsWith(const Placement&)
//                  (the code has to be exteneded to put IOPads intoIORows,
//                  and a lot more)
// 970918 pat  add DB argument to populateRowsWith
// 970919 pat  added missing field to RBPSubRow copier
// 970930 pat  added findSubRow and findCoreRow signatures
// 971001 pat  moved comparison functors from *.cxx
// 971007 pat  added getSpacing to subRow
// 980222 pat  handle additional wierd cases in subrow construction
// 980222  dv  changed the interface of populateRowsWith, added two more
//             unsigned Data Members, added support for isPopulated.
// 980603  dv  removed orientedRBPlacement class
// 980309 pat  more const declarations; added siteRoundup method
// 980427 pat  added RBPCoreRow::findSubRow

//======================================================================
//                           Row-based Placement
//====================================================================== 

#ifndef _RBROWS_H_
#define _RBROWS_H_

#include "ABKCommon/abkcommon.h"
#include "Placement/placement.h"
#include "Placement/symOri.h"
#include "RBCellCoord.h"

//========================== CLASS INTERFACES =============================

class  RBPSubRow;
struct RBPSite;
class  RBPCoreRow;
 
typedef std::vector<unsigned>::const_iterator itRBPCellIds;

class RBPSubRow
{
    friend class RBPlacement;
    friend class RBPlaceFromDB;
    friend class RBCPlacement;
    friend class RBPCoreRow;

  vector<unsigned> 	_cellIds;  //ids of cells in this subRow (in x-order)
  double 		_xStart, _xEnd; 
  unsigned		_numSites;
  bool			_needsSorting;
  RBPCoreRow*     	_coreRow; //coreRow containing this subRow


  void     clearAllCells() {_cellIds.erase(_cellIds.begin(), _cellIds.end());}
  void     removeCell(unsigned id); //NOTE: uses the location of <id> to find it
  void	   addCell(unsigned id);    //NOTE: uses the location of <id> to 
					//put the cell in the right order
  void     addCell(unsigned id, const RBCellCoord& coord);
  void     removeCell(unsigned id, const RBCellCoord& coord);

  void     addCellToEnd(unsigned id); //append the cell id. they will not be
					//in the correct order within this row,
					//and should be sorted before use.

  void     sortCellsByLoc();
  void     resetNumSites();

 public:

  RBPSubRow() 
    : _xStart(0), _xEnd(0), _numSites(0), _needsSorting(false), _coreRow(NULL) 
    {}

  RBPSubRow(double xStart, double xEnd)
    : _xStart(xStart), _xEnd(xEnd), _numSites(0), 
      _needsSorting(false),_coreRow(NULL)
    {}

  RBPSubRow(double xStart, double xEnd, RBPCoreRow& coreRow);

  RBPSubRow(const RBPSubRow& orig)
    : _xStart(orig._xStart), _xEnd(orig._xEnd), _numSites(orig._numSites),
      _needsSorting(false), _coreRow(orig._coreRow)
    {abkassert(orig.getNumCells() == 0,"Can't copy constr. populated subRow");}

    double 	  getXStart()      const { return _xStart;}	
    double	  getXEnd()        const { return _xEnd;}	
    double        getLength()      const { return _xEnd - _xStart;}
    unsigned      getNumSites()	   const { return _numSites;}

    Orient        getOrientation() const;
    const RBPSite& getSite()        const;
    double        getSpacing()     const;
    double        getSiteWidth()   const;
    double        getYCoord()      const;
    double	  getHeight()      const;
    const RBPCoreRow& getCoreRow()     const { return *_coreRow;}

    unsigned	  getNumCells()    const {return _cellIds.size();}
    unsigned      operator[](unsigned i) const;
    itRBPCellIds  cellIdsBegin() const { return _cellIds.begin();}
    itRBPCellIds  cellIdsEnd()   const { return _cellIds.end();}

    bool   operator< (const RBPSubRow& sr2) const
    { return this->_xEnd <= sr2._xStart;}

    friend ostream& operator<<(ostream&, const RBPSubRow&);
};


struct RBPSite
{
    double     height;
    double     width;
    Symmetry   symmetry;

    RBPSite() : height(1), width(1) {}

    RBPSite(double ht, double wth, Symmetry sym)
	:height(ht), width(wth), symmetry(sym)
        {}
    ~RBPSite(){}
    RBPSite(const RBPSite& orig)
	: height(orig.height), width(orig.width), symmetry(orig.symmetry)
	{}
    RBPSite& operator=(const RBPSite& orig)
	{ height=orig.height; width=orig.width; symmetry=orig.symmetry;
	  return *this;
	}
    bool operator==(const RBPSite& s2) const
    {	
	return (height==s2.height && width==s2.width);
    }
};


typedef std::vector<RBPSubRow>::const_iterator itRBPSubRow;

  //RBPCoreRow is a set of rows of core cells with same y-coord
class RBPCoreRow
{
    friend class RBPlacement;
    friend class RBPlaceFromDB;
    friend class RBCPlacement;
    friend class RBPSubRow;

    Orient            _orient;   // information about allowed cell orientations 
    RBPSite	      _site;
    double 	      _spacing;  // distance between site beginings
    double 	      _y; 

    vector<RBPSubRow> _subRows;

    const Placement*  _placement;//this is a READ-ONLY reference!!

    void  appendNewSubRow(double startX, double endX)
    {	_subRows.push_back(RBPSubRow(startX, endX, *this)); 
	abkfatal(_subRows.back()._coreRow == this, "subRow init problem");
    }

    void save(ostream& os) const;

  public:

    RBPCoreRow(double y, Orient orient, const RBPSite& site, 
	       const Placement& pl, double spacing)
      : _orient(orient), _site(site), _spacing(spacing), _y(y), _placement(&pl)
      {}
    RBPCoreRow(double y)
      : _y(y), _placement(NULL)
      {}
    RBPCoreRow () 
      : _y(0), _placement(NULL)
      {}

    RBPCoreRow(const RBPCoreRow& orig)
      : _orient(orig._orient), _site(orig._site), _spacing(orig._spacing),
	_y(orig._y), _placement(orig._placement)
      {abkfatal(orig._subRows.size() == 0, "copy ctor of non-empty coreRow");}

    Orient                   getOrientation() const { return _orient;} 
    const RBPSite&           getSite()        const { return _site;}
    double                   getSpacing()     const { return _spacing;}
    double                   getSiteWidth()   const { return _site.width;}
    double		     getYCoord()      const { return _y;}
    double		     getHeight()      const {return _site.height;}
    Symmetry                 getSymmetry()    const {return _site.symmetry;}

    unsigned 		     getNumSubRows()  const { return _subRows.size();}
    itRBPSubRow              findSubRow(const double x) const;
    itRBPSubRow              subRowsBegin()   const {return _subRows.begin();}
    itRBPSubRow              subRowsEnd()     const {return _subRows.end();  }

    double		     getXStart()      const 
				{return _subRows[0].getXStart();}
    double		     getXEnd()        const 
				{return _subRows.back().getXEnd();}

    double 		     siteRoundUp(double length) const
		{ return ceil(length/_spacing)*_spacing;}
    double 		     siteRoundDown(double length) const
		{ return floor(length/_spacing)*_spacing;}

    unsigned 		     partSites(double length) const
		{ return static_cast<unsigned>(ceil(length/_spacing));}
    unsigned 		     wholeSites(double length) const
		{ return static_cast<unsigned>(floor(length/_spacing));}
 
    const RBPSubRow& operator[](unsigned idx) const {return _subRows[idx];}
          RBPSubRow& operator[](unsigned idx) {return _subRows[idx];}
 
    friend ostream& operator<<(ostream&, const RBPCoreRow&);
};

inline Orient      RBPSubRow::getOrientation() const 
				{return _coreRow->getOrientation();}
inline const RBPSite&    RBPSubRow::getSite()        const 
				{return _coreRow->getSite();}
inline double            RBPSubRow::getSpacing()     const 
				{return _coreRow->getSpacing();}
inline double            RBPSubRow::getSiteWidth()     const 
				{return _coreRow->getSiteWidth();}
inline double            RBPSubRow::getYCoord()      const 
				{return _coreRow->getYCoord();}
inline double	         RBPSubRow::getHeight()      const 
				{return _coreRow->getHeight();}

inline unsigned RBPSubRow::operator[](unsigned i) const 
{
    abkassert(!_needsSorting, "addToEnd called w/o sortCellsByLoc");
    return _cellIds[i];
}

struct compareCoreRowY
{
  bool operator()(const RBPCoreRow& cr1, const RBPCoreRow& cr2) const
  { return cr1.getYCoord() < cr2.getYCoord(); }
};

struct compareSubRowX
{
    bool operator()(const RBPSubRow& sr1, const RBPSubRow& sr2) const
    { return sr1.getXEnd() <= sr2.getXStart(); }
};


struct srowXSlt
{
  bool operator() (const RBPSubRow* sr1, const RBPSubRow* sr2) const
  { return sr1->getXStart() < sr2->getXStart(); }
};
 
class srowXElt
{
public:
  srowXElt () {}
  bool operator() (const RBPSubRow* sr1, const RBPSubRow* sr2) const
  {
    return sr1->getXEnd() < sr2->getXEnd();
  }
};
 
class srowYlt
{
public:
  srowYlt () {}
  bool operator() (const RBPSubRow* sr1, const RBPSubRow* sr2) const
  {
    return sr1->getYCoord() < sr2->getYCoord();
  }
};

#endif

