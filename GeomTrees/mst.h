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






#ifndef _MST8_WRAPPER_H_
#define _MST8_WRAPPER_H_

#include "Geoms/point.h"
#include "geomTreeBase.h"
#include "Placement/placement.h"

//: Minimum spanning tree
double getMSTCost(const Placement& pl);
double getMSTCost(const Placement& pl, unsigned *pairs,unsigned nPairs);
                                  // allows for "equivalent points",
                                   // e.g., pins within a net which belong to
                                   // the same cell 
                                   // The pairs array should ne 2*nPairs long
                                   // pairs[2*nPairs] is equivalent to
                                   // pairs[2*nPairs+1]

class MST8
{
    vector<GeomTreeEdge>    _mstEdges;
    double                  _mstCost;

  public:
    MST8(const vector<Point>& points);
    MST8(const vector<Point>& points, unsigned *pairs, unsigned nPairs);

    double getCost() const { return _mstCost; }
    const vector<GeomTreeEdge>& getEdges() const { return _mstEdges; }
};

#endif
