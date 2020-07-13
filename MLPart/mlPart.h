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











// Created by Igor Markov, 070298
// Version 4 by Andrew Caldwell, 090998

#ifndef  __ML_PART__H__
#define  __ML_PART__H__

#include "Partitioning/partitioning.h"
#include "Placement/placeWOri.h"
#include "baseMLPart.h"

class FillableHierarchy;

class MLPart : public BaseMLPart
{
    void _startWithPartProblem(PartitioningProblem&, ClusteredHGraph& );

    bool _go();	//returns true if the solutions generated are valid
    bool _doMultiLevel();	//returns true if the solns generated are valid
    void _figureBestCost(PartitioningProblem&);
    
    double _peakMemUsage;

  public:

    MLPart(PartitioningProblem& problem, const MLPartParams& params,
		FillableHierarchy* hier = NULL);
    MLPart(PartitioningProblem& problem, const MLPartParams& params,
	   PlacementWOrient * placement,
	   FillableHierarchy* hier = NULL);

    ~MLPart() {}
};

#endif
