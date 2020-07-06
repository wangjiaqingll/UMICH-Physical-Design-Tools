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
// 970806 aec removed 'const' from _index, postProcessor changes it when
//		it re-orders nets.
//	      made dbNetlist a friend to dbNet
// 970807 ilm added cell and pin "by index" accessors to class dbNet
// 970814 aec added friend class DB
// 970815 aec added const to 'loopCheck' functions
// 970817 ilm added "compare" classes from dbSortComp.h
// 970819 aec added CompareNetsByDegree
// 970825 ilm improved diagnostics for nested forall macros
// 970905 aec added dbNet::_numSources
// 970906 aec changed dbNet::_numSources to _numOut, added _numInOut
//		added sourcesBegin/End, sourceSinksBegin/End, sinksBegin/End
//		iterators for cells.
// 971130 ilm added "prohibiting" copy-ctor and op= for dbNet
// 971209 mro added snetOffset to distinguish special-net indices
//            from ordinary ones, isSpecial() inspector, and CompareNetsByName
// 971214 ilm publicized ("nationalized") cells/pins in dbNet


#ifndef __DB_NET_H_
#define __DB_NET_H_

#include <algorithm>
#include "dbDefines.h"
#include "dbTypes.h"
#include "dbPin.h"
#include "dbCell.h"
#include "dbNet.h"
#include "dbContext.h"

const unsigned snetOffset = 1000000000; //special-net indices start here

class dbNet
{
    MakeFriendsWithParsers
    friend class dbNetlist;
    friend class DB;

    char *      	_name;
    unsigned   		_index;
    const dbNetType  	_type;        // regular, timing, clock
 public:
    ctainerCellsLocal  	_cells;       // source cell(s) come<s> first
    ctainerPinsLocal   	_pins;
    ctainerLayerGeoms  	_wiring;      // should be empty except for special
                                      // nets.
    unsigned		_numOut;
    unsigned		_numInOut;

    bool                _hasLogicalPin; 
    double              _weight;      //weight of the net. 

 private:    
    static char      _forAllCellsFlag;
    static char      _forAllPinsFlag;   
    
    // net weights should be taken care of by placer/elsewhere

        // this is what you can't do  --
        // default copy-ctor and assignment are wrong

        dbNet(const dbNet& from):_type(from._type)
                 { abkfatal(0,"Can't copy-construct dbNet"); }
        void operator=(const dbNet&)
                 { abkfatal(0,"Can't assign dbNet");}

    
    public:
        dbNet(const char * name, unsigned idx, dbNetType type,
              const ctainerPinsLocal& pins):
	      _index(idx),_type(type),_pins(pins),_hasLogicalPin(false),
	      _weight(1)
              {  /* Extrapolate vector<dbCell*> cells from pins */
              /* Go through each pins and set the cell ptr */
              /* This should be verified as part of a future integrity check */
              _name = new char[strlen(name)+1];strcpy(_name,name);
              }
        ~dbNet();
        unsigned  getNumCells() const { return _cells.size(); }
        unsigned  getNumPins()  const { return _pins.size() ; }
        unsigned  getDegree()   const { return _cells.size(); }
        unsigned  getIndex()    const { return _index; }
        dbNetType getType()     const { return _type; }
        const char* getName()   const { return _name; }
 	unsigned  getNumOut()   const { return _numOut; }
	unsigned  getNumInOut() const { return _numInOut; }
	unsigned  getNumIn()	const { return _cells.size() - 
					(_numOut + _numInOut); }
	double    getWeight()   const { return _weight; }

        unsigned   hasLogicalPin() const {return _hasLogicalPin;}
          
        itCellLocal cellsBegin() const { return _cells.begin(); }
        itCellLocal cellsEnd()   const { return _cells.end();   }

	itCellLocal sourcesBegin() const { return _cells.begin(); }
	itCellLocal sourcesEnd()   const { return _cells.begin() + _numOut; }

	itCellLocal sourceSinksBegin() const 
		{ return _cells.begin() + _numOut; }
	itCellLocal sourceSinksEnd()   const 
		{ return _cells.begin() + _numOut + _numInOut; }

	itCellLocal sinksBegin() const 
		{ return _cells.begin() + _numOut + _numInOut; }
	itCellLocal sinksEnd()   const 
		{ return _cells.end(); }

        const dbCell&  getCellByIdx(unsigned i) const
        {
          abkassert(i < _cells.size(),"Cell index out of range");
          return *(_cells[i]);
        }

        itPinLocal pinsBegin()  const { return _pins.begin();  }
        itPinLocal pinsEnd()    const { return _pins.end();    }

        const dbPin&  getPinByIdx(unsigned i) const
        {
          abkassert(i < _pins.size(),"Pin index out of range");
          return *(_pins[i]);
        }

        itLayerGeom wiringBegin() const { return _wiring.begin();}
        itLayerGeom wiringEnd()  const { return _wiring.end();  }

        const dbLayerGeom& getWiringLayerGeom(unsigned i) const
              {return *(_wiring[i]);}

        unsigned getNumWiringLayerGeoms() const {return _wiring.size();}
          
        void loopCellsBegin() const;
        void loopCellsEnd(itCellLocal itr) const;
        void loopPinsBegin() const;
        void loopPinsEnd(itPinLocal itr) const;
          
          
        const dbPin *commonPin(const dbCell* cell) const;
        bool   commonPinIsCellInput (const dbCell* cell) const;
        bool   commonPinIsCellOutput(const dbCell* cell) const;
          
        double getInputCap(const dbCell* cell) const;
        double getDriverResist(const dbCell* cell) const;

        bool isSpecial() const {return _index >= snetOffset;} //CAREFUL:
                                                              //works only
                                                              //*after*
                                                              //postprocessing

    private:		
	void induceCellsFromPins();	
	void sortCellsByIndex();
	void sortCellsByDir();
        void removeDupPins();
          
};

class CompareNetsByCellIndex    //comp. class for sorting the netlist's nets
{
   public:
    CompareNetsByCellIndex(){}

    bool operator()( dbNet* n1, dbNet* n2) const;
};

class CompareNetsByDegree
{
    public:
     CompareNetsByDegree(){}

     bool operator()( dbNet* n1, dbNet* n2) const;
};

class CompareNetsByName
{
    public:
     CompareNetsByName(){}

     bool operator()( dbNet* n1, dbNet* n2) const;
};

class CompareNetsByDirection
{
    const dbCell& _cell;
  public:
    CompareNetsByDirection(const dbCell& cell)
	: _cell(cell)
	{}
    bool operator()( dbNet* n1, dbNet* n2) const;
};

// =========================  IMPLEMENTATIONS ========================

inline void dbNet::sortCellsByIndex()
{
    std::sort(_cells.begin(), _cells.end(), CompareCellsByCellIndex());
}

inline void dbNet::sortCellsByDir()
{
    std::sort(_cells.begin(), _cells.end(), CompareCellsByDir(this));
}

inline void dbNet::loopCellsBegin() const
{
    abkassert(!_forAllCellsFlag,
        "ERROR: nesting of forallCells macros is invalid");
    _forAllCellsFlag = true;
}

inline void dbNet::loopCellsEnd(itCellLocal itr) const
{
    if(itr == cellsEnd())_forAllCellsFlag = false;
}


inline void dbNet::loopPinsBegin() const
{
    abkassert(!_forAllPinsFlag,
        "ERROR: nesting of forallPins macros is invalid");
    _forAllPinsFlag = true;
}

inline void dbNet::loopPinsEnd(itPinLocal itr) const
{
    if(itr == pinsEnd())_forAllPinsFlag = false;
}

#endif

