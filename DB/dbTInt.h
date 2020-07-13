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









struct dbTTimingConstraint
{
   float maxRiseDelay, maxFallDelay;
   float minRiseDelay, minFallDelay;

   // global indexes of starting and ending pins
   slist<unsigned> from;
   slist<unsigned> to;
};

struct dbTEdgeTimingInfo
// Information to be stored for each of the edges of the timing graph.
{
        EdgeType _edgeType;
    
        bool _hasUnateness;
        Unateness _unateness; 
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
            _edgeType( Wire ), _isCTLF( 0 ), _hasUnateness( 0 ),
            _numRiseIntrinsic( 0 ), _hasRiseRS( 0 ), _hasRiseCS( 0 ),
            _hasRiseSatT( 0 ), _hasRiseT0( 0 ),
            _numFallIntrinsic( 0 ), _hasFallRS( 0 ), _hasFallCS( 0 ),
            _hasFallSatT( 0 ), _hasFallT0( 0 )
        {}
                
};

struct dbTNodeTimingInfo
// Timing information contained in each node of the Timing Graph
{
    const dbPin *_pin;
    
    double onResistance, loadCap;
    
    dbTNodeTimingInfo( const dbPin *pin )
        : _pin( pin )
    {}
};

struct MasterPinPair {
    const dbMasterPin *_from, *_to;
    
    MasterPinPair( const dbMasterPin *from, const dbMasterPin *to )
        : _from( from ), _to( to )
    {}
};

