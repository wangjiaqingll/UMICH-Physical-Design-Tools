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









//	CHANGES
//   970729 aec  default constructor now initializes numPads, IOs, coreCells
//   970804 aec  added num's changed postProc function to set them
//   970805 aec  added processLGC(ifstream&)
//   970806 aec  added sorting of nets in portprocess. moved PP
//			functions to dbPostProcess.h
//   970807 ilm  added dbNetlist::getNumNets()
//   970814 aec  added argument, spatial, to postProc
//   970814 aec  removed above.
//			moved postprocessing sorting to DB
//			added friend class DB
//   970819 aec  added loop functions for SNets
//   970819 aec  added halfPerimEval()
//   970820 ilm  added copy-ctor and assigment with abkfatals to dbNetlist
//   970819 mro  include <stdio.h> (for sprintf() ).
//   970822 aec  added _pin, _cellLib and _pinLib begin/end iterators
//   970825 aec  added halfPerim public functions, w & w/o orientations.
//   970825 aec  added orientations to halfPerim private function
//   970825 aec  added const to a number of functions
//   970825 ilm  improved diagnostics for nested forall macros
//   970826 ilm  renamed halfPerimEval() into evalHalfPerimCost()
//   970826 ilm  added evalMSTPerimCost()
//   970826 aec  added getNumSCs and getNumCCs
//   970827 aec  added getNumAllPads getNumIOPads getNumCoreCells getNumIOs
//   970907 aec  moved findLEQ/EEQ functions to dbNetlist.cxx
//   970911 aec  added evalHalfPerim functions for single net, and one 
//			without pinlocations.  Copied Maogang's pinlocation
//			code into HP and MST eval functions
//   971006 ilm  added dNetlist::printNetStats(ostream&) and
//                     dNetlist::printNetStats()   (prints to cout)
//   971115 ilm  added dbNetlist::evalWHalfPerimWireLength()
//               for weighted wirelength
//   971130 ilm  dbNetlist::evalHalfPerimCost() for nets is now returning
//               half-perim as double, as opposite to the whole BBox
//   971130 ilm  corrected a dozen const-correctness bugs in
//                wirelength cost evaluators in dbNetlist
//   971209 mro  changed getSNetByIdx to accomodate snetOffset.
//   980121 ilm  added optional thresholding to WL estimators
//               as well as IONetsOnly flag
//   980302 ilm  commented #include <stdio.h> to move it to the *.cxx file



#ifndef __DB_NETLIST_
#define __DB_NETLIST_

//#include <stdio.h>
#include <fstream>
#include <vector>
using std::vector;
#include <algorithm>
#include "dbDefines.h"
#include "dbTypes.h"
#include "dbNet.h"
#include "dbCell.h"
#include "dbPin.h"
#include "Placement/placement.h"

class CompareNetsByCellIndex;
class CompareMCsByName;
class CompareCellsByName;
class CompareCellsByCellIndex;

class dbNetlist 
{
    MakeFriendsWithParsers
    friend class DB;
    
    //  maxArea and totalArea may be added later
    
    ctainerMasterCells       _cellLib;
    ctainerMasterPinsGlobal  _pinLib;
    
    ctainerCellsGlobal       _cells;
    ctainerPinsGlobal        _pins;
    ctainerNetsGlobal        _nets;
    ctainerNetsGlobal        _sNets; //special nets
    
    static char _forAllCellsFlag;
    static char _forAllNetsFlag;
   
    unsigned		  _numIOPads; 
    unsigned		  _numAllPads;
    unsigned              _numIOPins;
    unsigned              _numCoreCells;

    unsigned		  _numCCs;
    unsigned		  _numSCs;
    unsigned	      	  _numDummy1;	//no current use
    unsigned	 	  _numDummy2;	

    double		  _coreCellArea;
    
    equivClassesMasterCells _cellLEQs;
    equivClassesMasterCells _cellEEQs;
    equivClassesMasterPins  _pinLEQs;

        // this is what you can't do  --
        // default copy-ctor and assignment are wrong

        dbNetlist(const dbNetlist&)
                 { abkfatal(0,"Can't copy-construct dbNetlist"); }
        dbNetlist& operator=(const dbNetlist&)
                 { abkfatal(0,"Can't assign dbNetlist"); return *this; }
    
    public:
        
        dbNetlist(); // will be populated by parsers
        ~dbNetlist();
        // copy-ctor and assignment are prohibited: see below

        eqClassMasterCells getCellLEQClass(dbMasterCell*) const;
        eqClassMasterCells getCellEEQClass(dbMasterCell*) const;
        eqClassMasterPins  getPinLEQClass(dbMasterPin*) const;
        
        
        bool isConsistent() const;
        
        unsigned getNumCells() const {return _cells.size();}
        unsigned getNumPins()  const {return _pins.size();}    
        unsigned getNumNets()        const { return _nets.size();   }
        unsigned getNumSNets()       const { return _sNets.size();  }
        unsigned getNumMasterCells() const { return _cellLib.size();}
        unsigned getNumMasterPins()  const { return _pinLib.size(); }

	unsigned getNumCCs()	const	{ return _numCCs; }
	unsigned getNumSCs()	const	{ return _numSCs; }
	unsigned getNumIOPads() const	{ return _numIOPads;}
	unsigned getNumAllPads() const 	{ return _numAllPads;}
	unsigned getNumIOPins()	const	{ return _numIOPins; }
	unsigned getNumCoreCells() const{ return _numCoreCells; }
        
        itCellGlobal cellsBegin() const { return _cells.begin();}
        itCellGlobal cellsEnd()   const { return _cells.end();  }

        itNetGlobal  netsBegin()  const { return _nets.begin(); }
        itNetGlobal  netsEnd()    const { return _nets.end();   }
        itNetGlobal  sNetsBegin() const { return _sNets.begin();}
        itNetGlobal  sNetsEnd()   const { return _sNets.end();  }

	itPinGlobal  pinsBegin()  const { return _pins.begin(); }
	itPinGlobal  pinsEnd()	  const { return _pins.end();   }

	itMasterPinGlobal pinLibBegin() const { return _pinLib.begin(); }
	itMasterPinGlobal pinLibEnd()   const { return _pinLib.end();   }
	itMasterCell	  cellLibBegin() const {return _cellLib.begin();}
	itMasterCell      cellLibEnd()   const {return _cellLib.end();  } 
 
        
        const dbCell& getCellByIdx(unsigned i) const;
        const dbNet&  getNetByIdx(unsigned i) const;
        const dbNet&  getSNetByIdx(unsigned i) const;
        const dbPin&  getPinByIdx(unsigned i) const;
        
        void loopCellsBegin() const;
        void loopCellsEnd(itCellGlobal itr) const;
        void loopNetsBegin() const;
        void loopNetsEnd(itNetGlobal itr) const;
	void loopSNetsBegin() const;
	void loopSNetsEnd(itNetGlobal itr) const;

  // It would be nice to provide iterators like forAllCoreCells()
  // which will get the next cell, check if it is of a given type
  // and, if not, skip to the next.
  
  // In the worst case (e.g. unsorted order, or alpha-sorted order)
// it will take O(N)  (N == tot#ofCells)  to traverse all k<=N cells of a kind.
  // If the cells are sorted so that cells of a kind go together, it will
  // take O(k) to traverse all k cells of a kind (at least for some kinds)

	void postProcess();        
	void processLGC( ifstream &lgcf,const Verbosity &verbose);

	double evalHalfPerimCost(const Placement &pl, 
                unsigned threshold=0, bool IONetsOnly=false) const
                { return 
                  evalHalfPerimCost(pl, NULL, threshold, IONetsOnly); 
                }

	double evalHalfPerimCost(const Placement &pl,
                                 const vector<Orientation>& ori,
               unsigned threshold=0,  bool IONetsOnly=false) const
		{ return 
                  evalHalfPerimCost(pl, &(ori), threshold, IONetsOnly);
                }

	double evalWHalfPerimCost(const Placement &pl, 
               unsigned threshold=0, bool IONetsOnly=false) const
                // weighted
		{ return 
                   evalWHalfPerimCost(pl, NULL, threshold, IONetsOnly);
                }

	double evalWHalfPerimCost(const Placement &pl, 
                                  const vector<Orientation>&ori, 
                unsigned threshold=0, bool IONetsOnly=false) const
		{ return
                  evalWHalfPerimCost(pl, &(ori), threshold,IONetsOnly);
                }

        double evalMSTCost(const Placement &pl) const
                { return evalMSTCost(pl, NULL); }
        double evalMSTCost(const Placement &pl, 
                  const vector<Orientation> &ori) const
                { return evalMSTCost(pl, &(ori)); }

	double evalHalfPerimCost(dbNet *net, const Placement &pl,
                        const vector<Orientation> *orientations,
                        bool IONetsOnly=false) const;

 	double evalHalfPerimCostNoPins(dbNet *net, const Placement &pl,
                        bool IONetsOnly=false) const;

        void printNetStats(ostream& out) const;
        void printNetStats()             const  { printNetStats(cout); }


private:
	double evalHalfPerimCost(const Placement &pl,
                                 const vector<Orientation> *ori,
                                 unsigned threshold=0,
                                 bool IONetsOnly=false) const;
	double evalMSTCost      (const Placement &pl, 
                                 const vector<Orientation> *ori) const;

        double evalWHalfPerimCost(const Placement &pl, 
                                 const vector<Orientation> *ori,
                                 unsigned threshold=0,
                                 bool IONetsOnly=false) const;
};

inline dbNetlist::dbNetlist():
_numIOPads(0),_numAllPads(0),_numIOPins(0),_numCoreCells(0)
{ }

inline const dbCell& dbNetlist::getCellByIdx(unsigned i) const
{    
    abkassert(i < _cells.size(),"Cell number exceeds maximum");
    return *(_cells[i]);
}

inline const dbNet& dbNetlist::getNetByIdx(unsigned i) const
{    
    abkassert(i < _nets.size(),"Net number exceeds maximum");
    return *(_nets[i]);
}

inline const dbNet& dbNetlist::getSNetByIdx(unsigned i) const
{    
    abkassert(i>=snetOffset,"Non-special net index passed to getSNetByIdx");
    abkassert(i-snetOffset < _sNets.size(),"Special net number exceeds maximum");
    return *(_sNets[i-snetOffset]);
}

inline const dbPin& dbNetlist::getPinByIdx(unsigned i) const
{    
    abkassert(i < _pins.size(),"Pin number exceeds maximum");
    return *(_pins[i]);
}

inline void dbNetlist::loopCellsBegin() const
{
    abkassert(!_forAllCellsFlag, 
        "ERROR: nesting of forallCells macros is invalid");
    _forAllCellsFlag = true;
}

inline void dbNetlist::loopCellsEnd(itCellGlobal itr) const
{
    if (itr == cellsEnd()) { _forAllCellsFlag = false; }
}

inline void dbNetlist::loopNetsBegin() const
{
    abkassert(!_forAllNetsFlag, 
        "ERROR: nesting of forallNets macros is invalid");
    _forAllNetsFlag = true;
}

inline void dbNetlist::loopNetsEnd(itNetGlobal itr) const
{
    if (itr == netsEnd()) { _forAllNetsFlag = false; }
} 

inline void dbNetlist::loopSNetsBegin() const
{
    abkassert(!_forAllNetsFlag,
        "ERROR: nesting of forallSNets macros is invalid");
    _forAllNetsFlag = true;
}
 
inline void dbNetlist::loopSNetsEnd(itNetGlobal itr) const
{
    if (itr == sNetsEnd()) { _forAllNetsFlag = false; }
} 

#endif

