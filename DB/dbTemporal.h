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











// Created   970716 by Igor Markov, VLSI CAD ABKGROUP, UCLA/CS
// additions 971205 by Max Moroz,   VLSI CAD ABKGROUP, UCLA/CS
// Altered   990409 by Pedro Rabinovitch, "  ABKGROUP, UCLA/CS

//======================================================================
//                          Temporal Package
//====================================================================== 

#ifndef _DBTEMPORAL_H_
#define _DBTEMPORAL_H_

#include <vector>
using std::vector;
#include <list>
using std::list;
#ifndef slist
#define slist list
#endif
#include <utility>
using std::pair;
#include <ABKCommon/sgi_hash_map.h>
#include "dbDefines.h"
#include "dbDesign.h"
#include "ABKCommon/abkcommon.h"
#include "Placement/placement.h"
// #include "WGraph/wgflexible.h"

typedef dbTemporal dbTiming; // ilm: use either one
struct dbTTimingConstraint;
struct dbTNodeTimingInfo;
struct dbTEdgeTimingInfo;
class dbTemporal;

struct dbTMasterPinPair {
    const dbMasterPin *_from, *_to;
    
    dbTMasterPinPair( const dbMasterPin *from, const dbMasterPin *to )
        : _from( from ), _to( to )
    {}
};

struct dbTTimingConstraint
{
   float maxRiseDelay, maxFallDelay;
   float minRiseDelay, minFallDelay;

   // global indexes of starting and ending pins
   slist<unsigned> from;
   slist<unsigned> to;
};

//========================== CLASS INTERFACES =============================

class dbTemporal        // will be populated by the parser
{                       // recommendation: create the vectors and then assign
    MakeFriendsWithParsers // them to private fields of dbTemporal;
       
public:
    enum DelayVariance { BestCase=0, TypicalCase, WorstCase };
    enum SignalEdge { Rise=0, Fall };
    enum TableType { Delay=0, Slew };
    enum Unateness { Invert=0, NonInvert, NonUnate };
    enum EdgeType { Gate=0, Wire };

    
private:
//    typedef WGraphFlexible< dbTNodeTimingInfo *, dbTEdgeTimingInfo * > 
      typedef Empty TimingGraph;
        
    //TimingGraph< dbTNodeTimingInfo *, dbTEdgeTimingInfo * >                _timingGraph;
    TimingGraph     _timingGraph;
    // TimingGraph (topology information). Each edge cointains a pointer to the
    // appropriate timing info structure (see below).

    struct TimingData {
        dbTEdgeTimingInfo *_info;
        dbTMasterPinPair _pins;
        
        TimingData( const dbMasterPin *from, const dbMasterPin *to );

    };
    
    vector< TimingData > _timingData;

    struct EqPinPair {
        // compares two dbTMasterPinPair pointers, true if they're the same.
        bool operator()( const dbTMasterPinPair p1, const dbTMasterPinPair p2 ) const
        { return ( p1._from == p2._from ) && ( p1._to == p2._to ); }
    };

    struct HashPinPair {
        // calculates the XOR of the hash of the two pointers in a dbTMasterPinPair.
        hash<unsigned> HashUnsigned;
        bool operator()( const dbTMasterPinPair p ) const
        { return HashUnsigned((unsigned)(long int)(p._from)) ^ HashUnsigned((unsigned)(long int)(p._to)); }
        
    };

    hash_map< const dbTMasterPinPair, unsigned, HashPinPair, EqPinPair > _pinToTiming;
    // Keeps relation dbTMasterPinPair -> TimingData index
    // (used to set the pointer in the edges of the TimingGraph).
    
    hash_map< const unsigned, unsigned, hash<unsigned>,
              std::equal_to<unsigned> > _pinToNode;
    
/*
//  vector<unsigned>           _clockIDs;   
    // I/O boundary conditions at pads (clock ID, offset)
    vector<double>             _offsets;
*/

    vector<dbTTimingConstraint>   _constr;
    slist<ULong>               _blockedNodes;
typedef pair<ULong, ULong>  UnsignedPair;
    slist<UnsignedPair>        _blockedEdges;
    
    dbTEdgeTimingInfo * _currentInfo;
    // Used for iterative adding.
    int _currentTable;
    // Used for iterative adding.
    
    void addTimingEdge( const dbPin *pin1, const dbPin *pin2, EdgeType etype );
    // Adds an edge to the timing graph, adding pins (nodes) if necessary.

    /* Functions to be called by parser: */
    void addInfoTo( const dbMasterPin *from, const dbMasterPin *to );
    void addInfoTo( vector<const dbMasterPin *> from_set, vector<const dbMasterPin *> to_set );
    // Starts adding info to set of pins;
    // must be called before any of the next functions.

    void addUnateness( dbTemporal::Unateness u );
    // Adds the unateness for the edge.

    void addRiseIntrinsic( double min, double max );
    // Adds Intrinsic Rise data to current edge (instantaneous model).
    void addRiseIntrinsic( double min1, double max1, double slew1,
                           double min2, double max2, double slew2 );
    // Adds Intrinsic Rise data to current edge (2-point model).
    void addRiseIntrinsic( double min1, double max1, double slew1,
                           double min2, double max2, double slew2,
                           double min3, double max3, double slew3 );
    // Adds Intrinsic Rise data to current edge (3-point model).
    void addRiseVariable( double min, double max );
    // Adds K factor for rise ESGD calculation.
    void addRiseRS( double min, double max );
    // Adds Rise source resistance data.
    void addRiseCS( double min, double max );
    // Adds Rise source capacitance data.
    void addRiseSatT( double min, double max );
    // Adds Rise maximum saturation time data.
    void addRiseT0( double min, double max );
    // Adds rise time data.

    void addFallIntrinsic( double min, double max );
    // Adds Intrinsic Fall data to current edge (instantaneous model).
    void addFallIntrinsic( double min1, double max1, double slew1,
                           double min2, double max2, double slew2 );
    // Adds Intrinsic Fall data to current edge (2-point model).
    void addFallIntrinsic( double min1, double max1, double slew1,
                           double min2, double max2, double slew2,
                           double min3, double max3, double slew3 );
    // Adds Intrinsic Fall data to current edge (3-point model).
    void addFallVariable( double min, double max );
    // Adds K factor for rise ESGD calculation.
    void addFallRS( double min, double max );
    // Adds Fall source resistance data.
    void addFallCS( double min, double max );
    // Adds Fall source capacitance data.
    void addFallSatT( double min, double max );
    // Adds Fall maximum saturation time data.
    void addFallT0( double min, double max );
    // Adds fall time data.

    void addTable( SignalEdge rf, TableType ttype, int cols, int rows );
    // Adds a new table and sets it as the current one we're working with.
    void addCapLoadControl( double cl );
    // Adds a capacitive load control point.
    void addInputSlewControl( double is );
    // Adds an input slew control point.
    void addTableEntry( int row, int col, double best, double typ, double worst );
    // Adds a table entry in pos (row, col) (row <- input slew, col <- cap load).
    

public:           
    //  empty constructor: the class will be populated by the parser
    dbTemporal()
    {}

    void postProcess( const dbNetlist &netlist );
    
    bool             isConsistent() const;

    const vector<dbTTimingConstraint>& getTimingConstraints() const;
    
    const dbTEdgeTimingInfo * getTimingInfo( const dbPin *pin1, const dbPin *pin2) ;

       // mmoroz: returns default slew
        // this fn will only be called by me for pins
        // that are "starting" pins in some constraints

  float getDefaultSlew(unsigned in, dbTiming::SignalEdge rf) const;

   const slist<ULong>& getBlockedNodes() const;
   const slist<pair<ULong, ULong> >& getBlockedEdges() const;
};

inline bool dbTemporal::isConsistent() const
{
    bool IamFine=true;      // add checks for _clockIds ? negative offsetts ?
//  if ( _clockIDs.size() != _offsets.size() ) IamFine=false;
    abkwarn(IamFine,"Temporal section inconsistent : size mismatch\n");
    return IamFine;
}

class DenseMatrix;

struct dbTEdgeTimingInfo
// Information to be stored for each of the edges of the timing graph.
{
    dbTemporal::EdgeType _edgeType;
    
    bool _hasUnateness;
    dbTemporal::Unateness _unateness; 
    // Unateness of edge [common]
    bool _isCTLF;
    // True if data is in CTLF format (i.e., tables) [common]

    DenseMatrix *_tables[2][2][3];
    // _tables[ Rise/Fall ][ Delay/Slew ][ Best/Typ/Worst ].
    // Gate delay and output slew tables [CTLF]
    vector<double> _inputSlewAxes[2][2], _capLoadAxes[2][2];
    // Numbers in the axes (control points); dimensions from matrices above. [CTLF]
    // (note that the axes are the same for best/typ/worst cases.
    
    int _numRiseIntrinsic;
    // 0 if no intrinsic info, 1 if Instantaneous model,
    // 2 for two-point estimator or 3 for three-point.
    bool _hasRiseRS;
    bool _hasRiseCS;
    bool _hasRiseSatT;
    bool _hasRiseT0;

    int _numFallIntrinsic;
    // 0 if no intrinsic info, 1 if Instantaneous model,
    // 2 for two-point estimator or 3 for three-point.
    bool _hasFallRS;
    bool _hasFallCS;
    bool _hasFallSatT;
    bool _hasFallT0;
    
    double _riseMin[3];
    double _riseMax[3];
    double _riseSlew[3];
    double _riseVarMin, _riseVarMax;
    double _riseRSMin, _riseRSMax;
    double _riseCSMin, _riseCSMax;
    double _riseSatTMin, _riseSatTMax;
    double _riseT0Min, _riseT0Max;
    // Rise parameters.

    double _fallMin[3];
    double _fallMax[3];
    double _fallSlew[3];
    double _fallVarMin, _fallVarMax;
    double _fallRSMin, _fallRSMax;
    double _fallCSMin, _fallCSMax;
    double _fallSatTMin, _fallSatTMax;
    double _fallT0Min, _fallT0Max;
    // Fall parameters.
    

    dbTEdgeTimingInfo() :
        _edgeType( dbTemporal::Wire ), _hasUnateness( 0 ), _isCTLF( 0 ), 
        _numRiseIntrinsic( 0 ), _hasRiseRS( 0 ), _hasRiseCS( 0 ),
        _hasRiseSatT( 0 ), _hasRiseT0( 0 ),
        _numFallIntrinsic( 0 ), _hasFallRS( 0 ), _hasFallCS( 0 ),
        _hasFallSatT( 0 ), _hasFallT0( 0 )
    {}
                
};

#endif
