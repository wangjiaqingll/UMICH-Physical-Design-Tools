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

#include "mst.h"
#include "MST8/mst8.h"
#include "ABKCommon/abkcommon.h"
#include "Placement/placement.h"

extern int _MST_Nsoln;

double  getMSTCost(const Placement& pl)
{
  vector<Point> xyPoints;
  for (unsigned i=0; i<pl.getSize(); i++)
     xyPoints.push_back(Point(pl[i].x, pl[i].y));
  MST8 mst(xyPoints);
  return mst.getCost();
}

double getMSTCost(const Placement& pl, unsigned *pairs,unsigned nPairs)
				   // allows for "equivalent points",
                                   // e.g., pins within a net which belong to
                                   // the same cell 
                                   // The pairs array should ne 2*nPairs long
				   // pairs[2*nPairs] is equivalent to
				   // pairs[2*nPairs+1]
{
  vector<Point> xyPoints;
  for (unsigned i=0; i<pl.getSize(); i++)
     xyPoints.push_back(Point(pl[i].x, pl[i].y));
  MST8 mst(xyPoints, pairs, nPairs);
  return mst.getCost();
}

MST8::MST8(const vector<Point>& points)
{
    unsigned i, cost;
    unsigned numPoints = points.size();
    mst_package_init(numPoints + 5);
    start_net();
    for(i = 0; i < numPoints; i++)
       add_new_point(int(points[i].x),int(points[i].y),(void *)(long int)i);
    end_net();
    mst();
    cost = 0;
    for(i = 0; i < static_cast<unsigned>(_MST_Nsoln); i++)
    {
        int x1, y1, x2, y2;
        void *p1, *p2;
        int st = mst_arc(i, &x1, &y1, &p1, &x2, &y2, &p2);
        if (st)
        {
            _mstEdges.push_back(GeomTreeEdge(Point(x1,y1), Point(x2,y2)));
            cost += abs(x1-x2) + abs(y1-y2);
        }
    }
    mst_package_done();
    _mstCost = static_cast<double>(cost);
}

// This is the MST with equivalent points
// e.g., pins within a net which belong to the same cell
MST8::MST8(const vector<Point>& points, unsigned *pairs, unsigned nPairs)
{
    if(nPairs)
        abkfatal(pairs != NULL,
                 "Can\'t get MST cost: equiv. pairs array corrupted");

    unsigned i, cost;
    unsigned numPoints = points.size();
    mst_package_init(numPoints + 10);
    start_net();
    for(i = 0; i < numPoints; i++)
        add_new_point(int(points[i].x),int(points[i].y),(void *)(long int)i);
    end_net();
    for(i = 0; i < nPairs; i++)
        make_equiv((void*)(long int)pairs[2*i], (void*)(long int)pairs[2*i+1]);
    mst();
    cost = 0;
    for(i = 0; i < static_cast<unsigned>(_MST_Nsoln); i++)
    {
        int x1, y1, x2, y2;
        void *p1, *p2;
        int st = mst_arc(i, &x1, &y1, &p1, &x2, &y2, &p2);
        if (st)
        {
            _mstEdges.push_back(GeomTreeEdge(Point(x1,y1), Point(x2,y2)));
            cost += abs(x1-x2) + abs(y1-y2);
        }
    }
    mst_package_done();
    _mstCost = static_cast<double>(cost);
}

