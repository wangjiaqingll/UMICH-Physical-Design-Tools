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

// CHANGES
// 970815 aec  added _coreCellArea and _totalCellWidth
// 970829 ilm  removed definitions for ctainerCoords and ctainerIndices
//                vector<double> and vector<unsigned> are more appropriate
// 970829 ilm  added support for coreCellsEmbedding
// 970919 pat  added spatial parameter to populate functions
// 971006 ilm  added dbEasyAccess::_pin2CellIdx to cache numbers of 
//                containg cells for all pins.
//                Note: this is not a Mapping (not injective)
//        ilm  added inspectors and dbEasyAccess::populatePin2CellIdx()
// 971129 pat new types and signatures in support of improved
//            portLocation data
// 980506 ilm added hash tables for cell and net names to dbEasyAccess
//            (if you need non-const access, use cellIds to get non-const
//            pointers from dbNetlist checked out with 
//            getdbNetlistForSurgery)


//======================================================================
//                          EasyAccess Package
//====================================================================== 

#ifndef _DBEASYXS_H_
#define _DBEASYXS_H_

#ifdef  _MSC_VER
#pragma warning (disable:4786)
#endif

#include <vector>
using std::vector;
#include <utility>
using std::pair;
#include <ABKCommon/sgi_hash_map.h>
#include "ABKCommon/abkcommon.h"
#include "Geoms/intervalSeqLayer.h"  // from Placement
#include "dbDefines.h"
#include "dbContext.h"
#include "dbDesign.h"
#include "dbSimpleTiming.h"

typedef hash_map<const char*, const dbCell *, hash<const char*>, CompareNameEqual> CellMapType;
typedef hash_map<const char*, const dbNet *,  hash<const char*>, CompareNameEqual> NetMapType;

// typedef pair<IntervalSeq, IntervalSeq> ISeqPair;
  class ISeqPair : public pair<IntervalSeq, IntervalSeq> 
  {   
    public:  
        ISeqPair(const IntervalSeq& f, const IntervalSeq& s) :
        pair<IntervalSeq, IntervalSeq>(f,s) {}
//      { first=f; second=s; }
//      ISeqPair(const ISeqPair& isp) { first=isp.first; second=isp.second; }
        ISeqPair() {}
  };

   class ISPairLayer : public vector<ISeqPair> 
   {
     public: 
         ISPairLayer(){}
         ISPairLayer(unsigned n) : vector<ISeqPair>(n) {}
         ISPairLayer(unsigned n, const ISeqPair& isp):vector<ISeqPair>(n,isp) {}
   };
// typedef vector<ISeqPair> ISPairLayer;
//#define ISPairLayer vector<ISeqPair>

class dbEasyAccess;

//========================== CLASS INTERFACES =============================

class dbEasyAccess
{

  friend class DB;

  public:
        enum Examples { IntRandom, IntLessRandom };
        dbSimpleTiming _simpleTiming;

  protected:
  
        // no constructors, no public data members:
        // DB postprocessor will call method populate()
        // to fill in the members below;
        // recommendation: create members with their own ctors 
        // and then assign them to private fields of dbContext; ???

        double 			   _coreCellArea;
  	double			   _totalCellWidth;

        // These members are indexed by cell ID
        vector<double>             _cellWidths;
	vector<double>             _cellAreas;

	// This is a vector of IntervalSeqLayers, each of
	// which is indexed by cell ID
        vector<IntervalSeqLayer> _whitespace;

	// Like _whitespace, this is a vector indexed by layer,
	// of vectors indexed by pin ID
        vector<ISPairLayer>  _portLocations;

        vector<unsigned>           _netIds; // .. that is for each pin -ILM
		//important Note:  not all pins are connected to a net.
		//unconnected bins will have a netId == (unsigned)(-1) in
		//_netIds.
        Subset                     _coreCellEmbedding;
        BBox                       _coreBBox;

        // stores the number of containing cell for each pin 
        // not a Mapping, because not injective
        vector<unsigned>           _pin2CellIdx;

        CellMapType                _cellNameTable;
        NetMapType                 _netNameTable;
	
        void populateCellWidths (const dbNetlist&);
	void populateCellAreas (const dbNetlist&);
        void populateWhitespace (const dbContext&, const dbNetlist&,
				 const dbSpatial&);
        void populateNetIds (const dbNetlist&);
	void populatePortLocations (const dbContext&,
				    const dbNetlist&, const dbSpatial&); 
        void populateCoreCellEmbedding(const dbNetlist&);
        void populateCoreBBox(const dbLayout&);
        void populatePin2CellIdx(const dbNetlist&);
        void populateNameHashTables(const dbNetlist&);
        void populateSimpleTiming(const dbNetlist&, const char* rcFile);

  public:

        void populate(const dbContext&, const dbNetlist&,
		      const dbSpatial&, const vector<char*>& fileNames,
                      const char * dirName);
	void populate(Examples ex, unsigned numCells, 
		      unsigned numNets, unsigned numPins,
		      unsigned seed = 0);

	bool isConsistent (const dbContext&, const dbNetlist&) const;

        const vector<double>&  getCellWidths()   const { return _cellWidths; }
	const vector<double>&  getCellAreas()    const { return _cellAreas;  }
	const vector<IntervalSeqLayer>& getWhitespace() const
	                                               { return _whitespace; }
//      const vector<ISPairLayer>& getPortLocations() const
//                                        { return _portLocations;}
        unsigned getNumPortLocations() const { return _portLocations.size(); }
        const ISPairLayer& getPortLocationByIdx(unsigned idx) const
                                             { return _portLocations[idx];   }
	const vector<unsigned>&      getNetIds() const { return _netIds; }
        const Subset&    getCoreCells() const    { return _coreCellEmbedding;  }
        const BBox&      getCoreBBox()  const    { return _coreBBox;     }

        const vector<unsigned>& getPin2CellIdx()const { return _pin2CellIdx;   }
        unsigned getCellIdxByPinIdx(unsigned k) const { return _pin2CellIdx[k];}

	double getCoreCellArea()                const { return _coreCellArea;  }
	double getTotalCellWidth()              const { return _totalCellWidth;}	
        const dbCell& getCellByName(const char* name) const;
        const dbNet&  getNetByName (const char* name) const;
     
	friend ostream& operator<< (ostream& out, const dbEasyAccess& arg);
};

ostream& operator<<(ostream&, const dbEasyAccess&);

//================= IMPLEMENTATION ==============================

/*
inline void dummy (const dbEasyAccess& arg)
// This definition is just a workaround for a very strange 
// compiler error message, associated with the use of STL.
// Without it, a complaint issues
// from STL that the types cast below (which are the real types)
// are incomplete when first used in a file.
{
  abkfatal(false,"This function should never be called.");
  cout << (static_cast<vector<IntervalSeqLayer> >(arg.getWhitespace())).size();
  cout << arg.getNumPortLocations();
  for (unsigned i=0; i<10; i++)
  {
    const ISPairLayer &layer = arg.getPortLocationByIdx(i);
    cout << (static_cast<pair<IntervalSeq,IntervalSeq> >(layer[0])).first;
  }
}
*/


#endif

