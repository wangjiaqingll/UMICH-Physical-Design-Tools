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





#define SOR_FACTOR 1.7

#include "analytPl.h"

AnalyticalSolver::~AnalyticalSolver()
{
  _mapping.clear();
  _mappingNodesToBin.clear();
  _nodesPlacement.clear();
  _nodesPlacementNatural.clear();
  _binNodesSizes.clear();
}

AnalyticalSolver::AnalyticalSolver(RBPlacement& rbp, 
				   vector< vector<unsigned> >& nodes, 
				   vector<BBox>& binBBoxs) :  _rbplace(rbp),
				     _placement(rbp.getPlacement()),
				     _binBBoxs(binBBoxs), _nodes(nodes),
				     _skipNetsLargerThan(UINT_MAX)
{
  _numBins = _nodes.size();
  _nodesPlacement.resize(_numBins);
  _nodesPlacementNatural.resize(_numBins);
  _mapping.resize(_numBins);
  _binNodesSizes.resize(_numBins);

  const HGraphWDimensions& hgWDims = _rbplace.getHGraph();
  double binNodesSize=0;
  for(unsigned i=0; i<_numBins; ++i)
    {
      binNodesSize = 0;
      _nodesPlacement[i].resize(_nodes[i].size());
      _nodesPlacementNatural[i].resize(_nodes[i].size());
      for(unsigned j=0; j<_nodes[i].size(); ++j)
	{
	  unsigned index = _nodes[i][j];
	  _mapping[i][index] = j;
	  _nodesPlacement[i][j].x = _placement[index].x;
	  _nodesPlacement[i][j].y = _placement[index].y;
	  _nodesPlacementNatural[i][j] = _nodesPlacement[i][j];
	  _mappingNodesToBin[index] = i;
	  
	  binNodesSize += hgWDims.getNodeWidth(index) 
	  * hgWDims.getNodeHeight(index);
	}
      _binNodesSizes[i] = binNodesSize;
    }
}

AnalyticalSolver::AnalyticalSolver(RBPlacement& rbp,  
				   const PlacementWOrient& placement,
                                   vector< vector<unsigned> >& nodes,
                                   vector<BBox>& binBBoxs) :  _rbplace(rbp),
				   _placement(placement),
				   _binBBoxs(binBBoxs), _nodes(nodes),
				   _skipNetsLargerThan(UINT_MAX)

{
  _numBins = _nodes.size();
  _nodesPlacement.resize(_numBins);
  _nodesPlacementNatural.resize(_numBins);
  _mapping.resize(_numBins);
  _binNodesSizes.resize(_numBins);

  const HGraphWDimensions& hgWDims = _rbplace.getHGraph();
  double binNodesSize=0;
  for(unsigned i=0; i<_numBins; ++i)
    {
      binNodesSize = 0;
      _nodesPlacement[i].resize(_nodes[i].size());
      _nodesPlacementNatural[i].resize(_nodes[i].size());
      for(unsigned j=0; j<_nodes[i].size(); ++j)
	{
	  unsigned index = _nodes[i][j];
          _mapping[i][index] = j;
          _nodesPlacement[i][j].x = _placement[index].x;
          _nodesPlacement[i][j].y = _placement[index].y;
	  _nodesPlacementNatural[i][j] = _nodesPlacement[i][j];
	  _mappingNodesToBin[index] = i;
	  binNodesSize += hgWDims.getNodeWidth(index) 
	                  * hgWDims.getNodeHeight(index);
	}
      _binNodesSizes[i] = binNodesSize;
    }
}

void AnalyticalSolver::setSkipNetsLargerThan(unsigned maxNetDegree)
{
  _skipNetsLargerThan = maxNetDegree;
}

vector< vector<Point> >& AnalyticalSolver::getNodeLocs()
{
  return _nodesPlacement;
}

vector< vector<Point> >& AnalyticalSolver::getNodeNaturalLocs()
{
  return _nodesPlacementNatural;
}

void AnalyticalSolver::initNodeLocsToBinCenter()
{
  for(unsigned b=0; b<_numBins; ++b)
    {
      Point binCenter = _binBBoxs[b].getGeomCenter();
      initNodeLocs(binCenter, b);
    }
}

void AnalyticalSolver::initNodeLocs(Point& location, unsigned binIdx)
{
  for(unsigned i=0; i<_nodes[binIdx].size(); ++i)
    {
      _nodesPlacement[binIdx][i].x = location.x;
      _nodesPlacement[binIdx][i].y = location.y;
      _nodesPlacementNatural[binIdx][i].x = location.x;
      _nodesPlacementNatural[binIdx][i].y = location.y;
    } 
}

void AnalyticalSolver::solveQuadraticMin()
{
  double maxDimension = max(_binBBoxs[0].getHeight(),_binBBoxs[0].getWidth());
  double epsilon = (maxDimension*sqrt(double(_numBins)))/100;
  solveQuadraticMin(epsilon);
}

void AnalyticalSolver::solveQuadraticMin(double epsilon)
{
  solveSOR(epsilon, 0);  //get unconstrained solution
  solveSOR(epsilon, 1);  //get natural solution
  combineNaturalUnconstrained();
}

void AnalyticalSolver::combineNaturalUnconstrained()
{
  const HGraphWDimensions& hgWDims = _rbplace.getHGraph();
  for(unsigned b=0; b<_numBins; ++b)
    {
      double xLambda;
      double yLambda;
      double xWeightUnconstrained=0;
      double xWeightNatural=0;
      double yWeightUnconstrained=0;
      double yWeightNatural=0;
      for(unsigned i=0; i<_nodes[b].size(); ++i)
	{
	  unsigned index = _nodes[b][i];
	  double nodeSize = hgWDims.getNodeWidth(index)
	                    * hgWDims.getNodeHeight(index);
	  xWeightUnconstrained += nodeSize*_nodesPlacement[b][i].x;
	  yWeightUnconstrained += nodeSize*_nodesPlacement[b][i].y;
	  xWeightNatural += nodeSize*_nodesPlacementNatural[b][i].x;
	  yWeightNatural += nodeSize*_nodesPlacementNatural[b][i].y;
	}
      Point CG = _binBBoxs[b].getGeomCenter();
      xLambda = (_binNodesSizes[b]*CG.x - xWeightUnconstrained) 
	/ xWeightNatural;
      yLambda = (_binNodesSizes[b]*CG.y - yWeightUnconstrained)
	/ yWeightNatural;

      for(unsigned i=0; i<_nodes[b].size(); ++i)
	{
	  _nodesPlacement[b][i].x += xLambda*_nodesPlacementNatural[b][i].x;
	  _nodesPlacement[b][i].y += yLambda*_nodesPlacementNatural[b][i].y;
	}
    }
}

void AnalyticalSolver::solveSOR(double epsilon, bool naturalSoln)
{  
  double change = DBL_MAX;
  unsigned numIter = 0;
    
  while(change > epsilon && numIter <500  )
    {
      change = 0;
      numIter++;
      for(unsigned i=0; i<_numBins; ++i)
	change = max(doOneBinPassSOR(i, naturalSoln), change);
      cout<<"Iteration "<<numIter<<"  Max Movement "<<change<<endl;
    }
  cout<<"Epsilon is "<<epsilon<<"  numIter  "<<numIter<<endl;
}

double AnalyticalSolver::doOneBinPassSOR(unsigned binIdx, bool naturalSoln)
{
  double xchange, ychange, xOvershoot, yOvershoot, indChange;
  double change = 0;
  for(unsigned i=0; i<_nodes[binIdx].size(); ++i)
    {
      unsigned index = _nodes[binIdx][i];
      Point newLoc;
      if(naturalSoln == 0) //unconstrained solution
	{
	  newLoc = getNodeOptLocUnconstrained(index, binIdx);
	  xchange = newLoc.x - _nodesPlacement[binIdx][i].x;
	  ychange = newLoc.y - _nodesPlacement[binIdx][i].y;
	  xOvershoot = xchange*SOR_FACTOR;
	  yOvershoot = ychange*SOR_FACTOR;
	  _nodesPlacement[binIdx][i].x += xOvershoot;
	  _nodesPlacement[binIdx][i].y += yOvershoot;
	}
      else //natural solution
	{
	  newLoc = getNodeOptLocNatural(index, binIdx);
	  xchange = newLoc.x - _nodesPlacementNatural[binIdx][i].x;
	  ychange = newLoc.y - _nodesPlacementNatural[binIdx][i].y;
	  xOvershoot = xchange*SOR_FACTOR;
	  yOvershoot = ychange*SOR_FACTOR;
	  _nodesPlacementNatural[binIdx][i].x += xOvershoot;
	  _nodesPlacementNatural[binIdx][i].y += yOvershoot;
	}
      indChange = fabs(xchange) + fabs(ychange);
      change = max(change,indChange);
    }
  return change;
}

Point AnalyticalSolver::getNodeOptLocUnconstrained(unsigned index, 
						   unsigned binIdx)
{
  Point optLoc;
  const HGraphWDimensions& hgWDims = _rbplace.getHGraph();
  const HGFNode& origNode = hgWDims.getNodeByIdx(index);
  itHGFEdgeLocal e;
  itHGFNodeLocal v;
  double xSum=0, ySum=0;
  unsigned edgeOffset=0;
  double denominator = 0;
  for(e = origNode.edgesBegin(); e != origNode.edgesEnd(); ++e, ++edgeOffset)
    {
      HGFEdge& edge = **e;
      unsigned edgeId = edge.getIndex();

      Point pOffsetOrigNode = hgWDims.edgesOnNodePinOffset(edgeOffset, index,
						  _rbplace.getOrient(index));

      unsigned edgeDegree = edge.getDegree();
      double netXSum = 0;
      double netYSum = 0;
      unsigned nodeOffset = 0;      
      
      if(edgeDegree <= _skipNetsLargerThan)
	{
	  for (v=(*e)->nodesBegin(); v!=(*e)->nodesEnd(); v++, nodeOffset++)
	    {
	      HGFNode& node = (**v);
	      unsigned nodeIndex = node.getIndex();
	      
	      Point pOffset = hgWDims.nodesOnEdgePinOffset(nodeOffset, edgeId,
					        _rbplace.getOrient(nodeIndex));
	      if(_mapping[binIdx].count(nodeIndex) == 0) //node not in this 
		                                       //bin. so treat as fixed
		{
		  double pinXLoc, pinYLoc;
		  //node not in all bins 
		  if(_mappingNodesToBin.count(nodeIndex) == 0)
		    {
		      pinXLoc =
			_placement[nodeIndex].x + pOffset.x - pOffsetOrigNode.x;
		      pinYLoc = 
			_placement[nodeIndex].y + pOffset.y - pOffsetOrigNode.y;
		    }
		  else
		    {
		      unsigned nodeBinId = _mappingNodesToBin[nodeIndex];
		      unsigned mappedIdx = _mapping[nodeBinId][nodeIndex];
		      pinXLoc = _nodesPlacement[nodeBinId][mappedIdx].x 
			+ pOffset.x - pOffsetOrigNode.x; 
		      pinYLoc = _nodesPlacement[nodeBinId][mappedIdx].y 
			+ pOffset.y - pOffsetOrigNode.y; 
		    }
		  //do terminal propagation
		  if(pinXLoc < _binBBoxs[binIdx].xMin)
		    pinXLoc = _binBBoxs[binIdx].xMin;
		  else if(pinXLoc > _binBBoxs[binIdx].xMax)
		    pinXLoc = _binBBoxs[binIdx].xMax;
		  
		  if(pinYLoc < _binBBoxs[binIdx].yMin)
		    pinYLoc = _binBBoxs[binIdx].yMin;
		  else if(pinYLoc > _binBBoxs[binIdx].yMax)
		    pinYLoc = _binBBoxs[binIdx].yMax;
		  
		  netXSum += pinXLoc;
		  netYSum += pinYLoc;
		}
	      else if(nodeIndex != index)
		{
		  unsigned mappedIdx = _mapping[binIdx][nodeIndex];
		  netXSum += 
		    _nodesPlacement[binIdx][mappedIdx].x + pOffset.x 
		    - pOffsetOrigNode.x;
		  netYSum += 
		    _nodesPlacement[binIdx][mappedIdx].y + pOffset.y 
		    - pOffsetOrigNode.y;
		}
	    }
	}
      if(edgeDegree > 0 && edgeDegree <= _skipNetsLargerThan)
	{
	  double netWt = 2.0/edgeDegree;
	  netXSum = netWt*netXSum;
	  netYSum = netWt*netYSum;
	  xSum += netXSum;
	  ySum += netYSum;
	  
	  denominator += netWt*(edgeDegree-1.0);
	}
    }
  
  if(denominator != 0)
    {
      xSum /= denominator;
      ySum /= denominator;
      optLoc.x = xSum;
      optLoc.y = ySum;
    }
  else
    optLoc = _nodesPlacement[binIdx][_mapping[binIdx][index]];

  return optLoc;
}

Point AnalyticalSolver::getNodeOptLocNatural(unsigned index, unsigned binIdx)
{
  Point optLoc;
  const HGraphWDimensions& hgWDims = _rbplace.getHGraph();
  const HGFNode& origNode = hgWDims.getNodeByIdx(index);
  itHGFEdgeLocal e;
  itHGFNodeLocal v;
  double xSum=0, ySum=0;
  unsigned edgeOffset=0;
  double denominator = 0;
  for(e = origNode.edgesBegin(); e != origNode.edgesEnd(); ++e, ++edgeOffset)
    {
      HGFEdge& edge = **e;
      unsigned edgeId = edge.getIndex();

      Point pOffsetOrigNode = hgWDims.edgesOnNodePinOffset(edgeOffset, index,
						  _rbplace.getOrient(index));

      unsigned edgeDegree = edge.getDegree();
      double netXSum = 0;
      double netYSum = 0;
      unsigned nodeOffset = 0;      
      
      if(edgeDegree <= _skipNetsLargerThan)
	{
	  for (v=(*e)->nodesBegin(); v!=(*e)->nodesEnd(); v++, nodeOffset++)
	    {
	      HGFNode& node = (**v);
	      unsigned nodeIndex = node.getIndex();
	      
	      Point pOffset = hgWDims.nodesOnEdgePinOffset(nodeOffset, edgeId,
						_rbplace.getOrient(nodeIndex));
	      if(_mapping[binIdx].count(nodeIndex) == 0) //node not in this 
	                                               //bin. so treat as fixed
		{
		  //treat all fixed objects fixed to 0
		}
	      else if(nodeIndex != index)
		{
		  unsigned mappedIdx = _mapping[binIdx][nodeIndex];
		  netXSum += 
		    _nodesPlacement[binIdx][mappedIdx].x + pOffset.x 
		    - pOffsetOrigNode.x;
		  netYSum += 
		    _nodesPlacement[binIdx][mappedIdx].y + pOffset.y 
		    - pOffsetOrigNode.y;
		}
	    }
	}
      if(edgeDegree > 0 && edgeDegree <= _skipNetsLargerThan)
	{
	  double netWt = 2.0/edgeDegree;
	  netXSum = netWt*netXSum;
	  netYSum = netWt*netYSum;
	  xSum += netXSum;
	  ySum += netYSum;
	  
	  denominator += netWt*(edgeDegree-1.0);
	}
    }
  
  if(denominator != 0)
    {
      double nodeSize = hgWDims.getNodeWidth(index)
	                *hgWDims.getNodeHeight(index);
      xSum += nodeSize/_binNodesSizes[binIdx];
      ySum += nodeSize/_binNodesSizes[binIdx];
      xSum /= denominator;
      ySum /= denominator;
      optLoc.x = xSum;
      optLoc.y = ySum;
    }
  else
    optLoc = _nodesPlacementNatural[binIdx][_mapping[binIdx][index]];

  return optLoc;
}
