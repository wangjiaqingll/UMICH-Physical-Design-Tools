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



// created by Andrew Caldwell on 02/09/98
// major portions from LIFO-FM code by S.Dutt W.Deng

#ifndef _FM_NETCUT2WAYMM_H_
#define _FM_NETCUT2WAYMM_H_

#include "ABKCommon/abkcommon.h"
#include "Partitioners/moveMgrXFace.h"
#include "dcGainCont.h"
#include "Partitioners/svMoveLog.h"

using std::bit_vector;


class FMNetCut2WayMoveManager : public MoveManagerInterface
{
    const HGraphFixed& 		_hgraph;
    vector<unsigned>		_edgeWeights; //rounded values from HGraph

    vector<double> 		_partAreas;
    double 			_maxAreas[2];
    double 			_minAreas[2];

    unsigned	   		_curCost;

    DestCentricGainContainer 	_gainCo;

    unsigned short* 	_locked[2];	//these are indexed as [part][netId]
    unsigned short*   _unlocked[2];

    bit_vector	    	_lockedNode;
    SVMoveLog		_moveLog;

    bool		_allowCorkingNodes;


	//this gets around the const issues
    FMNetCut2WayMoveManager* _fakeThis;

    double _calcImbalance()
 	{return max(_partAreas[0]-_partAreas[1], _partAreas[1]-_partAreas[0]);}

    double _calcViolation()
	{ double viol_0 = 
		max(_partAreas[0]-_maxAreas[0], _minAreas[0]-_partAreas[0]);
	  double viol_1 = 
		max(_partAreas[1]-_maxAreas[1], _minAreas[1]-_partAreas[1]);
	  return max(0.0, max(viol_0, viol_1));
    	}

    void updateGains(unsigned id, unsigned from, unsigned to);
    void fixAllGains(HGFEdge& edge, int delta);
    void fixOneGain (HGFEdge& edge, int delta, unsigned side = 0);

  public:

    bool allowIllegalMoves;


    FMNetCut2WayMoveManager(const PartitioningProblem& problem,
		            const PartitionerParams& params,
				bool allowCorkingNodes);
    virtual ~FMNetCut2WayMoveManager();

	//statis functions

    virtual unsigned 	getCost()       const { return _curCost;}
    virtual double  	getCostDouble() const { return _curCost;}

    virtual double      getViolation() const
	{ return _fakeThis->_calcViolation();}

    virtual double	getImbalance() const
	{ return _fakeThis->_calcImbalance();}

    virtual const vector<double>& getAreas() const
	{ return _partAreas;}

	//movement functions

    virtual bool    	pickMoveApplyIt();

    virtual void        reinitialize();  //called after each pass
    virtual void        computeGains();  //does the initial gain computation
    virtual void        resetTo(PartitioningSolution& newSol); 
				//called before each start

    virtual void     undo(unsigned count) {_moveLog.undo(count);}
    const SVMoveLog& getMoveLog() const   { return _moveLog;    }

    virtual void     setupClip() {  _gainCo.setupClip(); }

};

#endif
