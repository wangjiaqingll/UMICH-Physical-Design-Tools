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









// Created      971129 by Max Moroz,   VLSI CAD ABKGROUP, UCLA/CS
// face-lifted  971205 by Igor Markov, VLSI CAD ABKGROUP, UCLA/CS
// Altered      990409 by Pedro Rabinovitch, "  ABKGROUP, UCLA/CS
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#define __DBTEMPORAL_CXX__

#include <list>
using std::list;
#ifndef slist
#define slist list
#endif

#include "Ctainers/dmatrix.h"
#include "dbTemporal.h"

struct dbTNodeTimingInfo
// Timing information contained in each node of the Timing Graph
{
    const dbPin *_pin;
    
    double onResistance, loadCap;
    
    dbTNodeTimingInfo( const dbPin *pin )
        : _pin( pin )
    {}
};

dbTemporal::TimingData::TimingData( const dbMasterPin *from, const dbMasterPin *to )
    : _pins( from, to )
{
    _info = new dbTEdgeTimingInfo;
}


// Below: dummy return values are okay for now
//        but will go away when dbTemporal is filled
//        by the parser when members of dbTemporal will be returned.
//        The corresp. accessorsshould be inlined then

float dbTemporal::getDefaultSlew(unsigned, dbTemporal::SignalEdge) const
{
   return 1;
}

const vector<dbTTimingConstraint>& dbTemporal::getTimingConstraints() const
{
	return _constr;

/*
   gTC[0].maxDelay=gTC[1].maxDelay=100; // dummy
	gTC[0].from.clear();
   gTC[0].from.push_front(0);
   gTC[0].from.push_front(3);
   gTC[0].from.push_front(42);
   gTC[0].from.push_front(43);
   gTC[0].from.push_front(36);
   gTC[0].from.push_front(33);
	gTC[0].to.clear();
   gTC[0].to.push_front(39);
   gTC[0].to.push_front(6);
   // gTC[1] is empty for now
   return gTC;*/
}

/* // Obsolete. EdgeDelayCalc's job.
float dbTemporal::getDelay( unsigned in, unsigned out,
                          float slew,  dbTemporal::SignalEdge edge,
                          dbTemporal::DelayVariance btw)  const 
{
   in, out, slew, edge, btw; 
   return 1; 
}
*/

/* // Obsolete. EdgeDelayCalc's job.
float dbTemporal::getSlew( unsigned in, unsigned out, float slew,
              dbTemporal::SignalEdge edge, dbTemporal::DelayVariance btw) const
{ 
   in, out, slew, edge, btw;
   return slew; // maybe slew * 1.05? 
}
*/

const slist<ULong>& dbTemporal::getBlockedNodes()  const 
{ 
	return _blockedNodes;
}

const slist<pair<ULong, ULong> >& dbTemporal::getBlockedEdges() const
{
	return _blockedEdges;
}

void dbTemporal::addUnateness( dbTemporal::Unateness u )
{
    _currentInfo->_unateness = u;
}

void dbTemporal::addRiseIntrinsic( double min, double max )
{
    _currentInfo->_numRiseIntrinsic = 1;
    _currentInfo->_riseMin[0] = min;
    _currentInfo->_riseMax[0] = max;
}

void dbTemporal::addRiseIntrinsic( double min1, double max1, double slew1,
                       double min2, double max2, double slew2 )
{
    _currentInfo->_numRiseIntrinsic = 2;
    _currentInfo->_riseMin[0] = min1;
    _currentInfo->_riseMax[0] = max1;
    _currentInfo->_riseSlew[0] = slew1;
    _currentInfo->_riseMin[1] = min2;
    _currentInfo->_riseMax[1] = max2;
    _currentInfo->_riseSlew[1] = slew2;
}

void dbTemporal::addRiseIntrinsic( double min1, double max1, double slew1,
                       double min2, double max2, double slew2,
                       double min3, double max3, double slew3 )
{
    _currentInfo->_numRiseIntrinsic = 3;
    _currentInfo->_riseMin[0] = min1;
    _currentInfo->_riseMax[0] = max1;
    _currentInfo->_riseSlew[0] = slew1;
    _currentInfo->_riseMin[1] = min2;
    _currentInfo->_riseMax[1] = max2;
    _currentInfo->_riseSlew[1] = slew2;
    _currentInfo->_riseMin[2] = min3;
    _currentInfo->_riseMax[2] = max3;
    _currentInfo->_riseSlew[2] = slew3;
}

void dbTemporal::addRiseRS( double min, double max )
{
    _currentInfo->_hasRiseRS = 1;
    _currentInfo->_riseRSMin = min;
    _currentInfo->_riseRSMax = max;
}

void dbTemporal::addRiseCS( double min, double max )
{
    _currentInfo->_hasRiseCS = 1;
    _currentInfo->_riseCSMin = min;
    _currentInfo->_riseCSMax = max;
}

void dbTemporal::addRiseSatT( double min, double max )
{
    _currentInfo->_hasRiseSatT = 1;
    _currentInfo->_riseSatTMin = min;
    _currentInfo->_riseSatTMax = max;
}

void dbTemporal::addRiseT0( double min, double max )
{
    _currentInfo->_hasRiseT0 = 1;
    _currentInfo->_riseT0Min = min;
    _currentInfo->_riseT0Max = max;
}
    
void dbTemporal::addRiseVariable( double min, double max )
{
    _currentInfo->_riseVarMin = min;
    _currentInfo->_riseVarMax = max;
}



void dbTemporal::addFallIntrinsic( double min, double max )
{
    _currentInfo->_numFallIntrinsic = 1;
    _currentInfo->_fallMin[0] = min;
    _currentInfo->_fallMax[0] = max;
}

void dbTemporal::addFallIntrinsic( double min1, double max1, double slew1,
                       double min2, double max2, double slew2 )
{
    _currentInfo->_numFallIntrinsic = 2;
    _currentInfo->_fallMin[0] = min1;
    _currentInfo->_fallMax[0] = max1;
    _currentInfo->_fallSlew[0] = slew1;
    _currentInfo->_fallMin[1] = min2;
    _currentInfo->_fallMax[1] = max2;
    _currentInfo->_fallSlew[1] = slew2;
}

void dbTemporal::addFallIntrinsic( double min1, double max1, double slew1,
                       double min2, double max2, double slew2,
                       double min3, double max3, double slew3 )
{
    _currentInfo->_numFallIntrinsic = 3;
    _currentInfo->_fallMin[0] = min1;
    _currentInfo->_fallMax[0] = max1;
    _currentInfo->_fallSlew[0] = slew1;
    _currentInfo->_fallMin[1] = min2;
    _currentInfo->_fallMax[1] = max2;
    _currentInfo->_fallSlew[1] = slew2;
    _currentInfo->_fallMin[2] = min3;
    _currentInfo->_fallMax[2] = max3;
    _currentInfo->_fallSlew[2] = slew3;
}

void dbTemporal::addFallRS( double min, double max )
{
    _currentInfo->_hasFallRS = 1;
    _currentInfo->_fallRSMin = min;
    _currentInfo->_fallRSMax = max;
}

void dbTemporal::addFallCS( double min, double max )
{
    _currentInfo->_hasFallCS = 1;
    _currentInfo->_fallCSMin = min;
    _currentInfo->_fallCSMax = max;
}

void dbTemporal::addFallSatT( double min, double max )
{
    _currentInfo->_hasFallSatT = 1;
    _currentInfo->_fallSatTMin = min;
    _currentInfo->_fallSatTMax = max;
}

void dbTemporal::addFallT0( double min, double max )
{
    _currentInfo->_hasFallT0 = 1;
    _currentInfo->_fallT0Min = min;
    _currentInfo->_fallT0Max = max;
}
    
void dbTemporal::addFallVariable( double min, double max )
{
    _currentInfo->_fallVarMin = min;
    _currentInfo->_fallVarMax = max;
}


void dbTemporal::addTable( dbTemporal::SignalEdge rf,
        dbTemporal::TableType ttype, int cols, int rows )
{
    _currentInfo->_isCTLF = 1;
    _currentInfo->_tables[rf][ttype][BestCase] = new DenseMatrix( rows, cols );
    _currentInfo->_tables[rf][ttype][TypicalCase] = new DenseMatrix( rows, cols );
    _currentInfo->_tables[rf][ttype][WorstCase] = new DenseMatrix( rows, cols );
    _currentInfo->_capLoadAxes[rf][ttype].reserve( cols );
    _currentInfo->_inputSlewAxes[rf][ttype].reserve( rows );
    
    _currentTable = 2*rf + ttype;
}

void dbTemporal::addCapLoadControl( double cl )
{
    _currentInfo->_capLoadAxes[_currentTable >> 1][_currentTable & 1].push_back( cl );
}

void dbTemporal::addInputSlewControl( double is )
{
    _currentInfo->_inputSlewAxes[_currentTable >> 1][_currentTable & 1].push_back( is );
}

void dbTemporal::addTableEntry( int row, int col, double best, double typ, double worst )
{
    (*_currentInfo->_tables[_currentTable >> 1][_currentTable & 1][BestCase]) ( row, col ) = best;
    (*_currentInfo->_tables[_currentTable >> 1][_currentTable & 1][TypicalCase]) ( row, col ) = typ;
    (*_currentInfo->_tables[_currentTable >> 1][_currentTable & 1][WorstCase]) ( row, col ) = worst;
}


void dbTemporal::addInfoTo( const dbMasterPin *from, const dbMasterPin *to )
{
    TimingData td( from, to );

    _timingData.push_back( td );
    _currentInfo = td._info;
    _currentInfo->_edgeType = Gate;
    _pinToTiming[ td._pins ] = _timingData.size() - 1;
}

void dbTemporal::addInfoTo( vector<const dbMasterPin *> from_set, vector<const dbMasterPin *> to_set )
{
    std::vector<const dbMasterPin *>::const_iterator i;
    std::vector<const dbMasterPin *>::const_iterator j;
    
    TimingData td( from_set.front(), to_set.front() );

    for( i = from_set.begin(); i != from_set.end(); i++ )
    {
        for( j = to_set.begin(); j != to_set.end(); j++ )
        {
            td._pins._from = *i;
            td._pins._to = *j;
            _pinToTiming[ td._pins ] = _timingData.size();
        }
    }

    _timingData.push_back( td );
    _currentInfo = td._info;
    _currentInfo->_edgeType = Gate;
}

void dbTemporal::addTimingEdge( const dbPin *pin1, const dbPin *pin2, EdgeType etype )
{
    abkfatal(0,"Not implemented");
    unsigned p1, p2;
/*
    if( _pinToNode.find( pin1->getIndex() ) == _pinToNode.end() )
    {
        _pinToNode[ pin1->getIndex() ] = _timingGraph.getNumNodes();
        _timingGraph.addNode( new dbTNodeTimingInfo( pin1 ) );
    } 

    if( _pinToNode.find( pin2->getIndex() ) == _pinToNode.end() )
    {
        _pinToNode[ pin2->getIndex() ] = _timingGraph.getNumNodes();
        _timingGraph.addNode( new dbTNodeTimingInfo( pin2 ) );
    } 
*/

    p1 = _pinToNode[ pin1->getIndex() ];
    p2 = _pinToNode[ pin2->getIndex() ];
   
    if( etype == dbTemporal::Wire )
    {
//      _timingGraph.addEdge( p1, p2, new dbTEdgeTimingInfo );
        // TODO Add dummy wire data.
    }
    else
    {
        const dbMasterPin &mpin1 = pin1->getMaster();
        const dbMasterPin &mpin2 = pin2->getMaster();
        dbTMasterPinPair mp( &mpin1, &mpin2 );
        
        if( _pinToTiming.find( mp ) == _pinToTiming.end() )
        {
          abkwarn(0,"dbTemporal: addTimingEdge: Timing information not found.");
//          _timingGraph.addEdge( p1, p2, new dbTEdgeTimingInfo );
        }
        else
        {
//          unsigned td_idx = _pinToTiming[ mp ];
//          _timingGraph.addEdge( p1, p2, _timingData[td_idx]._info );
        }
    }
}

void dbTemporal::postProcess( const dbNetlist &netlist )
{
    itPinGlobal pin1;
    
    if( _timingData.size() == 0 )
        return; // No timing data was populated.
    
    for( pin1 = netlist.pinsBegin(); pin1 != netlist.pinsEnd(); pin1++ )
    {
        if( !(*pin1)->isConnected() )
            continue;
        if( !(*pin1)->isUseSignal() )
            continue;
        if((*pin1)->isConnected() && !(*pin1)->isDirInput() )
        {
            const dbNet& net=(*pin1)->getNet();
            for (itPinLocal pin2=net.pinsBegin(); pin2!=net.pinsEnd(); ++pin2)
            {
                if ((*pin2)->isDirOutput())
                    continue; // inter-cell signals don't go to output pins
                addTimingEdge( *pin1, *pin2, dbTemporal::Wire );
            }
        }
        if( !(*pin1)->isDirOutput() )
        {
            const dbCell& cell = (*pin1)->getCell();
            if( !( cell.isCore() && cell.isCC() ))
                continue;
            
            for( itPinLocal pin2=cell.pinsBegin(); pin2!=cell.pinsEnd(); ++pin2 )
            {
               // intra-cell signals are of no interest to discon'ed pins
               if( !(*pin2)->isConnected() )
                  continue;
               // intra-cell signals don't go to non-signal pins
               if( !(*pin2)->isUseSignal() )
                  continue;
               // intra-cell signals don't go to input pins
               if( (*pin2)->isDirInput() )
                  continue;

               addTimingEdge( *pin1, *pin2, Gate );
            }
        }
    }
    
}

const dbTEdgeTimingInfo * dbTemporal::getTimingInfo( const dbPin *pin1, const dbPin *pin2) 
{
    const dbMasterPin &mpin1 = pin1->getMaster();
    const dbMasterPin &mpin2 = pin2->getMaster();
    dbTMasterPinPair mp( &mpin1, &mpin2 );
        
    unsigned td_idx = _pinToTiming[ mp ];
            
    return _timingData[td_idx]._info;
}
    

