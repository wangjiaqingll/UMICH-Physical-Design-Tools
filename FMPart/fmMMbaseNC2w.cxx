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




//created on 09/17/98 by Igor Markov 
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef _FMMMbaseNC2wNC2w_CXX_
#define _FMMMbaseNC2wNC2w_CXX_

#include <algorithm>
using std::fill;
#include "fmMMbaseNC2w.h"
using std::bit_vector;

FMMMbaseNC2w::FMMMbaseNC2w
      (const PartitioningProblem& problem, const PartitionerParams& params, 
      bool allowCorkingNodes)
        :MoveManagerInterface(problem), 
         _lockedModules(problem.getHGraph().getNumNodes()),
         _lockedNets        (problem.getHGraph().getNumEdges()),
         _lockedNetConfigIds(problem.getHGraph().getNumEdges()),
         _numMovable(problem.getHGraph().getNumNodes() -
	 			problem.getHGraph().getNumTerminals()),
         _numTerminals(problem.getHGraph().getNumTerminals()),
         _numParts(problem.getNumPartitions()),
         _gainCo(problem.getHGraph().getNumNodes(),problem.getNumPartitions(),params),
         _partLeg(problem),
         _moveLog(), 
         _movingModuleIdx(UINT_MAX), 
         _moveFromPart(UINT_MAX), 
	 _moveToPart(UINT_MAX), 
         _numIllegalMoves(0),
         _allowCorkingNodes(allowCorkingNodes),
	 _part(NULL), 
         _problem(&problem), 
         _hg(problem.getHGraph()), 
	 _terminalsCountAs(1),
         _tallies(NULL),
         _totalCost(UINT_MAX), 
	 _netCosts(problem.getHGraph().getNumEdges()),
         _edgeWeights(problem.getHGraph().getNumEdges()),
	 _maxEdgeWeight(0),
	 _useWts(params.useWts)
{  
   // need to call resetTo before the pass and then
   // computeGains()  - partitoner needs to call this when it is ready;
   //                   this will unlock the nodes that are being considered

   const vector<vector<double> >& maxAreas = problem.getMaxCapacities();  
   const vector<vector<double> >& minAreas = problem.getMinCapacities();
   _tol = min(maxAreas[0][0]-minAreas[0][0], maxAreas[1][0]-minAreas[1][0]);

   unsigned numShort=2*problem.getHGraph().getNumEdges();
   _tallies=new unsigned short[numShort];

   if(_useWts)
   {
     unsigned eWeight;
     const HGraphFixed& hg=problem.getHGraph();
     for(unsigned e = 0; e < hg.getNumEdges(); e++)	
     {
       eWeight=static_cast<unsigned>(rint(hg.getEdgeByIdx(e).getWeight()));
       _edgeWeights[e] = eWeight;
       if(eWeight>_maxEdgeWeight)_maxEdgeWeight=eWeight;
     }
   }
}

void FMMMbaseNC2w::initializeGainBuckets()
{
    unsigned maxDegree = _problem->getHGraph().getMaxNodeDegree();  
    int maxGain;
    if(_useWts)
       maxGain = maxDegree * 
                  ( getMaxCostOfOneNetWW() - getMinCostOfOneNet() );
    else
       maxGain = maxDegree * 
                  ( getMaxCostOfOneNet() - getMinCostOfOneNet() );
    _gainCo.setMaxGain(maxGain);
}

void FMMMbaseNC2w :: reinitialize()
{ 
   fill(_lockedNetConfigIds.begin(),_lockedNetConfigIds.end(),0);
   fill(_lockedNets        .begin(),_lockedNets        .end(),false);

   _partLeg.reinitialize(); 

   reinitialize_tallies();
   _gainCo.reinitialize();
   _gainCo.setPartitionBias(0);
// computeGains should be called by partioner

   const vector<double>& newAreas = _partLeg.getActualBalances();
   _partSol->partArea[0][0] = newAreas[0];
   _partSol->partArea[1][0] = newAreas[1];

   _moveLog.clear();

   if(_partLeg.getViolation() == 0)
	_solnLegalAtStart = true;
   else
	_solnLegalAtStart = false;
}

void FMMMbaseNC2w::resetTo(PartitioningSolution& newSol) 
{   
    _partSol = &newSol; 
    _curPart = &(newSol.part); 

    fill(_lockedNetConfigIds.begin(),_lockedNetConfigIds.end(),0);
    fill(_lockedNets        .begin(),_lockedNets        .end(),false);

    _partLeg.resetTo(newSol.part);

    //resetTo(newSol.part);
    _part=&newSol.part;
    reinitialize_tallies();

    initializeGainBuckets();  // uses Max/Min net cost from _eval
    _moveLog.resetTo(newSol.part);
 
    const vector<double>& newAreas = _partLeg.getActualBalances();
    for(unsigned k = 0; k < _numParts; k++)
	_partSol->partArea[k][0] = newAreas[k];

    _gainCo.reinitialize(); 
    _gainCo.setPartitionBias(0);
}

void 
FMMMbaseNC2w::computeGainsOfModule(unsigned moduleIdx)
{
  unsigned from = (*_curPart)[moduleIdx].isInPart(0) ? 0 : 1;
  PartitionIds canGoTo=(*_fixedConstr)[moduleIdx];

  /*cout<<"CurPart:"<<endl;
  for(int i = 0; i<_problem->getHGraph().getNumNodes(); i++)
  	cout<<(*_curPart)[i].isInPart(1)<<" ";
*/
  const HGFNode& node=_problem->getHGraph().getNodeByIdx(moduleIdx);
  unsigned to=1-from;

  bool tooLarge = (!_allowCorkingNodes  && _solnLegalAtStart &&
		   node.getWeight() >= _tol);

  //if(tooLarge) cout<<"Skipping node "<<moduleIdx<<endl;

  if(_lockedModules[moduleIdx]= (!canGoTo[to] || tooLarge))
    /* assignment */
    {
      for(itHGFEdgeLocal e = node.edgesBegin(); e!=node.edgesEnd(); e++)
	_lockedNetConfigIds[(*e)->getIndex()]|=(1<<from);
      return;
    }
  int gain=0;

  for(itHGFEdgeLocal e = node.edgesBegin(); e!=node.edgesEnd(); e++)
    {
      unsigned edgeIdx=(*e)->getIndex();
      if      (_tallies[2*edgeIdx+  to]==0)      gain--;
      else if (_tallies[2*edgeIdx+from]==1)      gain++;
    }    
    //cout << "adding element "<<moduleIdx<<" to "<<to<<" with gain "<<gain<<endl;
  _gainCo.addElement(moduleIdx,to,gain);
}


void 
FMMMbaseNC2w::computeGainsOfModuleWW(unsigned moduleIdx)
{
  unsigned from = (*_curPart)[moduleIdx].isInPart(0) ? 0 : 1;
  PartitionIds canGoTo=(*_fixedConstr)[moduleIdx];

  /*cout<<"CurPart:"<<endl;
  for(int i = 0; i<_problem->getHGraph().getNumNodes(); i++)
  	cout<<(*_curPart)[i].isInPart(1)<<" ";
  */
  const HGFNode& node=_problem->getHGraph().getNodeByIdx(moduleIdx);
  unsigned to=1-from;

  bool tooLarge = (!_allowCorkingNodes  && _solnLegalAtStart &&
		   node.getWeight() >= _tol);

  //if(tooLarge) cout<<"Skipping node "<<moduleIdx<<endl;

  if(_lockedModules[moduleIdx]= (!canGoTo[to] || tooLarge))
    /* assignment */
    {
      for(itHGFEdgeLocal e = node.edgesBegin(); e!=node.edgesEnd(); e++)
	_lockedNetConfigIds[(*e)->getIndex()]|=(1<<from);
      return;
    }
  int gain=0;

  for(itHGFEdgeLocal e = node.edgesBegin(); e!=node.edgesEnd(); e++)
    {
      unsigned edgeIdx=(*e)->getIndex();
      if      (_tallies[2*edgeIdx+  to]==0)      gain-=_edgeWeights[edgeIdx];
      else if (_tallies[2*edgeIdx+from]==1)      gain+=_edgeWeights[edgeIdx];
    }
    //cout << "adding element "<<moduleIdx<<" to "<<to<<" with gain "<<gain<<endl;
  _gainCo.addElement(moduleIdx,to,gain);
  }
#endif



