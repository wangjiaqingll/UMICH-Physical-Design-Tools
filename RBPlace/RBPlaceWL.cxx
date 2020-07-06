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



#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "RBPlacement.h"

#ifdef _MSC_VER
#ifndef rint
#define rint(a) floor((a)+0.5)
#endif
#endif

double RBPlacement::evalHPWL(bool usePinLocs)  const
{
    double totalWL = 0;
    for(unsigned e = 0; e < _hgWDims->getNumEdges(); e++)
	totalWL += _nonConstThis->oneNetHPWL(e, usePinLocs);

    return totalWL;
}


//by sadya to report X/Y components of HPWL
pair<double, double> RBPlacement::evalXYHPWL(bool usePinLocs) const
{
  double XWL = 0;
  double YWL = 0;
  pair<double , double> netWL;
  for(unsigned e = 0; e < _hgWDims->getNumEdges(); e++)
    {
      netWL = _nonConstThis->oneNetXYHPWL(e, usePinLocs);
      XWL += netWL.first;
      YWL += netWL.second;
    }
  pair<double , double> totalWL(XWL, YWL);
  return totalWL;
}

#define NumNetCostFactors 51

static double NetCostFactorTable[NumNetCostFactors]=
{
                0.0,
                1.000000, 1.000000, 1.000000, 1.082797, 1.153598,
                1.220592, 1.282322, 1.338501, 1.399094, 1.449260,
                1.497380, 1.545500, 1.593619, 1.641739, 1.689859,
                1.730376, 1.770893, 1.811410, 1.851927, 1.892444,
                1.928814, 1.965184, 2.001553, 2.037923, 2.074293,
                2.106117, 2.137941, 2.169766, 2.201590, 2.233414,
                2.264622, 2.295830, 2.327038, 2.358246, 2.389454,
                2.418676, 2.447898, 2.477119, 2.506341, 2.535563,
                2.560954, 2.586346, 2.611737, 2.637129, 2.662520,
                2.688684, 2.714848, 2.741013, 2.767177, 2.793341
};

double RBPlacement::evalWeightedWL(bool usePinLocs)  const
{
    double totalWL = 0;
    for(unsigned e = 0; e < _hgWDims->getNumEdges(); e++)
    {
	double netsWL = _nonConstThis->oneNetHPWL(e, usePinLocs);
	unsigned edgeDegree = _hgWDims->getEdgeByIdx(e).getDegree();

	if(edgeDegree < NumNetCostFactors)
	    netsWL *= NetCostFactorTable[edgeDegree];
	else
	    netsWL *= NetCostFactorTable[NumNetCostFactors-1];
	totalWL += netsWL;
    }

    return totalWL;
}

double RBPlacement::evalNetsHPWL(const vector<unsigned>& netsToEval,
				 bool usePinLocs) const
{
    double totalWL = 0;
    for(unsigned e = 0; e < netsToEval.size(); e++)
    {
	totalWL += _nonConstThis->oneNetHPWL(netsToEval[e], usePinLocs);
    }

    return totalWL;
}

double RBPlacement::oneNetHPWL(unsigned netId, bool usePinLocs)
{

   abkassert(netId < _hgWDims->getNumEdges(), "netID out of range");

    BBox       netBox;

    const HGFEdge& net = _hgWDims->getEdgeByIdx(netId);

    itHGFNodeLocal n;
    unsigned nodeOffset = 0;
    for(n = net.nodesBegin(); n != net.nodesEnd(); n++, nodeOffset++)
    {
        unsigned cellId = (*n)->getIndex();
	if(!usePinLocs)
	  {
	    double cellHeight = _hgWDims->getNodeHeight(cellId);
	    double cellWidth = _hgWDims->getNodeWidth(cellId);
	    Point centerOffset(cellWidth/2.0, cellHeight/2.0);
            netBox += (_placement[cellId] + centerOffset);
	  }
	else
	{
	    Point pinOffset = 
		_hgWDims->nodesOnEdgePinOffset(nodeOffset,netId,
					_placement.getOrient(cellId));

            netBox += (_placement[cellId]+pinOffset);
	}
    }

    return netBox.getHalfPerim();
}

//by sadya to get X/Y components of HPWL
pair<double, double> RBPlacement::oneNetXYHPWL(unsigned netId, bool usePinLocs)
{
  
  abkassert(netId < _hgWDims->getNumEdges(), "netID out of range");

  BBox       netBox;
  
  const HGFEdge& net = _hgWDims->getEdgeByIdx(netId);
  
  itHGFNodeLocal n;
  unsigned nodeOffset = 0;
  for(n = net.nodesBegin(); n != net.nodesEnd(); n++, nodeOffset++)
    {
      unsigned cellId = (*n)->getIndex();
      if(!usePinLocs)
	{
	  double cellHeight = _hgWDims->getNodeHeight(cellId);
	  double cellWidth = _hgWDims->getNodeWidth(cellId);
	  Point centerOffset(cellWidth/2.0, cellHeight/2.0);
	  netBox += (_placement[cellId] + centerOffset);
	}
      else
	{
	  Point pinOffset = 
	    _hgWDims->nodesOnEdgePinOffset(nodeOffset,netId,
					   _placement.getOrient(cellId));
	  
	  netBox += (_placement[cellId]+pinOffset);
	}
    }
  
  pair<double, double> netWL(netBox.getWidth(), netBox.getHeight());
  return netWL;
}


//added by sadya to calculate the HPWL of a group of cells
// will be used in overlap removal and rowIroning
double RBPlacement::calcInstHPWL(vector<unsigned> cellIds)
{
  bit_vector seenNets((*_hgWDims).getNumEdges(),0);

  double totalHPWL = 0;

  for(unsigned i=0;i<cellIds.size();++i)
    {
      const HGFNode& node=_hgWDims->getNodeByIdx(cellIds[i]);
      for(itHGFEdgeLocal e = node.edgesBegin();e != node.edgesEnd();e++)
	{
	  const HGFEdge& edge=(**e);
	  unsigned netIdx=edge.getIndex();
	  if(!seenNets[netIdx])
	    {
	      seenNets[netIdx] = 1;
	      totalHPWL += oneNetHPWL(netIdx,true);
	    }
	}
    }
  return totalHPWL;
}


// added by MRG to find mean of pins attached to nets of this cell 
Point RBPlacement::calcMeanLoc(unsigned cellId, bool usePinLocs)
{
  unsigned pinCount=0;
  Point meanLoc(0,0);
  bit_vector seenNets((*_hgWDims).getNumEdges(),0);
  
  // for each net attached to the cell
  const HGFNode& node=_hgWDims->getNodeByIdx(cellId);
  for(itHGFEdgeLocal e = node.edgesBegin();e != node.edgesEnd();e++)
    {
      const HGFEdge& edge=(**e);
      unsigned netIdx=edge.getIndex();
      if(!seenNets[netIdx])
	{
	  seenNets[netIdx] = 1;
	  // for each pin on the net
	  unsigned nodeOffset=0;
	  for(itHGFNodeLocal n = edge.nodesBegin(); n != edge.nodesEnd(); n++, nodeOffset++)
	    {
	      unsigned id = (*n)->getIndex();
	      // skip myself
	      if (id==cellId) continue;
	      if (!usePinLocs)
		{
		  meanLoc+=_placement[id];
		}
	      else
		{
		  Point pinOffset = 
		    _hgWDims->nodesOnEdgePinOffset(nodeOffset,netIdx,_placement.getOrient(id));
		  meanLoc+=_placement[id]+pinOffset;
		}
	      pinCount++;
	    }
	}
    }
  
  if (pinCount==0) {
    meanLoc=_placement[cellId];
  }
  else
    {
      meanLoc.x/=pinCount;
      meanLoc.y/=pinCount;
    }
  return(meanLoc);
}




