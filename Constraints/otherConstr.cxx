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






// Created: Aug 16, 1997 by Igor Markov
// 970819 mro parenthesized &splIndices
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <math.h>
#include <vector>
using std::vector;
#include "ABKCommon/abkcommon.h"
#include "Placement/placement.h"
#include "Placement/subPlacement.h"
#include "constraint.h"
#include "otherConstr.h"


Constraint*
TetheredGroupConstraint::_virtPullBack(const vector<unsigned>& hitIndices,
                                    const SubPlacement& newSubPl) const
{
// const vector<unsigned> (&splIndices)(_spl.getMapping());
   Placement centers(hitIndices.size());
   
   for(unsigned k=0;k<hitIndices.size();k++)
      centers[k]=_pl[hitIndices[k]];

   return new TetheredGroupConstraint(newSubPl,centers,_dist);
}

Constraint*
COGConstraint::_virtPullBack(const vector<unsigned>& hitIndices,
                                    const SubPlacement& newSubPl) const
{
   (void)  hitIndices;
   return new COGConstraint(newSubPl,_center);
}

Constraint*
EqualXConstraint::_virtPullBack(const vector<unsigned>& hitIndices,
                                    const SubPlacement& newSubPl) const
{
   (void)  hitIndices;
   return new EqualXConstraint(newSubPl);
}

Constraint*
EqualYConstraint::_virtPullBack(const vector<unsigned>& hitIndices,
                                    const SubPlacement& newSubPl) const
{  
   (void)  hitIndices;
   return new EqualYConstraint(newSubPl);
}

Constraint*
StayTogetherConstraint::_virtPullBack(const vector<unsigned>& hitIndices,
                                    const SubPlacement& newSubPl) const
{
   (void) hitIndices;
   return new StayTogetherConstraint(newSubPl,_bb);
}
