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





#ifndef _ANALYTICALSOLVER_H_
#define _ANALYTICALSOLVER_H_


#include "ABKCommon/sgi_hash_map.h"
#include <map>
#include "ABKCommon/abkcommon.h"
#include "RBPlace/RBPlacement.h"
using namespace std;

class AnalyticalSolver
{
 private:
  RBPlacement& _rbplace;
  const PlacementWOrient& _placement;
  vector<BBox>& _binBBoxs;  //BBox of bin region. used for terminal propagation
  vector< vector<unsigned> >& _nodes; //ids of nodes in bin

  //holds final placement and unconstrained solution if CG constraints
  vector< vector<Point> > _nodesPlacement; 
  //holds natural solution to be used only to impose CG constraints
  vector< vector<Point> > _nodesPlacementNatural; 

  vector< hash_map<unsigned, unsigned, hash<unsigned>, equal_to<unsigned> > > 
    _mapping;
  hash_map<unsigned, unsigned, hash<unsigned>, equal_to<unsigned> > 
    _mappingNodesToBin;

  //sizes of all nodes in the bin. used for imposing CG constraints
  vector<double> _binNodesSizes;

  unsigned _numBins;

  unsigned _skipNetsLargerThan; //this is set by default to UINT_MAX
                               //set this with function setSkipNetsLargerThan()

 public:
  AnalyticalSolver();
  AnalyticalSolver(RBPlacement& rbp, vector< vector<unsigned> >& nodes, 
		   vector<BBox>& binBBoxs);
  AnalyticalSolver(RBPlacement& rbp, const PlacementWOrient& placement, 
		   vector< vector<unsigned> >& nodes, vector<BBox>& binBBoxs);
  ~AnalyticalSolver();
  
  void setSkipNetsLargerThan(unsigned maxNetDegree);

  void solveQuadraticMin();
  void solveQuadraticMin(double epsilon);
  void combineNaturalUnconstrained();

  void solveSOR(double epsilon, bool naturalSoln=false);
  //do one iteration of SOR in the bin
  double doOneBinPassSOR(unsigned binIdx, bool naturalSoln=false); 

  Point getNodeOptLocUnconstrained(unsigned index, unsigned binIdx);
  Point getNodeOptLocNatural(unsigned index, unsigned binIdx);

  void initNodeLocsToBinCenter();
  void initNodeLocs(Point& location, unsigned binIdx);

  vector< vector<Point> >& getNodeLocs();
  vector< vector<Point> >& getNodeNaturalLocs();

};

#endif
