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






//created by Stefanus Mantik on 02/14/98  stefanus@cs.ucla.edu
//upgraded to PlaceEval
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef __ONE_STEINER_PLACE_EVALUATOR_CXX__
#define __ONE_STEINER_PLACE_EVALUATOR_CXX__

#include "oneSteinerPlEval.h"
#include "GeomTrees/b1Steiner.h"

void OneSteinerPlEval::_recomputeCost()
{
    for(itHGFEdgeGlobal e = _hg.edgesBegin(); e != _hg.edgesEnd(); e++)
    {
        if((*e)->getDegree() > 500)
        {
            BBox currentBBox;
            for(itHGFNodeLocal n = (*e)->nodesBegin();
                n != (*e)->nodesEnd(); n++)
                   currentBBox += (*_placement)[(*n)->getIndex()];
            _netCost[(*e)->getIndex()] = currentBBox.getHalfPerim();
        }
        else
            _netCost[(*e)->getIndex()] = _computeOneEdge(**e);
    }
}

double OneSteinerPlEval::_computeOneEdge(const HGFEdge &edge)
{
    unsigned degree = edge.getDegree();
    if (degree <= 1) return 0;
    itHGFNodeLocal n;
    if (degree <= 3)    // for: 2-pin and 3-pin nets, just use BBox to
    {                   // speed up calculation
        BBox currentBBox;
        for(n = edge.nodesBegin(); n != edge.nodesEnd(); n++)
            currentBBox += (*_placement)[(*n)->getIndex()];
        return currentBBox.getHalfPerim();
    }

    vector<Point> pointLocs;
    for(n = edge.nodesBegin(); n != edge.nodesEnd(); n++)
        pointLocs.push_back((*_placement)[(*n)->getIndex()]);
    BatchedI1Steiner steinerTree(pointLocs);
    return steinerTree.getCost();
}

#endif
